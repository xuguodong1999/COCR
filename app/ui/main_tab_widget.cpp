#include "main_tab_widget.h"
#include "ui_main_tab_widget.h"
#include "welcome_widget.h"
#include "paint_widget.h"
#include "../2d/view2d_widget.h"

#ifndef Q_OS_WASM

#include "../3d/view3d_widget.h"

#endif

#if not defined(Q_OS_WASM) and not defined(Q_OS_ANDROID)

#include "../camera/camera_widget.h"

#endif

#include "image_widget.h"
#include "ocr/ocr_runnable.hpp"
#include "application.hpp"
#include "chem/jmol.hpp"
#include "chem/jmol_manager.hpp"
#include <QTimer>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

MainTabWidget::MainTabWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::MainTabWidget),
          view2DWidget(nullptr),
#ifndef Q_OS_WASM
          view3DWidget(nullptr),
#endif
#if not defined(Q_OS_WASM) and not defined(Q_OS_ANDROID)
          cameraWidget(nullptr),
#endif
          ocrThread(new OCRThread(this)), lastSource(DataSource::PAINT),
          isOCRBtnClicked(false), isAgreementChecked(false), isMolLatest(false) {
    ui->setupUi(this);
    // 欢迎页
    auto l = new QHBoxLayout(ui->wel_tab);
    welcomeWidget = new WelcomeWidget(ui->wel_tab);
    l->addWidget(welcomeWidget);
    ui->wel_tab->setLayout(l);
    // 更新协议同意状态
    setAgreementChecked(welcomeWidget->isAgreed());
    connect(welcomeWidget, &WelcomeWidget::sig_agree_box_checked, this, &MainTabWidget::setAgreementChecked);
    qDebug() << "MainTabWidget::MainTabWidget WelcomeWidget";
    // 绘图页
    l = new QHBoxLayout(ui->draw_tab);
    paintWidget = new PaintWidget(ui->draw_tab);
    l->addWidget(paintWidget);
    ui->draw_tab->setLayout(l);
    // 设置状态更新避免重复识别
    connect(paintWidget, &PaintWidget::sig_modified, [&]() { isMolLatest = false; });
    // 从按钮启动 OCR 任务
    connect(paintWidget, &PaintWidget::sig_ocr_btn_clicked,
            this, QOverload<const QList<QList<QPointF>> &>::of(&MainTabWidget::doOCR));
    qDebug() << "MainTabWidget::MainTabWidget PaintWidget";
    // 2D
    v2dLayout = new QHBoxLayout(ui->v2d_tab);
    ui->v2d_tab->setLayout(v2dLayout);
    qDebug() << "MainTabWidget::MainTabWidget v2dLayout";
    ui->tabWidget->setTabVisible(3, false);
    ui->tabWidget->setTabVisible(4, false);
    ui->tabWidget->setTabVisible(5, false);
#ifndef Q_OS_WASM
    // 3D
    v3dLayout = new QHBoxLayout(ui->v3d_tab);
    ui->v3d_tab->setLayout(v3dLayout);
    ui->tabWidget->setTabVisible(3, true);
#endif

// FIXME: 安卓设备上 QCameraViewFinder 有 bug，据说不支持，最简单的做法是加 Qml
#if not defined(Q_OS_WASM) and not defined(Q_OS_ANDROID)
    // 图片加载页
    l = new QHBoxLayout(ui->img_tab);
    imageWidget = new ImageWidget(ui->img_tab);
    l->addWidget(imageWidget);
    ui->img_tab->setLayout(l);
    ui->tabWidget->setTabVisible(4, true);
    connect(imageWidget, &ImageWidget::sig_ocr_btn_clicked,
            this, QOverload<const QImage &>::of(&MainTabWidget::doOCR));
    connect(imageWidget, &ImageWidget::sig_modified, [&]() { isMolLatest = false; });
    // 拍照页
    camLayout = new QHBoxLayout(ui->cam_tab);
    ui->cam_tab->setLayout(camLayout);
    ui->tabWidget->setTabVisible(5, true);
#endif
    // 用户上一次观看 OCR 结果的偏好
    is2DLastUsed = leafxySettings.value(
            KEY_IS_2D_LAST_USED,
#ifdef Q_OS_WASM
            true
#else
            false
#endif
    ).toBool();
    // 页面切换需要根据 mol 数据刷新显示
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainTabWidget::handleTabChange);
    // OCR 任务完成后告知主窗体
    connect(ocrThread, &OCRThread::sig_mol_ready, this, &MainTabWidget::onOcrJobReady);
    // 设置起始页面：欢迎页或者绘图页
    ui->tabWidget->setCurrentIndex(isAgreementChecked ? 1 : 0);
    qDebug() << "MainTabWidget::MainTabWidget done";
}


