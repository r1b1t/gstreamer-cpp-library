#include "GStreamer/Manager.hpp"
#include "GStreamer/PipelineBuilder.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    // GStreamer ile Windows kamera görüntüsünü yakalama
    GstManager manager;
    manager.addPipeline(GstPipelineBuilder::windowsscreen(
        1280, // Genişlik
        720,  // Yükseklik
        30,   // FPS
        true  // Sink ekle
        ));
    manager.play();
    std::cout << "Press Enter to stop..." << std::endl;
    std::cin.get();
}