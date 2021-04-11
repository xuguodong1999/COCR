#ifndef PAINT_WIDGET_H
#define PAINT_WIDGET_H

#include <QWidget>
#include <QPen>
#include <vector>
#include <QPainter>

namespace Ui {
    class PaintWidget;
}
class SketchWidget;

class PaintWidget : public QWidget {
Q_OBJECT

public:
    explicit PaintWidget(QWidget *parent = nullptr);

    ~PaintWidget();

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void closeEvent(QCloseEvent *event) override;

private:
    Ui::PaintWidget *ui;
    SketchWidget *sketchWidget;
    QPen mPen;
    QPoint lastPos;
    QPixmap bufPixmap;
    QPainter painter;
    QColor mBgColor;

    int mPenWidthIndex;
    const int DEFAULT_PEN_WIDTH_INDEX = 1;
    const std::vector<qreal> mPenWidthOptions = {2, 3, 5, 8, 12};

    int mColorModeIndex;
    const int DEFAULT_COLOR_MODE_INDEX = 0;
    const std::vector<std::pair<QColor, QColor>> mColorModeOptions = {
            {Qt::white,     Qt::black},
            {Qt::black,     Qt::white},
            {Qt::lightGray, Qt::black},
            {Qt::darkGray,  Qt::white}
    };

    void updateDotIcon();

    void updateColorIcon();


    void readConfigurations();

private slots:

    void increaseColorMode();

    void increaseDotSize();
};

#endif // PAINT_WIDGET_H
