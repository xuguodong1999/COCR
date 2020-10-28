if (UNIX)
    include_directories(/usr/include/rdkit)
    include_directories(/usr/include/opencv4)
    include_directories(/usr/include)

    #    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

    set(UNIX_LIB_ALL_IN_ONE
            -lRDKitAlignment -lRDKitCatalogs -lRDKitChemReactions -lRDKitChemTransforms
            -lRDKitChemicalFeatures -lRDKitDataStructs -lRDKitDepictor -lRDKitDescriptors
            -lRDKitDistGeomHelpers -lRDKitDistGeometry -lRDKitEigenSolvers -lRDKitFMCS
            -lRDKitFileParsers -lRDKitFilterCatalog -lRDKitFingerprints -lRDKitForceField
            -lRDKitForceFieldHelpers -lRDKitFragCatalog -lRDKitGraphMol -lRDKitInfoTheory
            -lRDKitMMPA -lRDKitMolAlign -lRDKitMolCatalog -lRDKitMolChemicalFeatures
            -lRDKitMolDraw2D -lRDKitMolHash -lRDKitMolInterchange -lRDKitMolStandardize
            -lRDKitMolTransforms -lRDKitOptimizer -lRDKitPartialCharges -lRDKitRDBoost
            -lRDKitRDGeneral -lRDKitRDGeometryLib -lRDKitRDStreams -lRDKitRGroupDecomposition
            -lRDKitReducedGraphs -lRDKitSLNParse -lRDKitShapeHelpers -lRDKitSimDivPickers
            -lRDKitSmilesParse -lRDKitStructChecker -lRDKitSubgraphs -lRDKitSubstructLibrary
            -lRDKitSubstructMatch -lRDKitTrajectory -lRDKithc

            -lcoordgen -lmaeparser

            -lboost_atomic -lboost_chrono -lboost_container -lboost_context
            -lboost_contract -lboost_coroutine -lboost_date_time -lboost_fiber
            -lboost_filesystem -lboost_graph -lboost_graph_parallel
            -lboost_iostreams -lboost_locale -lboost_log -lboost_log_setup
            -lboost_math_c99 -lboost_math_c99f -lboost_math_c99l -lboost_math_tr1
            -lboost_math_tr1f -lboost_math_tr1l -lboost_mpi -lboost_mpi_python38
            -lboost_numpy38 -lboost_prg_exec_monitor -lboost_program_options
            -lboost_python38 -lboost_random -lboost_regex -lboost_serialization
            -lboost_stacktrace_addr2line -lboost_stacktrace_backtrace
            -lboost_stacktrace_basic -lboost_stacktrace_noop -lboost_system
            -lboost_thread -lboost_timer -lboost_type_erasure
            -lboost_unit_test_framework -lboost_wave -lboost_wserialization)
endif (UNIX)