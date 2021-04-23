#ifndef _XGD_UI_FORMAT_DIALOG_H_
#define _XGD_UI_FORMAT_DIALOG_H_

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

#endif // _XGD_UI_FORMAT_DIALOG_H_
