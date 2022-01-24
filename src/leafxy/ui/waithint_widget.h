#pragma once
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