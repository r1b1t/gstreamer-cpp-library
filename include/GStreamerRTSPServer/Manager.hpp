#pragma once
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <functional>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

class GstRTSPManager
{
public:
    explicit GstRTSPManager(const std::string &service = "8554");
    ~GstRTSPManager();

    // Yayın ekleme
    void addStream(const std::string &path, const std::string &pipelineDesc, bool shared = true);

    // Kontrol fonksiyonları (Artık Thread-Safe)
    void pauseStream(const std::string &path);
    void resumeStream(const std::string &path);
    void switchSource(const std::string &path, int sourceIndex);
    void removePipeline(const std::string &path); // Temizlik için

    void setSharedSource(const std::string &sourcePipeline, int width = 640, int height = 480);

    // Durum yazdırma
    void printStatus();

    // Sunucuyu arka planda başlatır (Main'i bloklamaz)
    void start();

    // Sunucuyu durdurur
    void stop();

private:
    // İç fonksiyonlar
    void loopFunction(); // Thread içinde dönecek fonksiyon

    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GMainLoop *mainLoop = nullptr;

    // Thread ve Senkronizasyon
    std::thread serverThread;
    std::mutex dataMutex; // Haritayı korumak için kilit
    bool isRunning = false;

    std::vector<std::string> paths;
    // Pipeline haritası
    std::map<std::string, GstElement *> activePipelines;

    // Callback'in özel erişimi olması lazım
    friend void on_media_configure(GstRTSPMediaFactory *factory,
                                   GstRTSPMedia *media,
                                   gpointer user_data);
};