#include "view2d_widget.h"
#include "ui_view2d_widget.h"
#include "mol2d_widget.hpp"
#include "chem/jmol.hpp"
#include "chem/jmol_manager.hpp"
#include "format_dialog.h"
#include "2d/base_item.hpp"
#include <QTimer>
#include <QDebug>
#include <QMessageBox>

View2DWidget::View2DWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::View2DWidget), hyBtnClickTimes(0), expBtnClickTimes(0) {
    ui->setupUi(this);

    BaseItem::SetView2DWidget(this);

    auto formatVec = GetWritableFormats();
    for (auto &format:formatVec) {
        if (std::string::npos != format.find("Read-only")) { continue; }
        auto suffix = format.substr(0, format.find(" "));
        if (!xgd::JMol::IsValidWritableFormat(suffix)) { continue; }
        ui->format_box->addItem(QString::fromStdString(suffix));
    }
    connect(ui->format_box, &QComboBox::currentTextChanged, [&](const QString &formatName) {
        currentFormat = formatName.toStdString();
    });
    ui->format_box->setCurrentText("smi");

    connect(ui->format_btn, &QToolButton::clicked, this, &View2DWidget::showFormatDialog);
    connect(ui->hydrogen_btn, &QToolButton::clicked, this, &View2DWidget::switchHydrogenState);
    connect(ui->expand_btn, &QToolButton::clicked, this, &View2DWidget::switchSuperAtomState);
    connect(ui->reset_btn, &QToolButton::clicked, this, &View2DWidget::reformatInputState);

    auto l = new QHBoxLayout();
    mol2DWidget = new Mol2DWidget(ui->container);
    l->addWidget(mol2DWidget);
    ui->container->setLayout(l);
}

View2DWidget::~View2DWidget() {
    delete ui;
}

void View2DWidget::syncMolToScene(std::shared_ptr<xgd::JMol> _mol) {
    mol2DWidget->syncMolToScene(_mol);
}

void View2DWidget::startWaitHint() {
    mol2DWidget->startWaitHint();
}

void View2DWidget::showFormatDialog() {
    qDebug() << __FUNCTION__ << currentFormat.c_str();
    auto formatDialog = new FormatDialog(this);
    formatDialog->resize(size() * 0.8);
    formatDialog->setModal(true);
    formatDialog->setWindowTitle(QString::fromStdString(currentFormat));
    auto mol = xgd::JMolManager::GetInstance().getFullHydrogenExpandedMol(false);
    if (mol) {
        bool hasSuperAtom = false;
        mol->loopAtomVec([&](xgd::JAtom &atom) {
            if (xgd::ElementType::SA == atom.getType()) {
                hasSuperAtom = true;
                return;
            }
        });
        try {
            if (hasSuperAtom) {
                QMessageBox::information(
                        nullptr, tr("Warning"),
                        tr("The molecule you see has SUPER ATOMS.\n"
                           "They are represented as element At(85, VIIA).\n"
                           "You can use 'expand' and 'see H' button to try again"),
                        QMessageBox::Yes);
            }
            QString content = QString::fromStdString(mol->writeAs(currentFormat));
            formatDialog->setFormatContent(content);
        } catch (...) {
            formatDialog->setFormatContent(tr("Format algorithm failed on your molecule"));
        }
    } else {
        formatDialog->setFormatContent(tr("No molecule found"));
    }
    formatDialog->exec();
}

void View2DWidget::switchHydrogenState() {
    std::shared_ptr<xgd::JMol> mol;
    if ((++hyBtnClickTimes) % 2) {
        mol = !(expBtnClickTimes % 2) ? xgd::JMolManager::GetInstance().getFullHydrogenInputMol()
                                      : xgd::JMolManager::GetInstance().getFullHydrogenExpandedMol();
    } else {
        mol = !(expBtnClickTimes % 2) ? xgd::JMolManager::GetInstance().getInputMol()
                                      : xgd::JMolManager::GetInstance().getExpandedMol();
    }
    syncMolToScene(mol);
}

void View2DWidget::switchSuperAtomState() {
    std::shared_ptr<xgd::JMol> mol;
    if ((++expBtnClickTimes) % 2) {
        mol = xgd::JMolManager::GetInstance().getExpandedMol();
    } else {
        mol = xgd::JMolManager::GetInstance().getInputMol();
    }
    syncMolToScene(mol);
}

void View2DWidget::reformatInputState() {
    auto mol = xgd::JMolManager::GetInstance().getCurrentMol();
    syncMolToScene(mol);
}

void View2DWidget::onAtomPicked(const size_t &_aid) {
    ui->pick_edit->setText(mol2DWidget->makeAtomInfo(_aid));
}

void View2DWidget::onBondPicked(const size_t &_bid) {
    ui->pick_edit->setText(mol2DWidget->makeBondInfo(_bid));
}
