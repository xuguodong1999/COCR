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

set(SRC_PRO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main_demo_2020_1213.cpp)

add_custom_target(app_translations ALL DEPENDS ${QM_FILES})

add_custom_target(app_resources ALL DEPENDS ${RES_FILES})

add_dependencies(app_resources app_translations)

set(SRC_LIST ${SRC_DIR}/config.cpp ${SRC_DIR}/cocr_types.cpp
        ${SRC_DIR}/box2graph.cpp

        ${SRC_DIR}/colors.cpp
        ${SRC_DIR}/bignumber.cpp
        ${SRC_DIR}/timer.cpp
        ${SRC_DIR}/polya.cpp
        ${SRC_DIR}/isomer.cpp
        ${SRC_DIR}/std_util.cpp

        ${SRC_DIR}/darknet_data.cpp

        ${SRC_DIR}/hw_base.cpp
        ${SRC_DIR}/hw_bond.cpp
        ${SRC_DIR}/hw_data.cpp
        ${SRC_DIR}/hw_item.cpp
        ${SRC_DIR}/hw_mol.cpp
        ${SRC_DIR}/hw_script.cpp
        ${SRC_DIR}/hw_str.cpp
        ${SRC_DIR}/hw_stroke.cpp

        ${SRC_DIR}/atom.cpp
        ${SRC_DIR}/bond.cpp
        ${SRC_DIR}/mol.cpp
        ${SRC_DIR}/mol_graph.cpp
        ${SRC_DIR}/mol_holder.cpp
        ${SRC_DIR}/mol_op.cpp
        ${SRC_DIR}/mol_util.cpp
        ${SRC_DIR}/mol2d.cpp
        ${SRC_DIR}/mol3d.cpp
        ${SRC_DIR}/mol3d_builder.cpp
        ${SRC_DIR}/mol3dwindow.cpp

        ${SRC_DIR}/openbabel_util.cpp

        ${SRC_DIR}/opencv_util.cpp
        ${SRC_DIR}/opencv_yolo.cpp

        ${SRC_DIR}/qml_sketchitem.cpp
        ${SRC_DIR}/qt_util.cpp
        ${SRC_DIR}/demowidget_2020_1213.cpp
        )

if (ALLOW_USER_TRAIN)
    list(APPEND SRC_LIST
            ${SRC_DIR}/torch_activation.cpp
            ${SRC_DIR}/torch_classifier.cpp
            ${SRC_DIR}/torch_crnn.cpp
            ${SRC_DIR}/torch_enet.cpp
            ${SRC_DIR}/torch_module.cpp
            ${SRC_DIR}/torch_mv3_large.cpp
            ${SRC_DIR}/torch_mv3_small.cpp
            ${SRC_DIR}/torch_transform.cpp
            ${SRC_DIR}/torch_util.cpp
            ${SRC_DIR}/torch_yolov4.cpp
            ${SRC_DIR}/dataset_cifar.cpp
            ${SRC_DIR}/dataset_couch.cpp
            )
endif (ALLOW_USER_TRAIN)

add_executable(${PROJECT_NAME} ${RES_FILES} ${BIG_RES_FILES} ${SRC_PRO_FILES} ${SRC_LIST})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

if (MSVC)
    set(LINKED_LIBS Qt5::Widgets Qt5::Quick Qt5::3DAnimation Qt5::3DCore
            Qt5::3DExtras Qt5::3DInput Qt5::3DLogic Qt5::3DRender
            openbabel coordgen)

    if (ALLOW_USER_TRAIN)
        list(APPEND LINKED_LIBS ${TORCH_LIBRARIES})
        list(APPEND LINKED_LIBS ${OpenCV_LIBS})
    else(ALLOW_USER_TRAIN)
        list(APPEND LINKED_LIBS opencv_core opencv_highgui 
            opencv_imgproc opencv_imgcodecs opencv_dnn)
    endif (ALLOW_USER_TRAIN)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LINKED_LIBS})
elseif (APPLE)
    message(FATAL "TODO: support app build on macOS")
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${COCR_UNIX_3RDPARTY_LIB})
endif (MSVC)

