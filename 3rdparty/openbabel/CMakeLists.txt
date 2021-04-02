project(openbabel)

set(CMAKE_CXX_STANDARD 17)

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

if (MSVC)
    list(APPEND OPENBABEL_SRCS ${RES_ROOT}/3rdparty.rc)
    include_directories(${RES_ROOT})
endif (MSVC)

add_definitions(-DMAKE_OBDLL)
add_library(${PROJECT_NAME} ${OPENBABEL_SRCS})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

# link qt core here
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)
