cmake_minimum_required(VERSION 3.15)

project(
    GStreamerCppLibrary
    VERSION 1.2.0
    DESCRIPTION "C++ library that wraps GStreamer, RTSP Server, and Poppler for PDF rendering and streaming."
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Bağımsız mı alt proje mi?
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(IS_TOP_LEVEL TRUE)
else()
    set(IS_TOP_LEVEL FALSE)
endif()

# Bağımlılıklar
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTREAMER_APP REQUIRED gstreamer-app-1.0)
pkg_check_modules(GSTREAMER_RTSP REQUIRED gstreamer-rtsp-server-1.0)
pkg_check_modules(POPPLER REQUIRED poppler-cpp)

# Kaynak dosyalar
set(LIBRARY_SOURCES
    src/GStreamer/Manager.cpp
    src/GStreamer/PipelineBuilder.cpp
    src/GStreamerRTSPServer/Manager.cpp
    src/GStreamerRTSPServer/MediaHub.cpp
    src/GStreamerRTSPServer/PipelineBuilder.cpp
    src/Poppler/PDFRendererToPNG.cpp
)

# Header dosyalar
set(LIBRARY_HEADERS
    include/GStreamer/Manager.hpp
    include/GStreamer/PipelineBuilder.hpp
    include/GStreamerRTSPServer/Manager.hpp
    include/GStreamerRTSPServer/MediaHub.hpp
    include/GStreamerRTSPServer/PipelineBuilder.hpp
    include/Poppler/PDFRendererToPNG.hpp
)

# Shared library
add_library(${PROJECT_NAME} SHARED ${LIBRARY_SOURCES})

# Include path’ler
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${GSTREAMER_INCLUDE_DIRS}
        ${GSTREAMER_APP_INCLUDE_DIRS}
        ${GSTREAMER_RTSP_INCLUDE_DIRS}
        ${POPPLER_INCLUDE_DIRS}
)

# Linkleme
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${GSTREAMER_LIBRARIES}
        ${GSTREAMER_APP_LIBRARIES}
        ${GSTREAMER_RTSP_LIBRARIES}
        ${POPPLER_LIBRARIES}
)

# Derleme opsiyonları
target_compile_options(${PROJECT_NAME} PRIVATE
    ${GSTREAMER_CFLAGS_OTHER}
    ${GSTREAMER_APP_CFLAGS_OTHER}
    ${GSTREAMER_RTSP_CFLAGS_OTHER}
    ${POPPLER_CFLAGS_OTHER}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

add_library(GStreamerCppLibrary::GStreamerCppLibrary ALIAS ${PROJECT_NAME})

# ==================================
# INSTALL bölümü (Windows uyumlu)
# ==================================
if(IS_TOP_LEVEL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}   # ⬅️ BUNU EKLEDİK
    )

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        FILES_MATCHING PATTERN "*.hpp"
    )

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

    # pkg-config dosyası
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
