#include "view2d_widget.h"
#include "ui_view2d_widget.h"
#include "mol2d_widget.hpp"
#include <QTimer>

View2DWidget::View2DWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::View2DWidget) {
    ui->setupUi(this);
    auto l = new QHBoxLayout();
    mol2DWidget = new Mol2DWidget(ui->container);
    l->addWidget(mol2DWidget);
    ui->container->setLayout(l);
}

View2DWidget::~View2DWidget() {
    delete ui;
}

void View2DWidget::syncMolToScene(std::shared_ptr<xgd::JMol> _mol) {
    if (mol2DWidget) {
        mol2DWidget->syncMolToScene(_mol);
    }
}

void View2DWidget::startWaitHint() {
    mol2DWidget->startWaitHint();
}
