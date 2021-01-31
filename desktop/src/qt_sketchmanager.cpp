#include "qt_sketchmanager.hpp"

#include "./ui_qt_sketchmanager.h"

SketchManager::SketchManager(QWidget *parent)
        : QWidget(parent), ui(new Ui::SketchManager) {
    ui->setupUi(this);
}

SketchManager::~SketchManager() {
    delete ui;
}
