cmake_minimum_required(VERSION 3.16)

project(gstreamer_cpp_library
    VERSION 1.0.0
    LANGUAGES CXX
)

# C++ standardı
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Derleyici uyarılarını aç
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# -------------------------------------------------------
# Kütüphane kaynakları
# -------------------------------------------------------
file(GLOB_RECURSE GSTCPP_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE GSTCPP_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(gstcpp STATIC ${GSTCPP_SOURCES} ${GSTCPP_HEADERS})
# Eğer shared istersen üstteki satırı yorum satırına alıp şu satırı aç:
# add_library(gstcpp SHARED ${GSTCPP_SOURCES} ${GSTCPP_HEADERS})

target_include_directories(gstcpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(gstcpp PUBLIC cxx_std_17)

# -------------------------------------------------------
# GStreamer, GTK, Poppler bağımlılıkları (opsiyonel)
# -------------------------------------------------------
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(DEPS QUIET
        gtk+-3.0
        gstreamer-1.0
        gstreamer-rtsp-server-1.0
        poppler-cpp
    )

    if(DEPS_FOUND)
        target_include_directories(gstcpp PUBLIC ${DEPS_INCLUDE_DIRS})
        target_link_directories(gstcpp PUBLIC ${DEPS_LIBRARY_DIRS})
        target_link_libraries(gstcpp PUBLIC ${DEPS_LIBRARIES})
    endif()
endif()

# -------------------------------------------------------
# Kurulum (install) ve find_package desteği
# -------------------------------------------------------
include(GNUInstallDirs)

install(TARGETS gstcpp
    EXPORT gstcppTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# CMake config dosyası
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/gstcppConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(EXPORT gstcppTargets
    FILE gstcppTargets.cmake
    NAMESPACE gstcpp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gstcpp
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/gstcppConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gstcpp
)

# -------------------------------------------------------
# Paketleme desteği (başkaları FetchContent ile kullansın diye)
# -------------------------------------------------------
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gstcppConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/gstcppConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gstcpp
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/gstcppConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gstcpp
)

# Örnek (isteğe bağlı)
option(BUILD_EXAMPLES "Build example apps" OFF)
if(BUILD_EXAMPLES)
    add_executable(example examples/example_app.cpp)
    target_link_libraries(example PRIVATE gstcpp)
endif()
