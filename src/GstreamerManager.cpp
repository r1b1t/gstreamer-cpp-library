#include "GStreamerManager.h"
#include <iostream>

//
// 🔹 Constructor: GStreamer’ı başlatır
//
GStreamerManager::GStreamerManager()
    : pipeline(nullptr), bus(nullptr)
{
    gst_init(nullptr, nullptr);

    /* GStreamer hata ayıklama seviyesini bilgi seviyesine ayarla
    gst_debug_set_default_threshold(GST_LEVEL_INFO);*/

    std::cout << "GStreamer başlatıldı.\n";
}

//
// 🔹 Destructor: Temizlik yapar
//
GStreamerManager::~GStreamerManager()
{
    cleanup();
    std::cout << "GStreamer temizlendi.\n";
}

//
// 🔹 Pipeline oluşturma
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
// 🔹 Pipeline’ı oynat
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

//
// 🔹 Hata veya EOS (End Of Stream) bekleme
//
void GStreamerManager::wait()
{
    if (!pipeline)
        return;

    bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(
        bus, GST_CLOCK_TIME_NONE,
        (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    if (msg)
    {
        if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR)
        {
            GError *err;
            gchar *debug_info;
            gst_message_parse_error(msg, &err, &debug_info);
            std::cerr << "Hata: " << err->message << std::endl;
            g_error_free(err);
            g_free(debug_info);
        }
        else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_EOS)
        {
            std::cout << "Pipeline tamamlandı.\n";
        }

        gst_message_unref(msg);
    }
}

//
// 🔹 Temizlik (unref, state null, vs.)
//
void GStreamerManager::cleanup()
{
    if (pipeline)
    {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_element_get_state(pipeline, nullptr, nullptr, GST_CLOCK_TIME_NONE);
        gst_object_unref(pipeline);
        pipeline = nullptr;
    }

    if (bus)
    {
        gst_object_unref(bus);
        bus = nullptr;
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
