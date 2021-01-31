#ifndef _SKETCH_MANAGER_HPP_
#define _SKETCH_MANAGER_HPP_

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class SketchManager; }
QT_END_NAMESPACE

class SketchManager : public QWidget {
    Q_OBJECT
public:
    SketchManager(QWidget *parent = nullptr);

    ~SketchManager();

private:
    Ui::SketchManager *ui;
};

#endif//_SKETCH_MANAGER_HPP_