#include "GStreamer/Manager.hpp"
#include "GStreamer/PipelineBuilder.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    // GStreamer ile Windows kamera görüntüsünü yakalama
    GstManager manager;
    manager.addPipeline(GstPipelineBuilder::windowsscreen());
}