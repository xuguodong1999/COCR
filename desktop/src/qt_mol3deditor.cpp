#include "qt_mol3deditor.hpp"

#include "./ui_qt_mol3deditor.h"
#include "mol3d_builder.hpp"
#include "mol3dwindow.hpp"

Mol3DEditor::Mol3DEditor(QWidget *parent)
        : QWidget(parent), ui(new Ui::Mol3DEditor) {
    ui->setupUi(this);
}

Mol3DEditor::~Mol3DEditor() {
    delete ui;
}
