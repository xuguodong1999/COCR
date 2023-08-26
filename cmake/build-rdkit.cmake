# rdkit
set(RD_ROOT_DIR ${XGD_THIRD_PARTY_DIR}/rdkit-src/rdkit)
set(RD_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/rdkit/include/rdkit)
# rdkit
function(xgd_link_rdkit_internal TARGET)
    cmake_parse_arguments(param "" "" "PRIVATE;PUBLIC" ${ARGN})
    if ((NOT param_PRIVATE) AND (NOT param_PUBLIC))
        message(FATAL "xgd_link_rdkit_internal: no components given")
    endif ()
    foreach (RDKIT_COMPONENT ${param_PRIVATE})
        # string(TOLOWER ${RDKIT_COMPONENT} RDKIT_COMPONENT_TARGET)
        set(RDKIT_COMPONENT_TARGET ${RDKIT_COMPONENT})
        set(RDKIT_COMPONENT_TARGET rdkit_${RDKIT_COMPONENT_TARGET})
        add_dependencies(${TARGET} ${RDKIT_COMPONENT_TARGET})
        target_link_libraries(${TARGET} PRIVATE ${RDKIT_COMPONENT_TARGET})
    endforeach ()
    foreach (RDKIT_COMPONENT ${param_PUBLIC})
        # string(TOLOWER ${RDKIT_COMPONENT} RDKIT_COMPONENT_TARGET)
        set(RDKIT_COMPONENT_TARGET ${RDKIT_COMPONENT})
        set(RDKIT_COMPONENT_TARGET rdkit_${RDKIT_COMPONENT_TARGET})
        add_dependencies(${TARGET} ${RDKIT_COMPONENT_TARGET})
        target_link_libraries(${TARGET} PUBLIC ${RDKIT_COMPONENT_TARGET})
    endforeach ()
endfunction()

function(xgd_build_rdkit_internal RDKIT_COMPONENT)
    cmake_parse_arguments(param "" "" "SRC_DIRS;EXCLUDE_SRC_FILES;SRC_FILES" ${ARGN})
    # string(TOLOWER ${RDKIT_COMPONENT} RDKIT_COMPONENT_TARGET)
    set(RDKIT_COMPONENT_TARGET ${RDKIT_COMPONENT})
    set(RDKIT_COMPONENT_TARGET rdkit_${RDKIT_COMPONENT_TARGET})
    xgd_add_library(
            ${RDKIT_COMPONENT_TARGET}
            SRC_DIRS
            ${param_SRC_DIRS}

            PRIVATE_INCLUDE_DIRS

            INCLUDE_DIRS
            ${RD_ROOT_DIR}/External
            ${RD_ROOT_DIR}/Code
            ${RD_ROOT_DIR}/Contrib
            ${RD_GEN_DIR}

            SRC_FILES
            ${param_SRC_FILES}

            EXCLUDE_SRC_FILES
            ${param_EXCLUDE_SRC_FILES}

            EXCLUDE_REGEXES
            "^(.*)test(.*)\\.cpp"
            "^(.*)Test(.*)\\.cpp"
            "^(.*)catch(.*)\\.cpp"
            "^(.*)bench(.*)\\.cpp"
            "^(.*)cmd(.*)\\.cpp"
            "^(.*)main(.*)\\.cpp"
            "^(.*)CLI(.*)\\.cpp"
            "^(.*)demo(.*)\\.cpp"
            "^(.*)Example(.*)\\.cpp"
    )
    xgd_generate_export_header_modules(${RDKIT_COMPONENT_TARGET} "rdkit" "${RDKIT_COMPONENT}" ".hpp")
    xgd_link_libraries(${RDKIT_COMPONENT_TARGET}
            PRIVATE boost_serialization
            PUBLIC boost_graph boost_property_map)
    if (NOT TARGET rdkit_all)
        add_custom_target(rdkit_all)
    endif ()
    add_dependencies(rdkit_all ${RDKIT_COMPONENT_TARGET})
endfunction()

function(xgd_build_rdkit_hc)
    set(RD_SRC_DIR ${RD_ROOT_DIR}/Code)
    xgd_add_library(
            rdkit_hc STATIC
            SRC_FILES
            ${RD_SRC_DIR}/ML/Cluster/Murtagh/hc.c
            ${RD_SRC_DIR}/ML/Cluster/Murtagh/hcdriver.c
    )
    xgd_disable_warnings(rdkit_hc)
