#include "itemform.h"
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QTextBrowser>

ItemForm::ItemForm(const QList<QPair<QString, QString>> &items, QWidget *parent)
        : QScrollArea(parent), firstItemButton(nullptr), currentPressedButton(nullptr) {
    setFont(QFont("Arial"));
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setFrameShape(Shape::Box);
    setFrameShadow(Shadow::Raised);
    auto layout = new QGridLayout(this);
    auto hint1 = new QLabel(this);
    hint1->setAlignment(Qt::AlignCenter);
    hint1->setText("控件");
    hint1->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    auto hint2 = new QLabel(this);
    hint2->setText("说明");
    hint2->setAlignment(Qt::AlignCenter);
    hint2->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(hint1, 0, 0);
    layout->addWidget(hint2, 0, 1);
    layout->setRowStretch(0, 1);
    for (int i = 0; i < items.size(); i++) {
        auto button = new QToolButton(this);
        button->setCheckable(true);
        if (i == 0) { firstItemButton = button; }
        button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        button->setText(items[i].first);
        connect(button, &QToolButton::clicked, this, [&]() {
            if (currentPressedButton != nullptr) {
                currentPressedButton->setChecked(false);
            }
            currentPressedButton = qobject_cast<QToolButton *>(sender());
            currentPressedButton->setChecked(true);
            emit itemChanged(qobject_cast<QToolButton *>(sender())->text());
        });
        hint[items[i].first] = items[i].second;
//        auto label = new QLabel(this);
//        label->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
//        label->setText(items[i].second);
        layout->addWidget(button, i + 1, 0);
//        layout->addWidget(label, i + 1, 1);
        layout->setRowStretch(i + 1, 2);
    }
    static auto pTextBrowser = new QTextBrowser(this);
    layout->addWidget(pTextBrowser, 1, 1, items.size(), 1);
    connect(this, &ItemForm::itemChanged, this, [&](const QString &text) {
        pTextBrowser->setText(hint[text]);
    });
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 2);
    layout->setSpacing(3);
    container = new QWidget(this);
    container->setLayout(layout);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidget(container);
}

ItemForm::~ItemForm() {
}

void ItemForm::clickFirstItem() {
    if (firstItemButton != nullptr) {
        firstItemButton->click();
    }
}

void ItemForm::resizeEvent(QResizeEvent *e) {
    QScrollArea::resizeEvent(e);
    container->resize(width() * 0.9, container->height());
}
