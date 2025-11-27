#include "GStreamer/Manager.hpp"
#include "GStreamer/PipelineBuilder.hpp"
#include "Poppler/PDFRendererToPNG.hpp"
#include <iostream>

// PDF dosyasını PNG resimlerine dönüştürür ve ardından bu PNG dosyalarından bir video oluşturur.
// Fps değeri düşük ve video süresi her sayfa için 5 saniye olacak şekilde ayarlanmıştır.
// Giriş PDF dosyası: "pngtovideo_output/pdf_input/example4pages.pdf"
// Çıkış PNG dosyaları: "pngtovideo_output/output_frames/frame_0.png", "frame_1.png", ...
// Çıkış video dosyası: "pngtovideo_output/lowfps20second.mp4"

int main(int argc, char *argv[])
{
    // PDF dosyasını PNG resimlerine dönüştürme
    PDFRendererToPNG pdfRenderer;
    if (!pdfRenderer.renderPDFtoPNG("pngtovideo_output/pdf_input/example4pages.pdf", "pngtovideo_output/output_frames/"))
        return 1;

    // GStreamer ile PNG dosyalarından video oluşturma
    GstManager manager;
    manager.addPipeline(
        GstPipelineBuilder::pngtovideo(
            "pngtovideo_output/output_frames/frame_%d.png",
            "pngtovideo_output/lowfps20second.mp4"));
    manager.play();
    std::cout << "Press Enter to stop..." << std::endl;
    std::cin.get();
}
