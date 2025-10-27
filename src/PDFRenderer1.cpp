// PDFRenderer1.cpp
#include "PDFRenderer1.h"
#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-page-renderer.h>
#include <poppler-image.h>
#include <iostream>
#include <sstream>
#include <string>

// PDF dosyasını PNG resimlerine dönüştürme
// inputPath: Giriş PDF dosya yolu
// outputDir: Çıkış PNG dosyalarının kaydedileceği dizin
// dpi: Çözünürlük (varsayılan 150)
// Başarılı ise true, başarısız ise false döner
// Örneği: examples/pngtovideo.cpp , examples/pngtovideo25fps.cpp

using namespace poppler;

bool PDFRenderer1::renderPDFtoPNG(const std::string &inputPath, const std::string &outputDir, int dpi)
{
    // PDF dosyasını yükle
    document *doc = document::load_from_file(inputPath);
    // PDF açılamadıysa hata ver
    if (!doc || doc->is_locked())
    {
        std::cerr << "PDF açılamadı!" << std::endl;
        return false;
    }
    // Toplam sayfa sayısını al
    int total_pages = doc->pages();
    // PDF boşsa hata ver
    if (total_pages == 0)
    {
        std::cerr << "PDF boş!" << std::endl;
        delete doc;
        return false;
    }
    // Sayfaları tek tek işle
    page_renderer renderer;
    // Kalite ayarları
    // Kenar yumuşatma ve metin kenar yumuşatma açık
    renderer.set_render_hint(page_renderer::antialiasing, true);
    renderer.set_render_hint(page_renderer::text_antialiasing, true);

    // Her sayfayı PNG olarak kaydet
    for (int i = 0; i < total_pages; ++i)
    {
        // Sayfayı oluştur
        page *p = doc->create_page(i);
        // Sayfa oluşturulamadıysa atla
        if (!p)
            continue;

        // Sayfayı belirtilen dpi ile render et
        poppler::image img = renderer.render_page(p, dpi, dpi);
        // PNG dosya adını oluştur
        std::ostringstream fname;
        // Örnek: outputDir/frame_0.png , outputDir/frame_1.png , ...
        fname << outputDir << "frame_" << i << ".png";
        // PNG dosyasını kaydet
        img.save(fname.str(), "png");
        // Sayfa nesnesini sil
        delete p;
    }

    // PDF nesnesini sil
    delete doc;
    // Başarı mesajı
    std::cout << total_pages << " Tane PDF sayfası " << outputDir << " yoluna PNG olarak kaydedildi.\n";
    // Başarılı dönüş
    return true;
}
