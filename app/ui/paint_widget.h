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

using ui_script_type = QList<QList<QPointF>>;

class PaintWidget : public QWidget {
Q_OBJECT

public:
    explicit PaintWidget(QWidget *parent = nullptr);

    ~PaintWidget();

    bool isLatest() const;

    void setIsLatest(bool isLatest);

    const ui_script_type &getScript() const;

protected:

    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::PaintWidget *ui;
    SketchWidget *sketchWidget;
    QPen mPen;
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

    /**
     * 更新线条粗细的按钮图标
     */
    void updateDotIcon();

    /**
     * 更新颜色模式的按钮图标
     */
    void updateColorIcon();

    /**
     * 从配置文件读取上一次的配置，用于初始化
     */
    void readConfigurations();

private slots:

    /**
     * 切换到下一个颜色模式
     */
    void increaseColorMode();

    /**
     * 切换到下一个线条粗细
     */
    void increaseDotSize();


signals:

    void sig_ocr_btn_clicked(const QList<QList<QPointF>> &_script);
};

#endif // PAINT_WIDGET_H
