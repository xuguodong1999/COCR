#include "qt_mainwindow.hpp"
#include "qt_mol3deditor.hpp"
#include "qt_sketchwidget.hpp"
#include "qt_ocrthread.hpp"
#include "./ui_qt_mainwindow.h"
#include <QMovie>
#include <QThreadPool>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow),
          isSketchLatest(true), currentWidget(nullptr) {
    ui->setupUi(this);
    waitLabel = new QLabel(this);
    waitMovie = new QMovie(":/img/wait.gif", "gif",this);
    waitLabel->setMovie(waitMovie);
    waitLabel->hide();
    mainLayout = new QHBoxLayout();
    ui->container->setLayout(mainLayout);
    sketchWidget = new SketchWidget(this);
    sketchWidget->show();
    mol3DEditor = new Mol3DEditor(this);
    mol3DEditor->hide();
    ocrThread = new OCRThread(this);
    connect(ocrThread, &QThread::finished, this, &MainWindow::open3DEditor);
    connect(ui->drawBtn, &QToolButton::clicked, this,
            &MainWindow::openSketchWidget);
    connect(ui->mol3DBtn, &QToolButton::clicked, this,
            &MainWindow::tryOpen3DEditor);
    connect(ui->clsBtn, &QToolButton::clicked, [&]() {
        sketchWidget->clear();
        openSketchWidget();
        isSketchLatest = false;
        ocrThread->exit();
    });
    connect(sketchWidget, &SketchWidget::new_script, this, [&]() {
        isSketchLatest = false;
    });
    connect(ui->actionAbout,&QAction::triggered,this,
            &MainWindow::showAboutDialog);
    openSketchWidget();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openSketchWidget() {
    setMainWidget(sketchWidget);
    statusBar()->showMessage(tr("draw a molecule")+" *(^o^)*");
}

void MainWindow::tryOpen3DEditor() {
    if (isSketchLatest) {
        setMainWidget(mol3DEditor);
        if (mols.empty()) {
            statusBar()->showMessage(tr("nothing found ?"));
        } else {
            statusBar()->showMessage(tr("rendered by qt3d")+" *(^o^)*");
        }
    } else {
        ocrThread->setHwScript(sketchWidget->getScript());
        ocrThread->setQImage(sketchWidget->getQImage());
        ocrThread->start();
        setMainWidget(waitLabel);
        statusBar()->showMessage(tr("detecting..."));
        waitLabel->movie()->setScaledSize(waitLabel->size());
        waitLabel->movie()->start();
    }
}

void MainWindow::open3DEditor() {
    setMainWidget(mol3DEditor);
    mols = std::move(ocrThread->getMols());
    mol3DEditor->setMols(mols);
    isSketchLatest = true;
    if (mols.empty()) {
        statusBar()->showMessage(tr("nothing found ?"));
    } else {
        statusBar()->showMessage(tr("rendered by qt3d")+" *(^o^)*");
    }
}

void MainWindow::setMainWidget(QWidget *target) {
    if (currentWidget) {
        currentWidget->hide();
        mainLayout->removeWidget(currentWidget);
    }
    currentWidget = target;
    mainLayout->addWidget(currentWidget);
    currentWidget->show();
}

void MainWindow::showAboutDialog() {
    QString curTime=QDate::currentDate().toString("yyyy-MM-dd");
    QString qtVersionHint = tr(
      "<h3>About COCR</h3>"
      "<p>This program uses Qt version %1.</p>").arg(QLatin1String(QT_VERSION_STR));
    const QString aboutInfo = tr(
      "<p>COCR is an OCR tool for chemistry.</p>"
      "<p>It converts a image containing structural formulas into molecule graph</p>"
      "<p>It renders molecule graph as stick-and-ball model using qt3d.</p>"
      "<p>It supports export of molecule graph in formats that openbabel provides.</p>"
      "<p>Copyright (C) 2018-%1 Guodong Xu. All rights reserved.</p>"
      "<p>Development of COCR project was once supported by NANJING UNIVERSITY SOFTWARE INSTITUTE as a National College Student Innovation and Entrepreneurship Training Program.</p>").arg(curTime.mid(0,curTime.indexOf("-")));
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setWindowTitle(tr("About cocr"));
    msgBox->setText(qtVersionHint);
    msgBox->setInformativeText(aboutInfo);
    QPixmap pm(":/img/logo.ico");
    if (!pm.isNull())
        msgBox->setIconPixmap(pm);
    msgBox->exec();
}
