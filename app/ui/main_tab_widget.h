#ifndef MAIN_TAB_WIDGET_H
#define MAIN_TAB_WIDGET_H

#include <QWidget>

namespace Ui {
    class MainTabWidget;
}

class WelcomeWidget;

class PaintWidget;

class View2DWidget;

class View3DWidget;

class ImageWidget;

class CameraWidget;

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
};

#endif // MAIN_TAB_WIDGET_H
