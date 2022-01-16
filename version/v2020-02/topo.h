#ifndef TOPO_H
#define TOPO_H

#include "bimap.h"
#include "itemfactory.h"

class QGraphicsScene;

class ItemTopo {
public:
    ItemTopo();

    int insertAtom(const char *className, const char *memberName, const QPointF &p);

    int insertBond(const char *className);

    void setFromTo(int bid, int from, int to);

    bool FromNull(int bid);

    bool ToNull(int bid);

    int getAtomNum();

    int getBondNum();

    void clear();

    void setToScene(QGraphicsScene *scene);

private:
    std::pair<int, int> index;

    int getNextAtomId();

    int getNextBondId();

    BiMap<QGraphicsItem *, int> abMap;

    int insert(QGraphicsItem *atomItem, const QPointF &p);

    int insert(QGraphicsItem *bondItem);
};

#endif // TOPO_H
