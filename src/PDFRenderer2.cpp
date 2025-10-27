// PDFRenderer1.cpp
#include "PDFRenderer2.h"
#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-page-renderer.h>
#include <poppler-image.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>

// PDF dosyasını PNG resimlerine dönüştürme
// inputPath: Giriş PDF dosya yolu
// outputDir: Çıkış PNG dosyalarının kaydedileceği dizin
// outputDirCopy: PNG dosyalarının kopyalanacağı dizin (frame tekrarları için)
// dpi: Çözünürlük (varsayılan 150)
// Başarılı ise true, başarısız ise false döner
// Örneği: examples/pngtovideo2.cpp , examples/pngtovideo25fps.cpp

using namespace poppler;

// Sabitler
const int PAGE_DURATION_SECONDS = 5; // Her sayfa 5 saniye
const int TARGET_FPS = 25;           // Çıktı videosu 25 FPS

// Toplam tekrar sayısı
const int REPEAT_COUNT = PAGE_DURATION_SECONDS * TARGET_FPS; // 5 * 25 = 125

bool PDFRenderer2::renderPDFtoPNG(const std::string &inputPath, const std::string &outputDir, const std::string &outputDirCopy, int dpi)
{
    document *doc = document::load_from_file(inputPath);
    if (!doc || doc->is_locked())
    {
        std::cerr << "PDF açılamadı!" << std::endl;
        return false;
    }

    int total_pages = doc->pages();
    if (total_pages == 0)
    {
        std::cerr << "PDF boş!" << std::endl;
        delete doc;
        return false;
    }

    page_renderer renderer;
    renderer.set_render_hint(page_renderer::antialiasing, true);
    renderer.set_render_hint(page_renderer::text_antialiasing, true);

    for (int i = 0; i < total_pages; ++i)
    {
        page *p = doc->create_page(i);
        if (!p)
            continue;

        poppler::image img = renderer.render_page(p, dpi, dpi);
        std::ostringstream fname;
        fname << outputDir << "frame_" << i << ".png";
        img.save(fname.str(), "png");
        delete p;
    }

    delete doc;
    std::cout << total_pages << " Tane PDF sayfası " << outputDir << " yoluna PNG olarak kaydedildi.\n";

    // --- PNG Kopyalama (Frame Tekrarı) ---
    // Bu kısım, her sayfayı 125 kez (5s * 25FPS) kopyalar.
    // multifilesrc'nin düzgün okuması için kopyaların sıralı indexlenmesi gerekir.
    int frame_index = 0;
    for (int page_i = 0; page_i < total_pages; ++page_i)
    {
        // Kaynak dosya adı
        std::ostringstream src_fname_oss;
        // Kaynak dosya adı oluşturma
        src_fname_oss << outputDir << "frame_" << page_i << ".png";
        // Kaynak dosya adı string
        std::string src_fname = src_fname_oss.str();

        for (int repeat = 0; repeat < REPEAT_COUNT; ++repeat)
        {
            // Hedef dosya adı oluşturma
            std::ostringstream dest_fname_oss;
            // Hedef dosya adı oluşturma
            dest_fname_oss << outputDirCopy << "frame_" << std::setw(4) << std::setfill('0') << frame_index << ".png";
            // Hedef dosya adı string
            std::string dest_fname = dest_fname_oss.str();

            // Dosya kopyalama
            std::ifstream source(src_fname, std::ios::binary);
            // Hedef dosyayı oluştur
            std::ofstream dest(dest_fname, std::ios::binary);

            if (!source.is_open() || !dest.is_open())
            {
                std::cerr << "Dosya kopyalama hatası: " << src_fname << " -> " << dest_fname << std::endl;
                //
                return 1;
            }
            // Kopyalama işlemi
            dest << source.rdbuf();
            frame_index++;
        }
    }

    // Toplam kare sayısı
    int total_frames = total_pages * REPEAT_COUNT;
    std::cout << "Toplam " << total_frames << " frame dosyası kopyalandı.\n";
    return true;
}
