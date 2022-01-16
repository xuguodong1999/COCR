set(CV_VERSION_CODE 450)
add_library(opencv_core SHARED IMPORTED)
set_property(TARGET opencv_core APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(opencv_core PROPERTIES
        IMPORTED_IMPLIB_DEBUG "${INSTALL_DIR}/lib/opencv_core${CV_VERSION_CODE}d.lib"
        IMPORTED_LOCATION_DEBUG "${INSTALL_DIR}/lib/opencv_core${CV_VERSION_CODE}d.dll")
set_property(TARGET opencv_core APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_core PROPERTIES
        IMPORTED_IMPLIB_RELEASE "${INSTALL_DIR}/lib/opencv_core${CV_VERSION_CODE}.lib"
        IMPORTED_LOCATION_RELEASE "${INSTALL_DIR}/lib/opencv_core${CV_VERSION_CODE}.dll")

add_library(opencv_dnn SHARED IMPORTED)
set_property(TARGET opencv_dnn APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(opencv_dnn PROPERTIES
        IMPORTED_IMPLIB_DEBUG "${INSTALL_DIR}/lib/opencv_dnn${CV_VERSION_CODE}d.lib"
        IMPORTED_LOCATION_DEBUG "${INSTALL_DIR}/lib/opencv_dnn${CV_VERSION_CODE}d.dll")
set_property(TARGET opencv_dnn APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_dnn PROPERTIES
        IMPORTED_IMPLIB_RELEASE "${INSTALL_DIR}/lib/opencv_dnn${CV_VERSION_CODE}.lib"
        IMPORTED_LOCATION_RELEASE "${INSTALL_DIR}/lib/opencv_dnn${CV_VERSION_CODE}.dll")

add_library(opencv_imgproc SHARED IMPORTED)
set_property(TARGET opencv_imgproc APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(opencv_imgproc PROPERTIES
        IMPORTED_IMPLIB_DEBUG "${INSTALL_DIR}/lib/opencv_imgproc${CV_VERSION_CODE}d.lib"
        IMPORTED_LOCATION_DEBUG "${INSTALL_DIR}/lib/opencv_imgproc${CV_VERSION_CODE}d.dll")
set_property(TARGET opencv_imgproc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_imgproc PROPERTIES
        IMPORTED_IMPLIB_RELEASE "${INSTALL_DIR}/lib/opencv_imgproc${CV_VERSION_CODE}.lib"
        IMPORTED_LOCATION_RELEASE "${INSTALL_DIR}/lib/opencv_imgproc${CV_VERSION_CODE}.dll")

add_library(opencv_imgcodecs SHARED IMPORTED)
set_property(TARGET opencv_imgcodecs APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(opencv_imgcodecs PROPERTIES
        IMPORTED_IMPLIB_DEBUG "${INSTALL_DIR}/lib/opencv_imgcodecs${CV_VERSION_CODE}d.lib"
        IMPORTED_LOCATION_DEBUG "${INSTALL_DIR}/lib/opencv_imgcodecs${CV_VERSION_CODE}d.dll")
set_property(TARGET opencv_imgcodecs APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_imgcodecs PROPERTIES
        IMPORTED_IMPLIB_RELEASE "${INSTALL_DIR}/lib/opencv_imgcodecs${CV_VERSION_CODE}.lib"
        IMPORTED_LOCATION_RELEASE "${INSTALL_DIR}/lib/opencv_imgcodecs${CV_VERSION_CODE}.dll")

add_library(opencv_highgui SHARED IMPORTED)
set_property(TARGET opencv_highgui APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(opencv_highgui PROPERTIES
        IMPORTED_IMPLIB_DEBUG "${INSTALL_DIR}/lib/opencv_highgui${CV_VERSION_CODE}d.lib"
        IMPORTED_LOCATION_DEBUG "${INSTALL_DIR}/lib/opencv_highgui${CV_VERSION_CODE}d.dll")
set_property(TARGET opencv_highgui APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(opencv_highgui PROPERTIES
        IMPORTED_IMPLIB_RELEASE "${INSTALL_DIR}/lib/opencv_highgui${CV_VERSION_CODE}.lib"
        IMPORTED_LOCATION_RELEASE "${INSTALL_DIR}/lib/opencv_highgui${CV_VERSION_CODE}.dll")
