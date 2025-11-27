#include "GStreamerRTSPServer/MediaHub.hpp"
#include <sstream>

MediaHub &MediaHub::instance()
{
    // 'static' kelimesi burada SİHİRLİDİR.
    // Bu satır program ömrü boyunca sadece İLK çalıştırıldığında işler.
    // İkinci, üçüncü çağrışta bu satır atlanır ve var olan 'inst' kullanılır.
    static MediaHub inst;
    return inst;
}

MediaHub::~MediaHub()
{
    stop();
}

void MediaHub::start(const std::string &sourceStr, int width, int height)
{
    if (isRunning)
        return;

    std::ostringstream oss;
    oss << sourceStr << " ! "
        << "videoconvert ! videoscale ! "
        << "video/x-raw,format=I420,width=" << width << ",height=" << height << ",framerate=30/1 ! "
        << "appsink name=mysink emit-signals=true sync=false max-buffers=1 drop=true";

    GError *error = nullptr;
    pipeline = gst_parse_launch(oss.str().c_str(), &error);

    if (error)
    {
        std::cerr << "[MediaHub] HATA: Pipeline kurulamadı -> " << error->message << std::endl;
        g_error_free(error);
        return;
    }

    GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "mysink");
    if (sink)
    {
        g_signal_connect(sink, "new-sample", G_CALLBACK(on_new_sample), this);
        gst_object_unref(sink);
    }

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    isRunning = true;
    std::cout << "[MediaHub] Kaynak baslatildi. (Source: " << sourceStr << ")\n";
}

void MediaHub::stop()
{
    if (pipeline)
    {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = nullptr;
    }
    isRunning = false;

    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.clear();
}

void MediaHub::addClient(GstElement *appsrc)
{
    std::lock_guard<std::mutex> lock(clientsMutex);

    g_object_set(appsrc,
                 "format", 3,
                 "is-live", TRUE,
                 "do-timestamp", TRUE,
                 "min-latency", 0,
                 "max-latency", 500,
                 "emit-signals", FALSE,
                 NULL);

    clients.push_back(appsrc);
}

// --- İŞTE EKSİK OLAN KISIM MUHTEMELEN BURASI ---
void MediaHub::removeClient(GstElement *appsrc)
{
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(std::remove(clients.begin(), clients.end(), appsrc), clients.end());
}
// -----------------------------------------------

GstFlowReturn MediaHub::on_new_sample(GstElement *sink, gpointer user_data)
{
    MediaHub *self = static_cast<MediaHub *>(user_data);

    GstSample *sample;
    g_signal_emit_by_name(sink, "pull-sample", &sample);

    if (sample)
    {
        GstBuffer *buffer = gst_sample_get_buffer(sample);
        std::lock_guard<std::mutex> lock(self->clientsMutex);

        for (GstElement *src : self->clients)
        {
            GstBuffer *bufCopy = gst_buffer_ref(buffer);

            GST_BUFFER_PTS(bufCopy) = GST_CLOCK_TIME_NONE;
            GST_BUFFER_DTS(bufCopy) = GST_CLOCK_TIME_NONE;

            GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(src), bufCopy);
            if (ret != GST_FLOW_OK)
            {
                // Hata yönetimi
            }
        }
        gst_sample_unref(sample);
        return GST_FLOW_OK;
    }
    return GST_FLOW_ERROR;
}