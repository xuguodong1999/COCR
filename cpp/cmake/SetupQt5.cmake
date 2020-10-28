if (MSVC)
    find_package(Qt5 COMPONENTS AxBase AxContainer AxServer WinExtras)
endif (MSVC)
find_package(Qt5 COMPONENTS
        3DAnimation 3DCore 3DExtras 3DInput 3DLogic 3DQuick 3DQuickAnimation
        3DQuickExtras 3DQuickInput 3DQuickRender 3DQuickScene2D 3DRender
        AccessibilitySupport Bluetooth Bodymovin Charts Concurrent Core
        DataVisualization DBus Designer DesignerComponents DeviceDiscoverySupport
        EdidSupport EglSupport EventDispatcherSupport
        FbSupport FontDatabaseSupport Gamepad Gui Help LinguistTools Location
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
        Widgets WindowsUIAutomationSupport Xml XmlPatterns Zlib)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
