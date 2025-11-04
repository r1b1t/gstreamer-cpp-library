#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "gstcpp::gstcpp" for configuration ""
set_property(TARGET gstcpp::gstcpp APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(gstcpp::gstcpp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libgstcpp.a"
  )

list(APPEND _cmake_import_check_targets gstcpp::gstcpp )
list(APPEND _cmake_import_check_files_for_gstcpp::gstcpp "${_IMPORT_PREFIX}/lib/libgstcpp.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
