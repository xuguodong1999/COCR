#ifndef _BOX_2_GRAPH_HPP_
#define _BOX_2_GRAPH_HPP_

#include "gt_box.hpp"
#include "mol.hpp"
#include "bond_type.hpp"
#include <utility>

class BoxGraphConverter {
    // <idx1,idx2,feature>
    using float_index_type = std::tuple<size_t, size_t, float>;
    // 低阶距离特征：atom-bond,bond-bond,bond-circle,atom-circle
    std::vector<float_index_type> abDisGrid, bbDisGrid, bcDisGrid, acDisGrid;
    using callback_type=std::function<void(const size_t &, const size_t &, const float &)>;
    /**
     * 遍历低阶距离特征
     * @param _func 回调函数，处理索引坐标、特征为 {size_t,size_t,float} 的关系
     */
    void featureTraverse(const callback_type &_func);

    /**
     * analysis features and construct a jmol
     * call by BoxGraphConverter::accept
     */
    void then();

    JMol &mol;
    enum ItemType {
        ExplicitAtom = 0x100,
        LineBond = 0x010,
        CircleBond = 0x001
    };

    ItemType getTypeFromLabelIdx(const int &_label);

    JBondType getBondTypeFromLabelIdx(const int&_label);

    std::pair<cv::Point2f, cv::Point2f> getFromTo4LineBond(
            const gt_box&_gtBox, const cv::Mat &_img);

public:
    /**
     * take charge of this JMol
     * @param _mol
     */
    BoxGraphConverter(JMol &_mol);

    /**
     * extract features from boxes and img
     * construct the jmol taken charge of before
     * @param _boxes
     * @param _img
     */
    void accept(const std::vector<gt_box> &_boxes, const cv::Mat &_img);
};

#endif //_BOX_2_GRAPH_HPP_
