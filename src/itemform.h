#ifndef _ITEMFORM_H_
#define _ITEMFORM_H_

#include <QFrame>
#include <QHash>
#include <QToolButton>
class ItemForm : public QFrame {
Q_OBJECT

public:
    explicit ItemForm(const QList<QPair<QString,QString>> &items, QWidget *parent = nullptr);

    ~ItemForm();

    void clickFirstItem();

signals:

    void itemChanged(const QString &item);

private:
    QHash<QString,QString> hint;
    QToolButton*firstItemButton,*currentPressedButton;
};

#endif // _ITEMFORM_H_
