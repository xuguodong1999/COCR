#ifndef WELCOME_WIDGET_H
#define WELCOME_WIDGET_H

#include <QWidget>

namespace Ui {
    class WelcomeWidget;
}

class WelcomeWidget : public QWidget {
Q_OBJECT
    const char *KEY_IS_AGREE = "welcome_widget/is_agreed";
public:
    explicit WelcomeWidget(QWidget *parent = nullptr);

    ~WelcomeWidget();

    bool isAgreed() const;

private:
    Ui::WelcomeWidget *ui;
signals:

    void sig_agree_box_checked(bool isChecked);
};

#endif // WELCOME_WIDGET_H
