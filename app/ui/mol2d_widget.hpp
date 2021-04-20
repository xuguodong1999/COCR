#ifndef _XGD_MOL2D_WIDGET_HPP_
#define _XGD_MOL2D_WIDGET_HPP_

#include "jmol.hpp"
#include "gesture_view.hpp"
#include <QGraphicsScene>
#include <QGraphicsView>

class WaitHintWidget;

class Mol2DWidget : public GestureView {
Q_OBJECT
    QGraphicsScene *scene;
    std::shared_ptr<xgd::JMol> mol;
    WaitHintWidget *hintWidget;
public:
    Mol2DWidget(QWidget *parent = nullptr);

    void syncMolToScene(std::shared_ptr<xgd::JMol> _mol);

    void startWaitHint();

protected:
    void mouseDoubleClickEvent(QMouseEvent *) override;

    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void normalizeMol();

    void endWaitHint();
};

#endif//_XGD_MOL2D_WIDGET_HPP_
