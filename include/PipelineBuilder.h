// PipelineBuilder.h
#pragma once
#include <string>

class PipelineBuilder
{
public:
    // PNG to video pipeline oluşturur
    static std::string pngtovideo(const std::string &inputPattern, const std::string &outputPattern);
    // PNG to video pipeline oluşturur (25 fps, Toplam Video Az Saniye)
    static std::string pngtovideo25fps(const std::string &inputPattern, const std::string &outputPattern);
    // PNG to video pipeline oluşturur (Hem 25 FPS Hem de Uzun Video süresi) - Frame Kopyalama Yöntemi
    static std::string pngtovideo2(const std::string &inputPattern, const std::string &outputPattern);
    // Windows Kamera ile video yakalama pipeline oluşturur
    static std::string windowscam(int width, int height);
    // Video oynatma pipeline oluşturur
    static std::string videoplayer(const std::string &videoPattern);
};
