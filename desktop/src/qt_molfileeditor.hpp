#ifndef _MOL_File_EDITOR_HPP_
#define _MOL_File_EDITOR_HPP_
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MolFileEditor; }
QT_END_NAMESPACE

class MolFileEditor : public QWidget
{
Q_OBJECT

public:
    MolFileEditor(QWidget *parent = nullptr);
    ~MolFileEditor();

private:
    Ui::MolFileEditor *ui;
};
#endif//_MOL_File_EDITOR_HPP_
