#ifndef IMAGE_WIDGET_H
#define IMAGE_WIDGET_H

#include <QWidget>

namespace Ui {
class ImageWidget;
}

class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageWidget(QWidget *parent = nullptr);
    ~ImageWidget();

private:
    Ui::ImageWidget *ui;
};

#endif // IMAGE_WIDGET_H
