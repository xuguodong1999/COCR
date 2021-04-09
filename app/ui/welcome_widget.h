#ifndef WELCOME_WIDGET_H
#define WELCOME_WIDGET_H

#include <QWidget>

namespace Ui {
class WelcomeWidget;
}

class WelcomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeWidget(QWidget *parent = nullptr);
    ~WelcomeWidget();

private:
    Ui::WelcomeWidget *ui;
};

#endif // WELCOME_WIDGET_H
