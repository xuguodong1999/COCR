# what cocr needs
find_package(Qt5 COMPONENTS Core Widgets Quick LinguistTools TextToSpeech
        3DAnimation 3DCore 3DExtras 3DInput 3DLogic 3DRender)
# full qt packages
if (MSVC)
    find_package(Qt5 COMPONENTS AxBase AxContainer AxServer WinExtras
            3DQuick 3DQuickAnimation 3DQuickExtras 3DQuickInput 3DQuickRender 3DQuickScene2D
            AccessibilitySupport Bluetooth Bodymovin Charts Concurrent
            DataVisualization DBus Designer DesignerComponents DeviceDiscoverySupport
            EdidSupport EglSupport EventDispatcherSupport
            FbSupport FontDatabaseSupport Gamepad Gui Help Location
            Multimedia MultimediaQuick MultimediaWidgets
            Network NetworkAuth Nfc OpenGL OpenGLExtensions
            PacketProtocol PlatformCompositorSupport Positioning PositioningQuick
            PrintSupport Purchasing Qml QmlDebug QmlDevTools QmlImportScanner
            QmlModels QmlWorkerScript Quick3D Quick3DAssetImport Quick3DRender
            Quick3DRuntimeRender Quick3DUtils QuickCompiler QuickControls2 QuickParticles
            QuickShapes QuickTemplates2 QuickTest QuickWidgets RemoteObjects RepParser
            Script ScriptTools Scxml Sensors SerialBus SerialPort Sql Svg
            Test ThemeSupport UiPlugin UiTools VirtualKeyboard
            WebChannel WebEngine WebEngineCore WebEngineWidgets WebSockets WebView
            WindowsUIAutomationSupport Xml XmlPatterns Zlib)
endif (MSVC)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
