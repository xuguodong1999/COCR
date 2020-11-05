project(cocr-app)

include(${CMAKE_SOURCE_DIR}/cmake/SetupQt5.cmake)

set(TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/trans_zh_CN.ts)

qt5_create_translation(QM_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/main.qml
        ${TS_FILES})

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
        "<RCC>\n<qresource prefix=\"/\">\n<file alias=\"trans_zh_CN.qm\">trans_zh_CN.qm</file>\n</qresource>\n</RCC>")

qt5_add_resources(RES_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/qml.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/res/res.qrc
        ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc)

qt5_add_big_resources(BIG_RES_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/big_res.qrc)

set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${SRC_DIR}/config.cpp
        ${SRC_DIR}/sketchitem.cpp)

add_custom_target(app_translations ALL DEPENDS ${QM_FILES})

add_custom_target(app_resources ALL DEPENDS ${RES_FILES})

add_dependencies(app_resources app_translations)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${RES_FILES} ${BIG_RES_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Quick Qt5::Widgets)
