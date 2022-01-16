#include "qt_mol3deditor.hpp"

#include "./ui_qt_mol3deditor.h"

#include "mol3d_builder.hpp"
#include "mol3dwindow.hpp"
#include "mol_util.hpp"
#include "qt_molfileeditor.hpp"
#include <QComboBox>
#include <QMessageBox>
#include <QErrorMessage>

extern std::shared_ptr<MolUtil> molUtil;

Mol3DEditor::Mol3DEditor(QWidget *parent)
        : QWidget(parent), ui(new Ui::Mol3DEditor) {
    ui->setupUi(this);
    layout = new QHBoxLayout(ui->container);
    ui->container->setLayout(layout);
    auto formats = molUtil->getFormatChoices();
    for (auto &format:formats) {
        // skip Read-only formats
        if (format.find("Read-only") == std::string::npos) {
            ui->formatBox->addItem(format.substr(
                    0, format.find(" ")).c_str());
        }
    }
    connect(ui->formatBox, &QComboBox::textActivated, this, &Mol3DEditor::onFormatSelected);
}

Mol3DEditor::~Mol3DEditor() {
    delete ui;
}

void Mol3DEditor::setMols(const std::vector<std::shared_ptr<JMol>> &_mols) {
    mols = _mols;
    for (auto &widget:widgets) {
        layout->removeWidget(widget);
        widget->deleteLater();
        widget = nullptr;
    }
    widgets.clear();
    for (auto &mol:_mols) {
        auto rootEntity = new Qt3DCore::QEntity();
        auto molBuilder = new Mol3DBuilder(rootEntity, mol);
        try {
            if (!molBuilder->build())continue;
        } catch (std::exception &e) {
            qDebug() << e.what();
            continue;
        }
        auto view = new Mol3DWindow(rootEntity);
        auto container = QWidget::createWindowContainer(view);
        widgets.push_back(container);
        layout->addWidget(container);
    }
}

void Mol3DEditor::onFormatSelected(QString index) {
    if (mols.empty()) {
        QErrorMessage dialog;
        dialog.setModal(true);
        dialog.setWindowTitle(QObject::tr("empty molecule"));
        dialog.showMessage(QObject::tr("no molecule detected"));
        dialog.exec();
    } else {
        QDialog formatPanel;
        auto formatLayout = new QHBoxLayout(&formatPanel);
        for (auto &mol:mols) {
            if (mol->atomsNum() == 0)continue;
            auto formatBox = new MolFileEditor(index, molUtil->getFormat(
                    *mol, index.toStdString()).c_str());
            formatLayout->addWidget(formatBox);
        }
        formatPanel.setLayout(formatLayout);
//        formatPanel->setWindowModality(Qt::WindowModal);
        formatPanel.resize(size() / 2);
        formatPanel.move(x() + width() / 4, y() + height() / 4);
        formatPanel.setModal(true);
        formatPanel.exec();
    }
}
