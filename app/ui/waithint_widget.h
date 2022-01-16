#ifndef WAITHINT_WIDGET_H
#define WAITHINT_WIDGET_H

#include <QWidget>

namespace Ui {
    class WaitHintWidget;
}

class WaitHintWidget : public QWidget {
Q_OBJECT

public:
    explicit WaitHintWidget(QWidget *parent);

    ~WaitHintWidget();

    void startWaitHint();

private:
    Ui::WaitHintWidget *ui;
};

#endif // WAITHINT_WIDGET_H
