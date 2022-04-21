#include <QGuiApplication>
#include <QQmlApplicationEngine>


int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral(QML_PREFIX));
    const QUrl url(QStringLiteral(QML_PREFIX "main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl)
                    QGuiApplication::exit(EXIT_FAILURE);
            }, Qt::QueuedConnection);
    engine.load(url);

    return QGuiApplication::exec();
}
