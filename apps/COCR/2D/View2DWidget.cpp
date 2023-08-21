#include "View2DWidget.h"
#include "ui_View2DWidget.h"
#include "Mol2DWidget.h"
#include "BaseItem.h"
#include "Common/FormatDialog.h"

#include "XGD/Chem/Mol.h"
#include "XGD/Chem/Atom.h"
#include "XGD/Chem/MolManager.h"
#include "XGD/Chem/MolUtil.h"

#include <QTimer>
#include <QDebug>
#include <QMessageBox>

View2DWidget::View2DWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::View2DWidget), hyBtnClickTimes(0), expBtnClickTimes(0) {
    ui->setupUi(this);

    BaseItem::SetView2DWidget(this);

    auto formatVec = MolUtil::GetWritableFormats();
    for (auto &format: formatVec) {
        if (std::string::npos != format.find("Read-only")) { continue; }
        auto suffix = format.substr(0, format.find(" "));
        if (!MolUtil::IsValidWritableFormat(suffix)) { continue; }
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

void View2DWidget::syncMolToScene(std::shared_ptr<GuiMol> _mol) {
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
    auto mol = MolManager::GetInstance().getFullHydrogenExpandedMol(false);
    if (mol) {
        bool hasSuperAtom = false;
        mol->loopAtomVec([&](Atom &atom) {
            if (ElementType::SA == atom.getType()) {
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
    formatDialog->open();
}

void View2DWidget::switchHydrogenState() {
    std::shared_ptr<GuiMol> mol;
    if ((++hyBtnClickTimes) % 2) {
        mol = !(expBtnClickTimes % 2) ? MolManager::GetInstance().getFullHydrogenInputMol()
                                      : MolManager::GetInstance().getFullHydrogenExpandedMol();
    } else {
        mol = !(expBtnClickTimes % 2) ? MolManager::GetInstance().getInputMol()
                                      : MolManager::GetInstance().getExpandedMol();
    }
    syncMolToScene(mol);
}

void View2DWidget::switchSuperAtomState() {
    std::shared_ptr<GuiMol> mol;
    if ((++expBtnClickTimes) % 2) {
        mol = MolManager::GetInstance().getExpandedMol();
    } else {
        mol = MolManager::GetInstance().getInputMol();
    }
    syncMolToScene(mol);
}

void View2DWidget::reformatInputState() {
    auto mol = MolManager::GetInstance().getCurrentMol();
    syncMolToScene(mol);
}

void View2DWidget::onAtomPicked(const size_t &_aid) {
    ui->pick_edit->setText(mol2DWidget->makeAtomInfo(_aid));
}

void View2DWidget::onBondPicked(const size_t &_bid) {
    ui->pick_edit->setText(mol2DWidget->makeBondInfo(_bid));
}
