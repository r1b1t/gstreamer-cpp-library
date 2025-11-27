#include "GStreamerRTSPServer/PipelineBuilder.hpp"
#include <sstream>

std::string GstRTSPPipelineBuilder::shared_stream(int width, int height, int bitrate)
{
    std::ostringstream desc;
    // key-int-max=15 yaptık (Yarım saniyede bir tam resim yollar, hızlı açılır)
    // tune=zerolatency zaten var, bu gecikmeyi düşürür.

    desc << "appsrc name=shared_src is-live=true format=time do-timestamp=true ! "
         << "video/x-raw,format=I420,width=" << width << ",height=" << height << ",framerate=30/1 ! "
         << "videoconvert ! "
         << "x264enc bitrate=" << bitrate
         << " speed-preset=ultrafast tune=zerolatency key-int-max=15 sliced-threads=true ! "
         << "rtph264pay name=pay0 pt=96 config-interval=1";
    return desc.str();
}
// -------------------------------------------------------------
// GERÇEK KAYNAK KAMERA (ksvideosrc - Windows kamera)
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::windowscam(int width, int height, int fps, int deviceIndex, bool mirror, int bitrate, int payloadType)
{
    std::ostringstream desc;

    // 1. Kaynak
    desc << "ksvideosrc device-index=" << deviceIndex << " ! ";

    // 2. Aynalama
    if (mirror)
    {
        desc << "videoflip method=horizontal-flip ! ";
    }

    // 3. Ham Video Formatı
    desc << "video/x-raw,width=" << width
         << ",height=" << height
         << ",framerate=" << fps << "/1 ! ";

    // 4. Encoder
    desc << "videoconvert ! "
         << "x264enc bitrate=" << bitrate
         << " speed-preset=superfast tune=zerolatency key-int-max=" << fps << " ! ";

    // 5. Payloader (name=pay0 SABİT KALMALI, pt dinamik oldu)
    desc << "rtph264pay name=pay0 pt=" << payloadType << " config-interval=1";

    return desc.str();
}
// -------------------------------------------------------------
// SANAL KAYNAK (videotestsrc - test pattern)
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::videotestsrc(int width, int height, int fps, int bitrate, int payload, int pattern)
{
    std::ostringstream desc;

    // 1. Kaynak: Pattern ve Live Modu
    // is-live=true: Gerçek bir kamera gibi davranmasını sağlar (timestamp yönetimi için önemli)
    desc << "videotestsrc pattern=" << pattern << " is-live=true ! ";

    // 2. Ham Video Ayarları (Width, Height, FPS)
    desc << "video/x-raw,width=" << width
         << ",height=" << height
         << ",framerate=" << fps << "/1 ! ";

    // 4. Dönüştürme ve Encode
    desc << "videoconvert ! "
         << "x264enc bitrate=" << bitrate
         << " speed-preset=ultrafast tune=zerolatency ! ";

    // 5. Payloader
    desc << "rtph264pay name=pay0 pt=" << payload << " config-interval=1";

    return desc.str();
}

// -------------------------------------------------------------
// UDP YAYININI KAYNAK ALAN PİPELİNE
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::udpsrc(const std::string &ip, int port, int payload, int bitrate, int width, int height, int fps)
{
    std::ostringstream desc;

    // 1. Kaynak (Source)
    desc << "udpsrc port=" << port;
    if (!ip.empty())
    {
        desc << " multicast-group=" << ip << " auto-multicast=true buffer-size=5242880";
    }

    // Input Caps
    desc << " caps=\"application/x-rtp,media=video,encoding-name=H264,payload=" << payload << "\" ! ";

    // 2. Decode ve Convert
    // 'videoconvert' raw veriyi hazırlar
    desc << "rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! ";

    // 3. RESIZE (Boyutlandırma)
    if (width > 0 && height > 0)
    {
        // 'video/x-raw' caps'i videoscale ile birleştirilir
        desc << "videoscale ! video/x-raw,width=" << width << ",height=" << height << " ! ";
    }

    // 4. FRAMERATE (FPS Ayarı) - YENİ EKLENEN KISIM
    if (fps > 0)
    {
        // videorate elementi akışı düzenler, sonraki caps ile kaç fps olacağını söyleriz
        // "fps/1" formatı kullanılır (örneğin 30/1)
        desc << "videorate ! video/x-raw,framerate=" << fps << "/1 ! ";
    }

    // 5. Encode
    desc << "x264enc bitrate=" << bitrate
         << " speed-preset=ultrafast tune=zerolatency ! ";

    // 6. Output Payloader
    desc << "rtph264pay name=pay0 pt=" << payload << " config-interval=1";

    return desc.str();
}

