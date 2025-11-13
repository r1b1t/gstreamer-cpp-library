#pragma once
#include <string>
#include <vector>
#include <map>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

class GstRTSPManager
{
public:
    explicit GstRTSPManager(const std::string &service = "8554");
    ~GstRTSPManager();

    // Yeni yayın ekleme
    void addStream(const std::string &path, const std::string &pipelineDesc, bool shared = true);

    // Yayın kontrol fonksiyonları
    void pauseStream(const std::string &path);
    void resumeStream(const std::string &path);
    void switchSource(const std::string &path, int sourceIndex);

    // Sunucuyu başlat
    void run();

private:
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    std::vector<std::string> paths;
    std::map<std::string, GstElement *> activePipelines;

    GMainLoop *mainLoop = nullptr;

    friend void on_media_configure(GstRTSPMediaFactory *factory,
                                   GstRTSPMedia *media,
                                   gpointer user_data);
};
