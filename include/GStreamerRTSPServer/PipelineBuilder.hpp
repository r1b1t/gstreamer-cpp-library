#pragma once
#include <string>

class GstRTSPPipelineBuilder
{
public:
    // GERÇEK KAYNAK KAMERA (ksvideosrc - Windows kamera)
    static std::string windowscam(int width = 640, int height = 480, int bitrate = 2000);
    // SANAL KAYNAK (videotestsrc - test pattern)
    static std::string videotestsrc(int width = 640, int height = 480, int bitrate = 2000);
    // UDP YAYININI KAYNAK ALAN PİPELİNE
    static std::string udpsrc(int port, int bitrate = 2000);
    // RTSP YAYININI KAYNAK ALAN PİPELİNE
    static std::string rtspsrc(const std::string &sourceUrl);
    // UDP YAYININI DECODE -> FİLTRELE -> ENCODE YAPAN PİPELİNE
    static std::string udpsrcfilter(int port, int width = 640, int height = 480, int bitrate = 2000);
    // ÇOKLU KAYNAKLI VE KAYNAK SEÇİCİLİ PİPELİNE (input-selector)
    static std::string inputselector();
    // Video dosyası oynatıcı → RTSP
    static std::string videoplayer(const std::string &videoFile, int bitrate = 2000);
    // Custom pipeline → RTSP uyumlu hale getir (otomatik encode + pay0)
    static std::string custom(const std::string &pipeline);
};
