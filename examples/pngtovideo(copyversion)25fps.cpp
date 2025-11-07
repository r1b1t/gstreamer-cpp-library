#include "PDFRendererToPNG.h"
#include "GStreamerManager.h"
#include "PipelineBuilder.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

// Sabitler
const int PAGE_DURATION_SECONDS = 5;                         // Her sayfa 5 saniye
const int TARGET_FPS = 25;                                   // 25 FPS
const int REPEAT_COUNT = PAGE_DURATION_SECONDS * TARGET_FPS; // 5 * 25 = 125 tekrar

// Yardımcı fonksiyon: PNG dosyalarını kopyalar (frame tekrarları)
bool duplicateFrames(const std::string &srcDir, const std::string &dstDir, int total_pages)
{
    int frame_index = 0;
    for (int page_i = 0; page_i < total_pages; ++page_i)
    {
        std::ostringstream src_fname_oss;
        src_fname_oss << srcDir << "frame_" << page_i << ".png";
        std::string src_fname = src_fname_oss.str();

        for (int repeat = 0; repeat < REPEAT_COUNT; ++repeat)
        {
            std::ostringstream dest_fname_oss;
            dest_fname_oss << dstDir << "frame_" << std::setw(4) << std::setfill('0') << frame_index << ".png";
            std::string dest_fname = dest_fname_oss.str();

            std::ifstream source(src_fname, std::ios::binary);
            std::ofstream dest(dest_fname, std::ios::binary);

            if (!source.is_open() || !dest.is_open())
            {
                std::cerr << "Dosya kopyalama hatası: " << src_fname << " -> " << dest_fname << std::endl;
                return false;
            }

            dest << source.rdbuf();
            frame_index++;
        }
    }

    int total_frames = total_pages * REPEAT_COUNT;
    std::cout << "Toplam " << total_frames << " frame dosyası oluşturuldu (kopyalandı).\n";
    return true;
}

int main(int argc, char *argv[])
{
    std::string inputPDF = "pngtovideo2_output/pdf_input/example4pages.pdf";
    std::string outputFrames = "pngtovideo2_output/output_frames/";
    std::string outputFramesCopy = "pngtovideo2_output/output_frames_copy/";
    std::string outputVideo = "pngtovideo2_output/25fps20second.mp4";

    PDFRenderer1 pdfRenderer;

    // PDF'yi PNG'lere dönüştür
    if (!pdfRenderer.renderPDFtoPNG(inputPDF, outputFrames))
        return 1;

    // Toplam sayfa sayısını tekrar hesaplamak için küçük bir tahmin:
    // (frame dosyalarını sayabiliriz ama burada manuel olarak biliyoruz, istersen parametre döndürtebilirsin)
    int total_pages = 4; // Örnek PDF 4 sayfa — istersen PDFRenderer1 içinde return ettirebiliriz

    // PNG’leri 25FPS ve 5s/sayfa olacak şekilde çoğalt
    if (!duplicateFrames(outputFrames, outputFramesCopy, total_pages))
        return 1;

    // GStreamer pipeline’ı çalıştır
    GStreamerManager manager;
    manager.runPipeline(
        PipelineBuilder::pngtovideo2(
            outputFramesCopy + "frame_%04d.png",
            outputVideo));

    return 0;
}
