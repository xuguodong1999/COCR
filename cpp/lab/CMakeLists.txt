project(cocr-lab)

include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenMP.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupQt5.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupVTK.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupOpenCV.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupTorch.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupBoost.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SetupRDKit.cmake)

find_package(doctest REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(FunctionalPlus REQUIRED)
find_package(frugally-deep REQUIRED)
find_package(Taskflow REQUIRED)

message('opencv found' ${OpenCV_FOUND})
message('torch found' ${Torch_FOUND})
message('rdkit found' ${rdkit_FOUND})
message('rapidjson found' ${RapidJSON_FOUND})
message('qt5 found' ${Qt5_FOUND})
message('boost found' ${BOOST_FOUND})
message('openmp found' ${OpenMP_FOUND})
message('vtk found' ${VTK_FOUND})

set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick
        ${VTK_LIBRARIES}
        ${OpenCV_LIBS}
        ${Boost_LIBRARIES}
        ${TORCH_LIBRARIES}
        coordgen
        maeparser
        ${RDKIT_LIBRARIES}
        ${RDKit_THREAD_LIBS}
        Threads::Threads)
