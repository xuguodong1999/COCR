#ifndef _MAIN_WINDOW_HPP_
#define _MAIN_WINDOW_HPP_

#include "mol.hpp"
#include <QMainWindow>

class Mol2DEditor;

class Mol3DEditor;

class SketchWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT
    Mol2DEditor *mol2DEditor;
    Mol3DEditor *mol3DEditor;
    SketchWidget *sketchWidget;
    std::vector<std::shared_ptr<JMol>> mols;
public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private:
    Ui::MainWindow *ui;
private slots:
    void openSketchWidget();
void open2DRawEditor();
void open2DNormEditor();
void open3DEditor();
};

#endif//_MAIN_WINDOW_HPP_
