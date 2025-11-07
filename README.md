# 🧩 GStreamer C++ Library

**GStreamerCppLibrary**, [GStreamer](https://gstreamer.freedesktop.org/) altyapısını C++ projelerinde kolayca kullanmanızı sağlayan bir yardımcı kütüphanedir.  
Bu kütüphane ile video oluşturma, oynatma, ekran veya kamera yakalama, PDF dosyalarını resme dönüştürme gibi işlemleri sade bir API ile yapabilirsiniz.

---

## 🚀 Özellikler

- 🎬 **Kolay GStreamer yönetimi** – `GStreamerManager` sınıfı ile pipeline oluşturma, oynatma ve temizleme işlemleri.
- 🧱 **Hazır pipeline üreticisi** – `PipelineBuilder` ile PNG → video, kamera, ekran yakalama, multicast/unicast yayın gibi örnekler.
- 📄 **PDF → PNG dönüştürme** – `PDFRenderer1` sınıfı ile Poppler kullanarak PDF sayfalarını PNG’ye çevirir.
- ⚙️ **CMake desteği** – `FetchContent` veya `add_subdirectory` ile kolayca entegre edilebilir.
- 💻 **Windows (MSYS2/MinGW) ve Linux desteği.**

---

## 🧠 Sınıf Özeti

| 🔧 Sınıf Adı | 🎯 Açıklama |
|--------------|-------------|
| **GStreamerManager** | 🎬 GStreamer pipeline’larını oluşturur, oynatır ve yönetir. Pipeline durumlarını izler, hata ve sonlandırma işlemlerini otomatik gerçekleştirir. |
| **PipelineBuilder** | 🧱 PNG → video, kamera, ekran yakalama, multicast/unicast yayın gibi hazır GStreamer pipeline tanımları oluşturur. |
| **PDFRenderer1** | 📄 Poppler kütüphanesini kullanarak PDF dosyalarını sayfa sayfa PNG resimlerine dönüştürür. |
| **CMake Entegrasyonu** | ⚙️ Kütüphane, `FetchContent` veya `add_subdirectory` yöntemiyle başka projelere kolayca entegre edilebilir. |
| **Platform Desteği** | 💻 Windows (MSYS2 / MinGW64) ve Linux sistemlerinde derlenebilir ve kullanılabilir. |



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

```
which mingw32-make
```

Çıktı aşağıdaki gibi olmalıdır:

```
/mingw64/bin/cmake
```

```
/mingw64/bin/g++
```

```
/mingw64/bin/mingw32-make
```

Eğer bu yollar farklıysa şu paketleri yükleyin:

```
pacman -S mingw-w64-x86_64-cmake
```

```
pacman -S mingw-w64-x86_64-make
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
mingw32-make
```

```
mingw32-make install
```

## ✅ Açıklama:
mingw32-make → kütüphaneyi derler.
mingw32-make install → derlenen dosyaları /mingw64/lib altına kopyalar.
Başlık dosyaları /mingw64/include/GStreamerCppLibrary/ dizinine yerleştirilir.

### 5️⃣ Kütüphaneyi Kaldırma

Kurulumu kaldırmak için:

```
mingw32-make uninstall
```

Bu komut, kurulum sırasında yüklenen dosyaları sistemden temizler.

### 📦 CMake Entegrasyonu

Projeye FetchContent kullanarak dahil edebilirsiniz:

```
include(FetchContent)
FetchContent_Declare(
  GStreamerCppLibrary
  GIT_REPOSITORY https://github.com/r1b1t/gstreamer-cpp-library.git
)
FetchContent_MakeAvailable(GStreamerCppLibrary)

target_link_libraries(your_target PRIVATE GStreamerCppLibrary)
```






