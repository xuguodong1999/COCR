#include "box2graph.hpp"
#include "opencv_util.hpp"
#include <vector>
#include <unordered_map>

std::vector<std::string> CLASSES = {
        "Br",
        "O",
        "I",
        "S",
        "H",
        "N",
        "C",
        "B",
        "-",
        "--",
        "-+",
        "=",
        "F",
        "#",
        "Cl",
        "P",
        "[o]"};

void BoxGraphConverter::then() {
    // analysis features here
    // modify mol here

}

BoxGraphConverter::BoxGraphConverter(JMol &_mol) : mol(_mol) {
}

void BoxGraphConverter::accept(const std::vector<gt_box> &_boxes, const cv::Mat &_img) {
    // construct features here
    if (_boxes.empty()) {
        mol.clear();
        return;
    } else if (_boxes.size() == 1) {
        int label = _boxes[0].label;
        switch (getTypeFromLabelIdx(label)) {
            case ItemType::CircleBond:
                mol.addAtom(6);// 误分类氧原子为苯环里的圈
                break;
            case ItemType::ExplicitAtom:
                mol.addAtom(0)->setElementType(CLASSES[label]);
                break;
            case ItemType::LineBond:
                auto a1 = mol.addAtom(6);
                auto a2 = mol.addAtom(6);
                mol.addBond(a1->getId(), a2->getId());// FIXME: 写到这里
        }
        return;
    }
    const size_t box_num = _boxes.size();
    using float_index_type = std::tuple<size_t, size_t, float>;
    std::vector<float_index_type> distanceGrid;
    for (size_t i = 0; i < box_num; i++) {
        for (size_t j = i + 1; j < box_num; j++) {
            distanceGrid.emplace_back(i, j, getDistance2D(
                    getRectCenter2D(_boxes[i].bBox), getRectCenter2D(_boxes[j].bBox)));
        }
    }
    // <box_id,[<box_id,feature=(cur_dis/min_dis,cur_dis/avg(min_dis)),>,...]>
    std::unordered_map<size_t, std::vector<std::pair<size_t, float>>> tmp;
    then();
}

BoxGraphConverter::ItemType BoxGraphConverter::getTypeFromLabelIdx(const int &_label) {
    switch (_label) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 12:
        case 14:
        case 15:
            return ItemType::ExplicitAtom;
        case 8:
        case 9:
        case 10:
        case 11:
        case 13:
            return ItemType::LineBond;
        case 16:
            return ItemType::CircleBond;
        default: {
            std::cerr << "unknow label" << _label
                      << "in BoxGraphConverter::getTypeFromLabelIdx" << std::endl;
            exit(-1);
        }
    }
}

std::pair<cv::Point2f, cv::Point2f>
BoxGraphConverter::getFromTo4LineBond(const cv::Rect2d &_rect, const cv::Mat &_img) {
    float k = _rect.width / _rect.height;
    const float kThresh = 10;
    if (k > kThresh) {

    } else if (k < 1.0 / kThresh) {

    }
    auto mask = _img(_rect);// FIXME: 处理可能的越界

    return std::pair<cv::Point2f, cv::Point2f>();
}
