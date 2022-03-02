#include "welcome_widget.h"
#include "ui_welcome_widget.h"
#include "application.hpp"
#include <QCheckBox>
#include <QFile>
#include <QScrollBar>

WelcomeWidget::WelcomeWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::WelcomeWidget) {
    ui->setupUi(this);
    ui->agree_box->setCheckState(leafxySettings.value(
            KEY_IS_AGREE, false).toBool() ? Qt::Checked : Qt::Unchecked);
    ui->intro_edit->setText(tr(
            "<p>LEAFXY app is an OCSR (Optical Chemical Structure Recognition) toolset capable of handling handwritten samples.</p>"
            "<p>Honor of its creation belongs to Guodong Xu, an undergraduate of Nanjing University in 2021.</p>"
            "<p>LEAFXY is a proper name. It is not an an acronym for anything.</p>"));

    ui->guide_edit->setText(tr(
            "<p>Before playing with this app, you have to accept GNU GPLv3 license and check the box beside.</p>"
            "<p>Then, you may click the DRAW tab below to open a sketch board.</p>"
            "<p>You can draw chemical structure formulas and click OCR button to recognize your script.</p>"
            "<p>For hello world, draw a N-butane with three continuous line segments, click the purple.</p>"
            "<p>A few moments later, you can see result from VIEW3D tab after a simple molecule dynamic job.</p>"
            "<p>Single, double, triple bonds, solid and hash wedge bonds, wave and circle bonds as well as element or abbreviation strings are supported.</p>"
            "<p>You can switch to VIEW2D tab to see 2d result.</p>"
            "<p>Just double-click white area or click RESET button to reformat 2d result.</p>"
            "<p>Load image from your device is also supported from FILE tab.</p>"
            "<p>CAMERA tab works in a few devices. This feature is still under development.</p>"));

    ui->agree_edit->setText(tr("LEAFXY app and its source code are licensed under GNU GPLv3."));

    connect(ui->agree_box, &QCheckBox::stateChanged, [&](int status) {
        bool checked = (status == Qt::Checked);
        emit sig_agree_box_checked(checked);
        leafxySettings.setValue(KEY_IS_AGREE, checked);
    });
}

WelcomeWidget::~WelcomeWidget() {
    delete ui;
}

bool WelcomeWidget::isAgreed() const {
    return ui->agree_box->isChecked();
}

