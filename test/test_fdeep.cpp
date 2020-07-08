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
}