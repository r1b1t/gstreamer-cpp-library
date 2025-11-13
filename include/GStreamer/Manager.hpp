#pragma once
#include <string>
#include <gst/gst.h>

class GstManager
{
public:
    // Yapıcı ve yıkıcı metodlar
    GstManager();
    ~GstManager();

    // Pipeline Ekleme
    bool addPipeline(const std::string &pipelineDesc);

    bool run();
    void pause();
    void resume();
    void restart();

private:
    // Yardımcı fonksiyonlar

    void wait();
    void cleanup();

    // GStreamer objeleri
    GstElement *pipeline;
    GstBus *bus;
};
