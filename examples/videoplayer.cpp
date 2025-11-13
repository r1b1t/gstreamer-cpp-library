#include "GStreamer/Manager.hpp"
#include "GStreamer/PipelineBuilder.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    // GStreamer ile video oynatma
    GstManager manager;
    manager.addPipeline(GstPipelineBuilder::videoplayer("videoplayer_output/examplevideo.mp4"));
}
