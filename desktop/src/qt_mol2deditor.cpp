#include "qt_mol2deditor.hpp"

#include "./ui_qt_mol2deditor.h"

Mol2DEditor::Mol2DEditor(QWidget *parent)
        : QWidget(parent), ui(new Ui::Mol2DEditor) {
    ui->setupUi(this);
}

Mol2DEditor::~Mol2DEditor() {
    delete ui;
}
