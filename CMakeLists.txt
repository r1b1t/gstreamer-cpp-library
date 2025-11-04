cmake_minimum_required(VERSION 3.16)

# 📌 Proje bilgisi
project(gstreamer_cpp_library LANGUAGES CXX)

# 📌 C++ standardı
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# (İsteğe bağlı) G++ derleyici yolunu elle belirtebilirsin
# set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/g++.exe")

# 📁 Kaynak ve header dosyalarını bul
file(GLOB_RECURSE GSTCPP_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE GSTCPP_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")

# 📦 Kütüphaneyi oluştur
add_library(gstcpp STATIC ${GSTCPP_SOURCES} ${GSTCPP_HEADERS})
# Eğer dinamik (DLL) kütüphane istersen, üstteki satırı yorum satırına alıp bunu aç:
# add_library(gstcpp SHARED ${GSTCPP_SOURCES} ${GSTCPP_HEADERS})

# 📌 Header dosyalarının bulunduğu include klasörünü herkese açık yap
target_include_directories(gstcpp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 🧩 GStreamer, GTK, Poppler ve diğer bağımlılıkları ekle
find_package(PkgConfig REQUIRED)

pkg_check_modules(DEPS REQUIRED
    gtk+-3.0
    gstreamer-1.0
    gstreamer-rtsp-server-1.0
    poppler-cpp
)

# 📎 Include ve link ayarlarını hedefe bağla
target_include_directories(gstcpp PUBLIC ${DEPS_INCLUDE_DIRS})
target_link_directories(gstcpp PUBLIC ${DEPS_LIBRARY_DIRS})
target_link_libraries(gstcpp PUBLIC ${DEPS_LIBRARIES})

# ⚙️ Derleme uyarılarını ve hata ayarlarını aç
target_compile_options(gstcpp PRIVATE -Wall -Wextra -g)

# 🪟 Windows / MSYS2 için tanımlar
add_definitions(-DUNICODE -D_UNICODE)

# 📤 Build çıktısını belirli bir klasöre yönlendir
set_target_properties(gstcpp PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/output"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/output"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/output"
)

# 🧾 Bilgi mesajları
message(STATUS "GStreamer include dirs: ${DEPS_INCLUDE_DIRS}")
message(STATUS "GStreamer libs: ${DEPS_LIBRARIES}")
message(STATUS "Output directory: ${PROJECT_SOURCE_DIR}/output")
