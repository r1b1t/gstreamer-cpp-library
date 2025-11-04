#include "PDFRenderer2.h"
#include "GStreamerManager.h"
#include "PipelineBuilder.h"
#include <iostream>

// PDF dosyasını PNG resimlerine dönüştürür ve ardından bu PNG dosyalarından bir video oluşturur.
// Fps değeri yüksek (25 fps) ve video süresi her sayfa için
// 5 saniye olacak şekilde ayarlanmıştır. Bunu başarmak için,
// her sayfa için 125 (5s * 25FPS) frame kopyalanır.
// Çıkış PNG dosyaları: "pngtovideo2_output/output_frames/frame_0.png", "frame_1.png", ...
// Çıkış video dosyası: "pngtovideo2_output/25fps20second.mp4"

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
