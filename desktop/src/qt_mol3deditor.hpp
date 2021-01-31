#ifndef _MOL_3D_EDITOR_HPP_
#define _MOL_3D_EDITOR_HPP_

#include "mol.hpp"
#include <QWidget>
#include <QHBoxLayout>
QT_BEGIN_NAMESPACE
namespace Ui { class Mol3DEditor; }
QT_END_NAMESPACE

class Mol3DEditor : public QWidget {
    Q_OBJECT
    QHBoxLayout*layout;
    std::vector<QWidget*>widgets;
public:
    Mol3DEditor(QWidget *parent = nullptr);

    ~Mol3DEditor();

    void setMols(const std::vector<std::shared_ptr<JMol>>&_mols);

private:
    Ui::Mol3DEditor *ui;
};

#endif//_MOL_3D_EDITOR_HPP_