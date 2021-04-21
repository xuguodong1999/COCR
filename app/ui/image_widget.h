#ifndef IMAGE_WIDGET_H
#define IMAGE_WIDGET_H

#include <QWidget>

namespace Ui {
    class ImageWidget;
}
class QImage;

class ImageWidget : public QWidget {
Q_OBJECT
    std::shared_ptr<QImage> image;
public:
    explicit ImageWidget(QWidget *parent = nullptr);

    ~ImageWidget();

private:
    Ui::ImageWidget *ui;
signals:

    void sig_ocr_btn_clicked(const QImage &_image);

    void sig_modified();

public slots:

    void clearImage();
};

#endif // IMAGE_WIDGET_H
