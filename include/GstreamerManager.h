#pragma once
#include <string>
#include <gst/gst.h>

class GStreamerManager
{
public:
    // Yapıcı ve yıkıcı metodlar
    GStreamerManager();
    ~GStreamerManager();

    // Ana çalışma fonksiyonu
    bool runPipeline(const std::string &pipelineDesc);

private:
    // Yardımcı fonksiyonlar
    bool createPipeline(const std::string &pipelineDesc);
    bool play();
    void wait();
    void cleanup();

    // GStreamer objeleri
    GstElement *pipeline;
    GstBus *bus;
};
