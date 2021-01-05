project(obusage)

include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenMP.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupQt5.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenCV.cmake)

if (MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenBabel.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupCoordgen.cmake)
elseif (UNIX)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupUnix.cmake)
endif (MSVC)

set(SRC_PRO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)


add_executable(${PROJECT_NAME} ${SRC_PRO_FILES}

        ${SRC_DIR}/atom.cpp
        ${SRC_DIR}/bond.cpp
        ${SRC_DIR}/mol.cpp
        ${SRC_DIR}/mol2d.cpp
        ${SRC_DIR}/mol3d.cpp
        ${SRC_DIR}/mol3d_builder.cpp
        ${SRC_DIR}/mol_op.cpp
        ${SRC_DIR}/mol_holder.cpp
        ${SRC_DIR}/mol_graph.cpp
        ${SRC_DIR}/mol_util.cpp

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
        ${SRC_DIR}/qt_util.cpp)

if (MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt5::Widgets Qt5::Quick
            Qt5::3DAnimation Qt5::3DCore Qt5::3DExtras
            Qt5::3DInput Qt5::3DLogic Qt5::3DRender
            ${OpenBabel_LIBS}
            ${OpenCV_LIBS}
            coordgen)
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt5::Widgets Qt5::Quick
            Qt5::3DAnimation Qt5::3DCore Qt5::3DExtras
            Qt5::3DInput Qt5::3DLogic Qt5::3DRender
            ${COCR_UNIX_LIB} ${OpenCV_LIBS})
endif (MSVC)