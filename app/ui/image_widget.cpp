#include "image_widget.h"
#include "ui_image_widget.h"
#include "opencv_util.hpp"
#include <QFileDialog>
#include <QDebug>
#include <QImage>
#include <QMessageBox>

ImageWidget::ImageWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ImageWidget) {
    ui->setupUi(this);
    connect(ui->ocr_btn, &QToolButton::clicked, [&]() {
        if (image) {
            emit sig_ocr_btn_clicked(*image);
        } else {
            QMessageBox::information(
                    this, tr("Empty image"),
                    tr("You have to load a valid image from your device"),
                    QMessageBox::Ok);
        }
    });
    connect(ui->load_btn, &QToolButton::clicked, this, &ImageWidget::openImage);
    connect(ui->reset_btn, &QToolButton::clicked, this, &ImageWidget::showImage);
    connect(ui->bin_btn, &QToolButton::clicked, this, &ImageWidget::binaryImage);
}

ImageWidget::~ImageWidget() {
    delete ui;
}

void ImageWidget::clearImage() {
    image = nullptr;
    ui->display_label->clear();
    emit sig_modified();
}

void ImageWidget::openImage() {
    QString fileName = QFileDialog::getOpenFileName(
            this, tr("Open Image"), QDir::homePath(),
            tr("Image Files (*.png *.jpg *.bmp *.tiff *.webp *.jpeg)"));
    qDebug() << "QFileDialog::getOpenFileName:" << fileName;
    if (!QFile(fileName).exists()) {
        return;
    }
    image = std::make_shared<QImage>();
    if (image->load(fileName)) {
        showImage();
        emit sig_modified();
    } else {
        QMessageBox::information(
                this, tr("Invalid image"),
                tr("You have to load a valid image from your device"),
                QMessageBox::Ok);
    }
}

void ImageWidget::binaryImage() {
    if (!image) { return; }
    cv::Mat cvImage = xgd::convertQImageToMat(*image);
    int w = ui->display_label->width(), h = ui->display_label->height();
    if (image->width() > w || image->height() > h) {
        ui->display_label->setPixmap(QPixmap::fromImage(xgd::binaryAlphaImage(cvImage).scaled(
                w, h, Qt::KeepAspectRatio)));
    } else {
        ui->display_label->setPixmap(QPixmap::fromImage(xgd::binaryAlphaImage(cvImage)));
    }
}

void ImageWidget::showImage() {
    if (!image) { return; }
    int w = ui->display_label->width(), h = ui->display_label->height();
    if (image->width() > w || image->height() > h) {
        ui->display_label->setPixmap(QPixmap::fromImage(image->scaled(
                w, h, Qt::KeepAspectRatio)));
    } else {
        ui->display_label->setPixmap(QPixmap::fromImage(*image));
    }
}
