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

private:
    Ui::FormatDialog *ui;
};

#endif // _XGD_UI_FORMAT_DIALOG_H_
