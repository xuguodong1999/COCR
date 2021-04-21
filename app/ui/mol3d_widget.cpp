#include "mol3d_widget.hpp"
#include "jmol.hpp"
#include "../3d/mol3d_window.hpp"
#include "../3d/mol3d_builder.hpp"
#include "waithint_widget.h"
#include <Qt3DCore/QEntity>
#include <QHBoxLayout>
#include <QDebug>
#include <QGesture>
#include <QThreadPool>

Mol3DWidget::Mol3DWidget(QWidget *parent) : GestureWidget(parent), mol(nullptr), newMol(nullptr) {
    root = new Qt3DCore::QEntity();

    builder = new Mol3DBuilder(this, root);
    window = new Mol3DWindow(root);
    window->setMolRootTrans(builder->getMolRootTrans());
    window->setAxisRoot(builder->getAxisRoot());
    auto l = new QHBoxLayout(this);
    mol3DWindowContainer = QWidget::createWindowContainer(window, this);
    l->addWidget(mol3DWindowContainer);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    setLayout(l);

    hintWidget = new WaitHintWidget(this);
    hintWidget->hide();

    // prepare 运行在子线程， build 运行在 UI 线程
    connect(builder, &Mol3DBuilder::sig_mol_prepare_done, builder, &Mol3DBuilder::build);
    connect(builder, &Mol3DBuilder::sig_mol_build_done, this, &Mol3DWidget::endWaitHint);
}

void Mol3DWidget::syncMolToScene(std::shared_ptr<xgd::JMol> _mol) {
    mol = _mol;
    newMol = mol->deepClone();
    newMol->addAllHydrogens();
    qDebug() << __FUNCTION__;
    static QVector3D viewSize = window->getViewSize() / 1.5;
    QThreadPool::globalInstance()->start([&]() {
//        QThread::msleep(500);
        builder->prepare(newMol, viewSize);
    });
    startWaitHint();
}

void Mol3DWidget::startWaitHint() {
    mol3DWindowContainer->hide();
    hintWidget->startWaitHint();
}

void Mol3DWidget::endWaitHint() {
    hintWidget->hide();
    mol3DWindowContainer->show();
}

void Mol3DWidget::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
}


//bool Mol3DWidget::eventFilter(QObject *watched, QEvent *e) {
//    if (watched == window) {
//        switch (e->type()) {
//            case QEvent::TouchBegin:
//            case QEvent::TouchCancel:
//            case QEvent::TouchEnd:
//            case QEvent::TouchUpdate:
//            case QEvent::Gesture:
//            case QEvent::GestureOverride:
//            case QEvent::NativeGesture:
//                event(e);
//                return true;
//            default:
//                return false;
//        }
//    }
//    return false;
//}
