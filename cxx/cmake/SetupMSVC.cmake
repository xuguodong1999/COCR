set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

set(HEADERS_DIR C:/include)
set(SHARED_LIBS_DIR C:/shared)
set(OpenBabel_DIR ${SHARED_LIBS_DIR}/openbabel3.1.1/bin/cmake/openbabel3)

# set(doctest_DIR ${HEADERS_DIR}/doctest/lib/cmake/doctest)
set(Eigen3_DIR ${HEADERS_DIR}/eigen3/cmake)
# set(nlohmann_json_DIR ${HEADERS_DIR}/nlohmann_json/lib/cmake/nlohmann_json)
# set(FunctionalPlus_DIR ${HEADERS_DIR}/FunctionalPlus/lib/cmake/FunctionalPlus)
# set(frugally-deep_DIR ${HEADERS_DIR}/frugally-deep/lib/cmake/frugally-deep)
# set(RapidJSON_DIR ${HEADERS_DIR}/rapidjson/cmake)
# set(Taskflow_DIR ${HEADERS_DIR}/taskflow/lib/cmake/Taskflow)

set(Qt5_DIR ${SHARED_LIBS_DIR}/qt5.15.2/lib/cmake/Qt5)
# set(VTK_DIR ${SHARED_LIBS_DIR}/vtk/lib/cmake/vtk-8.2)
set(OpenCV_DIR ${SHARED_LIBS_DIR}/opencv4.5.0/x64/vc16/lib)
set(Boost_DIR ${SHARED_LIBS_DIR}/boost1.74.0/lib/cmake/Boost-1.74.0)
set(Torch_DIR ${SHARED_LIBS_DIR}/libtorch1.7.0/share/cmake/Torch)

# set(maeparser_DIR ${SHARED_LIBS_DIR}/maeparser/lib/cmake)
set(coordgen_DIR ${SHARED_LIBS_DIR}/coordgenlibs/lib/cmake)
set(rdkit_DIR ${SHARED_LIBS_DIR}/rdkit/lib/cmake/rdkit)

# include_directories(${EIGEN3_INCLUDE_DIR})

include_directories(${Boost_INCLUDE_DIR})

# include_directories(${maeparser_DIR}/../../include)
include_directories(${coordgen_DIR}/../../include)
include_directories(${rdkit_DIR}/../../../include)
include_directories(${rdkit_DIR}/../../../include/rdkit)

# include_directories(${RAPIDJSON_INCLUDE_DIRS})
# include_directories(${doctest_DIR}/../../../include)
# include_directories(${nlohmann_json_DIR}/../../../include)
# include_directories(${FunctionalPlus_DIR}/../../../include)
# include_directories(${frugally-deep_DIR}/../../../include)
# include_directories(${Taskflow_INCLUDE_DIR})