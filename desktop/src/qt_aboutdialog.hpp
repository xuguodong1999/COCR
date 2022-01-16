#ifndef _ABOUT_DIALOG_HPP_
#define _ABOUT_DIALOG_HPP_

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class AboutDialog; }
QT_END_NAMESPACE

class AboutDialog : public QDialog {
    Q_OBJECT
public:
    AboutDialog(QWidget *parent = nullptr);

    ~AboutDialog();

private:
    Ui::AboutDialog *ui;
};

#endif//_ABOUT_DIALOG_HPP_