#include "camera/camera_widget.h"
#include "ui_camera_widget.h"

#include <QtMultimediaWidgets/QCameraViewfinder>
#include <QtMultimedia/QCameraInfo>
#include <QtMultimedia/QMediaMetaData>

#include <QMessageBox>
#include <QPalette>
#include <QActionGroup>
#include <QKeyEvent>
#include <QTimer>
#include <QDir>
#include <QVideoProbe>
#include <QPainter>

CameraWidget::CameraWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::CameraWidget) {
    ui->setupUi(this);

    mDevices = QCameraInfo::availableCameras();
    connect(ui->device_list, &QListWidget::currentItemChanged,
            this, &CameraWidget::setDeviceItem);
    connect(ui->shot_btn, &QToolButton::clicked, [&]() {
        if (m_camera.isNull()) { return; }
        if (!m_camera->isAvailable()) { return; }
        m_camera->searchAndLock();
        takeImage();
        m_camera->unlock();
    });
    connect(ui->exposure_slider, &QAbstractSlider::valueChanged,
            this, &CameraWidget::setExposureCompensation);
    connect(ui->lock_btn, &QToolButton::clicked, this, &CameraWidget::toggleLock);

    for (size_t i = 0; i < mDevices.size(); i++) {
        auto &camInfo = mDevices[i];
        auto item = new QListWidgetItem(camInfo.description());
        mDeviceMap[item] = i;
        ui->device_list->addItem(item);
        if (camInfo == QCameraInfo::defaultCamera()) {
            // if available, load camera here
            ui->device_list->setCurrentItem(item);
            item->setCheckState(Qt::CheckState::Checked);
        } else {
            item->setCheckState(Qt::CheckState::Unchecked);
        }
    }
}


void CameraWidget::setDeviceItem(QListWidgetItem *current, QListWidgetItem *previous) {
    if (previous) {
        previous->setCheckState(Qt::CheckState::Unchecked);
    }
    if (current) {
        current->setCheckState(Qt::CheckState::Checked);
        auto it = mDeviceMap.find(current);
        if (mDeviceMap.end() != it && it.value() < mDevices.size()) {
            setCamera(mDevices[it.value()]);
        }
    }
}


CameraWidget::~CameraWidget() {
    delete ui;
}

void CameraWidget::setExposureCompensation(int index) {
    if (m_camera.isNull()) {
        return;
    }
    m_camera->exposure()->setExposureCompensation(index * 0.5);
}

void CameraWidget::setCamera(const QCameraInfo &cameraInfo) {
    m_camera.reset(new QCamera(cameraInfo));
    m_camera->setViewfinder(ui->viewfinder);
    m_imageCapture.reset(new QCameraImageCapture(m_camera.data()));
    m_imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
    m_imageCapture->setBufferFormat(QVideoFrame::PixelFormat::Format_Jpeg);
    connect(m_camera.data(), &QCamera::stateChanged,
            this, &CameraWidget::updateCameraState);
    connect(m_camera.data(), &QCamera::errorOccurred,
            this, &CameraWidget::displayCameraError);


    updateCameraState(m_camera->state());
    updateLockStatus(m_camera->lockStatus(), QCamera::UserRequest);


    connect(m_imageCapture.data(), &QCameraImageCapture::readyForCaptureChanged,
            this, &CameraWidget::readyForCapture);
    connect(m_imageCapture.data(), &QCameraImageCapture::imageCaptured,
            this, &CameraWidget::processCapturedImage);
    connect(m_imageCapture.data(), &QCameraImageCapture::imageSaved,
            this, &CameraWidget::imageSaved);
    connect(m_imageCapture.data(),
            QOverload<int, QCameraImageCapture::Error, const QString &>::of(&QCameraImageCapture::error),
            this, &CameraWidget::displayCaptureError);

    connect(m_camera.data(), QOverload<QCamera::LockStatus, QCamera::LockChangeReason>::of(
            &QCamera::lockStatusChanged), this, &CameraWidget::updateLockStatus);

    displayViewfinder();
}

void CameraWidget::processCapturedImage(int requestId, const QImage &img) {
    Q_UNUSED(requestId);
    stopCamera();
    displayCapturedImage();
    ui->preview_label->setPixmap(QPixmap::fromImage(img.scaled(
            ui->preview_label->size() * 0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    // TODO: emit image here
    QTimer::singleShot(2000, this, &CameraWidget::displayViewfinder);
}


void CameraWidget::toggleLock() {
    switch (m_camera->lockStatus()) {
        case QCamera::Searching:
        case QCamera::Locked:
            m_camera->unlock();
            break;
        case QCamera::Unlocked:
            m_camera->searchAndLock();
    }
}

void CameraWidget::updateLockStatus(QCamera::LockStatus status, QCamera::LockChangeReason reason) {
    QColor indicationColor = Qt::black;
    switch (status) {
        case QCamera::Searching:
            indicationColor = Qt::yellow;
            ui->camera_label->setText(tr("Focusing..."));
            ui->lock_btn->setText(tr("Focusing..."));
            break;
        case QCamera::Locked:
            indicationColor = Qt::darkGreen;
            ui->lock_btn->setText(tr("Unlock"));
            ui->camera_label->setText(tr("Focused"));
            break;
        case QCamera::Unlocked:
            indicationColor = reason == QCamera::LockFailed ? Qt::red : Qt::black;
            ui->lock_btn->setText(tr("Focus"));
            if (reason == QCamera::LockFailed)
                ui->camera_label->setText(tr("Focus Failed"));
    }

    QPalette palette = ui->lock_btn->palette();
    palette.setColor(QPalette::ButtonText, indicationColor);
    ui->lock_btn->setPalette(palette);
}

void CameraWidget::takeImage() {
    m_imageCapture->capture();
}

void CameraWidget::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString) {
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
}

void CameraWidget::startCamera() {
    if (!m_camera.isNull()) {
        if (m_camera->state() != QCamera::ActiveState) {
            m_camera->start();
        }
    }
}

void CameraWidget::stopCamera() {
    if (!m_camera.isNull()) {
        if (m_camera->state() == QCamera::ActiveState) {
            m_camera->stop();
        }
    }
}


void CameraWidget::updateCameraState(QCamera::State state) {
    switch (state) {
        case QCamera::ActiveState:
            ui->ctrl_tab_widget->setEnabled(true);
            break;
        case QCamera::UnloadedState:
        case QCamera::LoadedState:
            ui->ctrl_tab_widget->setEnabled(false);
    }
}


void CameraWidget::displayCameraError() {
    QMessageBox::warning(this, tr("Camera Error"), m_camera->errorString());
}

void CameraWidget::displayViewfinder() {
    startCamera();
    ui->stacked_widget->setCurrentIndex(0);
}

void CameraWidget::displayCapturedImage() {
    ui->stacked_widget->setCurrentIndex(1);
}

void CameraWidget::readyForCapture(bool ready) {
    ui->shot_btn->setEnabled(ready);
}

void CameraWidget::imageSaved(int id, const QString &fileName) {
    Q_UNUSED(id);
    ui->camera_label->setText(tr("Captured \"%1\"").arg(QDir::toNativeSeparators(fileName)));
}

void CameraWidget::closeEvent(QCloseEvent *event) {
    stopCamera();
    QWidget::closeEvent(event);
}



