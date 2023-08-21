#include "WaitHintWidget.h"
#include "ui_WaitHintWidget.h"

#include <QMovie>

WaitHintWidget::WaitHintWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::WaitHintWidget) {
    ui->setupUi(this);
    // frameless
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    // transparent bg
    setAttribute(Qt::WA_TranslucentBackground);
}

WaitHintWidget::~WaitHintWidget() {
    delete ui;
}

void WaitHintWidget::startWaitHint() {
    // align to size x size
    int pw = parentWidget()->width(), ph = parentWidget()->height();
    int showSize = std::max(64, std::min(pw, ph)) / 2;
    qDebug() << showSize;
    resize(showSize, showSize);
    // align center
    move((pw - showSize) / 2, (ph - showSize) / 2);
    qDebug() << size();
    show();
    QMovie *movie = new QMovie(":/images/waiting.gif");
    ui->gifLabel->setMovie(movie);
    movie->setScaledSize(ui->gifLabel->size());
    movie->start();
}
