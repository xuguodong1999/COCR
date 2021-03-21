#include "mol3d_widget.hpp"
#include "../3D/mol3d_window.hpp"
#include "../3D/mol3d_builder.hpp"

#include <Qt3DCore/QEntity>
#include <QHBoxLayout>
#include <QDebug>
#include <QThreadPool>


Mol3DWidget::Mol3DWidget(QWidget *parent, std::shared_ptr<xgd::JMol> _mol) : QWidget(parent), mol(std::move(_mol)) {
    root = new Qt3DCore::QEntity();
    builder = new Mol3DBuilder(this, root);
    window = new Mol3DWindow(root);
    auto layout = new QHBoxLayout();
    layout->addWidget(QWidget::createWindowContainer(window));
    layout->setMargin(2);
    setLayout(layout);
    // prepare 运行在子线程， build 运行在 UI 线程
    connect(builder, &Mol3DBuilder::on_mol_prepare_done, builder, &Mol3DBuilder::build);
    connect(builder, &Mol3DBuilder::on_mol_build_done, [&]() { update(); });
}

void Mol3DWidget::syncMolToScene() {
    qDebug() << __FUNCTION__;
    QVector3D viewSize = {200, 200, 200};
    QThreadPool::globalInstance()->start([&]() {
        builder->prepare(mol, viewSize);
    });
}
