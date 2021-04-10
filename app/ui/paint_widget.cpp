#include "paint_widget.h"
#include "ui_paint_widget.h"

PaintWidget::PaintWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::PaintWidget) {
    ui->setupUi(this);
}

PaintWidget::~PaintWidget() {
    delete ui;
}
