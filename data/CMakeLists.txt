project(data)

set(CMAKE_CXX_STANDARD 17)

include_directories(../openbabel/include)

find_package(OpenCV REQUIRED)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

set(CMAKE_AUTORCC ON)

aux_source_directory(. PROJECT_SOURCES)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ../resources/obdata.qrc)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

# link opencv, patched openbabel and coordgen here
target_link_libraries(${PROJECT_NAME} PRIVATE
        -lRDKitDistGeomHelpers -lRDKitForceField -lRDKitForceFieldHelpers
        -lRDKitDepictor -lRDKitGraphMol -lRDKitRDGeneral -lRDKitSmilesParse
        # -lRDKitAlignment -lRDKitCatalogs -lRDKitChemReactions -lRDKitChemTransforms
        # -lRDKitChemicalFeatures -lRDKitDataStructs
        # -lRDKitDescriptors -lRDKitDistGeometry -lRDKitEigenSolvers -lRDKitFMCS
        # -lRDKitFileParsers -lRDKitFilterCatalog -lRDKitFingerprints
        # -lRDKitFragCatalog -lRDKitInfoTheory
        # -lRDKitMMPA -lRDKitMolAlign -lRDKitMolCatalog -lRDKitMolChemicalFeatures
        # -lRDKitMolDraw2D -lRDKitMolHash -lRDKitMolInterchange -lRDKitMolStandardize
        # -lRDKitMolTransforms -lRDKitOptimizer -lRDKitPartialCharges -lRDKitRDBoost
        # -lRDKitRDGeometryLib -lRDKitRDStreams -lRDKitRGroupDecomposition
        # -lRDKitReducedGraphs -lRDKitSLNParse -lRDKitShapeHelpers -lRDKitSimDivPickers
        # -lRDKitStructChecker -lRDKitSubgraphs -lRDKitSubstructLibrary
        # -lRDKitSubstructMatch -lRDKitTrajectory -lRDKithc

        -lcoordgen
        # -lmaeparser

        -lboost_iostreams -lboost_system
        # -lboost_atomic -lboost_chrono -lboost_container -lboost_context
        # -lboost_contract -lboost_coroutine -lboost_date_time -lboost_fiber
        # -lboost_filesystem -lboost_graph -lboost_graph_parallel
        # -lboost_locale -lboost_log -lboost_log_setup
        # -lboost_math_c99 -lboost_math_c99f -lboost_math_c99l -lboost_math_tr1
        # -lboost_math_tr1f -lboost_math_tr1l -lboost_mpi -lboost_mpi_python38
        # -lboost_numpy38 -lboost_prg_exec_monitor -lboost_program_options
        # -lboost_python38 -lboost_random -lboost_regex -lboost_serialization
        # -lboost_stacktrace_addr2line -lboost_stacktrace_backtrace
        # -lboost_stacktrace_basic -lboost_stacktrace_noop
        # -lboost_thread -lboost_timer -lboost_type_erasure
        # -lboost_unit_test_framework -lboost_wave -lboost_wserialization

        ${OpenCV_LIBS} Qt${QT_VERSION_MAJOR}::Core openbabel)
