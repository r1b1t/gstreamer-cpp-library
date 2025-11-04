cmake_minimum_required(VERSION 3.16)

# Proje ismi ve dil ayarı
project(gstreamer_cpp_library LANGUAGES CXX)

# C++17 standardı
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# G++ yolunu elle belirtebilirsin (isteğe bağlı)
# set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/g++.exe")

# Kaynak ve header dosyalarını bul
file(GLOB_RECURSE GSTCPP_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/**/*.h")
file(GLOB_RECURSE GSTCPP_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Kütüphaneyi oluştur
add_library(gstcpp STATIC ${GSTCPP_SOURCES} ${GSTCPP_HEADERS})
# Dinamik istersen:
# add_library(gstcpp SHARED ${GSTCPP_SOURCES} ${GSTCPP_HEADERS})

# Include dizinlerini belirt
target_include_directories(gstcpp
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# --- GStreamer, GTK, Poppler ve diğer bağımlılıkları ekle ---
find_package(PkgConfig REQUIRED)

pkg_check_modules(DEPS REQUIRED
    gtk+-3.0
    gstreamer-1.0
    gstreamer-rtsp-server-1.0
    poppler-cpp
)

# Include ve link ayarlarını hedefe bağla
target_include_directories(gstcpp PUBLIC ${DEPS_INCLUDE_DIRS})
target_link_directories(gstcpp PUBLIC ${DEPS_LIBRARY_DIRS})
target_link_libraries(gstcpp PUBLIC ${DEPS_LIBRARIES})

# Windows/MSYS2 için link hatalarını önle
add_definitions(-DUNICODE -D_UNICODE)

# Derleme uyarılarını aktif et (senin -Wall, -Wextra ayarların)
target_compile_options(gstcpp PRIVATE -Wall -Wextra -g)

# Build çıktısını ayrı klasöre al
set_target_properties(gstcpp PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/output"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/output"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/output"
)

# Build bilgilerini ekrana yaz
message(STATUS "GStreamer include dirs: ${DEPS_INCLUDE_DIRS}")
message(STATUS "GStreamer libs: ${DEPS_LIBRARIES}")
message(STATUS "Output directory: ${PROJECT_SOURCE_DIR}/output")
