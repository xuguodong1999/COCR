#ifndef _MOL_2D_EDITOR_HPP_
#define _MOL_2D_EDITOR_HPP_

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Mol2DEditor; }
QT_END_NAMESPACE

class Mol2DEditor : public QWidget {
    Q_OBJECT
public:
    Mol2DEditor(QWidget *parent = nullptr);

    ~Mol2DEditor();

private:
    Ui::Mol2DEditor *ui;
};

#endif//_MOL_2D_EDITOR_HPP_