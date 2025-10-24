// PipelineBuilder.h
#pragma once
#include <string>

class PipelineBuilder
{
public:
    // PNG to video pipeline oluşturur
    static std::string pngtovideo(const std::string &inputPattern, const std::string &outputPattern);
    // Windows Kamera ile video yakalama pipeline oluşturur
    static std::string windowscam(int width, int height);
};
