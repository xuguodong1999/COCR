#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "itemform.h"
#include "switchbutton.h"
#include "sketchwidget.h"
#include <QMainWindow>
#include <QMdiArea>
#include <QHash>
#include <QPair>
#include <QColorDialog>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *e) override;
    void keyPressEvent(QKeyEvent*event)override;

private:
    Ui::MainWindow *ui;
    QMdiArea *mdiArea;
    QColorDialog *colorDialog;
    ItemForm *itemForm;
    QString currentItem;
    SwitchButton *modeSwitchButton;
    SketchWidget *currentSketchWidget;
    QHash<SketchWidget *, QString> sketchWidgetNameMap;
    using StrPair = QPair<QString, QString>;
    const QList<StrPair> items = {
            StrPair("笔", "笔工具"),
            StrPair("C", "通过点击鼠标或者按空格键在焦点位置放下碳原子"),
            StrPair("H", "通过点击鼠标或者按空格键在焦点位置放下氢原子"),
            StrPair("O", "通过点击鼠标或者按空格键在焦点位置放下氧原子"),
            StrPair("N", "通过点击鼠标或者按空格键在焦点位置放下氮原子"),
            StrPair("P", "通过点击鼠标或者按空格键在焦点位置放下磷原子"),
            StrPair("S", "通过点击鼠标或者按空格键在焦点位置放下硫原子"),
            StrPair("/", "拖曳出一条表示单键的线段"),
            StrPair("//", "拖曳出表示双键的两条线段"),
            StrPair("///", "拖曳出表示三键的三条线段"),
            StrPair("▲", "拖曳出一条表示单键的实心楔形键"),
            StrPair("△", "拖曳出一条表示单键的空心楔形键"),
            StrPair("⌬", "通过点击鼠标或者按空格键在焦点位置放下苯环"),
    };

    void setupSketchControl();

    SketchWidget *newSketchWidget(const QString &docName = "");

private slots:

    void showAboutDialog();
};

#endif // MAINWINDOW_H
