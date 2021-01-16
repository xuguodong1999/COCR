include_directories(/usr/include)
include_directories(/usr/include/openbabel3)
include_directories(/usr/include/rdkit)
# wsl
# include_directories(/usr/include/opencv4)

set(QT_INCLUDE_DIR ${Qt5_DIR}/../../../include)
include_directories(${QT_INCLUDE_DIR})
include_directories(${QT_INCLUDE_DIR}/QtCore)
include_directories(${QT_INCLUDE_DIR}/QtGui)
include_directories(${QT_INCLUDE_DIR}/QtQuick)
include_directories(${QT_INCLUDE_DIR}/QtWidgets)
set(Qt5_LIBS Qt5::Widgets Qt5::Quick Qt5::3DAnimation Qt5::3DCore
        Qt5::3DExtras Qt5::3DInput Qt5::3DLogic Qt5::3DRender)

# 我试图把cuda训练子工程和不带cuda的应用子工程在一个父项目里编译
# g++ only allow -fPIC while nvcc only allow -Xcompiler=-fPIC
# qt's cmake set -fPIC as Qt5::Core's property, wtf.
# https://gitlab.kitware.com/cmake/cmake/-/issues/16915#note_270754
if (TARGET Qt5::Core)
    get_property(core_options TARGET Qt5::Core PROPERTY INTERFACE_COMPILE_OPTIONS)
    string(REPLACE "-fPIC" "" new_core_options "${core_options}")
    set_property(TARGET Qt5::Core PROPERTY INTERFACE_COMPILE_OPTIONS ${new_core_options})
    set_property(TARGET Qt5::Core PROPERTY INTERFACE_POSITION_INDEPENDENT_CODE "ON")
    set(CMAKE_CXX_COMPILE_OPTIONS_PIE "-fPIC")
endif ()

set(COCR_UNIX_3RDPARTY_LIB
        ${Qt5_LIBS}
        ${OpenCV_LIBS}
        ${TORCH_LIBRARIES}
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

        -lopenbabel

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
        )
