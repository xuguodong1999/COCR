#include "view3d_widget.h"
#include "../3d/entity_base.hpp"
#include "ui_view3d_widget.h"
#include "mol3d_widget.hpp"

View3DWidget::View3DWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::View3DWidget) {
    ui->setupUi(this);

    BaseEntity::SetView3DWidget(this);

    auto l = new QHBoxLayout();
    mol3DWidget = new Mol3DWidget(ui->container);
    l->addWidget(mol3DWidget);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    ui->container->setLayout(l);


}

View3DWidget::~View3DWidget() {
    delete ui;
}

void View3DWidget::syncMolToScene(std::shared_ptr<xgd::JMol> _mol) {
    mol3DWidget->syncMolToScene(_mol);
}

void View3DWidget::startWaitHint() {
    mol3DWidget->startWaitHint();
}

void View3DWidget::onAtomPicked(const size_t &_aid) {
    ui->pick_edit->setText(mol3DWidget->makeAtomInto(_aid));
}

void View3DWidget::onBondPicked(const size_t &_bid) {
    ui->pick_edit->setText(mol3DWidget->makeBondInto(_bid));
}

