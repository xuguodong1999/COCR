#include "itemfactory.h"
#include "template.h"

using namespace std;
using namespace item;

void InitializeItem() {
    static bool bInitialized = false;
    if (bInitialized == false) {
        static ProductRegistrar<Item, AtomItem> _a(AtomItem::getClassName());
        static ProductRegistrar<Item, SingleBondItem> _b(SingleBondItem::getClassName());
        static ProductRegistrar<Item, DoubleBondItem> _c(DoubleBondItem::getClassName());
        static ProductRegistrar<Item, TripleBondItem> _d(TripleBondItem::getClassName());
        static ProductRegistrar<Item, SolidBondItem> _e(SolidBondItem::getClassName());
        static ProductRegistrar<Item, DashBondItem> _f(DashBondItem::getClassName());
        bInitialized = true;
    }
}

ItemFactory::ItemFactory() { InitializeItem(); }

ItemFactory::~ItemFactory() {}

QGraphicsItem *ItemFactory::GetItem(const char *className, const char *memberName) {
    auto &factory = ProductFactory<Item>::Instance();
    return factory.GetProduct(className, memberName);
}

ItemFactory &ItemFactory::Instance() {
    static ItemFactory instance;
    return instance;
}
