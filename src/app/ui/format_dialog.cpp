#include "format_dialog.h"
#include "ui_format_dialog.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>

FormatDialog::FormatDialog(QWidget *parent) : QDialog(parent), ui(new Ui::FormatDialog) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->back_btn, &QToolButton::clicked, this, &QDialog::close);
    connect(ui->save_btn, &QToolButton::clicked, this, &FormatDialog::saveAs);
    connect(ui->copy_btn, &QToolButton::clicked, this, &FormatDialog::copyAs);
}

FormatDialog::~FormatDialog() {
    qDebug() << __FUNCTION__;
    delete ui;
}

void FormatDialog::setFormat(const QString &_formatName) {
    setWindowTitle(_formatName);
}

void FormatDialog::setFormatContent(const QString &_formatContent) {
    ui->info_edit->setText(_formatContent);
}

void FormatDialog::copyAs() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->info_edit->toPlainText());
    QMessageBox::information(
            nullptr, tr("Copy To Clipboard"), tr("Success"), QMessageBox::Yes);
    close();
}

void FormatDialog::saveAs() {
    QString formatSuffix = windowTitle();
    QFileDialog fileDialog;
    fileDialog.setModal(true);
    QString saveName = fileDialog.getSaveFileName(
            nullptr, "", QDir::homePath(), QString("*." + formatSuffix)
    );
    if (QFileInfo(saveName).fileName().isEmpty()) {
        return;
    }
    if (QFileInfo(saveName).suffix().isEmpty()) {
        saveName += "." + formatSuffix;
    }
    QFile file(saveName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(
                nullptr, tr("FileDialog"), tr("Fail to save"), QMessageBox::Yes);
    } else {
        file.write(ui->info_edit->toPlainText().toLocal8Bit());
        file.close();
        QMessageBox::information(
                nullptr, tr("FileDialog"), tr("Save success"), QMessageBox::Yes);
        close();
    }
}
