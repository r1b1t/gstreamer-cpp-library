#include "GStreamerRTSPServer/PipelineBuilder.hpp"
#include <sstream>

// -------------------------------------------------------------
// GERÇEK KAYNAK KAMERA (ksvideosrc - Windows kamera)
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::windowscam(int width, int height, int bitrate)
{
    std::ostringstream desc;
    desc << "ksvideosrc ! "
         << "videoflip method=horizontal-flip ! "
         << "video/x-raw,format=YUY2,width=" << width << ",height=" << height << ",framerate=30/1 ! "
         << "videoconvert ! "
         << "x264enc bitrate=" << bitrate
         << " speed-preset=superfast tune=zerolatency key-int-max=25 ! "
         << "rtph264pay name=pay0 pt=96 config-interval=1";
    return desc.str();
}

// -------------------------------------------------------------
// SANAL KAYNAK (videotestsrc - test pattern)
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::videotestsrc(int width, int height, int bitrate)
{
    std::ostringstream desc;
    desc << "videotestsrc pattern=smpte ! "
         << "video/x-raw,width=" << width << ",height=" << height << ",framerate=30/1 ! "
         << "videoconvert ! "
         << "x264enc bitrate=" << bitrate
         << " speed-preset=ultrafast tune=zerolatency ! "
         << "rtph264pay name=pay0 pt=96 config-interval=1";
    return desc.str();
}

// -------------------------------------------------------------
// UDP YAYININI KAYNAK ALAN PİPELİNE
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::udpsrc(int port, int bitrate)
{
    std::ostringstream desc;
    desc << "udpsrc port=" << port
         << " caps=\"application/x-rtp,media=video,encoding-name=H264,payload=96\" ! "
         << "rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! "
         << "x264enc bitrate=" << bitrate
         << " speed-preset=ultrafast tune=zerolatency ! "
         << "rtph264pay name=pay0 pt=96 config-interval=1";
    return desc.str();
}

// -------------------------------------------------------------
// RTSP YAYININI KAYNAK ALAN PİPELİNE
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::rtspsrc(const std::string &sourceUrl)
{
    std::ostringstream desc;
    desc << "rtspsrc location=" << sourceUrl << " latency=100 ! "
         << "rtpjitterbuffer ! rtph264depay ! h264parse ! avdec_h264 ! "
         << "videoconvert ! "
         << "x264enc bitrate=2000 speed-preset=ultrafast tune=zerolatency ! "
         << "rtph264pay name=pay0 pt=96 config-interval=1";
    return desc.str();
}

// -------------------------------------------------------------
// UDP YAYININI DECODE -> FİLTRELE -> ENCODE YAPAN PİPELİNE
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::udpsrcfilter(int port, int width, int height, int bitrate)
{
    std::ostringstream desc;
    desc << "udpsrc port=" << port
         << " caps=\"application/x-rtp,media=video,encoding-name=H264,payload=96\" ! "
         << "rtph264depay ! h264parse ! avdec_h264 ! "
         << "videoconvert ! videoscale ! "
         << "video/x-raw,width=" << width << ",height=" << height << ",framerate=30/1 ! "
         << "x264enc bitrate=" << bitrate
         << " speed-preset=superfast tune=zerolatency key-int-max=25 ! "
         << "rtph264pay name=pay0 pt=96 config-interval=1";
    return desc.str();
}

// -------------------------------------------------------------
// ÇOKLU KAYNAKLI VE KAYNAK SEÇİCİLİ PİPELİNE (input-selector)
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::inputselector()
{
    std::ostringstream desc;
    desc << "input-selector name=sel ! "
         << "videoconvert ! x264enc bitrate=3000 speed-preset=superfast tune=zerolatency ! "
         << "rtph264pay name=pay0 pt=96 config-interval=1 "
         << "videotestsrc pattern=smpte ! sel.sink_0 "
         << "videotestsrc pattern=ball ! sel.sink_1 "
         << "videotestsrc pattern=snow ! sel.sink_2";
    return desc.str();
}

// -------------------------------------------------------------
// Video dosyası oynatıcı → RTSP
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::videoplayer(const std::string &videoFile, int bitrate)
{
    std::ostringstream desc;
    desc << "filesrc location=" << videoFile << " ! "
         << "qtdemux ! h264parse ! avdec_h264 ! videoconvert "
         << "! x264enc bitrate=" << bitrate << " speed-preset=ultrafast tune=zerolatency "
         << "! rtph264pay name=pay0 pt=96 config-interval=1";
    return desc.str();
}

// -------------------------------------------------------------
// Custom pipeline → RTSP uyumlu hale getir (otomatik encode + pay0)
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::custom(const std::string &pipeline)
{
    std::ostringstream desc;
    desc << pipeline;

    return desc.str();
}
