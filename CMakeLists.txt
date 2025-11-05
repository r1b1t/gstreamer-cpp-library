cmake_minimum_required(VERSION 3.15)
project(GStreamerCppLibrary VERSION 1.0.0 LANGUAGES CXX)

# C++ standardı
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FetchContent ile kullanılıyorsa install etme
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(IS_TOP_LEVEL TRUE)
else()
    set(IS_TOP_LEVEL FALSE)
endif()

# GStreamer ve diğer bağımlılıkları bul
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTREAMER_RTSP REQUIRED gstreamer-rtsp-server-1.0)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
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

# Shared library oluştur
add_library(${PROJECT_NAME} SHARED ${LIBRARY_SOURCES})

# Static library de oluştur
add_library(${PROJECT_NAME}_static STATIC ${LIBRARY_SOURCES})
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

# Include dizinleri - SHARED library için
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${GSTREAMER_INCLUDE_DIRS}
        ${GSTREAMER_RTSP_INCLUDE_DIRS}
        ${GTK3_INCLUDE_DIRS}
        ${POPPLER_INCLUDE_DIRS}
)

# Include dizinleri - STATIC library için
target_include_directories(${PROJECT_NAME}_static
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${GSTREAMER_INCLUDE_DIRS}
        ${GSTREAMER_RTSP_INCLUDE_DIRS}
        ${GTK3_INCLUDE_DIRS}
        ${POPPLER_INCLUDE_DIRS}
)

# Bağımlılıkları link et - SHARED library için
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${GSTREAMER_LIBRARIES}
        ${GSTREAMER_RTSP_LIBRARIES}
        ${GTK3_LIBRARIES}
        ${POPPLER_LIBRARIES}
)

# Bağımlılıkları link et - STATIC library için
target_link_libraries(${PROJECT_NAME}_static
    PUBLIC
        ${GSTREAMER_LIBRARIES}
        ${GSTREAMER_RTSP_LIBRARIES}
        ${GTK3_LIBRARIES}
        ${POPPLER_LIBRARIES}
)

# Compiler flags - SHARED library için
target_compile_options(${PROJECT_NAME} PRIVATE
    ${GSTREAMER_CFLAGS_OTHER}
    ${GSTREAMER_RTSP_CFLAGS_OTHER}
    ${GTK3_CFLAGS_OTHER}
    ${POPPLER_CFLAGS_OTHER}
)

# Compiler flags - STATIC library için
target_compile_options(${PROJECT_NAME}_static PRIVATE
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

# Alias target oluştur (FetchContent için gerekli)
add_library(GStreamerCppLibrary::GStreamerCppLibrary ALIAS ${PROJECT_NAME})

# Install kuralları (sadece top-level build ise)
if(IS_TOP_LEVEL)
    include(GNUInstallDirs)

    # Kütüphane ve header dosyalarını install et
    install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_static
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    )

    # CMake config dosyalarını oluştur
    include(CMakePackageConfigHelpers)

    # Config dosyası oluştur
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    # Version dosyası oluştur
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # Export targets
    install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    # Config dosyalarını install et
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    # Export build tree targets
    export(EXPORT ${PROJECT_NAME}Targets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
    )

    # pkg-config dosyası oluştur
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}.pc.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
        @ONLY
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()