#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sketchwidget.h"
#include "sketchwidget.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mdiArea=ui->mdiArea;
    dragButton=ui->dragButton;
    drawButton=ui->drawButton;
    auto sketchArea=new SketchWidget(mdiArea);
    mdiArea->addSubWindow(sketchArea);
    connect(dragButton,&QToolButton::clicked,sketchArea,&SketchWidget::showSketchView);
    connect(drawButton,&QToolButton::clicked,sketchArea,&SketchWidget::showScribbleArea);
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}
