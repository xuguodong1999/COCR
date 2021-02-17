project(openbabel)

set(CMAKE_CXX_STANDARD 17)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

# rapidjson, eigen3 and openbabel
set(OPENBABEL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${OPENBABEL_SRC_DIR}/include ${OPENBABEL_SRC_DIR}/include/inchi)

set(OPENBABEL_SRCS)
set(SRC_DIR_LIST src src/charges src/depict src/descriptors
    src/fingerprints src/forcefields src/math src/ops src/stereo 
    src/formats src/formats/json src/formats/libinchi)
foreach(SRC_DIR ${SRC_DIR_LIST})
    aux_source_directory(${OPENBABEL_SRC_DIR}/${SRC_DIR} OPENBABEL_SRC)
    list(APPEND OPENBABEL_SRCS ${OPENBABEL_SRC})
endforeach(SRC_DIR SRC_DIR_LIST)

add_library(${PROJECT_NAME} SHARED ${OPENBABEL_SRCS})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

# link qt core here
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)
