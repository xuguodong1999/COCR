#include <gtest/gtest.h>

#include "cocr.h"
#include <fdeep/fdeep.hpp>
#include <QImage>
#include <QDebug>
#include <QCoreApplication>
#include <QApplication>
#include <exception>
#include <QPixmap>

TEST(Yolov3TestSuite, Yolov3Test) {
    using namespace fdeep;
    using namespace cocr;
    // gtest 使用qimage，报读文件错or加载qpixmap错
    auto model = load_model("../../res/weights/mv2-fdeep.json", true);
    EXPECT_EQ(model.hash(), "c2d823efd655ebeba39b3e48cb1071f6c24d49d75279c2c51f2385905ab1d196");

//    using namespace fdeep;
//    using namespace cocr;
//    auto model = load_model("../../res/weights/mv2-fdeep.json", false);
//    QImage img("../../res/testcase/1a0n.jpg");
//    img = img.convertToFormat(QImage::Format::Format_RGB666)
//            .scaled(mod32(img.width()), mod32(img.height()));
//    tensors input({tensor_from_bytes(img.constBits(), img.width(), img.height(), 3)});
//    auto output = model.predict(input);
//    auto res = show_tensors(output);
//
//    auto &scale32 = output.at(0);
//    LogName(show_tensor_shape(scale32.shape()));
//    auto &scale16 = output.at(1);
//    auto &scale8 = output.at(2);
//
//    auto vec32 = scale32.to_vector();
//    auto vec16 = scale16.to_vector();
//    auto vec8 = scale8.to_vector();
////    reverse(vec32.begin(),vec32.end());
////    reverse(vec16.begin(),vec16.end());
////    reverse(vec8.begin(),vec8.end());
//    Yolov3Layer yolov3({
//                               {10,  10},
//                               {5,   30},
//                               {30,  5},
//                               {30,  30},
//                               {15,  90},
//                               {90,  15},
//                               {60,  60},
//                               {30,  180},
//                               {180, 30},
//                       }, 11, 3, 32, 32);
//    yolov3.log();
//    list<RectObj> result;
//    yolov3.handleOneScale(result, vec32, 0, img.width(), img.height());
//    yolov3.handleOneScale(result, vec16, 1, img.width(), img.height());
//    yolov3.handleOneScale(result, vec8, 2, img.width(), img.height());
//    LogName(result.size());
//    nms(result, 0.5, 0.5);
////    show(result);
//    LogName(result.size());
//    QPainter painter(&img);
//    painter.setPen(QPen(Qt::black, 2));
//    QRectF outline(0, 0, img.width(), img.height());
//    for (auto &o:result) {
//        show(o);
//        auto rect = o.toQRectF();
//        if (outline.contains(rect))
//            painter.drawRect(rect);
//    }
//    QLabel label;
//    label.resize(img.size());
//    label.setPixmap(QPixmap::fromImage(img));
//    label.show();
//
//
////    auto getOutputSize = [](const tensor &t) {
////        return t.width() * t.height() * t.depth();
////    };
//
////    LogName(img.width());
////    LogName(img.height());
////    LogName(output.size());
////
////    LogName(getOutputSize(scale32));
////    LogName(getOutputSize(scale16));
////    LogName(getOutputSize(scale8));
////
////    LogName(show_tensor_shape(scale32.shape()));
////    LogName(show_tensor_shape(scale16.shape()));
////    LogName(show_tensor_shape(scale8.shape()));
//
////    auto &ic = cocr::IsomerCounter::getInstance();
////    ic.calculate(6);
////    cocr::show(ic.counter);
//
////    typedef unsigned char number;
////    stringstream input("6 1 "
////                       "5 1 "
////                       "2 1 "
////                       "3 1 "
////                       "7 2 "
////                       "0 3 "
////                       "0 4 ");
////    cocr::UGraph<number> ut;
////    input >> ut;
////    LogName(ut);
////    LogName(ut.toString());
}