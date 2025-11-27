#include "GstreamerRTSPServer/Manager.hpp"
#include "GstreamerRTSPServer/MediaHub.hpp"
#include <iostream>

// Helper struct: Temizlik callback'ine veri taşımak için
struct CleanupData
{
    GstRTSPManager *manager;
    std::string path;
};
// ---------------------------------------------------------
// Callback: İstemci bağlandığında çalışır
// ---------------------------------------------------------
void on_media_configure(GstRTSPMediaFactory *factory, GstRTSPMedia *media, gpointer user_data)
{
    GstRTSPManager *self = static_cast<GstRTSPManager *>(user_data);
    if (!self)
        return;

    GstElement *pipeline = gst_rtsp_media_get_element(media);

    auto *mountPathData = static_cast<std::string *>(g_object_get_data(G_OBJECT(factory), "mount-path"));
    std::string path = mountPathData ? *mountPathData : "Bilinmiyor";

    // 1. LİSTEYE EKLE
    {
        std::lock_guard<std::mutex> lock(self->dataMutex);
        self->activePipelines[path] = pipeline;
        std::cout << "\n[INFO] YENI BAGLANTI -> Path: " << path << std::endl;
    }

    // 2. MEDYA HUB ENTEGRASYONU
    GstElement *appsrc = gst_bin_get_by_name(GST_BIN(pipeline), "shared_src");
    if (appsrc)
    {
        MediaHub::instance().addClient(appsrc);
        g_object_set_data_full(G_OBJECT(media), "cleanup_data", gst_object_ref(appsrc),
                               [](gpointer data)
                               {
                                   GstElement *src = static_cast<GstElement *>(data);
                                   MediaHub::instance().removeClient(src);
                                   gst_object_unref(src);
                               });
        gst_object_unref(appsrc);
    }

    // 3. PİPELİNE TEMİZLİĞİ (DÜZELTİLEN KISIM)
    // Manager'a ve Path'e ihtiyacımız var. Struct oluşturuyoruz.
    CleanupData *info = new CleanupData{self, path};

    g_object_set_data_full(G_OBJECT(media), "manager_cleanup", info,
                           (GDestroyNotify)([](gpointer data)
                                            {
                                                // Callback tetiklendiğinde burası çalışır
                                                CleanupData *cData = static_cast<CleanupData *>(data);

                                                if (cData && cData->manager)
                                                {
                                                    std::cout << "[INFO] BAGLANTI KOPARTILDI -> " << cData->path << "\n";
                                                    // ARTIK SİLME FONKSİYONUNU ÇAĞIRIYORUZ:
                                                    cData->manager->removePipeline(cData->path);
                                                }

                                                delete cData; // Struct'ı temizle
                                            }));
}

// ---------------------------------------------------------
// Constructor & Destructor
// ---------------------------------------------------------
GstRTSPManager::GstRTSPManager(const std::string &service)
{
    gst_init(nullptr, nullptr);
    server = gst_rtsp_server_new();
    gst_rtsp_server_set_service(server, service.c_str());
    mounts = gst_rtsp_server_get_mount_points(server);
}

GstRTSPManager::~GstRTSPManager()
{
    stop(); // Önce thread'i durdur
    if (mounts)
        g_object_unref(mounts);
    if (server)
        g_object_unref(server);
}

// ---------------------------------------------------------
// Başlatma (Non-Blocking)
// ---------------------------------------------------------
void GstRTSPManager::start()
{
    if (isRunning)
        return;

    mainLoop = g_main_loop_new(NULL, FALSE);
    isRunning = true;

    // Sunucuyu attach et
    if (gst_rtsp_server_attach(server, nullptr) == 0)
    {
        std::cerr << "HATA: RTSP Sunucusu porta bağlanamadı!\n";
        return;
    }

    // Port bilgisini al
    gchar *port = gst_rtsp_server_get_service(server);

    // ---- BİLGİLENDİRME KISMI (ESKİ KODUNDAKİ GİBİ) ----
    std::cout << "\n==================================================\n";
    std::cout << " RTSP Sunucusu Aktif! (Arka Planda)\n";
    std::cout << "==================================================\n";

    for (const auto &path : paths)
    {
        std::cout << " [YAYIN] rtsp://127.0.0.1:" << (port ? port : "8554") << path << "\n";
    }
    std::cout << "==================================================\n\n";

    if (port)
        g_free(port); // Memory leak olmasın

    // Loop'u thread'e at
    serverThread = std::thread(&GstRTSPManager::loopFunction, this);
}

void GstRTSPManager::loopFunction()
{
    g_main_loop_run(mainLoop);
}

