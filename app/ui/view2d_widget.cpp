#include "view2d_widget.h"
#include "ui_view2d_widget.h"

View2DWidget::View2DWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::View2DWidget)
{
    ui->setupUi(this);
}

View2DWidget::~View2DWidget()
{
    delete ui;
}
