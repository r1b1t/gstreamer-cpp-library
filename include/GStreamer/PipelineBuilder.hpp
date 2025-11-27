// PipelineBuilder.h
#pragma once
#include <string>

class GstPipelineBuilder
{
public:
    // PNG to video pipeline oluşturur
    static std::string pngtovideo(const std::string &inputPattern, const std::string &outputPattern);
    // PNG to video pipeline oluşturur (25 fps, Toplam Video Az Saniye)
    static std::string pngtovideo25fps(const std::string &inputPattern, const std::string &outputPattern);
    // PNG to video pipeline oluşturur (Hem 25 FPS Hem de Uzun Video süresi) - Frame Kopyalama Yöntemi
    static std::string pngtovideo2(const std::string &inputPattern, const std::string &outputPattern);
    // Windows Kamera ile video yakalama pipeline oluşturur
    static std::string windowscam(int width, int height, int fps, int deviceIndex = 0, bool mirror = true, bool addSink = true);
    // Test video kaynağı pipeline oluşturur
    static std::string videotestsrc(int width, int height, int fps, std::string pattern = "ball", bool addSink = true);
    // Windows Ekran Yakalama
    static std::string windowsscreen(int width, int height, int fps, bool addSink = true);
    // Video Dosyası Oynatma
    static std::string videoplayer(const std::string &filePath, bool addSink = true);
    // UDP Tekli Yayın Gönderici
    static std::string udpUnicastSender(const std::string &sourcePipeline, const std::string &ip, int port, int bitrate = 2048, int payloadType = 96);
    // UDP Çoklu Yayın Gönderici
    static std::string udpMulticastSender(const std::string &sourcePipeline, const std::string &ip, int port, int bitrate = 4096, int payloadType = 96);
    // UDP Alıcı pipeline oluşturur
    static std::string udpReceiver(int port, std::string multicastIP = "", int payloadType = 96);
    // Çoklu Unicast ve Multicast Yayını için TEE pipeline tanımı
    static std::string multiunicastlivetee();
    // Kullanıcının kendi pipeline'ını doğrudan kullanabilmesi için
    static std::string custompipeline(const std::string &pipeline);
};
