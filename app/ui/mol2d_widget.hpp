#ifndef _XGD_MOL2D_WIDGET_HPP_
#define _XGD_MOL2D_WIDGET_HPP_

#include "jmol.hpp"
#include "gesture_view.hpp"
#include <QGraphicsScene>
#include <QGraphicsView>

class Mol2DWidget : public GestureView {
Q_OBJECT
    QGraphicsScene *scene;
    std::shared_ptr<xgd::JMol> mol;

public:
    Mol2DWidget(QWidget *parent = nullptr);

    void syncMolToScene(std::shared_ptr<xgd::JMol> _mol);

protected:
    void mouseDoubleClickEvent(QMouseEvent *) override;

    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void normalizeMol();
};

#endif//_XGD_MOL2D_WIDGET_HPP_