// -------------------------------------------------------------
// RTSP YAYININI KAYNAK ALAN PİPELİNE
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::rtspsrc(const std::string &sourceUrl, int latency, int bitrate, int width, int height, int fps, int payload)
{
    std::ostringstream desc;

    // 1. RTSP Kaynağı ve Latency Ayarı
    // 'latency' parametresi RTSP içindeki buffer süresini belirler (ms cinsinden).
    desc << "rtspsrc location=" << sourceUrl << " latency=" << latency << " ! ";

    // rtpjitterbuffer: Ağdaki dalgalanmaları (jitter) düzeltir.
    // rtspsrc'nin latency ayarı genelde yeterlidir ama ekstra güvenlik için buffer eklenebilir.
    // Eğer çok düşük gecikme istiyorsan bu 'rtpjitterbuffer' satırını kaldırabilirsin.
    desc << "rtpjitterbuffer latency=" << latency << " ! ";

    // 2. Decode İşlemi
    desc << "rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! ";

    // 3. RESIZE (Yeniden Boyutlandırma)
    // 0 gelirse orijinal boyut korunur.
    if (width > 0 && height > 0)
    {
        desc << "videoscale ! video/x-raw,width=" << width << ",height=" << height << " ! ";
    }

    // 4. FRAMERATE (FPS Ayarı)
    // 0 gelirse orijinal FPS korunur.
    if (fps > 0)
    {
        desc << "videorate ! video/x-raw,framerate=" << fps << "/1 ! ";
    }

    // 5. Encode (Sıkıştırma)
    desc << "x264enc bitrate=" << bitrate
         << " speed-preset=ultrafast tune=zerolatency ! ";

    // 6. Output Payloader
    desc << "rtph264pay name=pay0 pt=" << payload << " config-interval=1";

    return desc.str();
}

// -------------------------------------------------------------
// UDP YAYININI DECODE -> FİLTRELE -> ENCODE YAPAN PİPELİNE
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::udpsrcfilter(int port, int width, int height, int bitrate)
{
    std::ostringstream desc;
    desc << "udpsrc port=" << port
         << " caps=\"application/x-rtp,media=video,encoding-name=H264,payload=96\" ! "
         << "rtph264depay ! h264parse ! avdec_h264 ! "
         << "videoconvert ! videoscale ! "
         << "video/x-raw,width=" << width << ",height=" << height << ",framerate=30/1 ! "
         << "x264enc bitrate=" << bitrate
         << " speed-preset=superfast tune=zerolatency key-int-max=25 ! "
         << "rtph264pay name=pay0 pt=96 config-interval=1";
    return desc.str();
}

// -------------------------------------------------------------
// ÇOKLU KAYNAKLI VE KAYNAK SEÇİCİLİ PİPELİNE (input-selector)
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::inputselector()
{
    std::ostringstream desc;
    desc << "input-selector name=sel ! "
         << "videoconvert ! x264enc bitrate=3000 speed-preset=superfast tune=zerolatency ! "
         << "rtph264pay name=pay0 pt=96 config-interval=1 "
         << "videotestsrc pattern=smpte ! sel.sink_0 "
         << "videotestsrc pattern=ball ! sel.sink_1 "
         << "videotestsrc pattern=snow ! sel.sink_2";
    return desc.str();
}

// -------------------------------------------------------------
// Video dosyası oynatıcı → RTSP
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::videoplayer(const std::string &videoFile, int bitrate, int payload)
{
    std::ostringstream desc;
    desc << "filesrc location=" << videoFile << " ! "
         << "qtdemux ! h264parse ! avdec_h264 ! videoconvert "
         << "! x264enc bitrate=" << bitrate << " speed-preset=ultrafast tune=zerolatency "
         << "rtph264pay name=pay0 pt=" << payload << " config-interval=1";
    return desc.str();
}

// -------------------------------------------------------------
// Custom pipeline → RTSP uyumlu hale getir (otomatik encode + pay0)
// -------------------------------------------------------------
std::string GstRTSPPipelineBuilder::custom(const std::string &pipeline)
{
    std::ostringstream desc;
    desc << pipeline;

    return desc.str();
}
