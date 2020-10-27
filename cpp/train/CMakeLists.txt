project(cocr-train)

if (MSVC)
else(MSVC)
    set(Torch_DIR /home/xgd/install/libtorch-latest/share/cmake/Torch)
endif (MSVC)

# begin openmp
find_package(OpenMP)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# end openmp
# begin torch
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Torch REQUIRED)
# end torch
# begin boost
#set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_USE_STATIC_RUNTIME OFF)
#find_package(Boost REQUIRED COMPONENTS
#        atomic chrono container context contract coroutine
#        date_time fiber filesystem graph iostreams locale log_setup log
#        math_c99f math_c99l math_c99 math_tr1f math_tr1l math_tr1
#        nowide prg_exec_monitor program_options random regex
#        serialization stacktrace_noop stacktrace_windbg_cached stacktrace_windbg system
#        thread timer type_erasure unit_test_framework wave wserialization)
# end boost
# begin rdkit
find_package(Threads)
#find_package(maeparser REQUIRED)
#find_package(coordgen REQUIRED)
#find_package(rdkit REQUIRED)
#set(RDKIT_LIBRARIES RDKit::Abbreviations RDKit::MolCatalog RDKit::Alignment
#        RDKit::MolChemicalFeatures RDKit::Catalogs RDKit::MolDraw2D
#        RDKit::ChemicalFeatures RDKit::MolEnumerator RDKit::ChemReactions
#        RDKit::MolHash RDKit::ChemTransforms RDKit::MolInterchange
#        RDKit::CIPLabeler RDKit::MolStandardize RDKit::DataStructs
#        RDKit::MolTransforms RDKit::Depictor RDKit::O3AAlign
#        RDKit::Deprotect RDKit::Optimizer RDKit::Descriptors
#        RDKit::PartialCharges RDKit::DistGeometry RDKit::RDGeneral
#        RDKit::DistGeomHelpers RDKit::RDGeometryLib RDKit::EigenSolvers
#        RDKit::ReducedGraphs RDKit::FileParsers RDKit::RGroupDecomposition
#        RDKit::FilterCatalog RDKit::RingDecomposerLib RDKit::Fingerprints
#        RDKit::ScaffoldNetwork RDKit::FMCS RDKit::ShapeHelpers
#        RDKit::ForceField RDKit::SimDivPickers RDKit::ForceFieldHelpers
#        RDKit::SLNParse RDKit::FragCatalog RDKit::SmilesParse RDKit::GraphMol
#        RDKit::Subgraphs RDKit::hc RDKit::SubstructLibrary RDKit::InfoTheory
#        RDKit::SubstructMatch RDKit::MMPA RDKit::TautomerQuery RDKit::MolAlign
#        RDKit::Trajectory)
# end rdkit
find_package(RapidJSON REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR})
#include_directories(${Boost_INCLUDE_DIR})
#include_directories(${maeparser_DIR}/../../include)
#include_directories(${coordgen_DIR}/../../include)
#include_directories(${rdkit_DIR}/../../../include)
#include_directories(${rdkit_DIR}/../../../include/rdkit)
include_directories(${RAPIDJSON_INCLUDE_DIRS})

set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${SRC_DIR}/config.cpp
        ${SRC_DIR}/block.cpp
        ${SRC_DIR}/activation.cpp
        ${SRC_DIR}/module.cpp
        ${SRC_DIR}/mobilenetv3.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OpenCV_LIBS}
#        ${Boost_LIBRARIES}
        ${TORCH_LIBRARIES}
#        coordgen
#        maeparser
#        ${RDKIT_LIBRARIES}
#        ${RDKit_THREAD_LIBS}
        Threads::Threads)
