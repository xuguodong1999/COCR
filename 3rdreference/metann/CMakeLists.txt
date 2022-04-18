project(MetaNN LANGUAGES CXX)

set(MetaNN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "" FORCE)

if (BUILD_TESTS)
    add_subdirectory(test)
endif ()