#ifndef _SKETCHVIEW_H_
#define _SKETCHVIEW_H_

#include "cocr.h"
#include <QGraphicsView>


class SketchView : public QGraphicsView {
Q_OBJECT
public:
    explicit SketchView(QWidget *parent = nullptr);

    explicit SketchView(QGraphicsScene *scene, QWidget *parent = nullptr);

    ~SketchView();

protected:
    void focusOutEvent(QFocusEvent *event) override;

    void focusInEvent(QFocusEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

signals:

    void hideFromTop();

    void showAtTop();
};


#endif
