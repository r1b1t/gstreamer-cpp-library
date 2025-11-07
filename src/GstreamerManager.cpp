#include "GStreamerManager.h"
#include <iostream>

//
// Constructor: GStreamer’ı başlatır
//
GStreamerManager::GStreamerManager()
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
GStreamerManager::~GStreamerManager()
{
    cleanup();
    std::cout << "GStreamer temizlendi.\n";
}

//
// Pipeline oluşturma
//
bool GStreamerManager::createPipeline(const std::string &pipelineDesc)
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
bool GStreamerManager::play()
{
    if (!pipeline)
    {
        std::cerr << "Pipeline yok, önce oluşturulmalı!\n";
        return false;
    }

    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        std::cerr << "Pipeline başlatılamadı!\n";
        return false;
    }

    std::cout << "Pipeline oynatılıyor...\n";
    return true;
}

void GStreamerManager::wait()
{
    if (!pipeline)
        return;

    bus = gst_element_get_bus(pipeline);

    // Ana döngü
    gboolean terminate = FALSE;
    gint64 duration = GST_CLOCK_TIME_NONE;

    while (!terminate)
    {
        // Her 100 ms'de bir mesaj bekle (örnek: hata, durum değişikliği vs.)
        GstMessage *msg = gst_bus_timed_pop_filtered(
            bus, 100 * GST_MSECOND,
            static_cast<GstMessageType>(
                GST_MESSAGE_ERROR |
                GST_MESSAGE_EOS |
                GST_MESSAGE_STATE_CHANGED |
                GST_MESSAGE_DURATION));

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

            case GST_MESSAGE_DURATION:
                // Süre değiştiyse yeniden sorgula
                duration = GST_CLOCK_TIME_NONE;
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
        else
        {
            // 🔹 Pipeline halen PLAYING mi?
            GstState state;
            gst_element_get_state(pipeline, &state, nullptr, 0);

            if (state != GST_STATE_PLAYING)
                continue;

            gint64 current = -1;
            if (!gst_element_query_position(pipeline, GST_FORMAT_TIME, &current))
                continue;

            if (!GST_CLOCK_TIME_IS_VALID(duration))
                gst_element_query_duration(pipeline, GST_FORMAT_TIME, &duration);

            g_print("GStreamer Sayaç: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
                    GST_TIME_ARGS(current), GST_TIME_ARGS(duration));
            fflush(stdout);
        }
    }
}

//
// 🔹 Temizlik (unref, state null, vs.)
//
void GStreamerManager::cleanup()
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
// 🔹 Ana fonksiyon
//
bool GStreamerManager::runPipeline(const std::string &pipelineDesc)
{
    if (!createPipeline(pipelineDesc))
        return false;

    if (!play())
    {
        cleanup();
        return false;
    }

    wait();
    cleanup();

    std::cout << "GStreamer pipeline başarıyla tamamlandı.\n";
    return true;
}
