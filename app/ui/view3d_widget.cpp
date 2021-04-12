#include "view3d_widget.h"
#include "ui_view3d_widget.h"
#include "mol3d_widget.hpp"

View3DWidget::View3DWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::View3DWidget) {
    ui->setupUi(this);
    auto l = new QHBoxLayout();
    mol3DWidget = new Mol3DWidget(ui->container);
    l->addWidget(mol3DWidget);
    ui->container->setLayout(l);
}

View3DWidget::~View3DWidget() {
    delete ui;
}

void View3DWidget::syncMolToScene(std::shared_ptr<xgd::JMol> _mol) {
    if (mol3DWidget) { mol3DWidget->syncMolToScene(_mol); }
}
