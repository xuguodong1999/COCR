#include "mol3d_widget.hpp"
#include "../3d/mol3d_window.hpp"
#include "../3d/mol3d_builder.hpp"
#include "waithint_widget.h"
#include <Qt3DCore/QEntity>
#include <QHBoxLayout>
#include <QDebug>
#include <QThreadPool>


Mol3DWidget::Mol3DWidget(QWidget *parent, std::shared_ptr<xgd::JMol> _mol) : QWidget(parent), mol(std::move(_mol)) {
    root = new Qt3DCore::QEntity();
    builder = new Mol3DBuilder(this, root);
    window = new Mol3DWindow(root);
    auto l = new QHBoxLayout();
    auto w = QWidget::createWindowContainer(window);
    l->addWidget(w);
    l->setMargin(2);
    setLayout(l);
    hintWidget = new WaitHintWidget(this);
    hintWidget->hide();
    // prepare 运行在子线程， build 运行在 UI 线程
    connect(builder, &Mol3DBuilder::sig_mol_prepare_done, builder, &Mol3DBuilder::build);
    connect(builder, &Mol3DBuilder::sig_mol_build_done, [&]() {
        hintWidget->hide();
        window->show();
    });
}

void Mol3DWidget::syncMolToScene() {
    qDebug() << __FUNCTION__;
    static QVector3D viewSize = window->getViewSize() / 1.5;
    QThreadPool::globalInstance()->start([&]() {
        QThread::msleep(500);
        builder->prepare(mol, viewSize);
    });
    window->hide();
    hintWidget->startWaitHint();
}
