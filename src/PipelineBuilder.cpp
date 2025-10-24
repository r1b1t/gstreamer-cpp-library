// PipelineBuilder.cpp
#include "PipelineBuilder.h"
#include <sstream>

// PNG dosyalarından video oluşturmak için GStreamer pipeline tanımı
std::string PipelineBuilder::pngtovideo(const std::string &inputPattern, const std::string &outputFile)
{
    std::ostringstream desc;
    desc << "multifilesrc location=" << inputPattern << " index=0 caps=\"image/png\" "
         << "! pngdec ! videoconvert ! videoscale "
         << "! video/x-raw,width=1280,height=720,framerate=1/5,format=I420 "
         << "! x264enc bitrate=9000 speed-preset=superfast tune=zerolatency key-int-max=1 "
         << "! h264parse ! mp4mux faststart=true ! filesink location=" << outputFile;
    return desc.str();
}

// Windows kamera ile video yakalama için GStreamer pipeline tanımı
std::string PipelineBuilder::windowscam(int width, int height)
{
    std::ostringstream desc;
    desc << "ksvideosrc ! "
         << "video/x-raw,width=" << width
         << ",height=" << height
         << ",framerate=30/1"
         << " ! videoconvert ! autovideosink";
    return desc.str(); // sonda boşluk yok
}
