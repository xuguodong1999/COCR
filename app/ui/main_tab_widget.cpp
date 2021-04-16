#include <QThreadPool>
#include "main_tab_widget.h"
#include "ui_main_tab_widget.h"
#include "welcome_widget.h"
#include "paint_widget.h"
#include "view2d_widget.h"
#include "view3d_widget.h"
#include "image_widget.h"
#include "camera_widget.h"
#include "ocr_runnable.hpp"
#include "application.hpp"
#include "jmol.hpp"
#include <QTimer>

MainTabWidget::MainTabWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::MainTabWidget), welcomeWidget(nullptr), paintWidget(nullptr),
          view2DWidget(nullptr), view3DWidget(nullptr), imageWidget(nullptr), cameraWidget(nullptr),
          mol(nullptr), ocrThread(new OCRThread(this)) {
    ui->setupUi(this);
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
    if (!welcomeWidget) { attach_welcome_widget(); }
    if (!paintWidget) { attach_paint_widget(); }
    if (!view2DWidget) { attach_view2d_widget(); }
    if (!view3DWidget) { attach_view3d_widget(); }
    if (!imageWidget) { attach_image_widget(); }
    if (!cameraWidget) { attach_camera_widget(); }
    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, &MainTabWidget::handleTabChange);
    connect(paintWidget, &PaintWidget::sig_ocr_btn_clicked, this, &MainTabWidget::doOCR);
    ui->tabWidget->setCurrentIndex(0);
    connect(ocrThread, &OCRThread::sig_mol_ready, this, &MainTabWidget::onOcrJobReady);
    is2DLastUsed = leafxyApp->getSettings().value("main_tab_widget/is_2d_last_used", true).toBool();
    QTimer::singleShot(100, [&]() {
        ui->tabWidget->setCurrentIndex(1);
    });
}

void MainTabWidget::syncMolToView3D() {
    mol->addAllHydrogens();
    view3DWidget->syncMolToScene(mol);
    is2DLastUsed = false;
    leafxyApp->getSettings().setValue("main_tab_widget/is_2d_last_used", is2DLastUsed);
}

void MainTabWidget::syncMolToView2D() {
    view2DWidget->syncMolToScene(mol);
    is2DLastUsed = true;
    leafxyApp->getSettings().setValue("main_tab_widget/is_2d_last_used", is2DLastUsed);
}

MainTabWidget::~MainTabWidget() {
    delete ui;
}

void MainTabWidget::handleTabChange(int index) {
    qDebug() << "handleTabChange" << index;
    switch (index) {
        case 0: {
            break;
        }
        case 1: {
            break;
        }
        case 2: {
            is2DLastUsed = true;
            ui->tabWidget->setCurrentIndex(2);
            if (!paintWidget->isLatest()) {
                doOCR(paintWidget->getScript());
            } else {
                onOcrJobReady();
            }
            break;
        }
        case 3: {
            is2DLastUsed = false;
            ui->tabWidget->setCurrentIndex(3);
            if (!paintWidget->isLatest()) {
                doOCR(paintWidget->getScript());
            } else {
                onOcrJobReady();
            }
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
    if (index != 5 && cameraWidget) {
        cameraWidget->stopCamera();
    }
}

void MainTabWidget::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
//    // 保证 TAB 有足够高度
//    setStyleSheet(QString("QTabBar::tab{height:%0}").arg(
//            (std::max)(10, (std::min)(40, static_cast<int>(height() * 0.2)))));
}

void MainTabWidget::onOcrJobReady() {
    paintWidget->setIsLatest(true);
    mol = ocrThread->getMol();
    if (ui->tabWidget->currentIndex() == 2) {
        syncMolToView2D();
    } else if (ui->tabWidget->currentIndex() == 3) {
        syncMolToView3D();
    }
}

void MainTabWidget::doOCR(const QList<QList<QPointF>> &_script) {
    qDebug() << "_script.size()=" << _script.size();
    if (!paintWidget->isLatest()) {
        ocrThread->bindData(_script);
        ocrThread->start();
        if (is2DLastUsed) {
            ui->tabWidget->setCurrentIndex(2);
            view2DWidget->startWaitHint();
        } else {
            ui->tabWidget->setCurrentIndex(3);
            view3DWidget->startWaitHint();
        }
    } else {
        if (is2DLastUsed) {
            ui->tabWidget->setCurrentIndex(2);
        } else {
            ui->tabWidget->setCurrentIndex(3);
        }
        onOcrJobReady();
    }
}

