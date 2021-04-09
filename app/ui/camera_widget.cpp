#include "camera_widget.h"
#include "ui_camera_widget.h"

#include <QtMultimedia/QMediaService>
#include <QtMultimedia/QMediaRecorder>
#include <QtMultimediaWidgets/QCameraViewfinder>
#include <QtMultimedia/QCameraInfo>
#include <QtMultimedia/QMediaMetaData>

#include <QMessageBox>
#include <QPalette>
#include <QActionGroup>
#include <QKeyEvent>
#include <QTimer>
#include <QDir>

CameraWidget::CameraWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::CameraWidget) {
    ui->setupUi(this);
    //Camera devices:

//    QActionGroup *videoDevicesGroup = new QActionGroup(this);
//    videoDevicesGroup->setExclusive(true);
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    qDebug()<<availableCameras;
//    for (const QCameraInfo &cameraInfo : availableCameras) {
//        QAction *videoDeviceAction = new QAction(cameraInfo.description(), videoDevicesGroup);
//        videoDeviceAction->setCheckable(true);
//        videoDeviceAction->setData(QVariant::fromValue(cameraInfo));
//        if (cameraInfo == QCameraInfo::defaultCamera())
//            videoDeviceAction->setChecked(true);
//        // FIXME
////        ui->menuDevices->addAction(videoDeviceAction);
//    }

//    connect(videoDevicesGroup, &QActionGroup::triggered, this, &CameraWidget::updateCameraDevice);
//    connect(ui->captureWidget, &QTabWidget::currentChanged, this, &CameraWidget::updateCaptureMode);

    setCamera(QCameraInfo::defaultCamera());
}

CameraWidget::~CameraWidget() {
    delete ui;
}


void CameraWidget::setCamera(const QCameraInfo &cameraInfo) {
    m_camera.reset(new QCamera(cameraInfo));

    connect(m_camera.data(), &QCamera::stateChanged, this, &CameraWidget::updateCameraState);
    connect(m_camera.data(), QOverload<QCamera::Error>::of(&QCamera::error), this, &CameraWidget::displayCameraError);

    m_mediaRecorder.reset(new QMediaRecorder(m_camera.data()));
    connect(m_mediaRecorder.data(), &QMediaRecorder::stateChanged, this, &CameraWidget::updateRecorderState);

    m_imageCapture.reset(new QCameraImageCapture(m_camera.data()));

    connect(m_mediaRecorder.data(), &QMediaRecorder::durationChanged, this, &CameraWidget::updateRecordTime);
    connect(m_mediaRecorder.data(), QOverload<QMediaRecorder::Error>::of(&QMediaRecorder::error),
            this, &CameraWidget::displayRecorderError);

    m_mediaRecorder->setMetaData(QMediaMetaData::Title, QVariant(QLatin1String("Test Title")));

    connect(ui->exposureCompensation, &QAbstractSlider::valueChanged, this, &CameraWidget::setExposureCompensation);

    m_camera->setViewfinder(ui->viewfinder);

    updateCameraState(m_camera->state());
    updateLockStatus(m_camera->lockStatus(), QCamera::UserRequest);
    updateRecorderState(m_mediaRecorder->state());

    connect(m_imageCapture.data(), &QCameraImageCapture::readyForCaptureChanged, this, &CameraWidget::readyForCapture);
    connect(m_imageCapture.data(), &QCameraImageCapture::imageCaptured, this, &CameraWidget::processCapturedImage);
    connect(m_imageCapture.data(), &QCameraImageCapture::imageSaved, this, &CameraWidget::imageSaved);
    connect(m_imageCapture.data(),
            QOverload<int, QCameraImageCapture::Error, const QString &>::of(&QCameraImageCapture::error),
            this, &CameraWidget::displayCaptureError);

    connect(m_camera.data(), QOverload<QCamera::LockStatus, QCamera::LockChangeReason>::of(&QCamera::lockStatusChanged),
            this, &CameraWidget::updateLockStatus);

    ui->captureWidget->setTabEnabled(0, (m_camera->isCaptureModeSupported(QCamera::CaptureStillImage)));
    ui->captureWidget->setTabEnabled(1, (m_camera->isCaptureModeSupported(QCamera::CaptureVideo)));

    updateCaptureMode();
    m_camera->start();
}

