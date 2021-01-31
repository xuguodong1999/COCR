#include "qt_aboutdialog.hpp"
#include "./ui_qt_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
        : QDialog(parent), ui(new Ui::AboutDialog) {
    ui->setupUi(this);
}

AboutDialog::~AboutDialog() {
    delete ui;
}
