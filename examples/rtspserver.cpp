#include "GStreamerRTSPServer/Manager.hpp"
#include "GStreamerRTSPServer/PipelineBuilder.hpp"
#include "GStreamerRTSPServer/MediaHub.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    GstRTSPManager server("8554");

    // SEÇENEK A: GERÇEK KAMERA (Windows)
    std::string source = "ksvideosrc device-index=0 ! videoflip method=horizontal-flip";

    server.setSharedSource(source, 640, 480);

    // 2. RTSP Yayınlarını Ekle
    // Hepsi "shared_stream" kullanıyor, yani Hub'a bağlanacaklar.
    server.addStream("/yayin1", GstRTSPPipelineBuilder::shared_stream(640, 480, 2000));
    server.addStream("/yayin2", GstRTSPPipelineBuilder::shared_stream(640, 480, 500));            // Düşük kalite
    server.addStream("/yayin3", GstRTSPPipelineBuilder::videotestsrc(320, 240, 15, 1000, 18, 0)); // Test deseni

    server.start();

    std::cin.get();
    server.printStatus();
    std::cin.get();

    return 0;
}
