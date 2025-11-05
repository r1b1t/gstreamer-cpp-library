// PDFRenderer2.h
#pragma once
#include <string>

class PDFRenderer2
{
public:
    bool renderPDFtoPNG(const std::string &inputPath, const std::string &outputDir, const std::string &outputDirCopy, int dpi = 150);
};
