#include "qt_mainwindow.hpp"
//#include "qt_mol2deditor.hpp"
#include "qt_mol3deditor.hpp"
#include "qt_sketchwidget.hpp"
#include "qt_ocrthread.hpp"
#include "./ui_qt_mainwindow.h"

#include <QMovie>
#include <QThreadPool>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow),
        isSketchLatest(true),currentWidget(nullptr) {
    ui->setupUi(this);
    waitLabel = new QLabel(this);
    waitLabel->setMovie(new QMovie(
            ":/img/wait.gif", "gif", waitLabel));
    waitLabel->hide();

    mainLayout = new QHBoxLayout();
    ui->container->setLayout(mainLayout);

    sketchWidget = new SketchWidget(this);
    sketchWidget->show();
//    mol2DEditor = new Mol2DEditor(this);
//    mol2DEditor->hide();
    mol3DEditor = new Mol3DEditor(this);
    mol3DEditor->hide();
    ocrThread = new OCRThread(this);
    connect(ocrThread, &QThread::finished, this, &MainWindow::open3DEditor);
    connect(ui->drawBtn, &QToolButton::clicked, this,
            &MainWindow::openSketchWidget);
//    connect(ui->mol2dRawBtn, &QToolButton::clicked, this,
//            &MainWindow::tryOpen2DRawEditor);
//    connect(ui->mol2DNormBtn, &QToolButton::clicked, this,
//            &MainWindow::tryOpen2DNormEditor);
    connect(ui->mol3DBtn, &QToolButton::clicked, this,
            &MainWindow::tryOpen3DEditor);
    connect(ui->clsBtn, &QToolButton::clicked, [&](){
        sketchWidget->clear();
        openSketchWidget();
        isSketchLatest=false;
    });
    connect(sketchWidget,&SketchWidget::new_script,this,[&](){
       isSketchLatest=false;
    });
    openSketchWidget();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openSketchWidget() {
    setMainWidget(sketchWidget);
    statusBar()->showMessage("draw a molecule * >< *");
}

//void MainWindow::tryOpen2DRawEditor() {
//
//}
//
//void MainWindow::tryOpen2DNormEditor() {
//
//}

void MainWindow::tryOpen3DEditor() {
    if(isSketchLatest){
        setMainWidget(mol3DEditor);
        if(mols.empty()){
            statusBar()->showMessage("nothing found ?");
        }else{
            statusBar()->showMessage("rendered by qt3d * >< *");
        }
    }else {
        ocrThread->setHwScript(sketchWidget->getScript());
        ocrThread->start();
        setMainWidget(waitLabel);
        statusBar()->showMessage("detecting...");
        waitLabel->movie()->setScaledSize(waitLabel->size());
        waitLabel->movie()->start();
    }
}

//void MainWindow::open2DRawEditor() {
//
//}
//
//void MainWindow::open2DNormEditor() {
//
//}

void MainWindow::open3DEditor() {
    setMainWidget(mol3DEditor);
    mols = std::move(ocrThread->getMols());
//    qDebug() << "MainWindow::open3DEditor";
    mol3DEditor->setMols(mols);
    isSketchLatest=true;
    if(mols.empty()){
        statusBar()->showMessage("nothing found ?");
    }else{
        statusBar()->showMessage("rendered by qt3d * >< *");
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


