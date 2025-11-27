#include "GStreamer/Manager.hpp"
#include "GStreamer/PipelineBuilder.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    // GStreamer ile Windows kamera görüntüsünü yakalama
    GstManager manager;
    // --- SENARYO 1: Kamerayı Yayına Ver ---
    // 1. Önce kaynağı oluştur (addSink = false diyoruz çünkü ekranda değil yayında kullanılacak)
    std::string kaynakKamera = GstPipelineBuilder::windowsscreen(640, 480, 30, false);

    // 2. Kaynağı UDP fonksiyonuna ver
    std::string finalPipeline1 = GstPipelineBuilder::udpUnicastSender(kaynakKamera, "127.0.0.1", 5000, 2048, 96);
    manager.addPipeline(finalPipeline1);
    manager.play();
    std::cin.get();
}