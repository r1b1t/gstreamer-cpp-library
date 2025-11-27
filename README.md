# 🧩 GStreamer C++ Library

**GStreamerCppLibrary**, C++ projelerinde GStreamer, RTSP Server ve Poppler kütüphanelerini kolayca kullanmanızı sağlayan bir yardımcı kütüphanedir.  
Bu kütüphane ile video oluşturma, oynatma, ekran veya kamera yakalama, RTSP yayın açma, PDF dosyalarını PNG’ye dönüştürme gibi işlemleri sade bir API ile gerçekleştirebilirsiniz.

---

## 🚀 Özellikler

- 🎬 **Kolay GStreamer yönetimi** – `GstManager` sınıfı ile pipeline oluşturma, oynatma, durdurma ve temizleme işlemleri.
- 📡 **RTSP Sunucu Yönetimi** – `GstRTSPManager` ile çoklu RTSP yayını ekleme, kaynak değiştirme ve pipeline kontrolü.
- 🧱 **Hazır pipeline üreticisi** – PNG → video, kamera, ekran yakalama, UDP, RTSP re-stream, input-selector gibi birçok pipeline örneği.
- 📄 **PDF → PNG dönüştürme** – `PDFRendererToPNG` ile Poppler kullanarak PDF sayfalarını PNG’ye çevirir.
- ⚙️ **CMake entegrasyonu** – `FetchContent` veya `add_subdirectory` ile kolayca projeye eklenir.
- 💻 **Windows (MSYS2/MinGW) ve Linux desteği.**

---

## 🧠 Sınıf Özeti

| 🔧 Sınıf Adı | 🎯 Açıklama |
|--------------|-------------|
| **GstManager** | 🎬 GStreamer pipeline’larını oluşturur, oynatır, duraklatır ve yönetir. |
| **GstPipelineBuilder** | 🧱 PNG → video, kamera, ekran, UDP, RTSP, input-selector gibi hazır pipeline’lar oluşturur. |
| **GstRTSPManager** | 📡 RTSP sunucusu oluşturur, çoklu RTSP yayını ekler ve pipeline durumlarını yönetir. |
| **GstRTSPPipelineBuilder** | 🧱 RTSP uyumlu H264/RTP pipeline’lar oluşturur. |
| **PDFRendererToPNG** | 📄 Poppler kullanarak PDF dosyalarını PNG resimlerine dönüştürür. |
| **CMake Entegrasyonu** | ⚙️ `FetchContent` veya `add_subdirectory` yöntemiyle kolay entegrasyon. |
| **Platform Desteği** | 💻 Windows (MSYS2 / MinGW64) ve Linux üzerinde çalışır. |

---

## 🧰 Gerekli Kütüphaneler

Proje aşağıdaki bağımlılıkları gerektirir:

- `GStreamer 1.0`
- `GStreamer RTSP Server`
- `Poppler (poppler-cpp)`

---

## ⚙️ Windows (MSYS2 / MinGW64) Kurulum Adımları

> **Not:** Bu adımlar Windows’ta **MSYS2 MinGW64 terminali** kullanılarak test edilmiştir. Sizin de kullanmanız önerilir.

---

### 1️⃣ MinGW Araçlarını Kontrol Edin

Aşağıdaki komutları çalıştırın:

```
which cmake
```

```
which g++
```


Çıktı aşağıdaki gibi olmalıdır:

```
/mingw64/bin/cmake
```

```
/mingw64/bin/g++
```


Eğer bu yollar farklıysa şu paketleri yükleyin:

```
pacman -S mingw-w64-x86_64-cmake
```


### 2️⃣ GStreamer ve Eklentilerini Kurun

Eğer sisteminizde GStreamer kurulu değilse:

```
pacman -S mingw-w64-x86_64-gstreamer \
         mingw-w64-x86_64-gst-plugins-base \
         mingw-w64-x86_64-gst-plugins-good
```

### 3️⃣ Kütüphaneyi İndirin

MSYS2 MinGW64 Terminaline giriş yapın. Projeyi GitHub'dan indirip proje adresine gidin.

```
git clone https://github.com/r1b1t/gstreamer-cpp-library.git
cd gstreamer-cpp-library
```

### 4️⃣ CMake ile Derleme ve Kurulum

MSYS2 MinGW64 Terminaline giriş yapın. Projeyi GitHub'dan indirip proje adresine gidin.

```
mkdir build && cd build
```
```
/mingw64/bin/cmake -G "MinGW Makefiles" \
  -DCMAKE_INSTALL_PREFIX=/mingw64 \
  -DPKG_CONFIG_EXECUTABLE=/mingw64/bin/pkg-config \
  ..
```

```
cmake --build . 
```

```
cmake --install . 
```

## ✅ Açıklama:
- cmake --build .  → kütüphaneyi derler.
- cmake --install .  → derlenen dosyaları /mingw64/lib altına kopyalar.
- Başlık dosyaları /mingw64/include/GStreamerCppLibrary/ dizinine yerleştirilir.

### 5️⃣ Kütüphaneyi Kaldırma

Kurulumu kaldırmak için:

```
cmake --build . --target uninstall
```

Bu komut, kurulum sırasında yüklenen dosyaları sistemden temizler.

### 📦 CMake Entegrasyonu

Projeye FetchContent kullanarak dahil edebilirsiniz:

```
FetchContent_Declare(
    GStreamerCppLibrary
    GIT_REPOSITORY https://github.com/r1b1t/gstreamer-cpp-library.git
    GIT_TAG main  # veya belirli bir sürüm etiketi
)

FetchContent_MakeAvailable(GStreamerCppLibrary)

# Manuel include / link dizinleri (MSYS2 ortamı için)
include_directories(
    C:/msys64/mingw64/include/gstreamer-1.0
    C:/msys64/mingw64/include/glib-2.0
    C:/msys64/mingw64/lib/glib-2.0/include
)

link_directories(C:/msys64/mingw64/lib)

# --- Linkleme ---
target_link_libraries(QTGSTLibraryFetch
    PRIVATE
        Qt::Core
        gstreamer-1.0
        gstbase-1.0
        gstapp-1.0
        gobject-2.0
        glib-2.0
        GStreamerCppLibrary::GStreamerCppLibrary
)
```






