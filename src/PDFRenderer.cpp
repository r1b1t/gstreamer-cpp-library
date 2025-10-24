// PDFRenderer.cpp
#include "PDFRenderer.h"
#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-page-renderer.h>
#include <poppler-image.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace poppler;

bool PDFRenderer::renderPDFtoPNG(const std::string &inputPath, const std::string &outputDir, int dpi)
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
    return true;
}
