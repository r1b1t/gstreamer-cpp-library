#include "PDFRendererToPNG.h"
#include "GStreamerManager.h"
#include "PipelineBuilder.h"
#include <iostream>

// PDF dosyasını PNG resimlerine dönüştürür ve ardından bu PNG dosyalarından bir video oluşturur.
// Fps değeri düşük ve video süresi her sayfa için 5 saniye olacak şekilde ayarlanmıştır.
// Giriş PDF dosyası: "pngtovideo_output/pdf_input/example4pages.pdf"
// Çıkış PNG dosyaları: "pngtovideo_output/output_frames/frame_0.png", "frame_1.png", ...
// Çıkış video dosyası: "pngtovideo_output/lowfps20second.mp4"

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
}
