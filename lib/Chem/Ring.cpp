#include "XGD/Chem/Ring.h"

Ring::Ring() {
}

void Ring::addBond(const id_type &_bid) {
    bondIds.insert(_bid);
}
