#include "box2graph.hpp"
#include "opencv_util.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>
#include <unordered_map>

std::vector<std::string> CLASSES = {
        "Br", "O", "I", "S", "H", "N", "C", "B",
        "-", "--", "-+", "=", "F", "#", "Cl", "P", "[o]"};

std::vector<std::shared_ptr<JMol>> BoxGraphConverter::then() {
    // analysis features here
    // modify mol here
    // <box_id,[<box_id,feature=(cur_dis/min_dis,cur_dis/avg(min_dis)),>,...]>
    // 低阶距离特征：atom-bond,bond-bond,bond-circle,atom-circle
    // std::vector<float_index_type> abDisGrid, bbDisGrid, bcDisGrid, acDisGrid;
    std::vector<JMol> mols(1);
    JMol &mol(mols[0]);
}

BoxGraphConverter::BoxGraphConverter(bool _debug) {
    debug = _debug;
}

std::vector<std::shared_ptr<JMol>>
BoxGraphConverter::accept(const std::vector<gt_box> &_boxes, const cv::Mat &_img) {
    // construct features here
    const size_t box_num = _boxes.size();
    if (box_num == 0) {
        return {};
    }
    // extract bondDir as <id, <from,to>> first
    std::unordered_map<size_t, std::pair<cv::Point2f, cv::Point2f>> bondDir;
    for (size_t i = 0; i < box_num; i++) {
        if (getTypeFromLabelIdx(_boxes[i].label) != ItemType::LineBond)continue;
        bondDir[i] = getFromTo4LineBond(_boxes[i], _img);
    }
    if (box_num == 1) {
        auto mol = std::make_shared<JMol>();
        int label = _boxes[0].label;
        switch (getTypeFromLabelIdx(label)) {
            case ItemType::CircleBond: {
                auto atomOxygen = mol->addAtom(6);// 误分类氧原子为苯环里的圈
                mol->insertAtomPos2D(atomOxygen->getId(), true,
                                     getRectCenter2D(_boxes[0].bBox));
                break;
            }
            case ItemType::ExplicitAtom: {
                auto atom = mol->addAtom(0);
                atom->setElementType(CLASSES[label]);
                mol->insertAtomPos2D(atom->getId(), true,
                                     getRectCenter2D(_boxes[0].bBox));
                break;
            }
            case ItemType::LineBond: {
                auto atomFrom = mol->addAtom(6);
                auto atomTo = mol->addAtom(6);
                mol->addBond(atomFrom->getId(), atomTo->getId(),
                             getBondTypeFromLabelIdx(label));
                auto&[from, to]=bondDir[0];
                mol->insertAtomPos2D(atomFrom->getId(), false, from);
                mol->insertAtomPos2D(atomTo->getId(), false, to);
                break;
            }
            default:
                break;
        }
        return {std::move(mol)};
    }
    // 填充低阶特征
    iouGrid.resize(box_num, std::vector<float>(box_num, 0));
    for (size_t i = 0; i < box_num; i++) {
        labels[i] = _boxes[i].label;
        for (size_t j = i + 1; j < box_num; j++) {
            // 填充相交面积
            iouGrid[i][j] = iouGrid[j][i] = (_boxes[i].bBox & _boxes[j].bBox).area();
            // 填充关键距离
            ItemType itI = getTypeFromLabelIdx(_boxes[i].label);
            ItemType itJ = getTypeFromLabelIdx(_boxes[j].label);
            switch (itI | itJ) {
                case ItemType::ExplicitAtom | ItemType::LineBond: {
                    // 中心对端点二选一
                    size_t bIndex = itI == ItemType::LineBond ? i : j;
                    size_t aIndex = itI == ItemType::ExplicitAtom ? i : j;
                    auto &[from, to]=bondDir[bIndex];
                    auto &rect = _boxes[aIndex].bBox;
                    abDisGrid.emplace_back(i, j, (std::min)(
                            getDistance2D(from, cv::Point2f(getRectCenter2D(rect))),
                            getDistance2D(to, cv::Point2f(getRectCenter2D(rect)))
                    ));
                    break;
                }
                case ItemType::LineBond | ItemType::LineBond: {
                    // 端点对端点四选一
                    auto &[fromI, toI]=bondDir[i];
                    auto &[fromJ, toJ]=bondDir[j];
                    bbDisGrid.emplace_back(i, j, (std::min)(
                            (std::min)(
                                    getDistance2D(fromI, fromJ), getDistance2D(toI, toJ)
                            ), (std::min)(
                                    getDistance2D(toI, fromJ), getDistance2D(fromI, toJ)
                            )));
                    break;
                }
                case ItemType::LineBond | ItemType::CircleBond: {
                    // 中心对中心 TODO: 加入中心对端点的距离校验
                    bcDisGrid.emplace_back(i, j, getDistance2D(
                            getRectCenter2D(_boxes[i].bBox), getRectCenter2D(_boxes[j].bBox)));
                    break;
                }
                case ItemType::ExplicitAtom | ItemType::CircleBond: {
                    // 中心对中心
                    acDisGrid.emplace_back(i, j, getDistance2D(
                            getRectCenter2D(_boxes[i].bBox), getRectCenter2D(_boxes[j].bBox)));
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
    return std::move(then());
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
BoxGraphConverter::getFromTo4LineBond(const gt_box &_gtBox, const cv::Mat &_img) {
    // 是否方向无关
    bool isDirNeeded;
    switch (getBondTypeFromLabelIdx(_gtBox.label)) {
        case JBondType::SolidWedgeBond:
        case JBondType::DashWedgeBond:
            isDirNeeded = true;
            break;
        default:
            isDirNeeded = false;
    }
    const auto &rect = _gtBox.bBox;
    // 是否扁平
    float k = rect.width / rect.height;
    if (debug) {
        std::cout << rect << std::endl;
        std::cout << "k=" << k << std::endl;
    }
    const float kThresh = 2;
    const int paddingSize = 2;
    bool isVertical = k < 1.0 / kThresh;
    bool isHorizontal = k > kThresh;
    bool isLineLike = isVertical || isHorizontal;
    // RIO box
    cv::Rect2i rect2i = rect;
    // 越界处理
    rect2i.x = (std::max)(0, rect2i.x - paddingSize);
    rect2i.y = (std::max)(0, rect2i.y - paddingSize);
    rect2i.width = (std::min)(rect2i.x + _img.rows - 1, rect2i.width + 2 * paddingSize);
    rect2i.height = (std::min)(rect2i.y + _img.cols - 1, rect2i.height + 2 * paddingSize);
    // 扁平且方向无关
    if (isLineLike) {
        cv::Point2f from, to;// 返回短边中点
        if (isVertical) {
            from.x = to.x = rect.x + rect.width / 2;
            from.y = rect.y;
            to.y = rect.y + rect.height;
        } else {
            from.x = to.x;
            to.x = rect.x + rect.width;
            from.y = to.y = rect.y + rect.height / 2;
        }
        // 在输入图片边长是32的倍数的前提下，此时rect2i必然是一个边长至少为4且在图像内的框
        isDirNeeded=true;
        if (isDirNeeded) {
            // 判断是否需要交换(from,to)
            // 策略：区域二分，计算像素均值
            bool isSwapNeeded = false;// 左上为from
            cv::Scalar scalar1, scalar2;
            if (isVertical) {
                // 上下分割
                auto mask1 = _img(cv::Rect2i(rect2i.x, rect2i.y,
                                             rect2i.width, rect2i.height / 2));
                auto mask2 = _img(cv::Rect2i(rect2i.x, rect2i.y + rect2i.height / 2,
                                             rect2i.width, rect2i.height / 2));
                scalar1 = cv::mean(std::move(mask1));
                scalar2 = cv::mean(std::move(mask2));
            } else {
                // 左右分割
                auto mask1 = _img(cv::Rect2i(rect2i.x, rect2i.y,
                                             rect2i.width / 2, rect2i.height));
                auto mask2 = _img(cv::Rect2i(rect2i.x + rect2i.width / 2, rect2i.y,
                                             rect2i.width / 2, rect2i.height));
                scalar1 = cv::mean(std::move(mask1));
                scalar2 = cv::mean(std::move(mask2));
            }
            if (debug) {
                std::cout << getScalarSum(scalar1) << ", " << getScalarSum(scalar2) << std::endl;
                std::cout << "----------------\n";
            }
            // 白色像素多 -> sum 大 && 设为 from
            isSwapNeeded = getScalarSum(scalar1) < getScalarSum(scalar2);
            if (isSwapNeeded) {
                std::swap(from, to);
            }
        }
        return {from, to};
    }
    // 非扁平矩形，区域四分(1234左上右上左下右下)，统计交叉网格的特征像素计数
    auto mask1 = _img(cv::Rect2i(rect2i.x, rect2i.y,
                                 rect2i.width / 2, rect2i.height / 2));
    auto mask2 = _img(cv::Rect2i(rect2i.x + rect2i.width / 2, rect2i.y,
                                 rect2i.width / 2, rect2i.height / 2));
    auto mask3 = _img(cv::Rect2i(rect2i.x + rect2i.width / 2, rect2i.y,
                                 rect2i.width / 2, rect2i.height / 2));
    auto mask4 = _img(cv::Rect2i(rect2i.x + rect2i.width / 2,
                                 rect2i.y + rect2i.height / 2,
                                 rect2i.width / 2, rect2i.height / 2));
    auto sum1 = getScalarSum(cv::mean(std::move(mask1)));
    auto sum2 = getScalarSum(cv::mean(std::move(mask2)));
    auto sum3 = getScalarSum(cv::mean(std::move(mask3)));
    auto sum4 = getScalarSum(cv::mean(std::move(mask4)));
    if (debug) {
        std::cout << sum1 << ", " << sum2 << std::endl;
        std::cout << sum3 << ", " << sum4 << std::endl;
        std::cout << "----------------\n";
    }
    if (sum1 + sum3 > sum2 + sum4) {
        return {cv::Point2f(rect.tl()), cv::Point2f(rect.br())};
    } else {
        return {cv::Point2f(rect.x, rect.y + rect.height),
                cv::Point2f(rect.x + rect.width, rect.y)};
    }
}

void BoxGraphConverter::featureTraverse(const callback_type &_func) {
    for (const auto&[idx1, idx2, feature]:abDisGrid) {
        _func(idx1, idx2, feature);
    }
    for (const auto&[idx1, idx2, feature]:bbDisGrid) {
        _func(idx1, idx2, feature);
    }
    for (const auto&[idx1, idx2, feature]:bcDisGrid) {
        _func(idx1, idx2, feature);
    }
    for (const auto&[idx1, idx2, feature]:acDisGrid) {
        _func(idx1, idx2, feature);
    }
}

JBondType BoxGraphConverter::getBondTypeFromLabelIdx(const int &_label) {
    switch (_label) {
        case 8:
            return JBondType::SingleBond;
        case 11:
            return JBondType::DoubleBond;
        case 13:
            return JBondType::TripleBond;
        case 9:
            return JBondType::DashWedgeBond;
        case 10:
            return JBondType::SolidWedgeBond;
        default: {
            std::cerr << "error label for bond in BoxGraphConverter::getBondTypeFromLabelIdx: "
                      << _label << std::endl;
            exit(-1);
        }
    }
}
