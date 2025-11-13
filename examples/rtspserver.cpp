#include "GStreamerRTSPServer/Manager.hpp"
#include "GStreamerRTSPServer/PipelineBuilder.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    // ------------------------
    // RTSP Sunucusu Oluşturma (port 8554)
    // ------------------------
    GstRTSPManager server("8554");
    // GERÇEK KAYNAK KAMERA (ksvideosrc) PİPELİNE
    server.addStream("/test3", GstRTSPPipelineBuilder::windowscam(640, 480, 2000));
    server.addStream("/test1", GstRTSPPipelineBuilder::videotestsrc(640, 480, 2000));
    server.run();
}
