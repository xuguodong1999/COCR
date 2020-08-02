#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QMessageBox>
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
    modeSwitchButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->switchContainer->layout()->addWidget(modeSwitchButton);
    setupSketchControl();

    connect(ui->newSketchButton, &QToolButton::clicked, [&]() { newSketchWidget(); });
    connect(ui->clearSceneButton, &QToolButton::clicked, [&]() {
        if (currentSketchWidget != nullptr) {
            currentSketchWidget->clearScene();
            currentSketchWidget->update();
        }
    });
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);
    newSketchWidget();
    ui->widthSlider->setValue(3);
    showMaximized();// 先确定窗体布局
    colorDialog->colorSelected(Qt::black);
    itemForm->clickFirstItem();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
    ui->colorButton->setIconSize(ui->colorButton->size() / 2);
    itemForm->resize(mdiArea->width() / 5, mdiArea->height());
    colorDialog->resize(size() / 2);
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
        QString tmpDocName = "doc-" + QDateTime::currentDateTime()
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

void MainWindow::setupSketchControl() {
    connect(ui->widthSlider, &QSlider::valueChanged, this, [&](int value) {
        if (currentSketchWidget != nullptr) {
            currentSketchWidget->setPenWidth(value);
            ui->widthHintLabel->setText(
                    QString("w:%0px").arg(currentSketchWidget->getPenWidth(), 2, 10, QChar(' ')));
        }
    });
    colorDialog = new QColorDialog(this);
    connect(ui->colorButton, &QToolButton::clicked, colorDialog, [&]() {
        colorDialog->show();
    });
    connect(colorDialog, &QColorDialog::colorSelected,
            this, [&](const QColor &color) {
                QPixmap tmp(ui->colorButton->iconSize());
                tmp.fill(color);
                ui->colorButton->setIcon(QIcon(tmp));
                if (currentSketchWidget != nullptr) {
                    currentSketchWidget->setPenColor(color);
                }
            }
    );
    ui->shapeButton->setCheckable(true);
    itemForm = new ItemForm(items, this);
    itemForm->hide();
    connect(itemForm, &ItemForm::itemChanged, this, [&](const QString &item) {
        currentItem = item;
        ui->shapeButton->setText(currentItem);         // 改按钮图标
        if (currentSketchWidget != nullptr) {   // 改鼠标形状
            QCursor cursor;
            if (currentItem == "笔") {
                int w = qMax(24, qMin(currentSketchWidget->width(), currentSketchWidget->height() / 10));
                cursor = QCursor(QPixmap(cocr::getPenCursorXpm()).scaled(w, w), 0, w);
            } else {
                cursor.setShape(Qt::CrossCursor);
            }
            currentSketchWidget->setCursor(cursor);
        }
    });
    connect(ui->shapeButton, &QToolButton::clicked, this, [&](bool checked) {
        if (itemForm->isHidden()) {
            itemForm->move(ui->mdiArea->x(), ui->drawCtrlContainer->y());
            itemForm->show();
        } else {
            itemForm->hide();
        }
    });
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    QWidget::keyPressEvent(event);
    if (currentSketchWidget == nullptr || currentItem.isEmpty()) { return; }
    switch (event->key()) {
        case Qt::Key_Space:
//            currentSketchWidget->
            break;
        default:
            break;
    }
}