void CameraWidget::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat())
        return;

    switch (event->key()) {
        case Qt::Key_CameraFocus:
            displayViewfinder();
            m_camera->searchAndLock();
            event->accept();
            break;
        case Qt::Key_Camera:
            if (m_camera->captureMode() == QCamera::CaptureStillImage) {
                takeImage();
            } else {
                if (m_mediaRecorder->state() == QMediaRecorder::RecordingState)
                    stop();
                else
                    record();
            }
            event->accept();
            break;
        default:
            QWidget::keyPressEvent(event);
    }
}

void CameraWidget::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat())
        return;

    switch (event->key()) {
        case Qt::Key_CameraFocus:
            m_camera->unlock();
            break;
        default:
            QWidget::keyReleaseEvent(event);
    }
}

void CameraWidget::updateRecordTime() {
    QString str = QString("Recorded %1 sec").arg(m_mediaRecorder->duration() / 1000);
    ui->camera_label->setText(str);
//    ui->camera_label->setText(str);
}

void CameraWidget::processCapturedImage(int requestId, const QImage &img) {
    Q_UNUSED(requestId);
    QImage scaledImage = img.scaled(ui->viewfinder->size(),
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);

    ui->lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));

    // Display captured image for 4 seconds.
    displayCapturedImage();
    QTimer::singleShot(4000, this, &CameraWidget::displayViewfinder);
}

void CameraWidget::configureCaptureSettings() {
    switch (m_camera->captureMode()) {
        case QCamera::CaptureStillImage:
            configureImageSettings();
            break;
        case QCamera::CaptureVideo:
            configureVideoSettings();
            break;
        default:
            break;
    }
}

void CameraWidget::configureVideoSettings() {
//    VideoSettings settingsDialog(m_mediaRecorder.data());
//    settingsDialog.setWindowFlags(settingsDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
//
//    settingsDialog.setAudioSettings(m_audioSettings);
//    settingsDialog.setVideoSettings(m_videoSettings);
//    settingsDialog.setFormat(m_videoContainerFormat);
//
//    if (settingsDialog.exec()) {
//        m_audioSettings = settingsDialog.audioSettings();
//        m_videoSettings = settingsDialog.videoSettings();
//        m_videoContainerFormat = settingsDialog.format();
//
//        m_mediaRecorder->setEncodingSettings(
//                m_audioSettings,
//                m_videoSettings,
//                m_videoContainerFormat);
//
//        m_camera->unload();
//        m_camera->start();
//    }
}

void CameraWidget::configureImageSettings() {
//    ImageSettings settingsDialog(m_imageCapture.data());
//    settingsDialog.setWindowFlags(settingsDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
//
//    settingsDialog.setImageSettings(m_imageSettings);
//
//    if (settingsDialog.exec()) {
//        m_imageSettings = settingsDialog.imageSettings();
//        m_imageCapture->setEncodingSettings(m_imageSettings);
//    }
}

void CameraWidget::record() {
    m_mediaRecorder->record();
    updateRecordTime();
}

void CameraWidget::pause() {
    m_mediaRecorder->pause();
}

void CameraWidget::stop() {
    m_mediaRecorder->stop();
}

