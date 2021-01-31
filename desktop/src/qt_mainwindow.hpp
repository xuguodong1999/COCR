#ifndef _MAIN_WINDOW_HPP_
#define _MAIN_WINDOW_HPP_

#include "mol.hpp"
#include <QMainWindow>
#include <QLabel>
#include <QtWidgets/QHBoxLayout>

class Mol2DEditor;

class Mol3DEditor;

class SketchWidget;

class OCRThread;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT
    bool isSketchLatest;
    Mol2DEditor *mol2DEditor;
    Mol3DEditor *mol3DEditor;
    SketchWidget *sketchWidget;
    OCRThread *ocrThread;
    std::vector<std::shared_ptr<JMol>> mols;
    QLabel *waitLabel;
    QWidget *currentWidget;
    QHBoxLayout *mainLayout;
public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void setMainWidget(QWidget *target);

private slots:

    void openSketchWidget();

    void open2DRawEditor();

    void open2DNormEditor();

    void open3DEditor();

    void tryOpen2DRawEditor();

    void tryOpen2DNormEditor();

    void tryOpen3DEditor();
};

#endif//_MAIN_WINDOW_HPP_