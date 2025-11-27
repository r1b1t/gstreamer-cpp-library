#pragma once
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <vector>
#include <mutex>
#include <algorithm>
#include <string>
#include <iostream>

class MediaHub
{
public:
    // Singleton
    static MediaHub &instance();

    // GENEL KAYNAK BAŞLATMA
    // sourceStr: "videotestsrc" veya "ksvideosrc device-index=0" veya "filesrc location=..."
    // width/height: Hub'ın çıktıyı sabitleyeceği çözünürlük (önemli!)
    void start(const std::string &sourceStr, int width, int height);

    // Hub'ı durdurur
    void stop();

    // RTSP istemcilerini (appsrc) ekler/çıkarır
    void addClient(GstElement *appsrc);
    void removeClient(GstElement *appsrc);

private:
    MediaHub() = default; // Private constructor
    ~MediaHub();

    GstElement *pipeline = nullptr;
    std::vector<GstElement *> clients;
    std::mutex clientsMutex;
    bool isRunning = false;

    // GStreamer callback
    static GstFlowReturn on_new_sample(GstElement *sink, gpointer user_data);
};