endfunction()
function(xgd_build_rdkit_RDGeneral)
    set(RDK_USE_BOOST_SERIALIZATION ON)
    set(RDK_USE_BOOST_IOSTREAMS ON)
    set(RDK_USE_BOOST_STACKTRACE ON)
    set(RDK_OPTIMIZE_POPCNT ON)
    set(RDK_BUILD_THREADSAFE_SSS ON)
    set(RDK_TEST_MULTITHREADED ON)
    set(RDK_USE_STRICT_ROTOR_DEFINITI1 ON)
    set(RDK_BUILD_DESCRIPTORS3D ON)
    set(RDK_BUILD_COORDGEN_SUPPORT ON)
    set(RDK_BUILD_MAEPARSER_SUPPORT ON)
    set(RDK_BUILD_AVALON_SUPPORT ON)
    set(RDK_BUILD_INCHI_SUPPORT ON)
    set(RDK_BUILD_SLN_SUPPORT ON)
    set(RDK_BUILD_CAIRO_SUPPORT ON)
    set(RDK_BUILD_FREETYPE_SUPPORT ON)
    set(RDK_USE_URF ON)
    set(RDK_BUILD_YAEHMOP_SUPPORT ON)
    set(RDK_BUILD_XYZ2MOL_SUPPORT ON)
    set(RDGENERAL_GEN_INC_DIR ${RD_GEN_DIR}/RDGeneral)
    set(RD_SRC_DIR ${RD_ROOT_DIR}/Code)
    configure_file(
            ${RD_SRC_DIR}/RDGeneral/RDConfig.h.cmake
            ${RDGENERAL_GEN_INC_DIR}/RDConfig.h
    )
    configure_file(
            ${RD_SRC_DIR}/RDGeneral/versions.h.cmake
            ${RDGENERAL_GEN_INC_DIR}/versions.h
    )
    set(RD_VERSION_SRC ${CMAKE_CURRENT_BINARY_DIR}/generated/rdkit/src/RDGeneral/versions.cpp)
    configure_file(
            ${RD_SRC_DIR}/RDGeneral/versions.cpp.cmake
            ${RD_VERSION_SRC}
    )
    xgd_build_rdkit_internal(RDGeneral SRC_DIRS ${RD_ROOT_DIR}/Code/RDGeneral)
    target_sources(rdkit_RDGeneral PRIVATE ${RD_VERSION_SRC})
    target_include_directories(rdkit_RDGeneral PRIVATE ${RDGENERAL_GEN_INC_DIR})
endfunction()
xgd_build_rdkit_RDGeneral()

xgd_build_rdkit_internal(RDStreams SRC_DIRS ${RD_ROOT_DIR}/Code/RDStreams)
xgd_build_rdkit_internal(AvalonLib SRC_DIRS ${RD_ROOT_DIR}/External/AvalonTools)

xgd_build_rdkit_internal(Catalogs SRC_DIRS ${RD_ROOT_DIR}/Code/Catalogs)
xgd_build_rdkit_internal(ChemicalFeatures SRC_DIRS ${RD_ROOT_DIR}/Code/ChemicalFeatures)
xgd_build_rdkit_internal(DataStructs SRC_DIRS ${RD_ROOT_DIR}/Code/DataStructs)
xgd_build_rdkit_internal(Features SRC_DIRS ${RD_ROOT_DIR}/Code/Features)
xgd_build_rdkit_internal(
        FreeSASALib
        SRC_DIRS
        ${RD_ROOT_DIR}/External/FreeSASA
        EXCLUDE_SRC_FILES
        ${RD_ROOT_DIR}/External/FreeSASA/getline.c
        ${RD_ROOT_DIR}/External/FreeSASA/freesasa2.c
)
xgd_build_rdkit_internal(RDGeometryLib SRC_DIRS ${RD_ROOT_DIR}/Code/Geometry)

xgd_build_rdkit_internal(Alignment SRC_DIRS ${RD_ROOT_DIR}/Code/Numerics/Alignment)
xgd_build_rdkit_internal(EigenSolvers SRC_DIRS ${RD_ROOT_DIR}/Code/Numerics/EigenSolvers)
xgd_build_rdkit_internal(Optimizer SRC_DIRS ${RD_ROOT_DIR}/Code/Numerics/Optimizer)

