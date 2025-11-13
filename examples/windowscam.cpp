#include "GStreamer/Manager.hpp"
#include "GStreamer/PipelineBuilder.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    // GStreamer ile Windows kamera görüntüsünü yakalama

    GstManager player;
    player.addPipeline(GstPipelineBuilder::windowscam(640, 480));
    player.run();
}
