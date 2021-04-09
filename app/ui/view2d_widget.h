#ifndef VIEW2D_WIDGET_H
#define VIEW2D_WIDGET_H

#include <QWidget>

namespace Ui {
class View2DWidget;
}

class View2DWidget : public QWidget
{
    Q_OBJECT

public:
    explicit View2DWidget(QWidget *parent = nullptr);
    ~View2DWidget();

private:
    Ui::View2DWidget *ui;
};

#endif // VIEW2D_WIDGET_H
