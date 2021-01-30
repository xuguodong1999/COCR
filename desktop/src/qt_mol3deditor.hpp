#ifndef _MOL_3D_EDITOR_HPP_
#define _MOL_3D_EDITOR_HPP_

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Mol3DEditor; }
QT_END_NAMESPACE

class Mol3DEditor : public QWidget {
Q_OBJECT

public:
    Mol3DEditor(QWidget *parent = nullptr);

    ~Mol3DEditor();

private:
    Ui::Mol3DEditor *ui;
};

#endif//_MOL_3D_EDITOR_HPP_