void CameraWidget::setMuted(bool muted) {
    m_mediaRecorder->setMuted(muted);
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
            ui->lockButton->setText(tr("Focusing..."));
            break;
        case QCamera::Locked:
            indicationColor = Qt::darkGreen;
            ui->lockButton->setText(tr("Unlock"));
            ui->camera_label->setText(tr("Focused"));
            break;
        case QCamera::Unlocked:
            indicationColor = reason == QCamera::LockFailed ? Qt::red : Qt::black;
            ui->lockButton->setText(tr("Focus"));
            if (reason == QCamera::LockFailed)
                ui->camera_label->setText(tr("Focus Failed"));
    }

    QPalette palette = ui->lockButton->palette();
    palette.setColor(QPalette::ButtonText, indicationColor);
    ui->lockButton->setPalette(palette);
}

void CameraWidget::takeImage() {
    m_isCapturingImage = true;
    m_imageCapture->capture();
}

void CameraWidget::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString) {
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
    m_isCapturingImage = false;
}

void CameraWidget::startCamera() {
    m_camera->start();
}

void CameraWidget::stopCamera() {
    m_camera->stop();
}

void CameraWidget::updateCaptureMode() {
    int tabIndex = ui->captureWidget->currentIndex();
    QCamera::CaptureModes captureMode = tabIndex == 0 ? QCamera::CaptureStillImage : QCamera::CaptureVideo;

    if (m_camera->isCaptureModeSupported(captureMode))
        m_camera->setCaptureMode(captureMode);
}

void CameraWidget::updateCameraState(QCamera::State state) {
    switch (state) {
        case QCamera::ActiveState:
//            ui->actionStartCamera->setEnabled(false);
//            ui->actionStopCamera->setEnabled(true);
            ui->captureWidget->setEnabled(true);
//            ui->actionSettings->setEnabled(true);
            break;
        case QCamera::UnloadedState:
        case QCamera::LoadedState:
//            ui->actionStartCamera->setEnabled(true);
//            ui->actionStopCamera->setEnabled(false);
            ui->captureWidget->setEnabled(false);
//            ui->actionSettings->setEnabled(false);
    }
}

void CameraWidget::updateRecorderState(QMediaRecorder::State state) {
//    switch (state) {
//        case QMediaRecorder::StoppedState:
//            ui->recordButton->setEnabled(true);
//            ui->pauseButton->setEnabled(true);
//            ui->stopButton->setEnabled(false);
//            break;
//        case QMediaRecorder::PausedState:
//            ui->recordButton->setEnabled(true);
//            ui->pauseButton->setEnabled(false);
//            ui->stopButton->setEnabled(true);
//            break;
//        case QMediaRecorder::RecordingState:
//            ui->recordButton->setEnabled(false);
//            ui->pauseButton->setEnabled(true);
//            ui->stopButton->setEnabled(true);
//            break;
//    }
}

void CameraWidget::setExposureCompensation(int index) {
    m_camera->exposure()->setExposureCompensation(index * 0.5);
}

void CameraWidget::displayRecorderError() {
    QMessageBox::warning(this, tr("Capture Error"), m_mediaRecorder->errorString());
}

void CameraWidget::displayCameraError() {
    QMessageBox::warning(this, tr("Camera Error"), m_camera->errorString());
}

void CameraWidget::updateCameraDevice(QAction *action) {
//    setCamera(qvariant_cast<QCameraInfo>(action->data()));
}

void CameraWidget::displayViewfinder() {
    ui->stackedWidget->setCurrentIndex(0);
}

void CameraWidget::displayCapturedImage() {
    ui->stackedWidget->setCurrentIndex(1);
}

void CameraWidget::readyForCapture(bool ready) {
    ui->takeImageButton->setEnabled(ready);
}

void CameraWidget::imageSaved(int id, const QString &fileName) {
    Q_UNUSED(id);
    ui->camera_label->setText(tr("Captured \"%1\"").arg(QDir::toNativeSeparators(fileName)));

    m_isCapturingImage = false;
    if (m_applicationExiting)
        close();
}

void CameraWidget::closeEvent(QCloseEvent *event) {
    if (m_isCapturingImage) {
        setEnabled(false);
        m_applicationExiting = true;
        event->ignore();
    } else {
        event->accept();
    }
}
