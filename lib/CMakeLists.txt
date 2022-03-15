project(cocr LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

include(GenerateExportHeader)
function(makeDLL TARGET)
    unset(PROJECT_SOURCE)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET} PROJECT_SOURCE)
    # include windows dll info
    if (MSVC)
        list(APPEND PROJECT_SOURCE ${DEV_ASSETS_DIR}/windows/3rd.rc)
        include_directories(${DEV_ASSETS_DIR}/windows)
    endif ()
    set(TARGET_NAME ${PROJECT_NAME}_${TARGET})
    add_library(${TARGET_NAME} ${PROJECT_SOURCE})
    set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")
    generate_export_header(${TARGET_NAME})
    target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

makeDLL("base")
