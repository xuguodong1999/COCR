project(xgd_cocr)
xgd_add_library(
        ${PROJECT_NAME}
        SRC_FILES
        ./GraphComposer.cpp
        ./ObjectDetector.cpp
        ./OCRItem.cpp
        ./OCRManager.cpp
        ./TextCorrector.cpp
        ./TextRecognizer.cpp
        INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
xgd_generate_export_header(${PROJECT_NAME} ${PROJECT_NAME} ".h")
xgd_link_libraries(${PROJECT_NAME}
        PRIVATE
        spdlog ncnn
        PUBLIC
        spdlog_stream xgd_base xgd_chem xgd_math xgd_vision xgd_handwriting)

xgd_link_qt(${PROJECT_NAME} PRIVATE Gui)