xgd_build_rdkit_internal(InfoTheory SRC_DIRS ${RD_ROOT_DIR}/Code/ML/InfoTheory)

xgd_build_rdkit_internal(
        ForceField
        SRC_DIRS
        ${RD_ROOT_DIR}/Code/ForceField
        ${RD_ROOT_DIR}/Code/ForceField/MMFF
        ${RD_ROOT_DIR}/Code/ForceField/UFF
)

xgd_build_rdkit_internal(DistGeometry SRC_DIRS ${RD_ROOT_DIR}/Code/DistGeom)
xgd_build_rdkit_internal(ConformerParser SRC_DIRS ${RD_ROOT_DIR}/Contrib/ConformerParser)
xgd_build_rdkit_internal(EHTLIB SRC_DIRS ${RD_ROOT_DIR}/External/YAeHMOP)
xgd_build_rdkit_internal(RDInchiLib SRC_DIRS ${RD_ROOT_DIR}/External/INCHI-API)
xgd_build_rdkit_internal(PBF SRC_DIRS ${RD_ROOT_DIR}/Contrib/PBF)
xgd_build_rdkit_hc()
xgd_build_rdkit_internal(SimDivPickers SRC_DIRS ${RD_ROOT_DIR}/Code/SimDivPickers)
xgd_build_rdkit_internal(ga SRC_DIRS ${RD_ROOT_DIR}/External/GA/ga ${RD_ROOT_DIR}/External/GA/util)

xgd_build_rdkit_internal(
        GraphMol
        SRC_DIRS
        ${RD_ROOT_DIR}/Code/GraphMol
        ${RD_ROOT_DIR}/Code/GraphMol/Basement/FeatTrees
)
set(RDKIT_GRAPH_MOL_SUBMODULES
        Abbreviations
        ChemTransforms
        Deprotect
        DetermineBonds
        DistGeomHelpers
        FileParsers
        FilterCatalog
        Fingerprints
        FMCS
        FragCatalog
        GenericGroups
        MMPA
        MolCatalog
        MolChemicalFeatures
        MolEnumerator
        MolHash
        MolInterchange
        MolTransforms
        PartialCharges
        ReducedGraphs
        RGroupDecomposition
        ScaffoldNetwork
        ShapeHelpers
        StructChecker
        Subgraphs
        SubstructLibrary
        TautomerQuery
        Trajectory)
foreach (RDKIT_GRAPH_MOL_SUBMODULE ${RDKIT_GRAPH_MOL_SUBMODULES})
    xgd_build_rdkit_internal(
            ${RDKIT_GRAPH_MOL_SUBMODULE}
            SRC_DIRS
            ${RD_ROOT_DIR}/Code/GraphMol/${RDKIT_GRAPH_MOL_SUBMODULE}
    )
endforeach ()
xgd_build_rdkit_internal(
        Descriptors
        SRC_DIRS
        ${RD_ROOT_DIR}/Code/GraphMol/Descriptors
        EXCLUDE_SRC_FILES
        ${RD_ROOT_DIR}/Code/GraphMol/Descriptors/datas.cpp
)
xgd_build_rdkit_internal(
        SubstructMatch
        SRC_DIRS
        ${RD_ROOT_DIR}/Code/GraphMol/Substruct
)
xgd_build_rdkit_internal(
        MolAlign
        SRC_FILES
        ${RD_ROOT_DIR}/Code/GraphMol/MolAlign/AlignMolecules.cpp
)
xgd_build_rdkit_internal(
        O3AAlign
        SRC_FILES
        ${RD_ROOT_DIR}/Code/GraphMol/MolAlign/O3AAlignMolecules.cpp
)
function(xgd_build_rdkit_SLNParse)
    xgd_build_rdkit_internal(
            SLNParse
            SRC_DIRS
            ${RD_ROOT_DIR}/Code/GraphMol/SLNParse
    )
    set(CONFIGURE_FILES
            lex.yysln.cpp
            sln.tab.cpp
            sln.tab.hpp)
    foreach (CONFIGURE_FILE ${CONFIGURE_FILES})
        set(GEN_FILE ${RD_GEN_DIR}/SLNParse/${CONFIGURE_FILE})
        xgd_configure_file_copy_only(
                rdkit_SLNParse
                ${RD_ROOT_DIR}/Code/GraphMol/SLNParse/${CONFIGURE_FILE}.cmake
                ${GEN_FILE}
        )
    endforeach ()
    target_include_directories(
            rdkit_SLNParse
            PRIVATE ${RD_GEN_DIR}/SLNParse
    )
    target_compile_definitions(rdkit_SLNParse PRIVATE YY_NO_UNISTD_H)
