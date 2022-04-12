project(inchi LANGUAGES C)

include(${CMAKE_SOURCE_DIR}/cmake/Utils.cmake)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src PROJECT_SOURCE)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/ixa PROJECT_SOURCE)

addLibrary(${PROJECT_NAME} "${PROJECT_SOURCE}")

target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_API_LIB)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/inchi)
