#include "main_tab_widget.h"
#include "ui_main_tab_widget.h"
#include "welcome_widget.h"
#include "paint_widget.h"
#include "view2d_widget.h"
#include "view3d_widget.h"
#include "image_widget.h"
#include "camera_widget.h"

MainTabWidget::MainTabWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::MainTabWidget) {
    ui->setupUi(this);

    auto l = new QHBoxLayout();
    welcomeWidget = new WelcomeWidget();
    l->addWidget(welcomeWidget);
    ui->wel_tab->setLayout(l);

    l = new QHBoxLayout();
    paintWidget = new PaintWidget();
    l->addWidget(paintWidget);
    ui->draw_tab->setLayout(l);

    l = new QHBoxLayout();
    view2DWidget = new View2DWidget();
    l->addWidget(view2DWidget);
    ui->v2d_tab->setLayout(l);

    l = new QHBoxLayout();
    view3DWidget = new View3DWidget();
    l->addWidget(view3DWidget);
    ui->v3d_tab->setLayout(l);

    l = new QHBoxLayout();
    imageWidget = new ImageWidget();
    l->addWidget(imageWidget);
    ui->img_tab->setLayout(l);

    l = new QHBoxLayout();
    cameraWidget = new CameraWidget();
    l->addWidget(cameraWidget);
    ui->cam_tab->setLayout(l);

    connect(ui->tabWidget, &QTabWidget::currentChanged, [&](int index) {
        switch (index) {
            case 0: {
                break;
            }
            case 1: {
                break;
            }
            case 2: {
                break;
            }
            case 3: {
                break;
            }
            case 4: {
                break;
            }
            case 5: {
                cameraWidget->startCamera();
                break;
            }
        }
        if (index != 5) {
            cameraWidget->stopCamera();
        }
    });
}

MainTabWidget::~MainTabWidget() {
    delete ui;
}
