#include "GStreamer/Manager.hpp"
#include "GStreamer/PipelineBuilder.hpp"
#include "Poppler/PDFRendererToPNG.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    // PDF dosyasını PNG resimlerine dönüştürme
    PDFRendererToPNG pdfRenderer;
    if (!pdfRenderer.renderPDFtoPNG("pngtovideo_output/pdf_input/example4pages.pdf", "pngtovideo_output/output_frames/"))
        return 1;

    // GStreamer ile PNG dosyalarından video oluşturma
    GstManager manager;
    manager.addPipeline(
        GstPipelineBuilder::pngtovideo25fps(
            "pngtovideo_output/output_frames/frame_%d.png",
            "pngtovideo_output/25fps12second.mp4"));
    manager.play();
    std::cout << "Press Enter to stop..." << std::endl;
    std::cin.get();
}
