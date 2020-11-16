project(cocr-core)

include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenMP.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupQt5.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenCV.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupTorch.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupBoost.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupRDKit.cmake)


set(SRC_PRO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_executable(${PROJECT_NAME} ${SRC_PRO_FILES}

        ${SRC_DIR}/idmanager.cpp
        ${SRC_DIR}/mol.cpp
        ${SRC_DIR}/atom.cpp
        ${SRC_DIR}/bond.cpp
        ${SRC_DIR}/hw_mol.cpp

        ${SRC_DIR}/config.cpp
        ${SRC_DIR}/rdkit_util.cpp
        ${SRC_DIR}/std_util.cpp
        ${SRC_DIR}/opencv_util.cpp
        ${SRC_DIR}/torch_util.cpp
        ${SRC_DIR}/qt_util.cpp

        ${SRC_DIR}/polya.cpp
        ${SRC_DIR}/bignumber.cpp
        ${SRC_DIR}/isomer.cpp
        ${SRC_DIR}/generator.cpp
        ${SRC_DIR}/cocr_types.cpp
        ${SRC_DIR}/timer.cpp

        ${SRC_DIR}/torch_activation.cpp
        ${SRC_DIR}/torch_module.cpp
        ${SRC_DIR}/torch_mv3_large.cpp
        ${SRC_DIR}/torch_enet.cpp
        ${SRC_DIR}/torch_yolov4.cpp
        ${SRC_DIR}/torch_mv3_small.cpp
        ${SRC_DIR}/torch_classifier.cpp
        ${SRC_DIR}/torch_transform.cpp

        ${SRC_DIR}/couch_data.cpp
        ${SRC_DIR}/hw.cpp

        ${SRC_DIR}/dataset_cifar.cpp
        ${SRC_DIR}/dataset_couch.cpp

        ${SRC_DIR}/qml_sketchitem.cpp
        )

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Widgets Qt5::Quick
        ${OpenCV_LIBS}
        ${Boost_LIBRARIES}
        ${TORCH_LIBRARIES}
        ${RDKIT_LIBRARIES}
        ${RDKit_THREAD_LIBS})
