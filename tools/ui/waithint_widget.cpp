#include "waithint_widget.h"
#include "ui_waithint_widget.h"
#include <QMovie>

WaitHintWidget::WaitHintWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::WaitHintWidget) {
    ui->setupUi(this);
    // 无边框
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    // 背景透明
    setAttribute(Qt::WA_TranslucentBackground);
}

WaitHintWidget::~WaitHintWidget() {
    delete ui;
}

void WaitHintWidget::startWaitHint() {
    // 长宽相等
    int pw = parentWidget()->width(), ph = parentWidget()->height();
    int showSize = std::max(64, std::min(pw, ph)) / 2;
    qDebug() << showSize;
    resize(showSize, showSize);
    // 居中
    move((pw - showSize) / 2, (ph - showSize) / 2);
    qDebug() << size();
    show();
    // 加载 gif
    QMovie *movie = new QMovie(":/img/waiting.gif");
    ui->gifLabel->setMovie(movie);
    movie->setScaledSize(ui->gifLabel->size());
    movie->start();
}
