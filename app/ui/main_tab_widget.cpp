#include "main_tab_widget.h"
#include "ui_main_tab_widget.h"
#include "welcome_widget.h"
#include "paint_widget.h"
#include "view2d_widget.h"
#include "view3d_widget.h"
#include "image_widget.h"
#include "camera_widget.h"

MainTabWidget::MainTabWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::MainTabWidget), welcomeWidget(nullptr), paintWidget(nullptr),
          view2DWidget(nullptr), view3DWidget(nullptr), imageWidget(nullptr), cameraWidget(nullptr) {
    ui->setupUi(this);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainTabWidget::handleTabChange);
    handleTabChange(0);
}

MainTabWidget::~MainTabWidget() {
    delete ui;
}

void MainTabWidget::handleTabChange(int index) {
    static const auto attach_welcome_widget = [&]() {
        auto l = new QHBoxLayout();
        welcomeWidget = new WelcomeWidget(ui->wel_tab);
        l->addWidget(welcomeWidget);
        ui->wel_tab->setLayout(l);
    };
    static const auto attach_paint_widget = [&]() {
        auto l = new QHBoxLayout();
        paintWidget = new PaintWidget(ui->draw_tab);
        l->addWidget(paintWidget);
        ui->draw_tab->setLayout(l);
    };
    static const auto attach_view2d_widget = [&]() {
        auto l = new QHBoxLayout();
        view2DWidget = new View2DWidget(ui->v2d_tab);
        l->addWidget(view2DWidget);
        ui->v2d_tab->setLayout(l);
    };

    static const auto attach_view3d_widget = [&]() {
        auto l = new QHBoxLayout();
        view3DWidget = new View3DWidget(ui->v3d_tab);
        l->addWidget(view3DWidget);
        ui->v3d_tab->setLayout(l);
    };
    static const auto attach_image_widget = [&]() {
        auto l = new QHBoxLayout();
        imageWidget = new ImageWidget(ui->img_tab);
        l->addWidget(imageWidget);
        ui->img_tab->setLayout(l);
    };
    static const auto attach_camera_widget = [&]() {
        auto l = new QHBoxLayout();
        cameraWidget = new CameraWidget(ui->cam_tab);
        l->addWidget(cameraWidget);
        ui->cam_tab->setLayout(l);
    };
    switch (index) {
        case 0: {
            if (!welcomeWidget) { attach_welcome_widget(); }
            break;
        }
        case 1: {
            if (!paintWidget) { attach_paint_widget(); }
            break;
        }
        case 2: {
            if (!view2DWidget) { attach_view2d_widget(); }
            break;
        }
        case 3: {
            if (!view3DWidget) { attach_view3d_widget(); }
            break;
        }
        case 4: {
            if (!imageWidget) { attach_image_widget(); }
            break;
        }
        case 5: {
            if (!cameraWidget) { attach_camera_widget(); }
            cameraWidget->startCamera();
            break;
        }
    }
    if (index != 5 && cameraWidget) {
        cameraWidget->stopCamera();
    }
}
