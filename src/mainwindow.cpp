#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sketchwidget.h"
#include "switchbutton.h"
#include "itemform.h"

#include <QDateTime>
#include <QMessageBox>
#include <QColorDialog>
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), currentSketchWidget(nullptr),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
    auto animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(300);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
#ifdef  Q_OS_ANDROID
    ui->menubar->hide();
    ui->statusbar->hide();
#endif
    mdiArea = ui->mdiArea;

    modeSwitchButton = new SwitchButton(ui->switchContainer);

    setUpSketchCtrl();

    connect(ui->newSketchButton, &QToolButton::clicked, [&]() { newSketchWidget(); });
    connect(ui->clearSceneButton, &QToolButton::clicked, [&]() {
        if (currentSketchWidget != nullptr) {
            currentSketchWidget->clearScene();
            currentSketchWidget->update();
        }
    });
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);
    showMaximized();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
    modeSwitchButton->resize(ui->switchContainer->size());
}

SketchWidget *MainWindow::newSketchWidget(const QString &docName) {
    auto sketchWidget = new SketchWidget();
    connect(modeSwitchButton, &QToolButton::clicked, sketchWidget, &SketchWidget::switchMode);
    mdiArea->addSubWindow(sketchWidget, Qt::FramelessWindowHint);
    sketchWidget->showMaximized();
    sketchWidget->switchMode(modeSwitchButton->isChecked());
    // 点击MDI子窗口的关闭图标的时候，从map中删掉关联
    connect(sketchWidget, &SketchWidget::closed, [&]() {
        sketchWidgetNameMap.remove(sketchWidget);
    });
    // 一旦画图区域得到焦点，把当前绘图区指针指向这个窗体
    connect(sketchWidget, &SketchWidget::focusStateChanged, this, [&](bool focusIn) {
        if (focusIn) {
            currentSketchWidget = qobject_cast<SketchWidget *>(sender());
        }
    });
    if (docName.isEmpty()) {
        QString tmpDocName = "file-" + QDateTime::currentDateTime()
                .toString(Qt::DateFormat::ISODate)
                .replace(QRegExp("\\s|/|:"), "-");
        sketchWidget->setWindowTitle(tmpDocName);
        sketchWidgetNameMap.insert(sketchWidget, tmpDocName);
    } else {
        sketchWidget->setWindowTitle(docName);
        sketchWidgetNameMap.insert(sketchWidget, docName);
    }
    currentSketchWidget = sketchWidget;
    return sketchWidget;
}

void MainWindow::showAboutDialog() {
    QString qtVersionHint = tr(
            "<h3>About cocr</h3>"
            "<p>This program uses Qt version %1.</p>"
    ).arg(QLatin1String(QT_VERSION_STR));
    const QString aboutInfo = tr(
            "<p>Application cocr is a tool for chemistry.</p>"
            "<p>It provides easy access to chemical formats with ocr capacity.</p>"
            "<p>You may click <a href=\"http://%0/wasm/%2\">%2</a> "
            "to try cocr on a web browser.</p>"
            "<p>Copyright (C) %1 Guodong Xu. All rights reserved.</p>"
    ).arg(qUncompress(
            QByteArray::fromHex("0000000d789c3331d7333430d6333230d133b1040011ca028d")),
          QStringLiteral("2020"),
          QStringLiteral("cocr.html"));
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setWindowTitle(tr("About cocr"));
    msgBox->setText(qtVersionHint);
    msgBox->setInformativeText(aboutInfo);
    QPixmap pm(":/img/cocr.png");
    if (!pm.isNull())
        msgBox->setIconPixmap(pm);
    msgBox->exec();
}

void MainWindow::setUpSketchCtrl() {
    connect(ui->widthSlider, &QSlider::valueChanged, this, [&](int value) {
        if (currentSketchWidget != nullptr) {
            currentSketchWidget->setPenWidth(qMax(1, qRound(value / 4.0)));
            ui->widthHintLabel->setText(QString("线宽:%0px").arg(currentSketchWidget->getPenWidth()));
        }
    });
    static auto colorDialog = new QColorDialog(this);
    connect(ui->colorButton, &QToolButton::clicked, colorDialog, [&]() {
        colorDialog->resize(size() / 2);
        colorDialog->show();
    });
    connect(colorDialog, &QColorDialog::colorSelected,
            this, [&](const QColor &color) {
                QPixmap tmp(ui->colorButton->size());
                tmp.fill(color);
                ui->colorButton->setIcon(QIcon(tmp));
                if (currentSketchWidget != nullptr) {
                    currentSketchWidget->setPenColor(color);
                }
            }
    );
    static auto itemForm = new ItemForm(items, this);
    itemForm->hide();
    connect(itemForm, &ItemForm::itemChanged, this, [&](const QString&item) {
        ui->shapeButton->setText(item);
    });
    ui->shapeButton->setFont(QFont("simhei",ui->shapeButton->width()/3));
    connect(ui->shapeButton, &QToolButton::clicked, this, [&](bool checked) {
        if (itemForm->isHidden()) {
            itemForm->move(ui->mdiArea->x(),
                           ui->drawCtrlContainer->y());
            itemForm->resize(mdiArea->width() / 5, mdiArea->height());
            itemForm->show();
        } else {
            itemForm->hide();
        }
    });
    auto sketchWidget = newSketchWidget();

    colorDialog->colorSelected(Qt::black);
    ui->widthSlider->setValue(10);
    itemForm->clickFirstItem();
}
