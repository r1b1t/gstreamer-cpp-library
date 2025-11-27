#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <gst/gst.h>

class GstManager
{
public:

    // Yapıcı ve Yıkıcı Fonksiyonlar    
    GstManager();
    ~GstManager();

    // Pipeline Oluşturma Fonksiyonu
    bool addPipeline(const std::string &pipelineDesc);

    // Kontrol Fonksiyonları
    void play();
    void pause();
    void resume();
    void restart();
    void close();

    // Erişim Fonksiyonu (pipeline'ı döner)
    GstElement *getPipeline();

private:
    // Bus İzleme Döngüsü
    void busLoop();

    // Değişkenler
    GstElement *pipeline = nullptr;
    GstBus *bus = nullptr;

    // Thread Yönetimi
    std::thread busThread;
    // Çalışma Durumu
    std::atomic<bool> isRunning{false};
    // Pipeline Mutex
    std::mutex pipeMutex;
};