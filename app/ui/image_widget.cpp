#include "image_widget.h"
#include "ui_image_widget.h"

ImageWidget::ImageWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ImageWidget) {
    ui->setupUi(this);
}

ImageWidget::~ImageWidget() {
    delete ui;
}
