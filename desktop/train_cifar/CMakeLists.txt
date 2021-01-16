project(train-cifar)

set(SRC_PRO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/train_cifar.cpp)

set(SRC_LIST ${SRC_DIR}/config.cpp
#        ${SRC_DIR}/cocr_types.cpp

        ${SRC_DIR}/timer.cpp
        ${SRC_DIR}/std_util.cpp
        ${SRC_DIR}/opencv_util.cpp

        ${SRC_DIR}/torch_activation.cpp
        ${SRC_DIR}/torch_classifier.cpp
        ${SRC_DIR}/torch_module.cpp
        ${SRC_DIR}/torch_mv3_large.cpp
        ${SRC_DIR}/torch_mv3_small.cpp
        ${SRC_DIR}/torch_transform.cpp
        ${SRC_DIR}/torch_util.cpp
        ${SRC_DIR}/dataset_cifar.cpp
        )

add_executable(${PROJECT_NAME} ${SRC_PRO_FILES} ${SRC_LIST})

if (MSVC)
    set(LINKED_LIBS
#            Qt5::Widgets Qt5::Quick Qt5::3DAnimation Qt5::3DCore
#            Qt5::3DExtras Qt5::3DInput Qt5::3DLogic Qt5::3DRender
            opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs opencv_dnn
#            openbabel coordgen
            ${TORCH_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LINKED_LIBS})
elseif (APPLE)
    message(FATAL "dont support train build on macOS")
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${COCR_UNIX_3RDPARTY_LIB})
endif (MSVC)

