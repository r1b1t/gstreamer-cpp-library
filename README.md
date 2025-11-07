🧩 GStreamer C++ Library

GStreamerCppLibrary, GStreamer’ı C++ projelerinde daha kolay kullanmak için hazırlanmış bir yardımcı kütüphanedir.
Kütüphane; video işleme, ekran/kamera yakalama, PDF’ten görüntü üretme gibi işlemleri sade bir API üzerinden yapmanızı sağlar.


🚀 Özellikler

Kolay GStreamer entegrasyonu: GStreamerManager sınıfı ile GStreamer pipeline’larını kolayca başlatın, çalıştırın ve yönetin.

Hazır pipeline üreticisi: PipelineBuilder sınıfı ile sık kullanılan işlemler (png → video, kamera, ekran kaydı, multicast yayın vb.) için hazır tanımlar.

PDF → PNG dönüştürme: PDFRenderer1 sınıfı Poppler kütüphanesiyle PDF dosyalarını sayfa sayfa PNG’ye dönüştürür.

CMake uyumlu kütüphane kurulumu: add_subdirectory() veya FetchContent ile kolay entegrasyon.

Windows & Linux desteği: GStreamer platform bağımsız çalıştığı için çoklu sistem uyumluluğu sağlanır.



⚙️ Kurulum
Bağımlılıklar

Projeyi derlemek için aşağıdaki kütüphanelerin sisteminizde yüklü olması gerekir:

GStreamer 1.0+

GStreamer RTSP Server (gstreamer-rtsp-server-1.0)

Poppler (poppler-cpp)


⚙️ Windows (MSYS2 / MinGW64) Kurulum Adımları

Aşağıdaki adımlar Windows + MSYS2 (MinGW64 terminal) ortamında derleme için hazırlanmıştır.

1️⃣ Kütüphaneyi İndirin

Önce bir klasör açın ve terminalde o konuma gidin:

git clone https://github.com/r1b1t/gstreamer-cpp-library.git
cd gstreamer-cpp-library

2️⃣ MinGW araçlarını kontrol edin

Terminalde şu komutları çalıştırın:

which cmake
which g++
which mingw32-make


Çıktıların aşağıdaki gibi olması gerekir:

/mingw64/bin/cmake
/mingw64/bin/g++
/mingw64/bin/mingw32-make


Eğer bu yollar farklıysa aşağıdaki paketleri yükleyin:

pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-make

3️⃣ GStreamer ve eklentilerini kurun

Eğer sisteminizde GStreamer yüklü değilse:

pacman -S mingw-w64-x86_64-gstreamer \
         mingw-w64-x86_64-gst-plugins-base \
         mingw-w64-x86_64-gst-plugins-good

4️⃣ CMake ile derleme ve kurulum

Bir build klasörü oluşturun ve aşağıdaki adımları sırayla çalıştırın:

mkdir build
cd build

/mingw64/bin/cmake -G "MinGW Makefiles" \
  -DCMAKE_INSTALL_PREFIX=/mingw64 \
  -DPKG_CONFIG_EXECUTABLE=/mingw64/bin/pkg-config \
  ..

mingw32-make
mingw32-make install


Bu komutlar:

Kütüphaneyi derler (mingw32-make)

/mingw64/lib altına yükler (mingw32-make install)

Başlık dosyalarını /mingw64/include/GStreamerCppLibrary dizinine kopyalar.

5️⃣ Kütüphaneyi kaldırmak (isteğe bağlı)

Kurulumdan sonra kaldırmak isterseniz:

mingw32-make uninstall


Bu komut, kurulum sırasında eklenen dosyaları sistemden kaldırır.


Kurulum:

sudo cmake --install build

🔧 Kullanım

Yukarıdaki işlemleri yapmadan projenize indirip kullanmak için (CMake):

include(FetchContent)
FetchContent_Declare(
  GStreamerCppLibrary
  GIT_REPOSITORY https://github.com/r1b1t/gstreamer-cpp-library.git
)
FetchContent_MakeAvailable(GStreamerCppLibrary)

target_link_libraries(your_target PRIVATE GStreamerCppLibrary)


🧰 Sağlanan Sınıflar
Sınıf	Açıklama
GStreamerManager	GStreamer pipeline’larını yönetir (oluştur, çalıştır, beklet, temizle).
PipelineBuilder	PNG → video, kamera, ekran yakalama, unicast/multicast gibi hazır pipeline tanımları sağlar.
PDFRenderer1	Poppler kullanarak PDF dosyasını PNG serisine dönüştürür.
