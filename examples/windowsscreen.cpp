#include "GStreamerManager.h"
#include "PipelineBuilder.h"
#include <iostream>

int main(int argc, char *argv[])
{
    // GStreamer ile Windows kamera görüntüsünü yakalama
    GStreamerManager manager;
    manager.runPipeline(PipelineBuilder::windowsscreen());
}