#ifndef PAINT_WIDGET_H
#define PAINT_WIDGET_H

#include <QWidget>

namespace Ui {
class PaintWidget;
}

class PaintWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PaintWidget(QWidget *parent = nullptr);
    ~PaintWidget();

private:
    Ui::PaintWidget *ui;
};

#endif // PAINT_WIDGET_H
