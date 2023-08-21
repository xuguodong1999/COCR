project(COCR)
find_package(QT NAMES Qt6 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core LinguistTools REQUIRED)

set(SRC_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/2D
        ${CMAKE_CURRENT_SOURCE_DIR}/Common
        # ${CMAKE_CURRENT_SOURCE_DIR}/IO
        )
if (Qt${QT_VERSION_MAJOR}3DCore_FOUND)
    list(APPEND SRC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3D)
endif ()
xgd_add_executable(
        ${PROJECT_NAME}
        SRC_DIRS
        ${SRC_DIRS}
        INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        BUNDLE_QT_GUI
)
xgd_link_qt(${PROJECT_NAME} PRIVATE Core Widgets Gui)
if (Qt${QT_VERSION_MAJOR}3DCore_FOUND)
    xgd_link_qt(${PROJECT_NAME} PRIVATE 3DCore 3DExtras)
endif ()

xgd_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        spdlog_stream
        xgd_cocr xgd_chem xgd_base xgd_math xgd_vision xgd_init_qt xgd_init_spdlog xgd_init_openbabel
)

qt_add_big_resources(COCR_BIG_RES_SRC ${CMAKE_SOURCE_DIR}/assets/COCR_big_res.qrc)
target_sources(${PROJECT_NAME} PRIVATE ${COCR_BIG_RES_SRC} ${CMAKE_SOURCE_DIR}/assets/COCR_res.qrc)

qt_add_translations(${PROJECT_NAME} TS_FILES ${CMAKE_SOURCE_DIR}/assets/COCR_zh_CN.ts)
