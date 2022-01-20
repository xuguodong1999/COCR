#ifndef _XGD_MOL2D_WIDGET_HPP_
#define _XGD_MOL2D_WIDGET_HPP_

#include "gesture_view.hpp"
#include <QGraphicsScene>
#include <QGraphicsView>

class WaitHintWidget;
namespace xgd {
    class JMol;
}
class Mol2DWidget : public GestureView {
Q_OBJECT
    QGraphicsScene *scene;
    std::shared_ptr<xgd::JMol> mol;
    WaitHintWidget *hintWidget;
public:
    Mol2DWidget(QWidget *parent = nullptr);

    void syncMolToScene(std::shared_ptr<xgd::JMol> _mol);

    void startWaitHint();

    QString makeAtomInfo(const size_t &_aid);

    QString makeBondInfo(const size_t &_bid);

protected:
    void mouseDoubleClickEvent(QMouseEvent *) override;

    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void normalizeMol();

    void endWaitHint();
};

#endif//_XGD_MOL2D_WIDGET_HPP_
