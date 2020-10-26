project(cocr-data)

if (MSVC)
    set(Boost_DIR ${SHARED_LIBS_DIR}/boost/lib/cmake/Boost-1.74.0)
    set(Eigen3_DIR ${HEADERS_DIR}/eigen3/cmake)
    set(OpenCV_DIR ${SHARED_LIBS_DIR}/opencv/x64/vc16/lib)
    set(maeparser_DIR ${SHARED_LIBS_DIR}/maeparser/lib/cmake)
    set(coordgen_DIR ${SHARED_LIBS_DIR}/coordgenlibs/lib/cmake)
    set(rdkit_DIR ${SHARED_LIBS_DIR}/rdkit/lib/cmake/rdkit)
endif (MSVC)

find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
# begin boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS
        atomic chrono container context contract coroutine
        date_time fiber filesystem graph iostreams locale log_setup log
        math_c99f math_c99l math_c99 math_tr1f math_tr1l math_tr1
        nowide prg_exec_monitor program_options random regex
        serialization stacktrace_noop stacktrace_windbg_cached stacktrace_windbg system
        thread timer type_erasure unit_test_framework wave wserialization)
# end boost
# begin rdkit
find_package(Threads)
find_package(maeparser REQUIRED)
find_package(coordgen REQUIRED)
find_package(rdkit REQUIRED)
set(RDKIT_LIBRARIES RDKit::Abbreviations RDKit::MolCatalog RDKit::Alignment
        RDKit::MolChemicalFeatures RDKit::Catalogs RDKit::MolDraw2D
        RDKit::ChemicalFeatures RDKit::MolEnumerator RDKit::ChemReactions
        RDKit::MolHash RDKit::ChemTransforms RDKit::MolInterchange
        RDKit::CIPLabeler RDKit::MolStandardize RDKit::DataStructs
        RDKit::MolTransforms RDKit::Depictor RDKit::O3AAlign
        RDKit::Deprotect RDKit::Optimizer RDKit::Descriptors
        RDKit::PartialCharges RDKit::DistGeometry RDKit::RDGeneral
        RDKit::DistGeomHelpers RDKit::RDGeometryLib RDKit::EigenSolvers
        RDKit::ReducedGraphs RDKit::FileParsers RDKit::RGroupDecomposition
        RDKit::FilterCatalog RDKit::RingDecomposerLib RDKit::Fingerprints
        RDKit::ScaffoldNetwork RDKit::FMCS RDKit::ShapeHelpers
        RDKit::ForceField RDKit::SimDivPickers RDKit::ForceFieldHelpers
        RDKit::SLNParse RDKit::FragCatalog RDKit::SmilesParse RDKit::GraphMol
        RDKit::Subgraphs RDKit::hc RDKit::SubstructLibrary RDKit::InfoTheory
        RDKit::SubstructMatch RDKit::MMPA RDKit::TautomerQuery RDKit::MolAlign
        RDKit::Trajectory)
# end rdkit

include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${maeparser_DIR}/../../include)
include_directories(${coordgen_DIR}/../../include)
include_directories(${rdkit_DIR}/../../../include)
include_directories(${rdkit_DIR}/../../../include/rdkit)

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
