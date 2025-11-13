// PDFRendererToPNG.h
// gereksiz döngüye girmesin diye
#pragma once
#include <string>

class PDFRendererToPNG
{
public:
    bool renderPDFtoPNG(const std::string &inputPath, const std::string &outputDir, int dpi = 150);
};