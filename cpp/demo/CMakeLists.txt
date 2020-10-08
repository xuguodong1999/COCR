project(demo)

set(TS_FILES ${CMAKE_CURRENT_LIST_DIR}/res/demo_zh_CN.ts)
qt5_create_translation(QM_FILES
        ${CMAKE_CURRENT_LIST_DIR}/main.qml
        ${CMAKE_CURRENT_LIST_DIR}/Page1Form.ui.qml
        ${CMAKE_CURRENT_LIST_DIR}/Page2Form.ui.qml
        ${TS_FILES})

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
        "<RCC>\n<qresource prefix=\"/\">\n<file alias=\"demo_zh_CN.qm\">demo_zh_CN.qm</file>\n</qresource>\n</RCC>")

qt5_add_resources(RES_FILES
        ${CMAKE_CURRENT_LIST_DIR}/qml.qrc
        ${CMAKE_CURRENT_LIST_DIR}/res/res.qrc
        ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc)

qt5_add_big_resources(BIG_RES_FILES ${CMAKE_CURRENT_LIST_DIR}/res/big_res.qrc)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR} SRC_FILES)

set(EXECUTABLE_NAME ${PROJECT_NAME})

add_custom_target(translations ALL DEPENDS ${QM_FILES})

add_custom_target(resources ALL DEPENDS ${RES_FILES})

add_dependencies(resources translations)

if (ANDROID)
    add_library(${EXECUTABLE_NAME} SHARED ${SRC_FILES} ${RES_FILES} ${BIG_RES_FILES})
else ()
    add_executable(${EXECUTABLE_NAME} ${SRC_FILES} ${RES_FILES} ${BIG_RES_FILES})
endif ()

if (MSVC)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE -MT)
    if (CMAKE_BUILD_TYPE)
        if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
            target_compile_options(${EXECUTABLE_NAME} PRIVATE -MTd)
        endif ()
    endif (CMAKE_BUILD_TYPE)
endif (MSVC)

target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        ${OpenCV_LIBS}
        # ${Qt_LIBRARIES}
        Qt5::Core Qt5::Quick
        ${VTK_LIBRARIES}
        ${Boost_LIBRARIES}
        ${TORCH_LIBRARIES})

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES)

if (MSVC)
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE
            ${CMAKE_CURRENT_BINARY_DIR})
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG
            ${CMAKE_CURRENT_BINARY_DIR})
endif (MSVC)
