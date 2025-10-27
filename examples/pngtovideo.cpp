#include "PDFRenderer1.h"
#include "GStreamerManager.h"
#include "PipelineBuilder.h"
#include <iostream>

int main(int argc, char *argv[])
{
    // PDF dosyasını PNG resimlerine dönüştürme
    PDFRenderer1 pdfRenderer;
    if (!pdfRenderer.renderPDFtoPNG("pngtovideo_output/pdf_input/example4pages.pdf", "pngtovideo_output/output_frames/"))
        return 1;

    // GStreamer ile PNG dosyalarından video oluşturma
    GStreamerManager manager;
    manager.runPipeline(
        PipelineBuilder::pngtovideo(
            "pngtovideo_output/output_frames/frame_%d.png",
            "pngtovideo_output/lowfps20second.mp4"));

    std::cout << "Video oluşturma tamamlandı.\n";
}
