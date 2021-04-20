#include "welcome_widget.h"
#include "ui_welcome_widget.h"
#include "application.hpp"
#include <QCheckBox>

WelcomeWidget::WelcomeWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::WelcomeWidget) {
    ui->setupUi(this);
    ui->agree_box->setCheckState(leafxyApp->getSettings().value(
            KEY_IS_AGREE, false).toBool() ? Qt::Checked : Qt::Unchecked);
    connect(ui->agree_box, &QCheckBox::stateChanged, [&](int status) {
        bool checked = (status == Qt::Checked);
        emit sig_agree_box_checked(checked);
        leafxyApp->getSettings().setValue(KEY_IS_AGREE, checked);
    });
}

WelcomeWidget::~WelcomeWidget() {
    delete ui;
}

bool WelcomeWidget::isAgreed() const {
    return ui->agree_box->isChecked();
}