void GstRTSPManager::stop()
{
    if (!isRunning)
        return;

    if (mainLoop)
        g_main_loop_quit(mainLoop);

    if (serverThread.joinable())
    {
        serverThread.join();
    }

    if (mainLoop)
    {
        g_main_loop_unref(mainLoop);
        mainLoop = nullptr;
    }
    isRunning = false;
    std::cout << "Sunucu durduruldu.\n";
}

// ---------------------------------------------------------
// Stream Ekleme
// ---------------------------------------------------------
void GstRTSPManager::addStream(const std::string &path, const std::string &pipelineDesc, bool shared)
{
    GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();
    gst_rtsp_media_factory_set_launch(factory, pipelineDesc.c_str());
    gst_rtsp_media_factory_set_shared(factory, shared);

    // Mount path bilgisini factory içine gömüyoruz (callback'te geri almak için)
    auto mountPath = new std::string(path);
    g_object_set_data_full(G_OBJECT(factory), "mount-path", mountPath,
                           [](gpointer data)
                           { delete static_cast<std::string *>(data); });

    // Signal bağla
    g_signal_connect(factory, "media-configure", (GCallback)on_media_configure, this);

    gst_rtsp_mount_points_add_factory(mounts, path.c_str(), factory);
    paths.push_back(path);
}

// ---------------------------------------------------------
// Kaynak Değiştirme (Güvenli)
// ---------------------------------------------------------
void GstRTSPManager::switchSource(const std::string &path, int sourceIndex)
{
    // Mutex ile kilitle ki çakışma olmasın
    std::lock_guard<std::mutex> lock(dataMutex);

    auto it = activePipelines.find(path);
    if (it == activePipelines.end())
    {
        // Pipeline henüz oluşmamış olabilir (kimse izlemiyorsa pipeline yoktur)
        return;
    }

    GstElement *pipeline = it->second;
    // Pipeline'ın hala geçerli olup olmadığını kontrol etmek zordur,
    // shared=true ise genelde güvenlidir.

    GstElement *selector = gst_bin_get_by_name(GST_BIN(pipeline), "sel");
    if (selector)
    {
        std::string padName = "sink_" + std::to_string(sourceIndex);
        GstPad *pad = gst_element_get_static_pad(selector, padName.c_str());
        if (pad)
        {
            g_object_set(selector, "active-pad", pad, nullptr);
            std::cout << "[" << path << "] Source -> " << sourceIndex << std::endl;
            gst_object_unref(pad);
        }
        gst_object_unref(selector);
    }
}

// Pause ve Resume fonksiyonları da benzer şekilde Mutex korumalı olmalı...
void GstRTSPManager::pauseStream(const std::string &path)
{
    std::lock_guard<std::mutex> lock(dataMutex);
    auto it = activePipelines.find(path);
    if (it != activePipelines.end())
    {
        gst_element_set_state(it->second, GST_STATE_PAUSED);
    }
}

void GstRTSPManager::resumeStream(const std::string &path)
{
    std::lock_guard<std::mutex> lock(dataMutex);
    auto it = activePipelines.find(path);
    if (it != activePipelines.end())
    {
        gst_element_set_state(it->second, GST_STATE_PLAYING);
    }
}

void GstRTSPManager::printStatus()
{
    std::lock_guard<std::mutex> lock(dataMutex); // Thread güvenliği için

    std::cout << "\n--- ANLIK SUNUCU DURUMU ---\n";
    std::cout << "Tanımlı Yayın Sayısı: " << paths.size() << "\n";
    std::cout << "Aktif (İzlenen) Pipeline Sayısı: " << activePipelines.size() << "\n";

    if (activePipelines.empty())
    {
        std::cout << " -> Şu an hiçbir yayın izlenmiyor.\n";
    }
    else
    {
        for (auto const &[path, pipeline] : activePipelines)
        {
            std::cout << " -> AKTİF: " << path << " (Pipeline çalışıyor)\n";
        }
    }
    std::cout << "---------------------------\n";
}

void GstRTSPManager::setSharedSource(const std::string &sourcePipeline, int width, int height)
{
    // Kullanıcının işini kolaylaştırıyoruz.
    // O sadece kaynağı veriyor, biz başlatıyoruz.
    MediaHub::instance().start(sourcePipeline, width, height);

    std::cout << "[Server] Ortak kaynak (Shared Source) ayarlandi.\n";
}

void GstRTSPManager::removePipeline(const std::string &path)
{
    std::lock_guard<std::mutex> lock(dataMutex); // Thread güvenliği önemli

    auto it = activePipelines.find(path);
    if (it != activePipelines.end())
    {
        activePipelines.erase(it); // Listeden (Map) sil
        std::cout << "[INFO] Pipeline listeden temizlendi: " << path << std::endl;
    }
}