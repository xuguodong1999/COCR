#include "MainTabWidget.h"
#include "ui_MainTabWidget.h"
#include "WelcomeWidget.h"
#include "PaintWidget.h"
#include "2D/View2DWidget.h"

#ifdef QT_3DCORE_LIB

#include "3D/View3DWidget.h"

#endif

#include "OCRRunnable.h"
#include "App.h"

#include "XGD/Chem/Mol.h"
#include "XGD/Chem/MolManager.h"


#include <QTimer>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>
#include <QTabWidget>

MainTabWidget::MainTabWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::MainTabWidget),
          view2DWidget(nullptr),
#ifdef QT_3DCORE_LIB
          view3DWidget(nullptr),
#endif
          ocrThread(new OCRThread(this)), lastSource(DataSource::PAINT),
          isOCRBtnClicked(false), isAgreementChecked(false), isMolLatest(false) {
    ui->setupUi(this);
    auto l = new QHBoxLayout(ui->wel_tab);
    welcomeWidget = new WelcomeWidget(ui->wel_tab);
    l->addWidget(welcomeWidget);
    ui->wel_tab->setLayout(l);
    setAgreementChecked(welcomeWidget->isAgreed());
    connect(welcomeWidget, &WelcomeWidget::sig_agree_box_checked, this, &MainTabWidget::setAgreementChecked);
    qDebug() << "MainTabWidget::MainTabWidget WelcomeWidget";
    l = new QHBoxLayout(ui->draw_tab);
    paintWidget = new PaintWidget(ui->draw_tab);
    l->addWidget(paintWidget);
    ui->draw_tab->setLayout(l);
    // avoid duplicated task with isMolLatest
    connect(paintWidget, &PaintWidget::sig_modified, [&]() { isMolLatest = false; });
    // button to launch OCR task
    connect(paintWidget, &PaintWidget::sig_ocr_btn_clicked,
            this, QOverload<const QList<QList<QPointF>> &>::of(&MainTabWidget::doOCR));
    qDebug() << "MainTabWidget::MainTabWidget PaintWidget";
    // 2D
    v2dLayout = new QHBoxLayout(ui->v2d_tab);
    ui->v2d_tab->setLayout(v2dLayout);
    qDebug() << "MainTabWidget::MainTabWidget v2dLayout";
    // 3D
#ifdef QT_3DCORE_LIB
    v3dLayout = new QHBoxLayout(ui->v3d_tab);
    ui->v3d_tab->setLayout(v3dLayout);
#else
    ui->tabWidget->setTabVisible(3, false);
#endif
    ui->tabWidget->setTabVisible(4, false);
    ui->tabWidget->setTabVisible(5, false);
    ui->tabWidget->setTabVisible(0, false);
    // user preference, navigate to 2D or 3D
    is2DLastUsed = cocrSettings.value(
            KEY_IS_2D_LAST_USED,
#ifdef Q_OS_WASM
            true
#else
            false
#endif
    ).toBool();
    // update display according to mol when switching tabs
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainTabWidget::handleTabChange);
    // notify MainWindow on task done
    connect(ocrThread, &OCRThread::sig_mol_ready, this, &MainTabWidget::onOcrJobReady);
    // set startup pages
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
#ifdef QT_3DCORE_LIB
            safeDelete3DWidget();
#endif
            lastSource = DataSource::PAINT;
            break;
        }
        case 2: {
            safeAttach2DWidget();
            if (!isOCRBtnClicked) {
                isMolLatest ? onMolReady() : doOCR(paintWidget->getScript());
            }
            break;
        }
#ifdef QT_3DCORE_LIB
        case 3: {
            safeAttach3DWidget();
            if (!isOCRBtnClicked) {
                isMolLatest ? onMolReady() : doOCR(paintWidget->getScript());
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
    }
    isOCRBtnClicked = false;
}

void MainTabWidget::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
//    leave enough space for tab
//    setStyleSheet(QString("QTabBar::tab{height:%0}").arg(
//            (std::max)(10, (std::min)(40, static_cast<int>(height() * 0.2)))));
}

void MainTabWidget::onOcrJobReady() {
    MolManager::GetInstance().setInputMol(ocrThread->getMol());
    onMolReady();
}

void MainTabWidget::onMolReady() {
    isMolLatest = true;
    auto mol = MolManager::GetInstance().getInputMol();
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
#ifdef QT_3DCORE_LIB
    else if (ui->tabWidget->currentIndex() == 3) {
        mol = MolManager::GetInstance().getFullHydrogenInputMol();
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
#ifdef QT_3DCORE_LIB
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

#ifdef QT_3DCORE_LIB

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

void MainTabWidget::setAgreementChecked(bool isChecked) {
    isAgreementChecked = isChecked;
}

void MainTabWidget::setRecentlyUsedViewer(bool is2D) {
    is2DLastUsed = is2D;
    cocrSettings.setValue(KEY_IS_2D_LAST_USED, is2DLastUsed);
}

void MainTabWidget::doOCR(const QImage &_image) {
    qDebug() << __FUNCTION__ << _image.size();
    isOCRBtnClicked = true;
    if (is2DLastUsed) {
        ui->tabWidget->setCurrentIndex(2);
        view2DWidget->startWaitHint();
    }
#ifdef QT_3DCORE_LIB
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
    qApp->exit(0); // FIXME: find a way to kill suspending openbabel forcefield threads
}

