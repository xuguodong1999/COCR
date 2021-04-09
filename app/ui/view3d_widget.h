#ifndef VIEW3D_WIDGET_H
#define VIEW3D_WIDGET_H

#include <QWidget>

namespace Ui {
class View3DWidget;
}

class View3DWidget : public QWidget
{
    Q_OBJECT

public:
    explicit View3DWidget(QWidget *parent = nullptr);
    ~View3DWidget();

private:
    Ui::View3DWidget *ui;
};

#endif // VIEW3D_WIDGET_H
