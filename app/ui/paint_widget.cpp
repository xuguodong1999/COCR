#include "paint_widget.h"
#include "ui_paint_widget.h"
#include "application.hpp"
#include "sketch_widget.hpp"
#include <QDebug>
#include <QPainterPath>

PaintWidget::PaintWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::PaintWidget) {
    ui->setupUi(this);
    auto attach_sketch_board = [&]() {
        auto l = new QHBoxLayout(ui->container);
        sketchWidget = new SketchWidget(ui->container, &mPen, &mBgColor);
        l->addWidget(sketchWidget);
        ui->container->setLayout(l);
    };
    attach_sketch_board();

    readConfigurations();
    connect(ui->dot_btn, &QToolButton::clicked, this, &PaintWidget::increaseDotSize);
    connect(ui->color_btn, &QToolButton::clicked, this, &PaintWidget::increaseColorMode);
    connect(ui->reset_btn, &QToolButton::clicked, sketchWidget, &SketchWidget::reset);
    connect(ui->ocr_btn, &QToolButton::clicked, [&]() {
        emit sig_ocr_btn_clicked(sketchWidget->getScript());
    });
}

void PaintWidget::readConfigurations() {
    mPenWidthIndex = leafxyApp->getSettings().value(
            "paint_widget/pen_width_index", DEFAULT_PEN_WIDTH_INDEX).toInt();
    mColorModeIndex = leafxyApp->getSettings().value(
            "paint_widget/color_mode_index", DEFAULT_COLOR_MODE_INDEX).toInt();

    auto&[bgColor, penColor]=mColorModeOptions[mColorModeIndex];
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
    updateDotIcon();
    updateColorIcon();
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
    ui->dot_btn->setIconSize(dotBgSize * 0.7);
    ui->dot_btn->setIcon(dotIcon);
}


void PaintWidget::increaseDotSize() {
    mPenWidthIndex = (mPenWidthIndex + 1) % mPenWidthOptions.size();
    leafxyApp->getSettings().setValue("paint_widget/pen_width_index", mPenWidthIndex);

    mPen.setWidthF(mPenWidthOptions[mPenWidthIndex]);

    updateDotIcon();
    updateColorIcon();
    sketchWidget->sync();
}

void PaintWidget::increaseColorMode() {
    mColorModeIndex = (mColorModeIndex + 1) % mColorModeOptions.size();
    leafxyApp->getSettings().setValue("paint_widget/color_mode_index", mColorModeIndex);
    auto&[bgColor, penColor]=mColorModeOptions[mColorModeIndex];
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
    ui->color_btn->setIconSize(modeBgSize * 0.7);
    ui->color_btn->setIcon(modeIcon);
}


bool PaintWidget::isLatest() const {
    return sketchWidget->IsLatest();
}

void PaintWidget::setIsLatest(bool isLatest) {
    sketchWidget->setIsLatest(isLatest);
}

const ui_script_type &PaintWidget::getScript() const {
    return sketchWidget->getScript();
}