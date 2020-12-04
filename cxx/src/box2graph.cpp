#include "box2graph.hpp"

void BoxGraphConverter::then() {
    // analysis features here
    // modify mol here
}

BoxGraphConverter::BoxGraphConverter(JMol &_mol) : mol(_mol) {
}

void BoxGraphConverter::accept(const std::vector<gt_box> &_boxes, const cv::Mat &_img) {
    // construct features here
    then();
}
