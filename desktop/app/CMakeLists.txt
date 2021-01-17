project(cocr-desktop-app)

set(TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/trans_zh_CN.ts)

qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.qml ${TS_FILES})

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
        "<RCC>\n<qresource prefix=\"/\">\n<file alias=\"trans_zh_CN.qm\">trans_zh_CN.qm</file>\n</qresource>\n</RCC>")

qt5_add_resources(RES_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/qml.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/res/res.qrc
        ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc)

qt5_add_big_resources(BIG_RES_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/big_res.qrc)

add_custom_target(app_translations ALL DEPENDS ${QM_FILES})

add_custom_target(app_resources ALL DEPENDS ${RES_FILES})

add_dependencies(app_resources app_translations)

set(SRC_PRO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main_datagen.cpp)

set(SRC_ALL ${RES_FILES} ${BIG_RES_FILES} ${SRC_PRO_FILES}
        ${UI_SRC} ${MOL_SRC} ${HW_SRC} ${BASE_SRC})

if (ALLOW_USER_TRAIN)
    list(APPEND SRC_ALL ${TORCH_SRC})
endif (ALLOW_USER_TRAIN)

add_executable(${PROJECT_NAME} ${SRC_ALL})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

if (MSVC)
    set(MSVC_LIBS Qt5::Widgets Qt5::Quick Qt5::3DAnimation Qt5::3DCore
            Qt5::3DExtras Qt5::3DInput Qt5::3DLogic Qt5::3DRender
            openbabel coordgen)
    if (ALLOW_USER_TRAIN)
        list(APPEND MSVC_LIBS ${TORCH_LIBRARIES})
        list(APPEND MSVC_LIBS ${OpenCV_LIBS})
    else (ALLOW_USER_TRAIN)
        list(APPEND MSVC_LIBS opencv_core opencv_highgui
                opencv_imgproc opencv_imgcodecs opencv_dnn)
    endif (ALLOW_USER_TRAIN)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${MSVC_LIBS})
elseif (APPLE)
    message(FATAL "TODO: support app build on macOS")
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${COCR_UNIX_3RDPARTY_LIB})
endif (MSVC)
