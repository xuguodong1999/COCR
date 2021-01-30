#include "qt_molfileeditor.hpp"

#include "./ui_qt_molfileeditor.h"

MolFileEditor::MolFileEditor(QWidget *parent)
        : QWidget(parent)
        , ui(new Ui::MolFileEditor)
{
    ui->setupUi(this);
}

MolFileEditor::~MolFileEditor()
{
    delete ui;
}
