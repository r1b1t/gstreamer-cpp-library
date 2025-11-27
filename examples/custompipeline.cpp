#include "GStreamer/Manager.hpp"
#include "GStreamer/PipelineBuilder.hpp"
#include <iostream>

int main(int argc, char *argv[])
{

    GstManager manager;

    manager.addPipeline(GstPipelineBuilder::custompipeline(
        "webrtcbin name=sendrecv stun-server=stun://stun.l.google.com:19302 "
        "ksvideosrc ! videoconvert ! queue ! vp8enc deadline=1 ! rtpvp8pay ! "
        "queue ! application/x-rtp,media=video,encoding-name=VP8,payload=96 ! sendrecv. "));
    manager.play();

    std::cout << "Press Enter to stop..." << std::endl;
    std::cin.get();
}
