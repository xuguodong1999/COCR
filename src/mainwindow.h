#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QHash>

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

    void newSketchWidget(const QString &docName = "");
};

#endif // MAINWINDOW_H