MainTabWidget::~MainTabWidget() {
    delete ui;
}

void MainTabWidget::handleTabChange(int index) {
    qDebug() << "handleTabChange" << index;
    if (index != 0 && !isAgreementChecked) {
        QMessageBox::information(
                this, tr("Agreement not checked yet"),
                tr("You have to check out agreement to use this software"),
                QMessageBox::Ok);
        ui->tabWidget->setCurrentIndex(0);
        return;
    }
    switch (index) {
        case 1: {
            const DataSource currentSource = DataSource::PAINT;
            if (currentSource != lastSource) { isMolLatest = false; }
            safeDelete2DWidget();
#ifndef Q_OS_WASM
            safeDelete3DWidget();
#endif
#if not defined(Q_OS_WASM) and not defined(Q_OS_ANDROID)
            safeDeleteCamWidget();
#endif
            lastSource = DataSource::PAINT;
            break;
        }
        case 2: {
            safeAttach2DWidget();
            if (!isOCRBtnClicked) {
                isMolLatest ? onOcrJobReady() : doOCR(paintWidget->getScript());
            }
            break;
        }
#ifndef Q_OS_WASM
        case 3: {
            safeAttach3DWidget();
            if (!isOCRBtnClicked) {
                isMolLatest ? onOcrJobReady() : doOCR(paintWidget->getScript());
            }
            break;
        }
#endif
        case 4: {
            const DataSource currentSource = DataSource::IMAGE;
            if (currentSource != lastSource) { isMolLatest = false; }
            lastSource = DataSource::IMAGE;
            break;
        }
#if not defined(Q_OS_WASM) and not defined(Q_OS_ANDROID)
        case 5: {
            const DataSource currentSource = DataSource::CAMERA;
            if (currentSource != lastSource) { isMolLatest = false; }
            safeAttachCamWidget();
            lastSource = DataSource::CAMERA;
            break;
        }
#endif
    }
    isOCRBtnClicked = false;
}

void MainTabWidget::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
//    // 保证 TAB 有足够高度
//    setStyleSheet(QString("QTabBar::tab{height:%0}").arg(
//            (std::max)(10, (std::min)(40, static_cast<int>(height() * 0.2)))));
}

void MainTabWidget::onOcrJobReady() {
    isMolLatest = true;
    cocr::JMolManager::GetInstance().setInputMol(ocrThread->getMol());
    auto mol = cocr::JMolManager::GetInstance().getInputMol();
    if (!mol) {
        QMessageBox::information(
                this, tr("No Molecule Detected"),
                tr("Nothing detected and it may be a bug"),
                QMessageBox::Ok);
        return;
    }
    if (ui->tabWidget->currentIndex() == 2) {
        view2DWidget->syncMolToScene(mol);
    }
#ifndef Q_OS_WASM
    else if (ui->tabWidget->currentIndex() == 3) {
        mol = cocr::JMolManager::GetInstance().getFullHydrogenInputMol();
        if (!mol) {
            QMessageBox::information(
                    this, tr("3D Molecule build failed"),
                    tr("Nothing to preview and it may be a bug"),
                    QMessageBox::Ok);
            return;
        }
        view3DWidget->syncMolToScene(mol);
    }
#endif
}

