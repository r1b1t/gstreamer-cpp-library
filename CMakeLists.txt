# Bu dosyanın çalışması için en düşük CMake sürümünü belirler.
# CMake 3.15 veya üzeri kullanılmazsa hata verir.
cmake_minimum_required(VERSION 3.15)
# Projenin adını GStreamerCppLibrary olarak belirler, sürümünü 1.0.0 yapar ve yalnızca C++ diliyle derleneceğini söyler.
project(GStreamerCppLibrary VERSION 1.0.0 LANGUAGES CXX)

# Proje C++17 standardıyla derlenecek.
set(CMAKE_CXX_STANDARD 17)
# C++17 standardının zorunlu olduğunu belirtir.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FetchContent ile kullanılıyorsa install etme
# Yani CMAKE_SOURCE_DIR: En tepedeki (root) CMakeLists.txt’in bulunduğu dizin.
#PROJECT_SOURCE_DIR: Şu an içinde bulunduğun geçerli projenin (project(...) ile tanımlanan) kaynak dizini.

#Durumlar:
#Bu kütüphaneyi kendi başına derliyorsan (yani cmake -S . -B build dediğin dizinde bu proje en tepedeyse), her ikisi de aynı dizini gösterir ⇒ IS_TOP_LEVEL = TRUE.
#Bu kütüphaneyi başka bir projenin alt projesi olarak eklediysen (ör. add_subdirectory(vendor/GStreamerCppLibrary) ya da FetchContent_MakeAvailable), en tepedeki dizin üst projedir; kütüphanenin PROJECT_SOURCE_DIR’i alt klasördür ⇒ IS_TOP_LEVEL = FALSE.
#Kısacası: Bağımsız derleme = TRUE, başka projeye gömülü derleme = FALSE.CMAKE_SOURCE_DIR: En tepedeki (root) CMakeLists.txt’in bulunduğu dizin.
#PROJECT_SOURCE_DIR: Şu an içinde bulunduğun geçerli projenin (project(...) ile tanımlanan) kaynak dizini.

#Durumlar:
#Bu kütüphaneyi kendi başına derliyorsan (yani cmake -S . -B build dediğin dizinde bu proje en tepedeyse), her ikisi de aynı dizini gösterir ⇒ IS_TOP_LEVEL = TRUE.
#Bu kütüphaneyi başka bir projenin alt projesi olarak eklediysen (ör. add_subdirectory(vendor/GStreamerCppLibrary) ya da FetchContent_MakeAvailable), en tepedeki dizin üst projedir; kütüphanenin PROJECT_SOURCE_DIR’i alt klasördür ⇒ IS_TOP_LEVEL = FALSE.
#Kısacası: Bağımsız derleme = TRUE, başka projeye gömülü derleme = FALSE.
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(IS_TOP_LEVEL TRUE)
else()
    set(IS_TOP_LEVEL FALSE)
endif()

# GStreamer ve diğer bağımlılıkları bul
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTREAMER_RTSP REQUIRED gstreamer-rtsp-server-1.0)
pkg_check_modules(POPPLER REQUIRED poppler-cpp)

# Kütüphane kaynak dosyaları
set(LIBRARY_SOURCES
    src/GstreamerManager.cpp
    src/PDFRenderer1.cpp
    src/PDFRenderer2.cpp
    src/PipelineBuilder.cpp
)

# Kütüphane header dosyaları
set(LIBRARY_HEADERS
    include/GstreamerManager.h
    include/PDFRenderer1.h
    include/PDFRenderer2.h
    include/PipelineBuilder.h
)

# Sadece shared library oluştur
add_library(${PROJECT_NAME} SHARED ${LIBRARY_SOURCES})

# Include dizinleri
target_include_directories(${PROJECT_NAME}
    # Hem derleme zamanında hem de kütüphane kullanan projelerde geçerli olacak include dizinleri
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    #Sadece bu hedef derlenirken kullanılacak include dizinleri
    PRIVATE
        ${GSTREAMER_INCLUDE_DIRS}
        ${GSTREAMER_RTSP_INCLUDE_DIRS}
        ${POPPLER_INCLUDE_DIRS}
)

# Bağımlılıkları link et
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${GSTREAMER_LIBRARIES}
        ${GSTREAMER_RTSP_LIBRARIES}
        ${POPPLER_LIBRARIES}
)

# Derleme seçenekleri
target_compile_options(${PROJECT_NAME} PRIVATE
    ${GSTREAMER_CFLAGS_OTHER}
    ${GSTREAMER_RTSP_CFLAGS_OTHER}
    ${GTK3_CFLAGS_OTHER}
    ${POPPLER_CFLAGS_OTHER}
)

# Library özellikleri
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${LIBRARY_HEADERS}"
)

# Alias target (FetchContent için)
add_library(GStreamerCppLibrary::GStreamerCppLibrary ALIAS ${PROJECT_NAME})

# Install kuralları (sadece top-level build ise)
if(IS_TOP_LEVEL)
    include(GNUInstallDirs)

    install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    )

    include(CMakePackageConfigHelpers)

    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    export(EXPORT ${PROJECT_NAME}Targets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
    )

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}.pc.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
        @ONLY
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()

# Uninstall hedefi
if(IS_TOP_LEVEL)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()
