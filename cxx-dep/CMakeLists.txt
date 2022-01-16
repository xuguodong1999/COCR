# FIXME: fail to load qml plugin, add libtorch import
cmake_minimum_required(VERSION 3.18)

project(demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_DEBUG_POSTFIX d)

add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=ON)

set(PREFIX J:/)
set(Qt5_DIR ${PREFIX}/qt5.15.1/lib/cmake/Qt5)
set(OpenCV_DIR ${PREFIX}/opencv4.5.0/x64/vc16/staticlib)
set(Boost_DIR ${PREFIX}/boost1.74.0/lib/cmake/Boost-1.74.0)
set(rdkit_DIR ${PREFIX}/rdkit2020.09.1/lib/cmake/rdkit)
set(Torch_DIR ${PREFIX}/libtorch1.7.0/share/cmake/Torch)

find_package(Qt5 COMPONENTS  Positioning 3DAnimation PositioningQuick 3DCore 
PrintSupport 3DExtras Purchasing 3DInput Qml 3DLogic QmlDebug 3DQuick QmlDevTools
 3DQuickAnimation QmlImportScanner 3DQuickExtras QmlModels 3DQuickInput QmlWorkerScript
  3DQuickRender Quick 3DQuickScene2D Quick3D 3DRender Quick3DAssetImport 
  AccessibilitySupport Quick3DRender AttributionsScannerTools Quick3DRuntimeRender 
  AxBase Quick3DUtils AxContainer QuickCompiler AxServer QuickControls2 Bluetooth
   QuickParticles Bodymovin QuickShapes QuickTemplates2 Charts QuickTest Concurrent
    QuickWidgets Core RemoteObjects DataVisualization RepParser DBus Script Designer
     ScriptTools DesignerComponents Scxml DeviceDiscoverySupport Sensors DocTools 
     SerialBus EdidSupport SerialPort EglSupport Sql EventDispatcherSupport Svg 
     FbSupport Test FontDatabaseSupport TextToSpeech Gamepad ThemeSupport Gui 
     UiPlugin LinguistTools UiTools Location VirtualKeyboard Multimedia WebChannel
      MultimediaQuick WebSockets MultimediaWidgets WebView Network Widgets 
      NetworkAuth WindowsUIAutomationSupport Nfc WinExtras OpenGL Xml 
      OpenGLExtensions XmlPatterns PacketProtocol Zlib PlatformCompositorSupport)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(OpenCV REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost REQUIRED COMPONENTS atomic math_tr1l chrono nowide container
        prg_exec_monitor context program_options contract random coroutine
        regex date_time serialization exception stacktrace_noop fiber
        stacktrace_windbg filesystem stacktrace_windbg_cached graph system
        iostreams test_exec_monitor locale thread log timer log_setup
        type_erasure math_c99 unit_test_framework math_c99f wave math_c99l
        wserialization math_tr1 zlib math_tr1f)
set(BOOST_LIBRARIES Boost::atomic Boost::math_tr1l Boost::chrono Boost::nowide
        Boost::container Boost::prg_exec_monitor Boost::context Boost::program_options
        Boost::contract Boost::random Boost::coroutine Boost::regex Boost::date_time
        Boost::serialization Boost::exception Boost::stacktrace_noop Boost::fiber
        Boost::stacktrace_windbg Boost::filesystem Boost::stacktrace_windbg_cached
        Boost::graph Boost::system Boost::iostreams Boost::test_exec_monitor
        Boost::locale Boost::thread Boost::log Boost::timer Boost::log_setup
        Boost::type_erasure Boost::math_c99 Boost::unit_test_framework
        Boost::math_c99f Boost::wave Boost::math_c99l Boost::wserialization
        Boost::math_tr1 Boost::zlib Boost::math_tr1f)

find_package(rdkit REQUIRED)
set(RDKIT_LIBRARIES RDKit::rdkit_base RDKit::RDGeneral RDKit::RDStreams
        RDKit::DataStructs RDKit::RDGeometryLib RDKit::Alignment
        RDKit::EigenSolvers RDKit::Optimizer RDKit::ForceField RDKit::DistGeometry
        RDKit::Catalogs RDKit::GraphMol RDKit::Depictor RDKit::SmilesParse
        RDKit::FileParsers RDKit::SubstructMatch RDKit::ChemReactions
        RDKit::ChemTransforms RDKit::TautomerQuery RDKit::Subgraphs
        RDKit::FilterCatalog RDKit::FragCatalog RDKit::Descriptors
        RDKit::Fingerprints RDKit::PartialCharges RDKit::MolTransforms
        RDKit::ForceFieldHelpers RDKit::DistGeomHelpers RDKit::MolAlign
        RDKit::O3AAlign RDKit::MolChemicalFeatures RDKit::ShapeHelpers
        RDKit::MolCatalog RDKit::MolDraw2D RDKit::FMCS RDKit::MolHash
        RDKit::MMPA RDKit::CIPLabeler RDKit::Deprotect RDKit::ReducedGraphs
        RDKit::Trajectory RDKit::SubstructLibrary RDKit::RGroupDecomposition
        RDKit::MolStandardize RDKit::ScaffoldNetwork RDKit::MolEnumerator
        RDKit::Abbreviations RDKit::SimDivPickers RDKit::hc RDKit::InfoTheory
        RDKit::ChemicalFeatures)

find_package(Torch REQUIRED)
include_directories(${TORCH_INCLUDE_DIRS})

set(TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/trans_zh_CN.ts)

qt5_create_translation(QM_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/main.qml
        ${TS_FILES})

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
        "<RCC>\n<qresource prefix=\"/\">\n<file alias=\"trans_zh_CN.qm\">trans_zh_CN.qm</file>\n</qresource>\n</RCC>")

qt5_add_resources(RES_FILES
        # ${CMAKE_CURRENT_SOURCE_DIR}/qml.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/demo.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/res/res.qrc
        ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc)

qt5_add_big_resources(BIG_RES_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/big_res.qrc)

# aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_FILES)

add_custom_target(app_translations ALL DEPENDS ${QM_FILES})

add_custom_target(app_resources ALL DEPENDS ${RES_FILES})

add_dependencies(app_resources app_translations)

add_executable(${PROJECT_NAME}
        # WIN32
        ${SRC_FILES}
        ${RES_FILES} 
        ${BIG_RES_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        # ${CMAKE_CURRENT_SOURCE_DIR}/torch_demo.cpp
        "demo.rc")

target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/MTd>
        $<$<OR:$<CONFIG:Release>>:/MT>)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core Qt5::Widgets Qt5::Quick Qt5::QuickControls2
        opencv_world
        ${TORCH_LIBRARIES}
        ${RDKIT_LIBRARIES}
        ${BOOST_LIBRARIES})
