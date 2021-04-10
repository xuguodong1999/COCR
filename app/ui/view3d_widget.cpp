#include "view3d_widget.h"
#include "ui_view3d_widget.h"

View3DWidget::View3DWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::View3DWidget) {
    ui->setupUi(this);
}

View3DWidget::~View3DWidget() {
    delete ui;
}
