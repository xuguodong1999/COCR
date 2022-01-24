#pragma once
#include <QDialog>

namespace Ui {
    class FormatDialog;
}

class FormatDialog : public QDialog {
Q_OBJECT

public:
    explicit FormatDialog(QWidget *parent = nullptr);

    ~FormatDialog();

    void setFormat(const QString &_formatName);

    void setFormatContent(const QString &_formatContent);

private:
    Ui::FormatDialog *ui;
private slots:

    void saveAs();

    void copyAs();
};