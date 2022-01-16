#include "qt_util.hpp"
#include "opencv_util.hpp"

#include <opencv2/imgproc.hpp>

#include <QFont>
#include <QImage>
#include <QPainter>
#include <QtGui/QVector3D>

#include <iostream>

using namespace std;

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

cv::Mat convertQImageToMat(const QImage &_img) {
    cv::Mat mat;
    if (_img.isNull()) {
        std::cerr << "you are converting an empty QImage to cv::Mat" << std::endl;
        exit(-1);
    }
    void *dataPtr = const_cast<uchar *>(_img.constBits());
    int step = _img.bytesPerLine();
    switch (_img.format()) {
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(_img.height(), _img.width(), CV_8UC4, dataPtr, step);
            break;
        case QImage::Format_RGB888:
            mat = cv::Mat(_img.height(), _img.width(), CV_8UC3, dataPtr, step);
            cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
            break;
        case QImage::Format_Indexed8:
            mat = cv::Mat(_img.height(), _img.width(), CV_8UC1, dataPtr, step);
            break;
        default: {
            std::cerr << "unImpl image format:" << _img.format() << std::endl;
            exit(-1);
        }
    }
//    std::cout << mat.cols << "x" << mat.rows << std::endl;
    return std::move(mat);
}

cv::Mat convertQPixmapToMat(const QPixmap &_img) {
    return convertQImageToMat(_img.toImage());
}

const QVector3D
        axisX(1, 0, 0),
        axisY(0, 1, 0),
        axisZ(0, 0, 1),
        zeroP(0, 0, 0);

const std::vector<QVector3D> tetrahedral = {
        QVector3D(0, 0, 0.75 * std::sqrt(2)),
        QVector3D(0, -1, -0.25 * std::sqrt(2)),
        QVector3D(std::sqrt(3) / 2, 0.5, -0.25 * std::sqrt(2)),
        QVector3D(-std::sqrt(3) / 2, 0.5, -0.25 * std::sqrt(2))
};

