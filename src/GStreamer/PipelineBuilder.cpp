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
std::string GstPipelineBuilder::windowscam(int width, int height, int fps, int deviceIndex, bool mirror, bool addSink)
{
     std::ostringstream desc;

     // 1. Kaynak
     desc << "ksvideosrc device-index=" << deviceIndex << " ! ";

     // 2. Aynalama Kontrolü
     if (mirror)
     {
          desc << "videoflip method=horizontal-flip ! ";
     }

     // 3. Çözünürlük ve Format
     desc << "video/x-raw,width=" << width << ",height=" << height << ",framerate=" << fps << "/1 ! "
          << "videoconvert"; // Yayın için buraya kadar ham veri lazım

     // 4. Sink Kontrolü (UDP için false olmalı)
     if (addSink)
     {
          desc << " ! autovideosink sync=false";
     }

     return desc.str();
}

// Test video kaynağı için GStreamer pipeline tanımı
// Örneği: examples/videotestsrc.cpp
// Test video kaynağı oluşturur (örneğin, top hareketi)
std::string GstPipelineBuilder::videotestsrc(int width, int height, int fps, std::string pattern, bool addSink)
{
     std::ostringstream desc;
     desc << "videotestsrc pattern=" << pattern << " ! "
          << "video/x-raw,width=" << width << ",height=" << height << ",framerate=" << fps << "/1 ! "
          << "videoconvert";

     if (addSink)
     {
          desc << " ! autovideosink sync=false";
     }
     return desc.str();
}

// Windows ekran yakalama için GStreamer pipeline tanımı
// Örneği: examples/windowsscreen.cpp
// Windows işletim sisteminde ekran yakalamak için kullanılır.
std::string GstPipelineBuilder::windowsscreen(int width, int height, int fps, bool addSink)
{
     std::ostringstream desc;
     // Ekran yakalama D3D11 kullandığı için CPU belleğine indirmemiz gerekebilir
     desc << "d3d11screencapturesrc show-cursor=true ! "
          << "d3d11convert ! "
          << "video/x-raw(memory:D3D11Memory),width=" << width << ",height=" << height << ",framerate=" << fps << "/1 ! "
          << "d3d11download ! videoconvert";

     if (addSink)
     {
          desc << " ! autovideosink sync=false";
     }
     return desc.str();
}

// Video dosyası oynatma için GStreamer pipeline tanımı
// Örneği: examples/videoplayer.cpp
// Belirtilen video dosyasını oynatır.
std::string GstPipelineBuilder::videoplayer(const std::string &filePath, bool addSink)
{
     std::ostringstream desc;
     // Dosya yolunu tırnak içine alıyoruz ki boşluklu yollarda hata vermesin
     desc << "filesrc location=\"" << filePath << "\" ! "
          << "decodebin ! videoconvert";

     if (addSink)
     {
          desc << " ! autovideosink sync=true";
     }
     return desc.str();
}

// Çoklu Unicast ve Multicast Yayını için GStreamer pipeline tanımı
// Örneği: examples/multiunicastlive.cpp
// Windows kamera görüntüsünü hem multicast olarak yayınlar hem de unicast olarak bir adrese gönderir.
// Ayrıca yerel ekranda da görüntüler.
// Üçlü çıktı sağlar: multicast, unicast ve yerel ekran.
std::string GstPipelineBuilder::multiunicastlivetee()
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

// Kullanıcının kendi pipeline'ını doğrudan kullanabilmesi için
std::string GstPipelineBuilder::custompipeline(const std::string &pipeline)
{
     return pipeline;
}

// ==========================================
//  YAYINLAYICILAR (UDP)
// ==========================================

// UDP Tekli Yayın Gönderici için GStreamer pipeline tanımı
std::string GstPipelineBuilder::udpUnicastSender(const std::string &sourcePipeline, const std::string &ip, int port, int bitrate, int payloadType)
{
     std::ostringstream desc;
     desc << sourcePipeline << " ! "
          << "x264enc tune=zerolatency bitrate=" << bitrate << " speed-preset=ultrafast ! "
          // BURASI DEĞİŞTİ:
          << "rtph264pay pt=" << payloadType << " config-interval=1 ! "
          << "udpsink host=" << ip << " port=" << port << " sync=false async=false";
     return desc.str();
}
// UDP Çoklu Yayın Gönderici için GStreamer pipeline tanımı
std::string GstPipelineBuilder::udpMulticastSender(const std::string &sourcePipeline, const std::string &ip, int port, int bitrate, int payloadType)
{
     std::ostringstream desc;

     // 1. Kaynağı ekle
     desc << sourcePipeline << " ! ";

     // 2. Kodlama (Encoder)
     desc << "x264enc tune=zerolatency bitrate=" << bitrate << " speed-preset=ultrafast ! ";

     // 3. Paketleme (Payloader)
     // BURASI GÜNCELLENDİ: pt değerini dışarıdan alıyoruz
     desc << "rtph264pay pt=" << payloadType << " config-interval=1 ! ";

     // 4. Multicast Sink
     // auto-multicast=true: Multicast gruplarına katılım için şarttır.
     desc << "udpsink host=" << ip << " port=" << port << " auto-multicast=true sync=false async=false";

     return desc.str();
}
// UDP Alıcı pipeline tanımı
std::string GstPipelineBuilder::udpReceiver(int port, std::string multicastIP, int payloadType)
{
     std::ostringstream desc;
     desc << "udpsrc port=" << port;

     if (!multicastIP.empty())
     {
          desc << " address=" << multicastIP << " auto-multicast=true";
     }

     // BURASI DEĞİŞTİ: payload=(int)... kısmı dinamik oldu
     desc << " caps=\"application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)"
          << payloadType << "\" ! "
          << "rtph264depay ! avdec_h264 ! videoconvert ! autovideosink sync=false";

     return desc.str();
}


