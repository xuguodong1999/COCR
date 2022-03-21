#pragma once

#include "ui/gesture_view.hpp"
#include <QGraphicsScene>
#include <QGraphicsView>

class WaitHintWidget;
namespace cocr {
    class JMol;
}
class Mol2DWidget : public GestureView {
Q_OBJECT
    QGraphicsScene *scene;
    std::shared_ptr<cocr::JMol> mol;
    WaitHintWidget *hintWidget;
public:
    Mol2DWidget(QWidget *parent = nullptr);

    void syncMolToScene(std::shared_ptr<cocr::JMol> _mol);

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