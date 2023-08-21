#include "PaintWidget.h"
#include "ui_PaintWidget.h"

#include "App.h"
#include "SketchWidget.h"

#include <QDebug>
#include <QTimer>
#include <QPainterPath>
#include <cmath>

PaintWidget::PaintWidget(QWidget *parent) : QWidget(parent), ui(new Ui::PaintWidget) {
    ui->setupUi(this);

    auto l = new QHBoxLayout(ui->container);
    sketchWidget = new SketchWidget(ui->container, &mPen, &mBgColor);
    l->addWidget(sketchWidget);
    ui->container->setLayout(l);

    readConfigurations();

    connect(sketchWidget, &SketchWidget::sig_modified, [&]() { Q_EMIT sig_modified(); });
    connect(ui->dot_btn, &QToolButton::clicked, this, &PaintWidget::increaseDotSize);
    connect(ui->color_btn, &QToolButton::clicked, this, &PaintWidget::increaseColorMode);
    connect(ui->reset_btn, &QToolButton::clicked, sketchWidget, &SketchWidget::reset);
    connect(ui->ocr_btn, &QToolButton::clicked, [&]() {
        Q_EMIT sig_ocr_btn_clicked(sketchWidget->getScript());
    });
    connect(ui->undo_btn, &QToolButton::clicked, sketchWidget, &SketchWidget::undo);
    connect(ui->redo_btn, &QToolButton::clicked, sketchWidget, &SketchWidget::redo);
}

void PaintWidget::readConfigurations() {
    mPenWidthIndex = cocrSettings.value(
            KEY_PEN_WIDTH_INDEX, DEFAULT_PEN_WIDTH_INDEX).toInt();
    mColorModeIndex = cocrSettings.value(
            KEY_PEN_COLOR_INDEX, DEFAULT_COLOR_MODE_INDEX).toInt();

    auto &[bgColor, penColor] = mColorModeOptions[mColorModeIndex];
    mBgColor = bgColor;
    mPen.setColor(penColor);
    mPen.setWidthF(mPenWidthOptions[mPenWidthIndex]);
    mPen.setJoinStyle(Qt::RoundJoin);
    mPen.setCapStyle(Qt::RoundCap);
    mPen.setStyle(Qt::PenStyle::SolidLine);
}

PaintWidget::~PaintWidget() {
    delete ui;
}


void PaintWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    QTimer::singleShot(24, [&]() {
        updateDotIcon();
        updateColorIcon();
    });
}

void PaintWidget::updateDotIcon() {
    QSize dotBgSize = ui->dot_btn->size();
    qreal showDotSize = (std::min)(dotBgSize.width(), dotBgSize.height()) *
                        (mPen.widthF() / mPenWidthOptions.back()) / 2.2;
    QPainterPath ellipsePath;
    ellipsePath.addEllipse(QPointF(dotBgSize.width(), dotBgSize.height()) / 2, showDotSize, showDotSize);
    QPixmap dotPixmap(dotBgSize);
    dotPixmap.fill(mBgColor);
    QPainter dotPainter(&dotPixmap);
    dotPainter.fillPath(ellipsePath, QBrush(mPen.color()));

    QIcon dotIcon(dotPixmap);
    ui->dot_btn->setIconSize(dotBgSize * 0.8);
    ui->dot_btn->setIcon(dotIcon);
}


void PaintWidget::increaseDotSize() {
    mPenWidthIndex = (mPenWidthIndex + 1) % mPenWidthOptions.size();
    cocrSettings.setValue(KEY_PEN_WIDTH_INDEX, mPenWidthIndex);

    mPen.setWidthF(mPenWidthOptions[mPenWidthIndex]);

    updateDotIcon();
    updateColorIcon();
    sketchWidget->sync();
}

void PaintWidget::increaseColorMode() {
    mColorModeIndex = (mColorModeIndex + 1) % mColorModeOptions.size();
    cocrSettings.setValue(KEY_PEN_COLOR_INDEX, mColorModeIndex);
    auto &[bgColor, penColor] = mColorModeOptions[mColorModeIndex];
    mBgColor = bgColor;
    mPen.setColor(penColor);

    updateColorIcon();
    updateDotIcon();
    sketchWidget->sync();
}

void PaintWidget::updateColorIcon() {
    QSize modeBgSize = ui->color_btn->size();

    QPainterPath strokePath;
    strokePath.moveTo(0, modeBgSize.height());
    strokePath.cubicTo(QPointF(modeBgSize.width() / 3, -modeBgSize.height()),
                       QPointF(modeBgSize.width() / 3 * 2, 2 * modeBgSize.height()),
                       QPointF(modeBgSize.width(), 0));

    QPixmap modePixmap(modeBgSize);
    modePixmap.fill(mBgColor);
    QPainter modePainter(&modePixmap);
    modePainter.setPen(mPen);
    modePainter.drawPath(strokePath);

    QIcon modeIcon(modePixmap);
    ui->color_btn->setIconSize(modeBgSize * 0.8);
    ui->color_btn->setIcon(modeIcon);
}


const ui_script_type &PaintWidget::getScript() const {
    return sketchWidget->getScript();
}

void PaintWidget::saveLastScript() {
    sketchWidget->saveLastScript();
}