void MainTabWidget::doOCR(const QList<QList<QPointF>> &_script) {
    qDebug() << __FUNCTION__ << "_script.size()=" << _script.size();
    isOCRBtnClicked = true;
    if (paintWidget) { paintWidget->saveLastScript(); }
    if (is2DLastUsed) {
        ui->tabWidget->setCurrentIndex(2);
        view2DWidget->startWaitHint();
    }
#ifndef Q_OS_WASM
    else {
        ui->tabWidget->setCurrentIndex(3);
        view3DWidget->startWaitHint();
    }
#endif
    if (isMolLatest) {
        onOcrJobReady();
    } else {
        ocrThread->bindData(_script);
        ocrThread->start();
    }
}

void MainTabWidget::safeAttach2DWidget() {
    is2DLastUsed = true;
    setRecentlyUsedViewer(is2DLastUsed);
    if (!view2DWidget) {
        view2DWidget = new View2DWidget(ui->v2d_tab);
        v2dLayout->addWidget(view2DWidget);
    }
    view2DWidget->show();
}

void MainTabWidget::safeDelete2DWidget() {
    if (view2DWidget) {
        view2DWidget->hide();
        v2dLayout->removeWidget(view2DWidget);
        view2DWidget->setParent(nullptr);
        delete view2DWidget;
        view2DWidget = nullptr;
    }
}

#ifndef Q_OS_WASM

void MainTabWidget::safeAttach3DWidget() {
    is2DLastUsed = false;
    setRecentlyUsedViewer(is2DLastUsed);
    if (!view3DWidget) {
        view3DWidget = new View3DWidget(ui->v3d_tab);
        v3dLayout->addWidget(view3DWidget);
    }
    view3DWidget->show();
}

void MainTabWidget::safeDelete3DWidget() {
    if (view3DWidget) {
        view3DWidget->hide();
//        v3dLayout->removeWidget(view3DWidget);
//        view3DWidget->setParent(nullptr);
//        delete view3DWidget;
//        view3DWidget = nullptr;
    }
}

#endif
#if not defined(Q_OS_WASM) and not defined(Q_OS_ANDROID)

void MainTabWidget::safeAttachCamWidget() {
    if (!cameraWidget) {
        cameraWidget = new CameraWidget(ui->cam_tab);
        camLayout->addWidget(cameraWidget);
    }
    cameraWidget->show();
}

void MainTabWidget::safeDeleteCamWidget() {
    if (cameraWidget) {
        cameraWidget->hide();
        camLayout->removeWidget(cameraWidget);
        cameraWidget->setParent(nullptr);
        delete cameraWidget;
        cameraWidget = nullptr;
    }
}

#endif

void MainTabWidget::setAgreementChecked(bool isChecked) {
    isAgreementChecked = isChecked;
}

void MainTabWidget::setRecentlyUsedViewer(bool is2D) {
    is2DLastUsed = is2D;
    leafxySettings.setValue(KEY_IS_2D_LAST_USED, is2DLastUsed);
}

void MainTabWidget::doOCR(const QImage &_image) {
    qDebug() << __FUNCTION__ << _image.size();
    isOCRBtnClicked = true;
    if (is2DLastUsed) {
        ui->tabWidget->setCurrentIndex(2);
        view2DWidget->startWaitHint();
    }
#ifndef Q_OS_WASM
    else {
        ui->tabWidget->setCurrentIndex(3);
        view3DWidget->startWaitHint();
    }
#endif
//    if (isMolLatest) {
//        onOcrJobReady();
//    } else {
    ocrThread->bindData(_image);
    ocrThread->start();
//    }
}

void MainTabWidget::closeEvent(QCloseEvent *e) {
    qDebug() << __FUNCTION__ << "MainTabWidget";
    if (paintWidget) { paintWidget->saveLastScript(); }
    QWidget::closeEvent(e);
    exit(0);// 如果分子力场没跑完，这里要强杀线程
}

