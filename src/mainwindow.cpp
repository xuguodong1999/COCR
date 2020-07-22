#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sketchwidget.h"
#include "switchbutton.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), currentSketchWidget(nullptr),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
#ifdef  Q_OS_ANDROID
    ui->menubar->hide();
    ui->statusbar->hide();
#endif
    mdiArea = ui->mdiArea;

    modeSwitchButton = new SwitchButton(ui->switchContainer);

    newSketchWidget();
    connect(ui->newSketchButton, &QToolButton::clicked, [&]() { newSketchWidget(); });
    connect(ui->clearSceneButton, &QToolButton::clicked, [&]() {
        if (currentSketchWidget != nullptr) {
            currentSketchWidget->clearScene();
            currentSketchWidget->update();
        }
    });
    showMaximized();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
    modeSwitchButton->resize(ui->switchContainer->size());
}

void MainWindow::newSketchWidget(const QString &docName) {
    auto sketchWidget = new SketchWidget();
    connect(modeSwitchButton, &QToolButton::clicked, sketchWidget, &SketchWidget::switchMode);
    mdiArea->addSubWindow(sketchWidget, Qt::FramelessWindowHint);
    sketchWidget->showMaximized();
    sketchWidget->switchMode(modeSwitchButton->isChecked());
    // 点击MDI子窗口的关闭图标的时候，从map中删掉关联
    connect(sketchWidget, &SketchWidget::closed, [&]() {
        sketchWidgetNameMap.remove(sketchWidget);
    });
    // 一旦画图区域得到焦点，把当前绘图区指针指向这个窗体
    connect(sketchWidget, &SketchWidget::focusStateChanged, this, [&](bool focusIn) {
        if (focusIn) {
            currentSketchWidget = qobject_cast<SketchWidget *>(sender());
        }
    });
    if (docName.isEmpty()) {
        QString tmpDocName = "新文档-" + QDateTime::currentDateTime()
                .toString(Qt::DateFormat::ISODate)
                .replace(QRegExp("\\s|/|:"), "-");
        sketchWidget->setWindowTitle(tmpDocName);
        sketchWidgetNameMap.insert(sketchWidget, tmpDocName);
    } else {
        sketchWidget->setWindowTitle(docName);
        sketchWidgetNameMap.insert(sketchWidget, docName);
    }
    currentSketchWidget = sketchWidget;

}