endfunction()
xgd_build_rdkit_SLNParse()

function(xgd_build_rdkit_SmilesParse)
    xgd_build_rdkit_internal(
            SmilesParse
            SRC_DIRS
            ${RD_ROOT_DIR}/Code/GraphMol/SmilesParse
    )
    set(CONFIGURE_FILES
            lex.yysmarts.cpp
            lex.yysmiles.cpp
            smarts.tab.cpp
            smarts.tab.hpp
            smiles.tab.cpp
            smiles.tab.hpp)
    foreach (CONFIGURE_FILE ${CONFIGURE_FILES})
        set(GEN_FILE ${RD_GEN_DIR}/SmilesParse/${CONFIGURE_FILE})
        xgd_configure_file_copy_only(
                rdkit_SmilesParse
                ${RD_ROOT_DIR}/Code/GraphMol/SmilesParse/${CONFIGURE_FILE}.cmake
                ${GEN_FILE}
        )
    endforeach ()
    target_include_directories(
            rdkit_SmilesParse
            PRIVATE ${RD_GEN_DIR}/SmilesParse
    )
    target_compile_definitions(rdkit_SmilesParse PRIVATE YY_NO_UNISTD_H)
endfunction()
xgd_build_rdkit_SmilesParse()

xgd_build_rdkit_internal(
        MolDraw2D
        SRC_DIRS
        ${RD_ROOT_DIR}/Code/GraphMol/MolDraw2D
)
add_dependencies(rdkit_MolDraw2D freetype)
target_link_libraries(rdkit_MolDraw2D PRIVATE freetype)
add_dependencies(rdkit_MolDraw2D cairo)
target_link_libraries(rdkit_MolDraw2D PRIVATE cairo)

if (XGD_ENABLE_QT)
    message(STATUS "rdkit: enable MolDraw2D Qt")
    xgd_build_rdkit_internal(
            MolDraw2DQt
            SRC_DIRS
            ${RD_ROOT_DIR}/Code/GraphMol/MolDraw2D/Qt
            EXCLUDE_SRC_FILES
            ${RD_ROOT_DIR}/Code/GraphMol/MolDraw2D/Qt/DrawTextFTQt.cpp
    )
    set(RDK_QT_VERSION ${Qt${QT_VERSION_MAJOR}Core_VERSION})
    target_compile_definitions(rdkit_MolDraw2DQt PRIVATE "-DRDK_QT_VERSION=\"${RDK_QT_VERSION}\"")
    xgd_link_qt(rdkit_MolDraw2DQt PRIVATE Gui Core)
    xgd_link_rdkit_internal(rdkit_MolDraw2DQt PUBLIC MolDraw2D GraphMol RDGeneral)
endif ()

xgd_build_rdkit_internal(
        CIPLabeler
        SRC_DIRS
        ${RD_ROOT_DIR}/Code/GraphMol/CIPLabeler
        ${RD_ROOT_DIR}/Code/GraphMol/CIPLabeler/rules
        ${RD_ROOT_DIR}/Code/GraphMol/CIPLabeler/configs
)
xgd_build_rdkit_internal(
        ForceFieldHelpers
        SRC_DIRS
        ${RD_ROOT_DIR}/Code/GraphMol/ForceFieldHelpers
        ${RD_ROOT_DIR}/Code/GraphMol/ForceFieldHelpers/MMFF
        ${RD_ROOT_DIR}/Code/GraphMol/ForceFieldHelpers/UFF
        ${RD_ROOT_DIR}/Code/GraphMol/ForceFieldHelpers/CrystalFF
)
xgd_build_rdkit_internal(
        MolStandardize
        SRC_DIRS
        ${RD_ROOT_DIR}/Code/GraphMol/MolStandardize
        ${RD_ROOT_DIR}/Code/GraphMol/MolStandardize/AcidBaseCatalog
        ${RD_ROOT_DIR}/Code/GraphMol/MolStandardize/FragmentCatalog
        ${RD_ROOT_DIR}/Code/GraphMol/MolStandardize/TautomerCatalog
        ${RD_ROOT_DIR}/Code/GraphMol/MolStandardize/TransformCatalog
)
xgd_build_rdkit_internal(
        ChemReactions
        SRC_DIRS
        ${RD_ROOT_DIR}/Code/GraphMol/ChemReactions
        ${RD_ROOT_DIR}/Code/GraphMol/ChemReactions/Enumerate
)
xgd_build_rdkit_internal(
        Depictor
        SRC_DIRS
        ${RD_ROOT_DIR}/Code/GraphMol/Depictor
        ${RD_ROOT_DIR}/Code/GraphMol/Depictor/Basement
)

