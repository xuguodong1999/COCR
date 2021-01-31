#include "qt_mol3deditor.hpp"

#include "./ui_qt_mol3deditor.h"
#include "mol3d_builder.hpp"
#include "mol3dwindow.hpp"

Mol3DEditor::Mol3DEditor(QWidget *parent)
        : QWidget(parent), ui(new Ui::Mol3DEditor) {
    ui->setupUi(this);
    layout = new QHBoxLayout(ui->container);
    ui->container->setLayout(layout);
}

Mol3DEditor::~Mol3DEditor() {
    delete ui;
}

std::string pdb_str, sdf_str, xyz_str, smi_str;

void Mol3DEditor::setMols(const std::vector<std::shared_ptr<JMol>> &_mols) {
    for (auto &widget:widgets) {
        layout->removeWidget(widget);
        widget->deleteLater();
        widget = nullptr;
    }
    widgets.clear();
    for (auto &mol:_mols) {
        auto rootEntity = new Qt3DCore::QEntity();
        auto molBuilder = new Mol3DBuilder(rootEntity, mol);
        try {
            if (!molBuilder->build())continue;
        } catch (std::exception &e) {
            qDebug() << e.what();
            continue;
        }
        auto view = new Mol3DWindow(rootEntity);
        auto container = QWidget::createWindowContainer(view);
        widgets.push_back(container);
        layout->addWidget(container);
    }
    std::cout << pdb_str << "*****\n" << smi_str << "*****\n" << xyz_str << "*****\n" << sdf_str << std::endl;
}
