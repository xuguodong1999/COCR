#ifndef _ITEMFORM_H_
#define _ITEMFORM_H_

#include <QFrame>
#include <QHash>
#include <QToolButton>
#include <QScrollArea>

class ItemForm : public QScrollArea {
Q_OBJECT

public:
    explicit ItemForm(const QList<QPair<QString, QString>> &items, QWidget *parent = nullptr);

    ~ItemForm();

    void clickFirstItem();

signals:

    void itemChanged(const QString &item);

protected:
    void resizeEvent(QResizeEvent *e) override;

private:
    QHash<QString, QString> hint;
    QToolButton *firstItemButton, *currentPressedButton;
    QWidget *container;
};

#endif // _ITEMFORM_H_