xgd_link_libraries(rdkit_FreeSASALib PRIVATE freesasa)
xgd_link_libraries(rdkit_RDInchiLib PRIVATE inchi)
xgd_link_libraries(rdkit_Depictor PRIVATE coordgen)
xgd_link_libraries(rdkit_EHTLIB PRIVATE yaehmop)
xgd_link_libraries(rdkit_FileParsers PRIVATE maeparser rapidjson boost_crc)
xgd_link_libraries(rdkit_ForceField PRIVATE boost_flyweight)
xgd_link_libraries(rdkit_ChemTransforms PRIVATE boost_flyweight)
xgd_link_libraries(rdkit_Fingerprints PRIVATE boost_flyweight)
xgd_link_libraries(rdkit_PartialCharges PRIVATE boost_flyweight)
xgd_link_libraries(rdkit_MolDraw2D PRIVATE boost_assign)
xgd_link_libraries(rdkit_ReducedGraphs PRIVATE boost_flyweight)
xgd_link_libraries(rdkit_ForceFieldHelpers PRIVATE boost_flyweight)
xgd_link_libraries(rdkit_MolStandardize PRIVATE boost_flyweight)
xgd_link_libraries(rdkit_ChemReactions PRIVATE boost_multiprecision)
xgd_link_libraries(rdkit_RDGeneral PRIVATE boost_stacktrace PUBLIC boost_iostreams)
xgd_link_libraries(rdkit_RDStreams PUBLIC boost_iostreams)
xgd_link_libraries(rdkit_MolInterchange PRIVATE rapidjson)
xgd_link_libraries(rdkit_Descriptors PRIVATE eigen boost_flyweight)
xgd_link_libraries(rdkit_MolAlign PUBLIC boost_multi_array)
xgd_link_libraries(rdkit_PBF PRIVATE eigen)
xgd_link_libraries(rdkit_MolTransforms PUBLIC eigen)
xgd_link_libraries(rdkit_GraphMol PUBLIC ringdecomposerlib PRIVATE boost_multiprecision)
xgd_link_libraries(rdkit_DetermineBonds PRIVATE boost_multiprecision)
xgd_link_libraries(rdkit_AvalonLib PRIVATE avalontoolkit)

