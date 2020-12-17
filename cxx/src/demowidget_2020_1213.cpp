#include "demowidget_2020_1213.h"

#include "opencv_yolo.hpp"
#include "qt_util.hpp"
#include "box2graph.hpp"
#include "mol_hw.hpp"

#include "mol3d.hpp"
#include "std_util.hpp"
#include "mol3dwindow.hpp"

#include "hw.hpp"
#include "colors.hpp"

#include <QDebug>
#include <QMessageBox>
#include <QApplication>

#include <exception>
#include <iostream>

OpenCVYolo yolo;
ShapeItem shapes;

DemoWidget_2020_1213::DemoWidget_2020_1213(QWidget *parent) : QWidget(parent) {
    setMinimumSize(960, 640);
    panelWidget = new QWidget(this);
    panelLayout = new QHBoxLayout(panelWidget);
//    panelLayout->setSpacing(3);
    auto addBtn = [&](QToolButton *&btn, const char *text) {
        btn = new QToolButton(panelWidget);
        btn->setText(text);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        panelLayout->addWidget(btn, 1);
    };
    addBtn(drawBtn, "绘制");
//    addBtn(view2dBtn, "2D");
    addBtn(view3dBtn, "3D");
//    addBtn(saveBtn, "保存");
    addBtn(clsBtn, "清空");

    scribbleArea = new ScribbleArea(this);
    view3dContainer = nullptr;

    connect(drawBtn, &QToolButton::clicked, this, [&]() {
        viewMode = ViewMode::ScribbleMode;
        if (view3dContainer != nullptr) {
            view3dContainer->hide();
            view3dContainer->deleteLater();
            view3dContainer = nullptr;
        }
        scribbleArea->show();
    });
//    connect(view2dBtn, &QToolButton::clicked, this, [&]() {
//        scribbleArea->hide();
//        viewMode = ViewMode::View2DMode;
//        if (view3dContainer != nullptr) {
//            view3dContainer->hide();
//            view3dContainer->deleteLater();
//            view3dContainer = nullptr;
//        }
//    });
    connect(view3dBtn, &QToolButton::clicked, this, [&]() {
        if(!yolo.isWeightsLoaded()){
            std::string path = "C:/Users/xgd/source/COCR/weights/";
//    std::string path=qApp->applicationDirPath().toLocal8Bit().toStdString();
            yolo.init(":/soso17-yolov4-tiny-3l.cfg",
                      (path + "/yolov4-tiny-3l_92000.weights").c_str());
        }
        scribbleArea->hide();
        viewMode = ViewMode::View3DMode;
        if (view3dContainer != nullptr) {
            view3dContainer->deleteLater();
        }
        view3dContainer = new QWidget(this);
        auto sizeM = size();
        int ww = (std::max)(300, sizeM.width() / 2);
        int hh = (std::max)(100, ww / 8);
        view3dContainer->resize(sizeM.width(), sizeM.height() - hh);
        auto &script = scribbleArea->getPtsVec();
        if (script.empty()) {
            view3dContainer->show();
            return;
        }
        shapes.mData.clear();
        for (auto &stroke:script) {
            shapes.mData.push_back(Stroke());
            for (auto &pt:stroke) {
                shapes.mData.back().push_back(cv::Point2f(pt.x(), pt.y()));
            }
        }
        auto shapeBox = shapes.getBoundingBox();
        shapes.moveLeftTopTo(cv::Point2f(4, 4));
        std::vector<gt_box> gtBox;
        cv::Mat detMat;
        std::vector<std::shared_ptr<JMol>> mols;
        try {
            cv::Mat originMat = cv::Mat(shapeBox.height + 8, shapeBox.width + 8, CV_8UC3,
                                        getScalar(ColorName::rgbWhite));
            shapes.paintTo(originMat);
            auto[tmpBox, tmpMat] = yolo.forward(originMat, false);
            gtBox = std::move(tmpBox);
            detMat = std::move(tmpMat);

            float avgBoxSize = 0.1;
            for (auto &box:gtBox) {
                avgBoxSize += (std::max)(box.bBox.width, box.bBox.height);
            }
            avgBoxSize /= gtBox.size();
            float k = 60 / avgBoxSize;
            std::cout<<"60 / avgBoxSize="<<k<<std::endl;
            if ((0.2<k&&k<0.8)||(1.25<k&&k<5)) {
                // 放缩重检
                originMat = cv::Mat();
                shapes.mulK(k, k);
                shapeBox = shapes.getBoundingBox();
                shapes.moveLeftTopTo(cv::Point2f(4, 4));
                originMat = cv::Mat(shapeBox.height + 8,
                                    shapeBox.width + 8, CV_8UC3,
                                    getScalar(ColorName::rgbWhite));
                shapes.paintTo(originMat);
                auto[tmpBox, tmpMat] = yolo.forward(originMat, false);
                gtBox = std::move(tmpBox);
                detMat = std::move(tmpMat);
            }
            std::cout<<"detection done"<<std::endl;
            BoxGraphConverter converter(false);
            mols = converter.accept(gtBox, detMat);
            std::cout<<"extract done"<<std::endl;
        }
        catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
//            exit(-1);
            QMessageBox::information(nullptr, "Hello, Bug", e.what());
            view3dContainer->show();
            return;
        }
        auto layout = new QHBoxLayout(view3dContainer);
        for (auto &mol:mols) {
//            std::cout << mol->toSMILES(false) << std::endl;
            auto rootEntity = new Qt3DCore::QEntity();
            auto sceneBuilder = new Mol3D(rootEntity);
            auto view = new Mol3DWindow(rootEntity);
            try {
                sceneBuilder->resetMol(mol);
            } catch (std::exception&e) {
                std::cerr<<e.what()<<std::endl;
                rootEntity->deleteLater();
                QMessageBox::information(nullptr, "Hello, Bug", e.what());
                continue;
            }
            auto container = QWidget::createWindowContainer(view);
            layout->addWidget(container, 1);
//            container->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
//            container->showMaximized();
        }
        view3dContainer->show();
    });
