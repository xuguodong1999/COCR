#ifndef _XGD_MOL2D_WIDGET_HPP_
#define _XGD_MOL2D_WIDGET_HPP_

#include "jmol.hpp"
#include <QGraphicsScene>
#include <QGraphicsView>

class Mol2DWidget : public QGraphicsView {
    QGraphicsScene *scene;
    std::shared_ptr<xgd::JMol> mol;
public:
    Mol2DWidget(QWidget *parent = nullptr, std::shared_ptr<xgd::JMol> _mol = nullptr);

    void syncMolToScene();
};

#endif//_XGD_MOL2D_WIDGET_HPP_
