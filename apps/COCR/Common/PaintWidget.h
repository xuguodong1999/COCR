#pragma once

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

    const char *KEY_PEN_WIDTH_INDEX = "paint_widget/pen_width_index";
    const char *KEY_PEN_COLOR_INDEX = "paint_widget/color_mode_index";
public:
    explicit PaintWidget(QWidget *parent = nullptr);

    ~PaintWidget();

    const ui_script_type &getScript() const;

    void saveLastScript();

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
     * update icon that hints pen dot size
     */
    void updateDotIcon();

    /**
     * update icon that hint fore and back ground color
     */
    void updateColorIcon();

    /**
     * init config from last session
     */
    void readConfigurations();

private Q_SLOTS:

    /**
     * switch display color
     */
    void increaseColorMode();

    /**
     * switch pen width
     */
    void increaseDotSize();


Q_SIGNALS:

    void sig_ocr_btn_clicked(const QList<QList<QPointF>> &_script);

    void sig_modified();
};