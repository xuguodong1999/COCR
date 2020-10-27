project(cocr-app)

if (MSVC)
    set(Qt5_DIR C:/Qt/5.15.0/msvc2019_64/lib/cmake/Qt5)
endif (MSVC)

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

set(TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/trans_zh_CN.ts)

qt5_create_translation(QM_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/main.qml
        ${TS_FILES})

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
        "<RCC>\n<qresource prefix=\"/\">\n<file alias=\"trans_zh_CN.qm\">trans_zh_CN.qm</file>\n</qresource>\n</RCC>")

qt5_add_resources(RES_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/qml.qrc
        ${CMAKE_CURRENT_SOURCE_DIR}/res/res.qrc
        ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc)

qt5_add_big_resources(BIG_RES_FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/big_res.qrc)

set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
${SRC_DIR}/sketchitem.cpp)

add_custom_target(translations ALL DEPENDS ${QM_FILES})

add_custom_target(resources ALL DEPENDS ${RES_FILES})

add_dependencies(resources translations)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${RES_FILES} ${BIG_RES_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Quick)
