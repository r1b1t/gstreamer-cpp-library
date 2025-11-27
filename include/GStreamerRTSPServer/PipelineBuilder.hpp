#pragma once
#include <string>

class GstRTSPPipelineBuilder
{
public:
    static std::string shared_stream(int width, int height, int bitrate);
    // GERÇEK KAYNAK KAMERA (ksvideosrc - Windows kamera)
    static std::string windowscam(int width, int height, int fps, int deviceIndex, bool mirror, int bitrate, int payloadType);
    // SANAL KAYNAK (videotestsrc - test pattern)
    static std::string videotestsrc(int width, int height, int fps, int bitrate, int payload, int pattern);
    // UDP YAYININI KAYNAK ALAN PİPELİNE
    static std::string udpsrc(const std::string &ip, int port, int payload, int bitrate, int width, int height, int fps);
    // RTSP YAYININI KAYNAK ALAN PİPELİNE
    static std::string rtspsrc(const std::string &sourceUrl, int latency, int bitrate, int width, int height, int fps, int payload);
    // UDP YAYININI DECODE -> FİLTRELE -> ENCODE YAPAN PİPELİNE
    static std::string udpsrcfilter(int port, int width = 640, int height = 480, int bitrate = 2000);
    // ÇOKLU KAYNAKLI VE KAYNAK SEÇİCİLİ PİPELİNE (input-selector)
    static std::string inputselector();
    // Video dosyası oynatıcı → RTSP
    static std::string videoplayer(const std::string &videoFile, int bitrate, int payload);
    // Custom pipeline → RTSP uyumlu hale getir (otomatik encode + pay0)
    static std::string custom(const std::string &pipeline);
};
