// PDFRendererToPNG.h
#pragma once
#include <string>

class PDFRenderer1
{
public:
    bool renderPDFtoPNG(const std::string &inputPath, const std::string &outputDir, int dpi = 150);
};
