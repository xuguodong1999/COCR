add_library(openbabel SHARED IMPORTED)
set_property(TARGET openbabel APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(openbabel PROPERTIES
        IMPORTED_IMPLIB_DEBUG "${INSTALL_DIR}/lib/openbabel-3d.lib"
        IMPORTED_LOCATION_DEBUG "${INSTALL_DIR}/lib/openbabel-3d.dll")
set_property(TARGET openbabel APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(openbabel PROPERTIES
        IMPORTED_IMPLIB_RELEASE "${INSTALL_DIR}/lib/openbabel-3.lib"
        IMPORTED_LOCATION_RELEASE "${INSTALL_DIR}/lib/openbabel-3.dll")