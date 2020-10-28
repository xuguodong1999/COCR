project(cocr-data)

include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenCV.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupBoost.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupRDKit.cmake)

set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${SRC_DIR}/drugbank.cpp
        ${SRC_DIR}/atom.cpp
        ${SRC_DIR}/bond.cpp
        ${SRC_DIR}/mol.cpp
        ${SRC_DIR}/3rdutil.cpp
        ${SRC_DIR}/config.cpp
        ${SRC_DIR}/bonditem.cpp
        ${SRC_DIR}/couch.cpp
        ${SRC_DIR}/molitem.cpp
        ${SRC_DIR}/shapeitem.cpp
        ${SRC_DIR}/statistic.cpp
        ${SRC_DIR}/shapegroup.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OpenCV_LIBS}
        coordgen
        maeparser
        ${RDKIT_LIBRARIES}
        ${RDKit_THREAD_LIBS}
        Threads::Threads)
