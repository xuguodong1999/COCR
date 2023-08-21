#include "ImageWidget.h"
#include "ui_ImageWidget.h"
#include "QtUtil.h"

#include <QFileDialog>
#include <QDebug>
#include <QtGui/QImage>
#include <QMessageBox>

ImageWidget::ImageWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ImageWidget) {
    ui->setupUi(this);
    connect(ui->ocr_btn, &QToolButton::clicked, [&]() {
        if (image) {
            Q_EMIT sig_ocr_btn_clicked(*image);
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
    Q_EMIT sig_modified();
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
        *image = image->convertToFormat(QImage::Format_Grayscale8);
        showImage();
        Q_EMIT sig_modified();
    } else {
        QMessageBox::information(
                this, tr("Invalid image"),
                tr("You have to load a valid image from your device"),
                QMessageBox::Ok);
    }
}

void ImageWidget::binaryImage() {
    if (!image) { return; }
//    cv::Mat cvImage = convertQImageToMat(*image);
//    int w = ui->display_label->width(), h = ui->display_label->height();
//    if (image->width() > w || image->height() > h) {
//        ui->display_label->setPixmap(QPixmap::fromImage(binaryAlphaImage(cvImage).scaled(
//                w * 0.7, h * 0.7, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
//    } else {
//        ui->display_label->setPixmap(QPixmap::fromImage(binaryAlphaImage(cvImage)));
//    }
}

void ImageWidget::showImage() {
    if (!image) { return; }
    int w = ui->display_label->width(), h = ui->display_label->height();
    if (image->width() > w || image->height() > h) {
        ui->display_label->setPixmap(QPixmap::fromImage(image->scaled(
                w * 0.7, h * 0.7, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    } else {
        ui->display_label->setPixmap(QPixmap::fromImage(*image));
    }
}
