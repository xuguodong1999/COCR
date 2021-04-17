#ifndef MAIN_TAB_WIDGET_H
#define MAIN_TAB_WIDGET_H

#include <QWidget>

namespace Ui {
    class MainTabWidget;
}
namespace xgd {
    class JMol;
}
class WelcomeWidget;

class PaintWidget;

class View2DWidget;

class View3DWidget;

class ImageWidget;

class CameraWidget;

class OCRThread;

class MainTabWidget : public QWidget {
Q_OBJECT

public:
    explicit MainTabWidget(QWidget *parent = nullptr);

    ~MainTabWidget();

private:
    Ui::MainTabWidget *ui;
    WelcomeWidget *welcomeWidget;
    PaintWidget *paintWidget;
    View2DWidget *view2DWidget;
    View3DWidget *view3DWidget;
    ImageWidget *imageWidget;
    CameraWidget *cameraWidget;
    OCRThread *ocrThread;
    std::shared_ptr<xgd::JMol> mol;
    bool is2DLastUsed;
    bool isOCRBtnClicked;
protected:
    void resizeEvent(QResizeEvent *e) override;

private slots:

    void handleTabChange(int index);

    void syncMolToView3D();

    void syncMolToView2D();

    void onOcrJobReady();

    void doOCR(const QList<QList<QPointF>> &_script);
};

#endif // MAIN_TAB_WIDGET_H
