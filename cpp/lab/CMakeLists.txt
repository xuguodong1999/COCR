project(cocr-lab)

# begin openmp
find_package(OpenMP)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# end openmp
# begin vtk
find_package(Qt5 COMPONENTS
        3DAnimation 3DCore 3DExtras 3DInput 3DLogic 3DQuick 3DQuickAnimation
        3DQuickExtras 3DQuickInput 3DQuickRender 3DQuickScene2D 3DRender
        AccessibilitySupport AxBase AxContainer AxServer
        Bluetooth Bodymovin Charts Concurrent Core
        DataVisualization DBus Designer DesignerComponents DeviceDiscoverySupport
        EdidSupport EglSupport EventDispatcherSupport
        FbSupport FontDatabaseSupport
        Gamepad Gui Help LinguistTools Location
        Multimedia MultimediaQuick MultimediaWidgets
        Network NetworkAuth Nfc OpenGL OpenGLExtensions
        PacketProtocol PlatformCompositorSupport
        Positioning PositioningQuick PrintSupport Purchasing
        Qml QmlDebug QmlDevTools QmlImportScanner QmlModels QmlWorkerScript
        Quick Quick3D Quick3DAssetImport Quick3DRender Quick3DRuntimeRender
        Quick3DUtils QuickCompiler QuickControls2 QuickParticles QuickShapes
        QuickTemplates2 QuickTest QuickWidgets RemoteObjects RepParser
        Script ScriptTools Scxml Sensors SerialBus SerialPort Sql Svg
        Test TextToSpeech ThemeSupport UiPlugin UiTools VirtualKeyboard VulkanSupport
        WebChannel WebEngine WebEngineCore WebEngineWidgets WebSockets WebView
        Widgets WindowsUIAutomationSupport WinExtras Xml XmlPatterns Zlib)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(VTK REQUIRED)
# end vtk
# begin torch
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Torch REQUIRED)
# end torch
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
find_package(doctest REQUIRED)
# begin frugally-deep
find_package(RapidJSON REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(FunctionalPlus REQUIRED)
find_package(frugally-deep REQUIRED)
# end frugally-deep
find_package(Taskflow REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})
include_directories(${maeparser_DIR}/../../include)
include_directories(${coordgen_DIR}/../../include)
include_directories(${rdkit_DIR}/../../../include)
include_directories(${rdkit_DIR}/../../../include/rdkit)
include_directories(${RAPIDJSON_INCLUDE_DIRS})
include_directories(${doctest_DIR}/../../../include)
include_directories(${nlohmann_json_DIR}/../../../include)
include_directories(${FunctionalPlus_DIR}/../../../include)
include_directories(${frugally-deep_DIR}/../../../include)
include_directories(${Taskflow_INCLUDE_DIR})


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
