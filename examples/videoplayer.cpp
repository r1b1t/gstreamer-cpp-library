#include "GStreamerManager.h"
#include "PipelineBuilder.h"
#include <iostream>

int main(int argc, char *argv[])
{
    // GStreamer ile video oynatma
    GStreamerManager manager;
    manager.runPipeline(PipelineBuilder::videoplayer("videoplayer_output/examplevideo.mp4"));
}
