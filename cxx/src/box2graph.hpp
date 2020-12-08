#ifndef _BOX_2_GRAPH_HPP_
#define _BOX_2_GRAPH_HPP_

#include "gt_box.hpp"
#include "mol.hpp"
#include <utility>

class BoxGraphConverter {
    /**
     * analysis features and construct a jmol
     * call by BoxGraphConverter::accept
     */
    void then();

    JMol &mol;
    enum class ItemType{
        ExplicitAtom,
        LineBond,
        CircleBond
    };
    ItemType getTypeFromLabelIdx(const int&_label);

    std::pair<cv::Point2f,cv::Point2f> getFromTo4LineBond(
            const cv::Rect2d&_rect,const cv::Mat &_img);
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
