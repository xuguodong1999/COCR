#ifndef _BOX_2_GRAPH_HPP_
#define _BOX_2_GRAPH_HPP_

#include "gt_box.hpp"
#include "mol.hpp"

class BoxGraphConverter {
    /**
     * analysis features and construct a jmol
     * call by BoxGraphConverter::accept
     */
    void then();

    JMol &mol;
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
