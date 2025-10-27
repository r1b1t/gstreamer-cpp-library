// PipelineBuilder.cpp
#include "PipelineBuilder.h"
#include <sstream>

// PNG dosyalarından video oluşturmak için GStreamer pipeline tanımı
// Örneği: examples/pngtovideo.cpp
// PNG dosyalarından saniyede 5 kare hızında video oluşturur. Bizim örneğimizde video toplam 20 saniye olur.
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

// PNG dosyalarından video oluşturmak için GStreamer pipeline tanımı (25 fps)
// Örneği: examples/pngtovideo25fps.cpp
// PNG dosyalarından saniyede 25 kare hızında video oluşturur. Bizim örneğimizde video toplam 12 saniye olur.
// Bunun farkı, videorate elemanı ile kare hızının 25 fps'ye çıkarılmasıdır.
// Bu, daha akıcı bir video sağlar.
// Daha hızlı açılır ve oynatılır.
std::string PipelineBuilder::pngtovideo25fps(const std::string &inputPattern, const std::string &outputFile)
{
    std::ostringstream desc;
    desc << "multifilesrc location=" << inputPattern << " index=0 caps=\"image/png\" "
         << "! pngdec ! videoconvert ! videoscale "
         << "! video/x-raw,width=1280,height=720,framerate=1/5,format=I420 "
         << "! videorate drop-only=false "
         << "! video/x-raw,framerate=25/1 "
         << "! x264enc bitrate=9000 speed-preset=superfast tune=zerolatency byte-stream=true key-int-max=25 "
         << "! h264parse ! mp4mux faststart=true ! filesink location=" << outputFile;
    return desc.str();
}

// PNG dosyalarından video oluşturmak için GStreamer pipeline tanımı
// Örneği: examples/pngtovideo2.cpp
// PNG dosyalarından saniyede 25 kare hızında video oluşturur. Bizim örneğimizde video toplam 20 saniye olur.
// Bu yöntem, frame kopyalama yöntemi kullanır.
std::string PipelineBuilder::pngtovideo2(const std::string &inputPattern, const std::string &outputFile)
{
    std::ostringstream desc;
    desc << "multifilesrc location=" << inputPattern << " index=0 stop-index=500 caps=\"image/png\" "
         << "! pngdec ! videoconvert ! videoscale "
         << "! video/x-raw,width=1280,height=720,framerate=25/1,format=I420 "
         << "! x264enc bitrate=9000 speed-preset=superfast tune=zerolatency byte-stream=true key-int-max=25 "
         << "! h264parse ! mp4mux faststart=true ! filesink location=" << outputFile;
    return desc.str();
}

// Windows kamera ile video yakalama için GStreamer pipeline tanımı
// Örneği: examples/windowscam.cpp
// Windows işletim sisteminde kamera ile video yakalamak için kullanılır.
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

// Video oynatma için GStreamer pipeline tanımı
// Örneği: examples/videoplayer.cpp
// Belirtilen video dosyasını oynatmak için kullanılır.
std::string PipelineBuilder::videoplayer(const std::string &videoPattern)
{
    std::ostringstream desc;
    desc << "filesrc location=" << videoPattern << " ! "
         << "qtdemux ! h264parse ! avdec_h264 ! videoconvert ! autovideosink";
    return desc.str();
}