//    connect(saveBtn, &QToolButton::clicked, this, [&]() {
//        switch (viewMode) {
//            case ViewMode::ScribbleMode: {
//                break;
//            }
//            case ViewMode::View2DMode: {
//                break;
//            }
//            case ViewMode::View3DMode: {
//                break;
//            }
//            default:
//                break;
//        }
//    });
    connect(clsBtn, &QToolButton::clicked, this, [&]() {
        scribbleArea->clear();
        drawBtn->click();
    });
    drawBtn->click();
}

DemoWidget_2020_1213::~DemoWidget_2020_1213() {}

void DemoWidget_2020_1213::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    auto sizeM = event->size();
    int w = (std::max)(300, sizeM.width() / 2);
    int h = (std::max)(100, w / 8);
    panelWidget->move((sizeM.width() - w) / 2, sizeM.height() - h);
    panelWidget->resize(w, h);
    scribbleArea->resize(sizeM.width(), sizeM.height() - h);
}

void ScribbleArea::updateRect(const QPoint &p1, const QPoint &p2) {
    int xmin, ymin, xmax, ymax;
    if (p2.x() < p1.x()) {
        xmin = p2.x();
        xmax = p1.x();
    } else {
        xmax = p2.x();
        xmin = p1.x();
    }
    if (p2.y() < p1.y()) {
        ymin = p2.y();
        ymax = p1.y();
    } else {
        ymax = p2.y();
        ymin = p1.y();
    }
    update(QRect(QPoint(xmin - penWidth, ymin - penWidth),
                 QPoint(xmax + penWidth, ymax + penWidth)));
}

ScribbleArea::ScribbleArea(QWidget *parent) : QWidget(parent) {
    penWidth = 3;
    penColor = Qt::black;
    pen.setWidth(penWidth);
    pen.setColor(penColor);
}

void ScribbleArea::paintEvent(QPaintEvent *event) {
    QRect rect = event->rect();
    QPainter bufPainter(this);
    bufPainter.setRenderHint(QPainter::Antialiasing);
    bufPainter.drawPixmap(rect, bufPixmap, rect);
}

void ScribbleArea::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    lastPos = event->pos();
    painter.begin(&bufPixmap);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawLine(lastPos, lastPos);
    updateRect(lastPos, lastPos);
    ptsVec.push_back(std::vector<QPoint>());
    ptsVec.back().push_back(lastPos);
}

void ScribbleArea::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
    auto currentPos = event->pos();
    painter.drawLine(lastPos, currentPos);
    updateRect(currentPos, lastPos);
    lastPos = currentPos;
    ptsVec.back().push_back(lastPos);
}

void ScribbleArea::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);
    painter.end();
}

void ScribbleArea::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    syncPtsVec();
}

void ScribbleArea::syncPtsVec() {
    bufPixmap = QPixmap(size());
    bufPixmap.fill(Qt::white);
    if (bufPixmap.width() == 0 || bufPixmap.height() == 0) {
        return;
    }
    painter.begin(&bufPixmap);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    for (auto &pts : ptsVec) {
        if (pts.size() == 0) {
            continue;
        } else if (pts.size() == 1) {
            painter.drawPoint(pts[0]);
            continue;
        }
        for (size_t i = 1; i < pts.size(); i++) {
            painter.drawLine(pts[i - 1], pts[i]);
        }
    }
    painter.end();
    update();
}

void ScribbleArea::clear() {
    ptsVec.clear();
    syncPtsVec();
}

const std::vector<std::vector<QPoint>> &ScribbleArea::getPtsVec() const {
    return ptsVec;
}
