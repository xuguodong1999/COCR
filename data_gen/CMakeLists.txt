project(data_gen)

set(CMAKE_CXX_STANDARD 17)

include_directories(${3RD_LIB_DIR}/openbabel/include ${3RD_LIB_DIR}/coordgenlibs/include)

find_package(OpenCV REQUIRED)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTORCC ON)

aux_source_directory(. PROJECT_SOURCES)
list(REMOVE_ITEM PROJECT_SOURCES ./main_detect.cpp ./main_text.cpp ./main_display.cpp)

if (MSVC)
    add_executable(${PROJECT_NAME} main_display.cpp ${PROJECT_SOURCES} ${RES_ROOT}/obdata.qrc)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${OpenCV_LIBS} Qt${QT_VERSION_MAJOR}::Widgets openbabel coordgen)
    target_link_options(${PROJECT_NAME} PRIVATE "/FORCE:MULTIPLE")
else ()
    add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES})
    add_executable(${PROJECT_NAME}_detect main_detect.cpp ${RES_ROOT}/obdata.qrc)
    add_executable(${PROJECT_NAME}_text main_text.cpp ${RES_ROOT}/obdata.qrc)
    add_executable(${PROJECT_NAME}_display main_display.cpp ${RES_ROOT}/obdata.qrc)

    add_dependencies(${PROJECT_NAME}_detect ${PROJECT_NAME})
    add_dependencies(${PROJECT_NAME}_text ${PROJECT_NAME})
    add_dependencies(${PROJECT_NAME}_display ${PROJECT_NAME})

    target_compile_definitions(${PROJECT_NAME} PRIVATE
            $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

    # link opencv, patched openbabel and coordgen here
    target_link_libraries(${PROJECT_NAME} PUBLIC
            ${OpenCV_LIBS} Qt${QT_VERSION_MAJOR}::Widgets openbabel coordgen)

    target_link_libraries(${PROJECT_NAME}_detect PRIVATE ${PROJECT_NAME})
    target_link_libraries(${PROJECT_NAME}_display PRIVATE ${PROJECT_NAME})
    target_link_libraries(${PROJECT_NAME}_text PRIVATE ${PROJECT_NAME})
endif (MSVC)
