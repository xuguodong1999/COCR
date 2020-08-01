#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QHash>
#include <QPair>

namespace Ui {
    class MainWindow;
}
class SketchWidget;

class SwitchButton;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *e) override;

private:
    Ui::MainWindow *ui;
    QMdiArea *mdiArea;
    SwitchButton *modeSwitchButton;
    SketchWidget *currentSketchWidget;
    QHash<SketchWidget *, QString> sketchWidgetNameMap;
    using StrPair = QPair<QString, QString>;
    const QList<StrPair> items = {
            StrPair("pen", "scribble some strokes"),
            StrPair("C", "draw a carbon"),
            StrPair("H", "draw a hydrogen"),
            StrPair("O", "draw a oxygen"),
            StrPair("N", "draw a nitrogen"),
            StrPair("P", "draw a phosphorus"),
            StrPair("S", "draw a sulfur"),
            StrPair("/", "draw a single line"),
            StrPair("//", "draw a double line"),
            StrPair("///", "draw a triple line"),
            StrPair("▲", "draw a solid wedge line"),
            StrPair("△", "draw a dotted wedge line"),
            StrPair("⌬", "draw a benzene ring"),
    };

    void setUpSketchCtrl();

    SketchWidget *newSketchWidget(const QString &docName = "");

private slots:

    void showAboutDialog();
};

#endif // MAINWINDOW_H
