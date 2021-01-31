#include "qt_molfileeditor.hpp"

#include "./ui_qt_molfileeditor.h"

#include <QClipboard>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

MolFileEditor::MolFileEditor(
        const QString &_suffix, const QString &_content, QWidget *parent)
        : QWidget(parent), ui(new Ui::MolFileEditor),
          formatSuffix(_suffix), content(_content) {
    ui->setupUi(this);
    ui->textEdit->setText(_content);
    ui->textEdit->setReadOnly(true);
    setWindowTitle("Molecule Format in " + formatSuffix);
    connect(ui->saveBtn, &QToolButton::clicked, this, &MolFileEditor::saveAs);
    connect(ui->copyBtn, &QToolButton::clicked, this, &MolFileEditor::copyAs);
}

MolFileEditor::~MolFileEditor() {
    delete ui;
}

const QString &MolFileEditor::getFormatSuffix() const {
    return formatSuffix;
}

void MolFileEditor::setFormatSuffix(const QString &formatSuffix) {
    MolFileEditor::formatSuffix = formatSuffix;
}

const QString &MolFileEditor::getContent() const {
    return content;
}

void MolFileEditor::setContent(const QString &content) {
    MolFileEditor::content = content;
}

void MolFileEditor::saveAs() {
    QFileDialog fileDialog;
    fileDialog.setModal(true);
    QString saveName = fileDialog.getSaveFileName(
            nullptr, "", QApplication::applicationDirPath(), QString("*." + formatSuffix)
    );
    if(QFileInfo(saveName).fileName().isEmpty()){
        return;
    }
    if (QFileInfo(saveName).suffix().isEmpty()) {
        saveName += "." + formatSuffix;
    }
    QFile file(saveName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(nullptr, "FileDialog",
                                 "Fail to save",
                                 QMessageBox::Yes);
    } else {
        file.write(content.toLocal8Bit());
        file.close();
        QMessageBox::information(nullptr, "FileDialog",
                                 "Save success",
                                 QMessageBox::Yes);
    }
}

void MolFileEditor::copyAs() {
    QClipboard *clipboard = QApplication::clipboard();
    QString originalText = clipboard->text();
    clipboard->setText(content);
    QMessageBox::information(nullptr, "Copy To Clipboard", "Success",
                             QMessageBox::Yes);
}
