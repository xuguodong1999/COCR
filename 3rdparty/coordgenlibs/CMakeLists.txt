project(coordgen CXX)

set(CMAKE_CXX_STANDARD 17)

set(COORDGEN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${COORDGEN_SRC_DIR}/include ${COORDGEN_SRC_DIR}/include/coordgen)

aux_source_directory(${COORDGEN_SRC_DIR}/src COORDGEN_SRCS)

if (MSVC)
    list(APPEND COORDGEN_SRCS ${RES_ROOT}/3rdparty.rc)
    include_directories(${RES_ROOT})
endif (MSVC)

#set_source_files_properties(${COORDGEN_SRCS} PROPERTIES LANGUAGE CXX)

add_library(${PROJECT_NAME} SHARED ${COORDGEN_SRCS})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

# link qt core here
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)
