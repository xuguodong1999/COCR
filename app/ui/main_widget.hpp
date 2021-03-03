#ifndef _MAIN_WIDGET_HPP_
#define _MAIN_WIDGET_HPP_

#include <QWidget>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    Ui::MainWidget *ui;
};

#endif // _MAIN_WIDGET_HPP_
