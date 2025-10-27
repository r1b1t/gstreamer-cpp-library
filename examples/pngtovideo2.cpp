#include "PDFRenderer2.h"
#include "GStreamerManager.h"
#include "PipelineBuilder.h"
#include <iostream>

int main(int argc, char *argv[])
{
    // PDF dosyasını PNG resimlerine dönüştürme
    PDFRenderer2 pdfRenderer;
    if (!pdfRenderer.renderPDFtoPNG("pngtovideo2_output/pdf_input/example4pages.pdf", "pngtovideo2_output/output_frames/", "pngtovideo2_output/output_frames_copy/"))
        return 1;

    // GStreamer ile PNG dosyalarından video oluşturma
    GStreamerManager manager;
    manager.runPipeline(
        PipelineBuilder::pngtovideo2(
            "pngtovideo2_output/output_frames_copy/frame_%04d.png",
            "pngtovideo2_output/25fps20second.mp4"));

    std::cout << "Video oluşturma tamamlandı.\n";
}
