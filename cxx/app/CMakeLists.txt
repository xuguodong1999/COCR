project(cocr-app)

include(${CMAKE_SOURCE_DIR}/cmake/SetupQt5.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenCV.cmake)
if (MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupBoost.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupRDKit.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupCoordgen.cmake)
elseif (UNIX)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupUnix.cmake)
endif (MSVC)

set(TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/trans_zh_CN.ts)

qt5_create_translation(QM_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/main.qml
        ${TS_FILES})

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
        "<RCC>\n<qresource prefix=\"/\">\n<file alias=\"trans_zh_CN.qm\">trans_zh_CN.qm</file>\n</qresource>\n</RCC>")

qt5_add_resources(RES_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/qml.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/res/res.qrc
        ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc)

qt5_add_big_resources(BIG_RES_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/big_res.qrc)

set(SRC_PRO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_custom_target(app_translations ALL DEPENDS ${QM_FILES})

add_custom_target(app_resources ALL DEPENDS ${RES_FILES})

add_dependencies(app_resources app_translations)

add_executable(${PROJECT_NAME} ${RES_FILES} ${BIG_RES_FILES} ${SRC_PRO_FILES}

        ${SRC_DIR}/box2graph.cpp
        ${SRC_DIR}/mol.cpp
        ${SRC_DIR}/atom.cpp
        ${SRC_DIR}/bond.cpp
        ${SRC_DIR}/mol_hw.cpp
        ${SRC_DIR}/mol3d.cpp

        ${SRC_DIR}/config.cpp
        ${SRC_DIR}/rdkit_util.cpp
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
        ${SRC_DIR}/timer.cpp

        ${SRC_DIR}/qml_sketchitem.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

if (MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt5::Widgets Qt5::Quick
            Qt5::3DAnimation Qt5::3DCore Qt5::3DExtras
            Qt5::3DInput Qt5::3DLogic Qt5::3DRender
            ${OpenCV_LIBS} ${RDKIT_LIBRARIES} coordgen)
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt5::Widgets Qt5::Quick
            Qt5::3DAnimation Qt5::3DCore Qt5::3DExtras
            Qt5::3DInput Qt5::3DLogic Qt5::3DRender
            ${COCR_UNIX_LIB} ${OpenCV_LIBS} ${TORCH_LIBRARIES})
endif (MSVC)

