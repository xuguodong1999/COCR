#include <QtWebView/QtWebView>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

inline QByteArray getContainer(int ratio = 3);

// FIXME: 桌面端 tfjs 的 fetch 请求不接受 qrc scheme
// FIXME: 安卓上 tfjs 的 CORS 请求模型失败
//#include <QWebEngineUrlRequestInterceptor>
//#include <QtWebEngineWidgets/qwebengineprofile.h>
//class MyQWebEngineUrlRequestInterceptor : public QWebEngineUrlRequestInterceptor {
//public:
//    MyQWebEngineUrlRequestInterceptor() : QWebEngineUrlRequestInterceptor(nullptr) {}
//
//    void interceptRequest(QWebEngineUrlRequestInfo &info) override {
//        auto url = info.requestUrl();
//        qDebug()<<url;
//        if (url.scheme() == "qrc") {
//            url.setScheme("file");
//            info.redirect(url);
//        }
//    }
//};

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QtWebView::initialize();
    QGuiApplication app(argc, argv);
//    QWebEngineProfile::defaultProfile()->setUrlRequestInterceptor(
//            new MyQWebEngineUrlRequestInterceptor());
    QQmlApplicationEngine engine;
    engine.loadData(getContainer());
    return engine.rootObjects().isEmpty() ? EXIT_FAILURE : QGuiApplication::exec();
}

QByteArray getContainer(int ratio) {
    QSize size;
    QPoint pos;
    if (!QGuiApplication::styleHints()->showIsFullScreen()) {
        const QRect geometry = QGuiApplication::primaryScreen()->availableGeometry();
        size = geometry.size() / ratio;
        const QSize offset = (geometry.size() - size) / 2;
        pos = geometry.topLeft() + QPoint(offset.width(), offset.height());
    }
    return QString(
            "import QtQuick 2.2\n"
            "import QtQuick.Controls 1.1\n"
            "import QtWebView 1.1\n"
            "ApplicationWindow {\n"
            "    visible: true\n"
            "    x: %1\n"
            "    y: %2\n"
            "    width: %3\n"
            "    height: %4\n"
            "    WebView {\n"
            "        anchors.fill: parent\n"
            "        url: Qt.platform.os === \"android\" ? \"file:///android_asset/index.html\": \"qrc:/index.html\"\n"
            "    }\n"
            "}").arg(pos.x()).arg(pos.y()).arg(size.width()).arg(size.height()).toLocal8Bit();
}
