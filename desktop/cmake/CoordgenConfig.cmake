add_library(coordgen SHARED IMPORTED)
set_property(TARGET coordgen APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(coordgen PROPERTIES
        IMPORTED_IMPLIB_DEBUG "${INSTALL_DIR}/lib/coordgend.lib"
        IMPORTED_LOCATION_DEBUG "${INSTALL_DIR}/lib/coordgend.dll")
set_property(TARGET coordgen APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(coordgen PROPERTIES
        IMPORTED_IMPLIB_RELEASE "${INSTALL_DIR}/lib/coordgen.lib"
        IMPORTED_LOCATION_RELEASE "${INSTALL_DIR}/lib/coordgen.dll")