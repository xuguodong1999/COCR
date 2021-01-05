project(cocr-train)

set(SRC_PRO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/train_cifar.cpp)

set(SRC_LIST ${SRC_DIR}/config.cpp
        ${SRC_DIR}/cocr_types.cpp
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

        ${SRC_DIR}/openbabel_util.cpp

        ${SRC_DIR}/opencv_util.cpp
        ${SRC_DIR}/opencv_yolo.cpp

        ${SRC_DIR}/qt_util.cpp

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

add_executable(${PROJECT_NAME} ${SRC_PRO_FILES} ${SRC_LIST})

if (MSVC)
    set(LINKED_LIBS Qt5::Widgets Qt5::Quick Qt5::3DAnimation Qt5::3DCore
            Qt5::3DExtras Qt5::3DInput Qt5::3DLogic Qt5::3DRender
            opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs opencv_dnn
            openbabel coordgen ${TORCH_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LINKED_LIBS})
elseif (APPLE)
    message(FATAL "dont support train build on macOS")
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${COCR_UNIX_3RDPARTY_LIB} ${TORCH_LIBRARIES})
endif (MSVC)

