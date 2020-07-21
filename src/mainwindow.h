#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QToolButton>
namespace Ui {
class MainWindow;
}
class SketchWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    SketchWidget* sketchWidget;
    QMdiArea*mdiArea;
    QToolButton*dragButton,*drawButton;
};

#endif // MAINWINDOW_H
