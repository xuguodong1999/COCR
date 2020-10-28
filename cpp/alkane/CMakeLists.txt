project(cocr-alkane)

include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenMP.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenCV.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupBoost.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupRDKit.cmake)

set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${SRC_DIR}/atom.cpp
        ${SRC_DIR}/bond.cpp
        ${SRC_DIR}/mol.cpp
        ${SRC_DIR}/config.cpp
        ${SRC_DIR}/isomer.cpp
        ${SRC_DIR}/generator.cpp
        ${SRC_DIR}/bignumber.cpp
        ${SRC_DIR}/polya.cpp
        ${SRC_DIR}/3rdutil.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${Boost_LIBRARIES}
        ${OpenCV_LIBS}
        coordgen
        maeparser
        ${RDKIT_LIBRARIES}
        ${RDKit_THREAD_LIBS}
        Threads::Threads)
