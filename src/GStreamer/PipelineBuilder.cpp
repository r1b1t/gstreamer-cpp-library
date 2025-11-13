// PipelineBuilder.cpp
#include "GStreamer/PipelineBuilder.hpp"
#include <sstream>

// PNG dosyalarından video oluşturmak için GStreamer pipeline tanımı
// Örneği: examples/pngtovideo.cpp
// PNG dosyalarından saniyede 5 kare hızında video oluşturur. Bizim örneğimizde video toplam 20 saniye olur.
std::string GstPipelineBuilder::pngtovideo(const std::string &inputPattern, const std::string &outputFile)
{
     std::ostringstream desc;
     desc << "multifilesrc location=" << inputPattern << " index=0 caps=\"image/png\" "
          << "! pngdec ! videoconvert ! videoscale "
          << "! video/x-raw,width=1280,height=720,framerate=1/5,format=I420 "
          << "! x264enc bitrate=9000 speed-preset=superfast tune=zerolatency key-int-max=1 "
          << "! h264parse ! mp4mux faststart=true ! filesink location=" << outputFile;
     return desc.str();
}

// PNG dosyalarından video oluşturmak için GStreamer pipeline tanımı (25 fps)
// Örneği: examples/pngtovideo25fps.cpp
// PNG dosyalarından saniyede 25 kare hızında video oluşturur. Bizim örneğimizde video toplam 12 saniye olur.
// Bunun farkı, videorate elemanı ile kare hızının 25 fps'ye çıkarılmasıdır.
// Bu, daha akıcı bir video sağlar.
// Daha hızlı açılır ve oynatılır.
std::string GstPipelineBuilder::pngtovideo25fps(const std::string &inputPattern, const std::string &outputFile)
{
     std::ostringstream desc;
     desc << "multifilesrc location=" << inputPattern << " index=0 caps=\"image/png\" "
          << "! pngdec ! videoconvert ! videoscale "
          << "! video/x-raw,width=1280,height=720,framerate=1/5,format=I420 "
          << "! videorate drop-only=false "
          << "! video/x-raw,framerate=25/1 "
          << "! x264enc bitrate=9000 speed-preset=superfast tune=zerolatency byte-stream=true key-int-max=25 "
          << "! h264parse ! mp4mux faststart=true ! filesink location=" << outputFile;
     return desc.str();
}

// PNG dosyalarından video oluşturmak için GStreamer pipeline tanımı
// Örneği: examples/pngtovideo2.cpp
// PNG dosyalarından saniyede 25 kare hızında video oluşturur. Bizim örneğimizde video toplam 20 saniye olur.
// Bu yöntem, frame kopyalama yöntemi kullanır.
std::string GstPipelineBuilder::pngtovideo2(const std::string &inputPattern, const std::string &outputFile)
{
     std::ostringstream desc;
     desc << "multifilesrc location=" << inputPattern << " index=0 stop-index=500 caps=\"image/png\" "
          << "! pngdec ! videoconvert ! videoscale "
          << "! video/x-raw,width=1280,height=720,framerate=25/1,format=I420 "
          << "! x264enc bitrate=9000 speed-preset=superfast tune=zerolatency byte-stream=true key-int-max=25 "
          << "! h264parse ! mp4mux faststart=true ! filesink location=" << outputFile;
     return desc.str();
}

// Windows kamera ile video yakalama için GStreamer pipeline tanımı
// Örneği: examples/windowscam.cpp
// Windows işletim sisteminde kamera ile video yakalamak için kullanılır.
std::string GstPipelineBuilder::windowscam(int width, int height)
{
     std::ostringstream desc;
     desc << "ksvideosrc ! "
          << "videoflip method=horizontal-flip !"
          << "video/x-raw,width=" << width
          << ",height=" << height
          << ",framerate=30/1"
          << " ! videoconvert ! autovideosink";
     return desc.str(); // sonda boşluk yok
}

// Video oynatma için GStreamer pipeline tanımı
// Örneği: examples/videoplayer.cpp
// Belirtilen video dosyasını oynatmak için kullanılır.
std::string GstPipelineBuilder::videoplayer(const std::string &videoPattern)
{
     std::ostringstream desc;
     desc << "filesrc location=" << videoPattern << " ! "
          << "qtdemux ! h264parse ! avdec_h264 ! videoconvert ! autovideosink";
     return desc.str();
}

// Windows Ekran Yakalama için GStreamer pipeline tanımı
// Örneği: examples/windowsscreen.cpp
// Windows işletim sisteminde ekran yakalamak için kullanılır.
// Ekran çözünürlüğü ve kare hızı ayarlanabilir.
// Bu örnekte 1920x1080 çözünürlük ve 60 fps kullanılmıştır.
// İmleç gösterimi de aktiftir.
// Not: Bu pipeline için GStreamer'ın D3D11 eklentilerinin yüklü olması gerekir.
// Aksi takdirde çalışmaz.
std::string GstPipelineBuilder::windowsscreen()
{
     std::ostringstream desc;
     desc << "d3d11screencapturesrc show-cursor=true monitor-index=0 ! "                // monitor-index=0 birinci monitör için
                                                                                        // show-cursor=true imleci gösterir
          << "d3d11convert ! "                                                          // Ekran görüntüsünü D3D11 formatına dönüştürür
          << "video/x-raw(memory:D3D11Memory),width=1920,height=1080,framerate=60/1 ! " // Çözünürlük ve kare hızı ayarı
          // memory:D3D11Memory D3D11 bellek formatını kullanır
          // Bu, performansı artırır
          // width ve height değerleri ekran çözünürlüğüne göre ayarlanabilir
          << "d3d11videosink sync=false"; // Ekran görüntüsünü ekranda gösterir
     return desc.str();
}

// Çoklu Unicast ve Multicast Yayını için GStreamer pipeline tanımı
// Örneği: examples/multiunicastlive.cpp
// Windows kamera görüntüsünü hem multicast olarak yayınlar hem de unicast olarak bir adrese gönderir.
// Ayrıca yerel ekranda da görüntüler.
// Üçlü çıktı sağlar: multicast, unicast ve yerel ekran.
std::string GstPipelineBuilder::multiunicastlive()
{
     std::ostringstream desc;

     desc << "ksvideosrc ! " // Windows kamera veya capture kaynağı
          << "videoflip method=horizontal-flip !"
          << "videoconvert ! "
          << "tee name=t "; // Akışı 3 kola ayırır

     // --- Multicast yayını ---
     desc << "t. ! queue ! "
          << "x264enc tune=zerolatency bitrate=800 speed-preset=ultrafast ! "
          << "rtph264pay pt=97 ! "
          << "udpsink host=239.255.42.42 port=5002 auto-multicast=true "; // multicast

     // --- Unicast yayını ---
     desc << "t. ! queue ! "
          << "x264enc tune=zerolatency bitrate=9000 speed-preset=ultrafast ! "
          << "rtph264pay pt=96 ! "
          << "udpsink host=127.0.0.1 port=5000 "; // unicast

     // --- Yerel ekrana görüntüleme ---
     desc << "t. ! queue ! autovideosink"; // ekrana görüntü verir

     return desc.str();
}

std::string GstPipelineBuilder::custompipeline(const std::string &pipeline)
{
     return pipeline;
}