# auto generated by scripts/extract-rdkit-module-graph.ts
xgd_link_rdkit_internal(rdkit_Abbreviations PUBLIC SmilesParse SubstructMatch GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_Alignment PUBLIC RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_AvalonLib PUBLIC FileParsers SubstructMatch SmilesParse GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_Catalogs PUBLIC RDGeneral)
xgd_link_rdkit_internal(rdkit_Features PUBLIC RDGeneral)
xgd_link_rdkit_internal(rdkit_ChemicalFeatures PUBLIC RDGeometryLib)
xgd_link_rdkit_internal(rdkit_ChemReactions PUBLIC FilterCatalog Descriptors Fingerprints FileParsers ChemTransforms Depictor SubstructMatch SmilesParse GraphMol DataStructs RDGeneral)
xgd_link_rdkit_internal(rdkit_ChemTransforms PUBLIC SubstructMatch SmilesParse GraphMol RDGeometryLib RDGeneral Depictor)
xgd_link_rdkit_internal(rdkit_CIPLabeler PUBLIC GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_DataStructs PUBLIC RDGeneral)
xgd_link_rdkit_internal(rdkit_Depictor PUBLIC SmilesParse SubstructMatch MolTransforms GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_Deprotect PUBLIC ChemReactions GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_Descriptors PUBLIC PartialCharges Subgraphs MolTransforms SmilesParse SubstructMatch GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_DetermineBonds PUBLIC EHTLIB GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_DistGeometry PUBLIC EigenSolvers ForceFieldHelpers ForceField RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_DistGeomHelpers PUBLIC DistGeometry Alignment ForceFieldHelpers SubstructMatch ForceField GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_EHTLIB PUBLIC GraphMol RDGeometryLib)
xgd_link_rdkit_internal(rdkit_EigenSolvers PUBLIC RDGeneral)
xgd_link_rdkit_internal(rdkit_Optimizer PUBLIC RDGeneral)
xgd_link_rdkit_internal(rdkit_FileParsers PUBLIC Depictor ChemTransforms SmilesParse GenericGroups GraphMol RDGeometryLib RDStreams RDGeneral)
xgd_link_rdkit_internal(rdkit_FilterCatalog PUBLIC SubstructMatch SmilesParse Catalogs GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_Fingerprints PUBLIC Subgraphs SubstructMatch SmilesParse GraphMol DataStructs RDGeneral)
xgd_link_rdkit_internal(rdkit_FMCS PUBLIC SmilesParse SubstructMatch GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_ForceField PUBLIC Trajectory RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_ForceFieldHelpers PUBLIC SmilesParse SubstructMatch ForceField GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_FragCatalog PUBLIC Subgraphs SubstructMatch SmilesParse Catalogs GraphMol DataStructs RDGeneral)
xgd_link_rdkit_internal(rdkit_FreeSASALib PUBLIC GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_GenericGroups PUBLIC GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_GraphMol PUBLIC RDGeometryLib DataStructs RDGeneral)
xgd_link_rdkit_internal(rdkit_InfoTheory PUBLIC DataStructs RDGeneral)
xgd_link_rdkit_internal(rdkit_MMPA PUBLIC SubstructMatch SmilesParse GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_MolAlign PUBLIC MolTransforms SmilesParse SubstructMatch Alignment GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_MolCatalog PUBLIC GraphMol Catalogs RDGeneral)
xgd_link_rdkit_internal(rdkit_MolChemicalFeatures PUBLIC SubstructMatch SmilesParse GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_MolDraw2D PUBLIC ChemReactions FileParsers Depictor MolTransforms SmilesParse GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_MolEnumerator PUBLIC ChemReactions FileParsers GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_MolHash PUBLIC SmilesParse GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_MolInterchange PUBLIC GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_MolStandardize PUBLIC ChemReactions Descriptors Catalogs ChemTransforms SmilesParse SubstructMatch GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_MolTransforms PUBLIC GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_O3AAlign PUBLIC MolAlign MolTransforms ForceFieldHelpers GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_PartialCharges PUBLIC GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_RDGeometryLib PUBLIC DataStructs RDGeneral)
xgd_link_rdkit_internal(rdkit_RDInchiLib PUBLIC SubstructMatch SmilesParse GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_ReducedGraphs PUBLIC SubstructMatch SmilesParse GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_RGroupDecomposition PUBLIC FMCS Fingerprints ga ChemTransforms SubstructMatch SmilesParse GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_ScaffoldNetwork PUBLIC MolStandardize ChemReactions ChemTransforms SmilesParse GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_ShapeHelpers PUBLIC MolTransforms GraphMol RDGeometryLib DataStructs)
xgd_link_rdkit_internal(rdkit_SimDivPickers PUBLIC GraphMol RDGeneral hc)
xgd_link_rdkit_internal(rdkit_SLNParse PUBLIC GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_SmilesParse PUBLIC GraphMol RDGeometryLib RDGeneral)
xgd_link_rdkit_internal(rdkit_Subgraphs PUBLIC GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_SubstructLibrary PUBLIC TautomerQuery Fingerprints SubstructMatch SmilesParse GraphMol DataStructs RDGeneral)
xgd_link_rdkit_internal(rdkit_SubstructMatch PUBLIC GenericGroups GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_TautomerQuery PUBLIC MolStandardize Fingerprints SubstructMatch GraphMol RDGeneral)

# failed to autogen targets
xgd_link_rdkit_internal(rdkit_StructChecker PUBLIC Depictor SubstructMatch FileParsers ChemTransforms Descriptors GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_ConformerParser PUBLIC GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_Trajectory PUBLIC GraphMol RDGeneral)
xgd_link_rdkit_internal(rdkit_PBF PUBLIC GraphMol RDGeneral)
