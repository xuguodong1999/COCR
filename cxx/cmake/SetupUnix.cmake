include_directories(/usr/include/rdkit)
#include_directories(/usr/include/opencv4)
include_directories(/usr/include)
set(QT_INCLUDE_DIR ${Qt5_DIR}/../../../include)
include_directories(${QT_INCLUDE_DIR})
include_directories(${QT_INCLUDE_DIR}/QtCore)
include_directories(${QT_INCLUDE_DIR}/QtGui)
include_directories(${QT_INCLUDE_DIR}/QtQuick)
include_directories(${QT_INCLUDE_DIR}/QtWidgets)

set(COCR_UNIX_LIB
        # -lRDKitAlignment -lRDKitCatalogs -lRDKitChemReactions -lRDKitChemTransforms
        # -lRDKitChemicalFeatures -lRDKitDataStructs
        # -lRDKitDescriptors
        -lRDKitDistGeomHelpers
        # -lRDKitDistGeometry -lRDKitEigenSolvers -lRDKitFMCS
        # -lRDKitFileParsers -lRDKitFilterCatalog -lRDKitFingerprints
        -lRDKitForceField -lRDKitForceFieldHelpers
        # -lRDKitFragCatalog
        # -lRDKitInfoTheory
        # -lRDKitMMPA -lRDKitMolAlign -lRDKitMolCatalog -lRDKitMolChemicalFeatures
        # -lRDKitMolDraw2D -lRDKitMolHash -lRDKitMolInterchange -lRDKitMolStandardize
        # -lRDKitMolTransforms -lRDKitOptimizer -lRDKitPartialCharges -lRDKitRDBoost
        # -lRDKitRDGeometryLib -lRDKitRDStreams -lRDKitRGroupDecomposition
        # -lRDKitReducedGraphs -lRDKitSLNParse -lRDKitShapeHelpers -lRDKitSimDivPickers
        # -lRDKitStructChecker -lRDKitSubgraphs -lRDKitSubstructLibrary
        # -lRDKitSubstructMatch -lRDKitTrajectory -lRDKithc
        -lRDKitDepictor -lRDKitGraphMol -lRDKitRDGeneral -lRDKitSmilesParse
        -lcoordgen
        # -lmaeparser

        # -lboost_atomic -lboost_chrono -lboost_container -lboost_context
        # -lboost_contract
        # -lboost_coroutine -lboost_date_time -lboost_fiber
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
        -lboost_iostreams -lboost_system)