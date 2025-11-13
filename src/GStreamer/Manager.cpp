#include "GStreamer/Manager.hpp"
#include <iostream>

//
// Constructor: GStreamer’ı başlatır
//
GstManager::GstManager()
    : pipeline(nullptr), bus(nullptr)
{
    gst_init(nullptr, nullptr);

    /* GStreamer hata ayıklama seviyesini bilgi seviyesine ayarla (isteğe bağlı)
    gst_debug_set_default_threshold(GST_LEVEL_INFO);*/

    std::cout << "GStreamer başlatıldı.\n";
}

//
// Destructor: Temizlik yapar
//
GstManager::~GstManager()
{
    cleanup();
    std::cout << "GStreamer temizlendi.\n";
}

//
// Pipeline oluşturma
//
bool GstManager::addPipeline(const std::string &pipelineDesc)
{
    GError *error = nullptr;

    pipeline = gst_parse_launch(pipelineDesc.c_str(), &error);

    if (error)
    {
        std::cerr << "Pipeline hatası: " << error->message << std::endl;
        g_error_free(error);
        pipeline = nullptr;
        return false;
    }

    std::cout << "Pipeline oluşturuldu.\n";
    return true;
}

//
// Pipeline’ı oynat
//
bool GstManager::run()
{

    if (!pipeline)
    {
        std::cerr << "Pipeline yok, önce createPipeline çağrılmalı!\n";
        return false;
    }

    // Pipeline'ı başlat
    GstStateChangeReturn ret =
        gst_element_set_state(pipeline, GST_STATE_PLAYING);

    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        std::cerr << "Pipeline başlatılamadı!\n";
        cleanup();
        return false;
    }

    std::cout << "Pipeline çalışıyor...\n";

    // Ana döngü (wait)
    wait();

    // Pipeline artık bitti → otomatik temizleme
    cleanup();

    std::cout << "GStreamer pipeline başarıyla tamamlandı.\n";
    return true;
}

void GstManager::wait()
{
    if (!pipeline)
        return;

    bus = gst_element_get_bus(pipeline);

    // Ana döngü
    gboolean terminate = FALSE;

    while (!terminate)
    {
        // Her 100 ms'de bir mesaj bekle (örnek: hata, durum değişikliği vs.)
        GstMessage *msg = gst_bus_timed_pop_filtered(
            bus, 100 * GST_MSECOND,
            static_cast<GstMessageType>(
                GST_MESSAGE_ERROR |
                GST_MESSAGE_EOS |
                GST_MESSAGE_STATE_CHANGED));

        if (msg != nullptr)
        {
            switch (GST_MESSAGE_TYPE(msg))
            {
            case GST_MESSAGE_ERROR:
            {
                GError *err;
                gchar *debug_info;
                gst_message_parse_error(msg, &err, &debug_info);
                std::cerr << "Hata: " << err->message << std::endl;
                g_free(debug_info);
                g_error_free(err);
                terminate = TRUE;
                break;
            }
            case GST_MESSAGE_EOS:
                std::cout << "\nVideo tamamlandı (End Of Stream).\n";
                terminate = TRUE;
                break;

            case GST_MESSAGE_STATE_CHANGED:
                if (GST_MESSAGE_SRC(msg) == GST_OBJECT(pipeline))
                {
                    GstState old_state, new_state, pending_state;
                    gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
                    std::cout << "Pipeline Durumu: "
                              << gst_element_state_get_name(old_state)
                              << " → " << gst_element_state_get_name(new_state)
                              << std::endl;
                }
                break;

            default:
                break;
            }

            gst_message_unref(msg);
        }
    }
}

//
// Temizlik (unref, state null, vs.)
//
void GstManager::cleanup()
{
    // Pipeline varsa önce durdur
    if (pipeline)
    {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_element_get_state(pipeline, nullptr, nullptr, GST_CLOCK_TIME_NONE);
        gst_clear_object(&pipeline); // güvenli unref + nullptr ataması
    }

    // Bus varsa güvenli unref
    if (bus)
    {
        gst_clear_object(&bus); // güvenli unref + nullptr
    }
}

//
// Pipeline'ı duraklat
//
void GstManager::pause()
{
    if (pipeline)
    {
        gst_element_set_state(pipeline, GST_STATE_PAUSED);
        std::cout << "Pipeline duraklatıldı.\n";
    }
}

//
// Pipeline'ı devam ettir
//
void GstManager::resume()
{
    if (pipeline)
    {
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        std::cout << "Pipeline devam ediyor.\n";
    }
}

//
// Pipeline'ı yeniden başlat (NULL → PLAYING)
//
void GstManager::restart()
{
    if (pipeline)
    {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_element_get_state(pipeline, nullptr, nullptr, GST_CLOCK_TIME_NONE);
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        std::cout << "Pipeline yeniden başlatıldı.\n";
    }
}
