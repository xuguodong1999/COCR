#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H

#include "item.h"
#include <memory>
/*
添加键的流程：
①在 item.h/.cpp 中添加并实现 继承BondItem/SingleBondItem 的类，在函数getClassName中设置类的字符串别名
②在 itemfactory.cpp 中的函数 void InitializeItem() 里注册你的类
③用法：ItemFactory::Instance().GetItem("className","memberName")
*/
#define NEW_ITEM(className, memberName) ItemFactory::Instance().GetItem(className,memberName)

class QGraphicsItem;

class COCR_EXPORTS ItemFactory {
public:
    static ItemFactory &Instance();

    QGraphicsItem *GetItem(const char *className, const char *memberName = "");

private:
    ItemFactory();

    ~ItemFactory();

    ItemFactory(const ItemFactory &);

    const ItemFactory &operator=(const ItemFactory &);
};

#endif // ITEMFACTORY_H
