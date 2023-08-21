#pragma once

#include <QWidget>
#include <QtMultimedia/QCamera>
#include <QtMultimedia/QImageCapture>
#include <QtMultimedia/QMediaRecorder>
#include <QScopedPointer>

namespace Ui {
    class CameraWidget;
}
class QListWidgetItem;

class CameraWidget : public QWidget {
    Q_OBJECT

public:
    explicit CameraWidget(QWidget *parent = nullptr);

    ~CameraWidget();

public
    Q_SLOTS:

            void startCamera();

    void stopCamera();

private
    Q_SLOTS:

            void setCamera(
    const QCameraInfo &cameraInfo
    );

    void setExposureCompensation(int index);

    void setDeviceItem(QListWidgetItem *current, QListWidgetItem *previous);

    void updateCameraState(QCamera::State);

    void displayCaptureError(int, QCameraImageCapture::Error, const QString &errorString);


    void toggleLock();

    void takeImage();


    void displayCameraError();

    void processCapturedImage(int requestId, const QImage &img);

    void updateLockStatus(QCamera::LockStatus, QCamera::LockChangeReason);

    void displayViewfinder();

    void displayCapturedImage();

    void readyForCapture(bool ready);

    void imageSaved(int id, const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QMap<QListWidgetItem *, size_t> mDeviceMap;
    QList <QCameraInfo> mDevices;
    Ui::CameraWidget *ui;
    QScopedPointer <QCamera> m_camera;
    QScopedPointer <QCameraImageCapture> m_imageCapture;
};