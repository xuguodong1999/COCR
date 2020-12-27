#ifndef _MOL2D_HPP_
#define _MOL2D_HPP_

#include "mol.hpp"

#include <opencv2/core/types.hpp>

#include <optional>
/**
 * 使用开源库实现的需求
 * 1、2D坐标标准化
 * 2、力场 or 3D坐标标准化
 * 3、SMILES IO（用于debug）
 * 4、LSSR 和 SSSR 算法找环
 */
/**
 * 改造策略：
 * 基类持有所有数据，通过装饰器添加数据生成函数，一律进行虚继承
 * 使用键值对<枚举类型，bool>来判断数据是否填充完成，基本开销是一个 unordered_map
 * 继承树的设计需求：
 * 1、装饰两次和装饰一次的效果一致：通过重写构造和拷贝运算符实现
 * 2、任意不同的装饰器可以不冲突地随意嵌套：虚继承，内建表处理多重、多层继承导致的冲突
 * 3、不需要支持向父类转型，但是可以通过一次装饰完成向任意深度地继承树节点的转化
 * 4、基类持有钩子函数暴露子节点的接口，通过继承树上的类型提升完成 API 的分发
 * 5、如果没有在基类添加类型分发，那么必须明确通过子类调用具体功能
 * 6、任意节点的指针能够以枚举的方式返回自己在继承树叶子层的功能列表
 * 分析：
 * OpenBabel 用组合的方式拓展一个基类，类图混乱，不容易处理依赖关系
 * RDKit 使用只读类和可写类，问题是 API 设计混乱
 * 利用装饰器的多次继承平衡开发效率和执行效率
 * 需要透出给调试器的能力：
 * 1、Mol2D里面附着文字信息
 * 2、Mol3D里面附着空间坐标系和基本图元的法向量，标准法向量需要有计算（没有开源实现）
 */

class Mol2D {
    std::shared_ptr<JMol> mol;
    //2D 原子坐标 <id,<isExplicit, position>>
    std::unordered_map<size_t, std::pair<bool, cv::Point2f>> atomPosMap2D;
public:
    Mol2D(std::shared_ptr<JMol> _mol);

    std::unordered_map<size_t, cv::Point2f> calcCoord2D(bool _updateAtomPosMap2D = false);

    float calcAvgBondLength() const;

    void insertAtomPos2D(const size_t &_aid, bool _isExplicit, const cv::Point2f &_pos);
};

#endif//_MOL2D_HPP_
