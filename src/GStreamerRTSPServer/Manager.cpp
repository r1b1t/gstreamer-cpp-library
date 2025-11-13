#include "GStreamerRTSPServer/Manager.hpp"
#include <iostream>

// RTSP yayını oluşturulurken çağrılan callback fonksiyonu
void on_media_configure(GstRTSPMediaFactory *factory,
                        GstRTSPMedia *media,
                        gpointer user_data)
{
    if (!factory || !media || !user_data)
    {
        // cerr : "Console Error" – hata mesajları için kullanılır
        std::cerr << "on_media_configure: invalid arguments.\n";
        return;
    }

    // RTSPServer nesnesini güvenli şekilde geri al
    auto *self = static_cast<GstRTSPManager *>(user_data);
    if (!self)
    {
        std::cerr << "user_data null.\n";
        return;
    }

    // RTSPMedia içindeki pipeline'ı al
    GstElement *pipeline = gst_rtsp_media_get_element(media);
    if (!pipeline)
    {
        std::cerr << "media->pipeline alınamadı.\n";
        return;
    }

    // Factory içinden mount path bilgisini al (C++ tarzı)
    auto *mountPathData =
        static_cast<std::string *>(g_object_get_data(G_OBJECT(factory), "mount-path"));

    if (!mountPathData)
    {
        std::cerr << "mount-path bulunamadı.\n";
        return;
    }

    // Pipeline'ı RTSPServer içindeki haritaya kaydet
    const std::string &path = *mountPathData;
    self->activePipelines[path] = pipeline;

    std::cout << "Pipeline oluşturuldu ve kaydedildi: " << path << std::endl;

    // input-selector varsa ilk kaynağı aktif et
    if (auto selector = gst_bin_get_by_name(GST_BIN(pipeline), "sel"))
    {
        if (auto pad = gst_element_get_static_pad(selector, "sink_0"))
        {
            g_object_set(selector, "active-pad", pad, nullptr);
            std::cout << "[" << path << "] Başlangıçta 'sink_0' aktif edildi." << std::endl;
            gst_object_unref(pad);
        }
        else
        {
            std::cerr << "[" << path << "] 'sink_0' pad'i bulunamadı.\n";
        }
        gst_object_unref(selector);
    }
}

// Constructor
GstRTSPManager::GstRTSPManager(const std::string &service)
{
    gst_init(nullptr, nullptr);
    server = gst_rtsp_server_new();
    gst_rtsp_server_set_service(server, service.c_str());
    mounts = gst_rtsp_server_get_mount_points(server);
}

// Destructor
GstRTSPManager::~GstRTSPManager()
{

    if (mainLoop)
        g_main_loop_unref(mainLoop);
    if (mounts)
        g_object_unref(mounts);
    if (server)
        g_object_unref(server);
}

// ---------------------------------------------
// RTSP stream’i duraklat
// ---------------------------------------------
void GstRTSPManager::pauseStream(const std::string &path)
{
    auto it = activePipelines.find(path);
    if (it == activePipelines.end())
    {
        g_printerr("[%s] Pipeline bulunamadı.\n", path.c_str());
        return;
    }

    gst_element_set_state(it->second, GST_STATE_PAUSED);
    g_print("[%s] Yayın duraklatıldı.\n", path.c_str());
}

// ---------------------------------------------
// RTSP stream’i devam ettir
// ---------------------------------------------
void GstRTSPManager::resumeStream(const std::string &path)
{
    auto it = activePipelines.find(path);
    if (it == activePipelines.end())
    {
        g_printerr("[%s] Pipeline bulunamadı.\n", path.c_str());
        return;
    }

    gst_element_set_state(it->second, GST_STATE_PLAYING);
    g_print("[%s] Yayın devam ettirildi.\n", path.c_str());
}

// ---------------------------------------------
// Kaynak değiştir (input-selector)
// ---------------------------------------------
void GstRTSPManager::switchSource(const std::string &path, int sourceIndex)
{
    auto it = activePipelines.find(path);
    if (it == activePipelines.end())
    {
        g_printerr("[%s] Pipeline bulunamadı.\n", path.c_str());
        return;
    }

    GstElement *pipeline = it->second;
    GstElement *selector = gst_bin_get_by_name(GST_BIN(pipeline), "sel");
    if (!selector)
    {
        g_printerr("[%s] input-selector bulunamadı.\n", path.c_str());
        return;
    }

    std::string padName = "sink_" + std::to_string(sourceIndex);
    GstPad *pad = gst_element_get_static_pad(selector, padName.c_str());
    if (pad)
    {
        g_object_set(selector, "active-pad", pad, nullptr);
        std::cout << "[" << path << "] Kaynak değiştirildi: " << padName << " aktif.\n";
        gst_object_unref(pad);
    }
    else
    {
        std::cerr << "[" << path << "] Pad bulunamadı: " << padName << "\n";
    }
    gst_object_unref(selector);
}

// ---------------------------------------------
// Yeni RTSP mount point ekle
// ---------------------------------------------
void GstRTSPManager::addStream(const std::string &path, const std::string &pipelineDesc, bool shared)
{
    GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();
    gst_rtsp_media_factory_set_launch(factory, pipelineDesc.c_str());
    gst_rtsp_media_factory_set_shared(factory, shared);

    auto mountPath = new std::string(path);
    g_object_set_data_full(G_OBJECT(factory), "mount-path", mountPath,
                           [](gpointer data)
                           { delete static_cast<std::string *>(data); });

    g_signal_connect(factory, "media-configure", (GCallback)on_media_configure, this);
    gst_rtsp_mount_points_add_factory(mounts, path.c_str(), factory);
    paths.push_back(path);
}

// ---------------------------------------------
// RTSP sunucusunu çalıştır
// ---------------------------------------------
void GstRTSPManager::run()
{
    gst_rtsp_server_attach(server, nullptr);
    const gchar *port = gst_rtsp_server_get_service(server);

    std::cout << "RTSP sunucusu çalışıyor. Aktif serverlar:" << std::endl;

    for (const auto &path : paths)
        g_print("RTSP Yayını aktif: rtsp://127.0.0.1:%s%s\n", port, path.c_str());

    mainLoop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainLoop);
}
