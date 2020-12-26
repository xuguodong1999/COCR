project(cocr-box2graph)

include(${CMAKE_SOURCE_DIR}/cmake/SetupQt5.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenCV.cmake)
if (UNIX)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupUnix.cmake)
endif (UNIX)

set(SRC_PRO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# add_definitions(-D USE_COORDGEN2D)
# add_definitions(-D USE_OPENBABEL)
# add_definitions(-D USE_RDKIT)
add_definitions(-D __DISABLE_CPP_DEF__)

add_executable(${PROJECT_NAME} ${RES_FILES} ${BIG_RES_FILES} ${SRC_PRO_FILES}

        ${SRC_DIR}/box2graph.cpp
        ${SRC_DIR}/colors.cpp
        ${SRC_DIR}/mol.cpp
        ${SRC_DIR}/atom.cpp
        ${SRC_DIR}/bond.cpp
        ${SRC_DIR}/mol3d.cpp
        ${SRC_DIR}/mol2d.cpp
        ${SRC_DIR}/mol_rand.cpp
        ${SRC_DIR}/mol_base.cpp
        ${SRC_DIR}/mol_graph.cpp

        ${SRC_DIR}/config.cpp
        ${SRC_DIR}/std_util.cpp
        ${SRC_DIR}/opencv_util.cpp
        ${SRC_DIR}/opencv_yolo.cpp
        ${SRC_DIR}/qt_util.cpp

        ${SRC_DIR}/couch_data.cpp
        ${SRC_DIR}/hw.cpp

        ${SRC_DIR}/polya.cpp
        ${SRC_DIR}/bignumber.cpp
        ${SRC_DIR}/isomer.cpp
        ${SRC_DIR}/cocr_types.cpp
        ${SRC_DIR}/timer.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

if (MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt5::Widgets Qt5::Quick
            Qt5::3DAnimation Qt5::3DCore Qt5::3DExtras
            Qt5::3DInput Qt5::3DLogic Qt5::3DRender
            ${OpenCV_LIBS})
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt5::Widgets Qt5::Quick
            Qt5::3DAnimation Qt5::3DCore Qt5::3DExtras
            Qt5::3DInput Qt5::3DLogic Qt5::3DRender
            ${COCR_UNIX_LIB} ${OpenCV_LIBS} ${TORCH_LIBRARIES})
endif (MSVC)

