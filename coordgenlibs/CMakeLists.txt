project(coordgen)

set(CMAKE_CXX_STANDARD 17)

set(COORDGEN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${COORDGEN_SRC_DIR}/include ${COORDGEN_SRC_DIR}/include/coordgen)

aux_source_directory(${COORDGEN_SRC_DIR}/src COORDGEN_SRCS)

add_library(${PROJECT_NAME} SHARED ${COORDGEN_SRCS})
