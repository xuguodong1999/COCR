#include "fontpixitem.hpp"
#include "opencvutil.hpp"
#include <QImage>
#include <QFont>
#include <QPainter>
#include <QDebug>
#include <QFontDatabase>
#include <opencv2/opencv.hpp>

cv::Mat FontPixItem::GetFont(const QString &_text, const QString &_fontFamily) {
    const int baseSize = 128;
    QFont font;
    font.setPixelSize(baseSize);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 0);
    font.setFamily(_fontFamily);
    font.setWeight(rand() % 100);
    font.setItalic(rand() % 2);
//    qDebug()<< _text<<":"<<font.family();
    QFontMetricsF fontMetrics(font);
    qreal fontWidth = fontMetrics.horizontalAdvance(_text);
    if (font.italic()) {
        fontWidth *= 1.5;
    }
    qreal fontHeight = fontMetrics.height();
    QImage image(fontWidth, fontHeight, QImage::Format_Grayscale8);
    image.fill(Qt::white);
    QPainter painter(&image);
    painter.setFont(font);
    painter.drawText(QRect(0, 0, fontWidth, fontHeight),
                     Qt::AlignCenter, _text);
    cv::Mat cvImg(image.height(), image.width(),
                  CV_8UC1, (void *) image.constBits(), image.bytesPerLine());
    auto rectPtr = getBoundBoxForBWFont(cvImg);
    if (rectPtr) {
        cvImg = cvImg(cv::Rect(rectPtr.value()));
    }
//    cv::imshow("1",cvImg);
//    cv::waitKey(0);
    return std::move(cvImg);
}