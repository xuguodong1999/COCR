#include "qml_sketchitem.hpp"
#include "isomer.hpp"
#include "mol3d.hpp"
#include "std_util.hpp"
#include "mol3dwindow.hpp"

#include <QFontDatabase>
#include <QTranslator>
#include <QQmlApplicationEngine>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <deque>

const char *fontUrl = ":/simfang.subset.ttf";
const char *transUrl = ":/trans_zh_CN.qm";

inline void addFontData() {
    QFontDatabase database;
    int id = QFontDatabase::addApplicationFont(fontUrl);
    try {
        qApp->setFont(QFont(database.applicationFontFamilies(id).at(0)));
    } catch (std::exception e) {
        qWarning() << e.what();
    }
}

inline void addTranslator() {
    static QTranslator translator;
    if (translator.load(transUrl)) {
        if (!qApp->installTranslator(&translator))
            qWarning() << "fail to install " << translator.filePath();
    } else {
        qWarning() << "fail to load " << translator.filePath();
    }
}

int main(int argc, char **argv) {
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    // FIXME: qt3d high dpi error. disable HighDpiScaling before solving this bug
    // see Qt3DRender::QRenderSurfaceSelector
    // see https://codereview.qt-project.org/c/qt/qt3d/+/160557
//    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    addFontData();
    addTranslator();

//    qmlRegisterType<SketchItem>(SketchItem::uri, 1, 0,
//                                SketchItem::qmlName);
//    QQmlApplicationEngine engine(QUrl("qrc:/main.qml"));


    auto rootEntity = new Qt3DCore::QEntity();
    auto sceneBuilder = new Mol3D(rootEntity);

    auto &isomer = IsomerCounter::GetInstance();
    auto alkanes = isomer.getIsomers(
            {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13});
    size_t index = 0;
    std::unordered_map<size_t, std::shared_ptr<JMol>> molMap;
    auto view = new Mol3DWindow(rootEntity);
    QObject::connect(view, &Mol3DWindow::spaceOrEnterPressed, [&]() {
        index = (index + 1) % alkanes.size();
        if (notExist(molMap, index)) {
            auto mol = std::make_shared<JMol>();
            mol->set(alkanes[index]);
            mol->randomize();
            molMap.insert({index, mol});
        }
        sceneBuilder->resetMol(molMap[index]);
    });

    QObject::connect(view, &Mol3DWindow::forwardOrBackwardPressed, [&](bool isForward) {
        if (isForward) {
            index = (index + 1) % alkanes.size();
        } else {
            index = index != 0 ? index - 1 : alkanes.size() - 1;
        }
        if (notExist(molMap, index)) {
            auto mol = std::make_shared<JMol>();
            mol->set(alkanes[index]);
            mol->randomize();
            molMap.insert({index, mol});
        }
        sceneBuilder->resetMol(molMap[index]);
    });

    auto container = QWidget::createWindowContainer(view);
    auto panel = new QWidget();
    auto hLayout = new QHBoxLayout(panel);
    hLayout->addWidget(container, 5);
    auto hint = new QLabel("按动 ↑ ↓ ← →\nor\n按动 W S A D\nor\n按动 Space Enter\n"
                           "以继续...\n\n"
                           "滑动滚轮以缩放\n按下 Ctrl 后滑动滚轮以快速缩放\n点按并移动鼠标以旋转");
    hLayout->addWidget(hint, 1);
    panel->showMaximized();
    container->setFocus();
    view->forwardOrBackwardPressed(true);
    return app.exec();
}
