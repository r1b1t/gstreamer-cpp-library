#include "GStreamer/Manager.hpp"
#include <iostream>

// Yapıcı
GstManager::GstManager()
{
    if (!gst_is_initialized())
    {
        gst_init(nullptr, nullptr);
        std::cout << "[GstManager] GStreamer hazır.\n";
    } 
}

// Yıkıcı
GstManager::~GstManager()
{
    close();
}

// Pipeline Oluşturma Fonksiyonu
bool GstManager::addPipeline(const std::string &pipelineDesc)
{
    
    if (pipeline != nullptr)
    {
        std::cerr << "[GstManager] Hata: Zaten aktif bir pipeline var, önceki pipeline kapatılıyor.\n";
        close();
    }// Önceki bir işlem varsa temizle
    

    GError *error = nullptr;

    // Pipeline oluştur
    pipeline = gst_parse_launch(pipelineDesc.c_str(), &error);

    // Hata kontrolü
    if (error)
    {
        std::cerr << "[GstManager] Hata: " << error->message << std::endl;
        g_error_free(error);
        return false; // Hata varsa false dön ve fonksiyondan çık
    }

    // Bus al
    bus = gst_element_get_bus(pipeline);

    // Thread başlat
    isRunning = true;
    // Bus izleme thread'i başlat
    busThread = std::thread(&GstManager::busLoop, this);

    std::cout << "[GstManager] Pipeline oluşturuldu.\n";
    return true;
}

// Oynatma Fonksiyonu
void GstManager::play()
{
    std::lock_guard<std::mutex> lock(pipeMutex);
    if (pipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            std::cerr << "[GstManager] Oynatma başarısız: State değişimi hatası.\n";
        }
    }
    else
    {
        std::cerr << "[GstManager] Oynatma başarısız: Pipeline yok.\n";
    }
}

// Duraklatma Fonksiyonu
void GstManager::pause()
{
    std::lock_guard<std::mutex> lock(pipeMutex);
    if (pipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PAUSED);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            std::cerr << "[GstManager] Duraklatma başarısız: State değişimi hatası.\n";
        }
        else
        {
            std::cout << "[GstManager] Duraklatıldı.\n";
        }
    }
    else
    {
        std::cerr << "[GstManager] Duraklatma başarısız: Pipeline yok.\n";
    }
}

// Devam Ettirme Fonksiyonu
void GstManager::resume()
{
    std::lock_guard<std::mutex> lock(pipeMutex);
    if (pipeline)
    {
        // Resume teknik olarak Play ile aynıdır (PAUSED -> PLAYING)
        GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            std::cerr << "[GstManager] Devam ettirme başarısız: State değişimi hatası.\n";
        }
        else
        {
            std::cout << "[GstManager] Devam ettirildi.\n";
        }
    }
    else
    {
        std::cerr << "[GstManager] Devam ettirme başarısız: Pipeline yok.\n";
    }
}

// Yeniden Başlatma Fonksiyonu
void GstManager::restart()
{
    std::lock_guard<std::mutex> lock(pipeMutex);
    if (pipeline)
    {
        // Önce NULL yaparak başa sarıyoruz
        GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_NULL);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            std::cerr << "[GstManager] Yeniden başlatma başarısız: State değişimi hatası.\n";
            return;
        }

        // Durumu bekle
        gst_element_get_state(pipeline, nullptr, nullptr, GST_CLOCK_TIME_NONE);

        // Sonra tekrar PLAYING yaparak başlatıyoruz
        ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            std::cerr << "[GstManager] Yeniden başlatma başarısız: State değişimi hatası.\n";
            return;
        }
        else
        {
            std::cout << "[GstManager] Yeniden başlatıldı.\n";
        }
    }
    else
    {
        std::cerr << "[GstManager] Yeniden başlatma başarısız: Pipeline yok.\n";
    }
}

// Kapatma Fonksiyonu
void GstManager::close()
{
    // Çalışma döngüsünü durdur
    isRunning = false;
    /*“İşi bitirdin mi?” → joinable()
    “Evet, bitirmedim ama join edilebilirim.”
    “Tamam, bitirene kadar seni bekliyorum.” → join()
    */
    if (busThread.joinable())
    {
        busThread.join();
    }

    std::lock_guard<std::mutex> lock(pipeMutex);
    if (pipeline)
    {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        if (bus)
        {
            gst_object_unref(bus);
            bus = nullptr;
            std::cout << "[GstManager] Bus temizlendi.\n";
        }
        gst_object_unref(pipeline);
        pipeline = nullptr;
        std::cout << "[GstManager] Pipeline temizlendi.\n";
    }
    else
    {
        std::cout << "[GstManager] Temizlenecek pipeline yok.\n";
    }
}

// Bus İzleme Döngüsü
void GstManager::busLoop()
{
    // Mesaj döngüsü
    while (isRunning)
    {
        // Bus kontrolü
        if (!bus)
        {
            // Bus yoksa bekle ve devam et
            /*
            Eğer bus henüz hazır değilse (pipeline daha yeni set ediliyorsa)
            100ms bekler
            sonra döngünün başına döner
            */
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue; // Döngünün başına dön
        }

        // Belirli mesaj türlerini dinle (0.1 saniye timeout ile)
        GstMessage *msg = gst_bus_timed_pop_filtered(
            bus, 100 * GST_MSECOND,
            static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_STATE_CHANGED));

        if (msg != nullptr)
        {
            switch (GST_MESSAGE_TYPE(msg))
            {
            case GST_MESSAGE_ERROR:
            {
                GError *err; // Hata bilgisi
                gchar *debug_info; // Debug bilgisi
                // Hata mesajını ayrıştır
                gst_message_parse_error(msg, &err, &debug_info);

                // Ekrana basıyoruz
                std::cerr << "\n[GstManager] RUNTIME ERROR >> " << err->message << std::endl;

                // Serbest bırak
                g_free(debug_info);
                g_error_free(err);

                // Hata alındıysa (örn: pencere kapandıysa), pipeline artık çalışamaz.
                // 1. GStreamer'ı durdur (NULL state)
                gst_element_set_state(pipeline, GST_STATE_NULL);

                // 2. Bu thread döngüsünü sonlandır
                isRunning = false;

                std::cout << "[GstManager] Kritik hata nedeniyle pipeline durduruldu.\n";
                break;
            }
            case GST_MESSAGE_EOS:
                std::cout << "\n[GstManager] >>> Sona erdi (EOS)." << std::endl;
                break;

            case GST_MESSAGE_STATE_CHANGED:
                // Sadece bizim pipeline'ın durum değişikliklerini yazdır (Gürültüyü önler)
                if (GST_MESSAGE_SRC(msg) == GST_OBJECT(pipeline))
                {
                    GstState old_state, new_state, pending_state;
                    gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);

                    std::cout << "[GstManager] Durum değişti: "
                              << gst_element_state_get_name(old_state)
                              << " -> "
                              << gst_element_state_get_name(new_state)
                              << std::endl;
                }

            default:
                break;
            }
            gst_message_unref(msg);
        }
    }
}

// Pipeline Erişim Fonksiyonu
GstElement *GstManager::getPipeline()
{
    return this->pipeline; // Pipeline'ı döner
}