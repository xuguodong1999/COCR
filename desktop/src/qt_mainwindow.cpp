#include "qt_mainwindow.hpp"
#include "qt_mol2deditor.hpp"
#include "qt_mol3deditor.hpp"
#include "qt_sketchwidget.hpp"
#include "yolo.hpp"
#include "soso17_converter.hpp"
#include "./ui_qt_mainwindow.h"

extern std::shared_ptr<YoloDetector>yoloDetector;

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    sketchWidget=new SketchWidget(ui->container);
    mol2DEditor=new Mol2DEditor(ui->container);
    mol3DEditor=new Mol3DEditor(ui->container);
    connect(ui->drawBtn,&QToolButton::clicked,this,
            &MainWindow::openSketchWidget);
    connect(ui->drawBtn,&QToolButton::clicked,this,
            &MainWindow::openSketchWidget);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openSketchWidget() {

}

void MainWindow::open2DRawEditor() {

}

void MainWindow::open2DNormEditor() {

}

void MainWindow::open3DEditor() {

}


