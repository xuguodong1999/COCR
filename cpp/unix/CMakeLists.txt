project(cocr-train)

if (UNIX)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenCV.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupQt5.cmake)
    set(Torch_DIR /home/xgd/install/libtorch-latest/share/cmake/Torch)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupTorch.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupUNIX.cmake)

    message('opencv found' ${OpenCV_FOUND})
    message('torch found' ${Torch_FOUND})
    message('rdkit found' ${rdkit_FOUND})
    message('rapidjson found' ${RapidJSON_FOUND})
    message('qt5 found' ${Qt5_FOUND})
    message('boost found' ${BOOST_FOUND})
    message('openmp found' ${OpenMP_FOUND})
    message('vtk found' ${VTK_FOUND})

    include_directories(/usr/include/x86_64-linux-gnu/qt5)
    include_directories(/usr/include/x86_64-linux-gnu/qt5/QtCore)
    include_directories(/usr/include/x86_64-linux-gnu/qt5/QtGui)
    include_directories(/usr/include/x86_64-linux-gnu/qt5/QtQuick)

    aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC_FILES)

    add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/train/main.cpp
            ${SRC_DIR}/config.cpp
            ${SRC_DIR}/block.cpp
            ${SRC_DIR}/activation.cpp
            ${SRC_DIR}/module.cpp
            ${SRC_DIR}/mobilenetv3.cpp
            # ${SRC_FILES}
            )

    target_link_libraries(${PROJECT_NAME} PRIVATE
            #            Qt5::Gui Qt5::Quick
            ${UNIX_LIB_ALL_IN_ONE}
            ${OpenCV_LIBS}
            ${TORCH_LIBRARIES})
endif (UNIX)
