#include "image_widget.h"
#include "ui_image_widget.h"
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
    connect(ui->load_btn, &QToolButton::clicked, [&]() {
        QString fileName = QFileDialog::getOpenFileName(
                this, tr("Open Image"), QDir::homePath(),
                tr("Image Files (*.png *.jpg *.bmp *.tiff *.webp *.jpeg)"));
        qDebug() << "QFileDialog::getOpenFileName:" << fileName;
        if (!QFile(fileName).exists()) {
            return;
        }
        image = std::make_shared<QImage>();
        if (image->load(fileName)) {
            int w = ui->display_label->width(), h = ui->display_label->height();
            if (image->width() > w || image->height() > h) {
                ui->display_label->setPixmap(QPixmap::fromImage(image->scaled(
                        w, h, Qt::KeepAspectRatio)));
            } else {
                ui->display_label->setPixmap(QPixmap::fromImage(*image));
            }
            emit sig_modified();
        } else {
            QMessageBox::information(
                    this, tr("Invalid image"),
                    tr("You have to load a valid image from your device"),
                    QMessageBox::Ok);
        }
    });
    connect(ui->reset_btn, &QToolButton::clicked, [&]() {
        clearImage();
    });
}

ImageWidget::~ImageWidget() {
    delete ui;
}

void ImageWidget::clearImage() {
    image = nullptr;
    ui->display_label->clear();
    emit sig_modified();
}
