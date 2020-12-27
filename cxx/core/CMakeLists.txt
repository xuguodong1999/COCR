project(cocr-core)

include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenMP.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupQt5.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenCV.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupTorch.cmake)

if (MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupBoost.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupRDKit.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupCoordgen.cmake)
elseif (UNIX)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupUnix.cmake)
endif (MSVC)

set(SRC_PRO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_executable(${PROJECT_NAME} ${SRC_PRO_FILES}
        ${SRC_DIR}/hw_base.cpp
        ${SRC_DIR}/hw_bond.cpp
        ${SRC_DIR}/hw_data.cpp
        ${SRC_DIR}/hw_str.cpp
        ${SRC_DIR}/hw_script.cpp
        ${SRC_DIR}/hw_item.cpp
        ${SRC_DIR}/hw_stroke.cpp
        ${SRC_DIR}/hw_mol.cpp

        ${SRC_DIR}/atom.cpp
        ${SRC_DIR}/bond.cpp
        ${SRC_DIR}/mol.cpp
        ${SRC_DIR}/mol2d.cpp
        ${SRC_DIR}/mol3d.cpp
        ${SRC_DIR}/mol3d_builder.cpp
        ${SRC_DIR}/mol_op.cpp
        ${SRC_DIR}/mol_base.cpp
        ${SRC_DIR}/mol_graph.cpp
        ${SRC_DIR}/mol_util.cpp

        ${SRC_DIR}/box2graph.cpp

        ${SRC_DIR}/config.cpp
        ${SRC_DIR}/cocr_types.cpp

        ${SRC_DIR}/colors.cpp
        ${SRC_DIR}/timer.cpp
        ${SRC_DIR}/bignumber.cpp
        ${SRC_DIR}/polya.cpp
        ${SRC_DIR}/isomer.cpp

        ${SRC_DIR}/openbabel_util.cpp
        ${SRC_DIR}/std_util.cpp
        ${SRC_DIR}/opencv_util.cpp
        ${SRC_DIR}/torch_util.cpp
        ${SRC_DIR}/qt_util.cpp

        ${SRC_DIR}/torch_activation.cpp
        ${SRC_DIR}/torch_module.cpp
        ${SRC_DIR}/torch_mv3_large.cpp
        ${SRC_DIR}/torch_enet.cpp
        ${SRC_DIR}/torch_yolov4.cpp
        ${SRC_DIR}/torch_crnn.cpp
        ${SRC_DIR}/torch_mv3_small.cpp
        ${SRC_DIR}/torch_classifier.cpp
        ${SRC_DIR}/torch_transform.cpp

#        ${SRC_DIR}/couch_data.cpp
        ${SRC_DIR}/darknet_data.cpp
#        ${SRC_DIR}/hw.cpp

        ${SRC_DIR}/dataset_cifar.cpp
#        ${SRC_DIR}/dataset_couch.cpp

        ${SRC_DIR}/qml_sketchitem.cpp)
if (MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt5::Widgets Qt5::Quick
            #        ${Boost_LIBRARIES}
            #        ${RDKit_THREAD_LIBS}
            ${OpenCV_LIBS} ${TORCH_LIBRARIES} ${RDKIT_LIBRARIES}
            coordgen)
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt5::Widgets Qt5::Quick
            Qt5::3DAnimation Qt5::3DCore Qt5::3DExtras
            Qt5::3DInput Qt5::3DLogic Qt5::3DRender
            ${COCR_UNIX_LIB} ${OpenCV_LIBS} ${TORCH_LIBRARIES})
endif (MSVC)