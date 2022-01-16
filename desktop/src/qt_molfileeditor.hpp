#ifndef _MOL_File_EDITOR_HPP_
#define _MOL_File_EDITOR_HPP_
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MolFileEditor; }
QT_END_NAMESPACE

class MolFileEditor : public QWidget {
Q_OBJECT
    QString formatSuffix, content;
public:
    const QString &getFormatSuffix() const;

    void setFormatSuffix(const QString &formatSuffix);

    const QString &getContent() const;

    void setContent(const QString &content);

    MolFileEditor(const QString &_suffix, const QString &_content,
                  QWidget *parent = nullptr);

    ~MolFileEditor();

private:
    Ui::MolFileEditor *ui;
private slots:

    void saveAs();

    void copyAs();
};
#endif//_MOL_File_EDITOR_HPP_