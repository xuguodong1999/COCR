cmake_minimum_required(VERSION 3.17)

project(cocr_app)

find_package(Qt6 COMPONENTS Core Gui Quick Qml)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

set(RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res)
qt6_add_big_resources(BIG_RES_FILES ${RES_DIR}/big/big.qrc)

set(RES_FILES ${RES_DIR}/obabel/obabel.qrc
    ${RES_DIR}/img/img.qrc)

add_qt_gui_executable(${PROJECT_NAME}
    ${SRC_FILES} ${BIG_RES_FILES} ${RES_FILES})

file (GLOB_RECURSE QML_FILES_ABS *.qml *.conf)
set(QML_FILES "")
foreach(QML_FILE_ABS ${QML_FILES_ABS})
      file(RELATIVE_PATH QML_FILE
          ${CMAKE_CURRENT_SOURCE_DIR} ${QML_FILE_ABS})
      list(APPEND QML_FILES ${QML_FILE})
endforeach()

qt6_add_resources(${PROJECT_NAME} "cocr_app"
         PREFIX "/cocr_app/qml"
        FILES ${QML_FILES})


if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -MT)
    if (CMAKE_BUILD_TYPE)
        if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
            target_compile_options(${PROJECT_NAME} PRIVATE -MTd)
        endif ()
    endif (CMAKE_BUILD_TYPE)
endif (MSVC)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
    Qt::Core Qt::Gui Qt::Qml Qt::Quick
#         PRIVATE
#     ${OpenCV_LIBS}
#     ${OpenBabel_LIBS}
#     ${VTK_LIBRARIES}
#     ${Boost_LIBRARIES}
#     ${TORCH_LIBRARIES}
)
