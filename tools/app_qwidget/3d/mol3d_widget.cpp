#include "3d/mol3d_widget.hpp"
#include "chem/jmol.hpp"
#include "3d/mol3d_window.hpp"
#include "3d/mol3d_builder.hpp"
#include "ui/waithint_widget.h"
#include <Qt3DCore/QEntity>
#include <QHBoxLayout>
#include <QDebug>
#include <QGesture>
#include <QThreadPool>

Mol3DWidget::Mol3DWidget(QWidget *parent) : QWidget(parent), mol(nullptr), minViewWidth(200) {
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

void Mol3DWidget::syncMolToScene(std::shared_ptr<cocr::JMol> _mol) {
    mol = std::move(_mol);
    window->reset();
    qDebug() << __FUNCTION__;
    minViewWidth = (std::min)(width(), height()) / 6;
    QThreadPool::globalInstance()->start([&]() {
//        QThread::msleep(500);
        builder->prepare(mol, {minViewWidth, minViewWidth, minViewWidth});
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

QString Mol3DWidget::makeAtomInfo(const size_t &_aid) {
    QString info = tr("atom (global id=") + QString::number(_aid) + tr(") picked\n");
    if (mol) {
        auto atom = mol->getAtom(_aid);
        if (atom) {
            info.append("\n" + tr("element: ") + atom->getQName());
            if (ElementType::SA != atom->getType()) {
                info.append("\n" + tr("mass: ") + QString::number(atom->getMass(), 'f', 4));
            }
        }
    }
    return info;
}

QString Mol3DWidget::makeBondInfo(const size_t &_bid) {
    QString info = tr("bond (global id=") + QString::number(_bid) + tr(") picked\n");
    if (mol) {
        auto bond = mol->getBond(_bid);
        if (bond) {
            info.append("\n" + tr("type: ") + bond->getQName());
            auto from = bond->getFrom();
            auto to = bond->getTo();
            if (from && to) {
                info.append("\n" + tr("atom 1: ") + from->getQName());
                info.append("\n" + tr("atom 2: ") + to->getQName());
            }
        }
    }
    return info;
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
