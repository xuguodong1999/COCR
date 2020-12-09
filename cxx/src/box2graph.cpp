#include "box2graph.hpp"
#include "opencv_util.hpp"
#include "std_util.hpp"
#include "colors.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

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
    std::vector<std::shared_ptr<JMol>> mols;
    // 先支持所有图元属于一个分子骨架的情况
    auto mol = std::make_shared<JMol>();
    mols.push_back(mol);
    std::unordered_map<size_t, size_t> idCaster;
    for (auto&[aid, bid, feature]:abDisGrid) {
        if (notExist(idCaster, aid)) {
            auto atom = mol->addAtom(0);
            atom->setElementType(CLASSES[labels[aid]]);
            idCaster[aid] = atom->getId();
        }
    }
    return std::move(mols);
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
    iAreaGrid.resize(box_num, std::vector<float>(box_num, 0));
    for (size_t i = 0; i < box_num; i++) {
        labels[i] = _boxes[i].label;
        for (size_t j = i + 1; j < box_num; j++) {
            // 填充相交面积
            iAreaGrid[i][j] = iAreaGrid[j][i] = (_boxes[i].bBox & _boxes[j].bBox).area();
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
                    abDisGrid.emplace_back(aIndex, bIndex, (std::min)(
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
                    size_t bIndex = itI == ItemType::LineBond ? i : j;
                    size_t cIndex = itI == ItemType::CircleBond ? i : j;
                    bcDisGrid.emplace_back(bIndex, cIndex, getDistance2D(
                            getRectCenter2D(_boxes[i].bBox), getRectCenter2D(_boxes[j].bBox)));
                    break;
                }
                case ItemType::ExplicitAtom | ItemType::CircleBond: {
                    // 中心对中心
                    size_t cIndex = itI == ItemType::CircleBond ? i : j;
                    size_t aIndex = itI == ItemType::ExplicitAtom ? i : j;
                    acDisGrid.emplace_back(aIndex, cIndex, getDistance2D(
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
//    if (debug) {
//        std::cout << rect << std::endl;
//        std::cout << "k=" << k << std::endl;
//    }
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
        if (isDirNeeded) {
            // 判断是否需要交换(from,to)
            // 策略：区域二分，计算像素均值
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
//            if (debug) {
//                std::cout << getScalarSum(scalar1) << ", " << getScalarSum(scalar2) << std::endl;
//                std::cout << "----------------\n";
//            }
            // 白色像素多 -> sum 大 && 设为 from
            bool isSwapNeeded = getScalarSum(scalar1) < getScalarSum(scalar2);
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
    auto mask3 = _img(cv::Rect2i(rect2i.x, rect2i.y + rect.height / 2,
                                 rect2i.width / 2, rect2i.height / 2));
    auto mask4 = _img(cv::Rect2i(rect2i.x + rect2i.width / 2,
                                 rect2i.y + rect2i.height / 2,
                                 rect2i.width / 2, rect2i.height / 2));
    auto sum1 = getScalarSum(cv::mean(std::move(mask1)));
    auto sum2 = getScalarSum(cv::mean(std::move(mask2)));
    auto sum3 = getScalarSum(cv::mean(std::move(mask3)));
    auto sum4 = getScalarSum(cv::mean(std::move(mask4)));
//    if (debug) {
//        std::cout << sum1 << ", " << sum2 << std::endl;
//        std::cout << sum3 << ", " << sum4 << std::endl;
//        std::cout << "----------------\n";
//    }
    cv::Point2f from, to;
    bool isSwapNeeded;
    const float extraW = 0;//0.3;
    if (sum1 + sum4 < sum2 + sum3) {
        from = rect.tl();
        to = rect.br();
        if (isHorizontal) {
            isSwapNeeded = sum1 + sum3 * extraW < sum4 + sum2 * extraW;
        } else {
            isSwapNeeded = sum1 + sum2 * extraW < sum4 + sum3 * extraW;
        }
    } else {
        from.x = rect.x;
        from.y = rect.y + rect.height;
        to.x = rect.x + rect.width;
        to.y = rect.y;
        if (isHorizontal) {
            isSwapNeeded = sum2 + sum4 * extraW < sum3 + sum1 * extraW;
        } else {
            isSwapNeeded = sum2 + sum1 * extraW < sum3 + sum4 * extraW;
        }
    }
    if (isSwapNeeded)std::swap(from, to);
    // TODO: 预定义模板去匹配区域
    auto get_template = [&](const int &_w, const int &_h, const int &_direction) -> cv::Mat {
//        cv::Mat tmpMat;
        cv::Mat tmpMat(_h, _w, CV_8UC1,
                       getScalar(ColorName::rgbWhite));
        std::vector<cv::Point2i> pts(3);
        switch (_direction) {
            case 1:
                pts[0] = {0, 0};
                pts[1] = {_w - 1, _h * 2 / 3};
                pts[2] = {_w * 2 / 3, _h - 1};
                break;
            case 2:
                pts[0] = {_w - 1, 0};
                pts[1] = {0, _h * 2 / 3};
                pts[2] = {_w * 1 / 3, _h - 1};
                break;
            case 3:
                pts[0] = {0, _h - 1};
                pts[1] = {_w - 1, _h * 1 / 3};
                pts[2] = {_w * 2 / 3, 0};
                break;
            case 4:
                pts[0] = {_w - 1, _h - 1};
                pts[1] = {0, _h * 1 / 3};
                pts[2] = {_w * 1 / 3, 0};
                break;
        }
        cv::fillPoly(tmpMat, pts, getScalar(ColorName::rgbBlack), cv::LINE_AA);
//        cv::imshow("fuck",tmpMat);
//        cv::waitKey(0);
        return std::move(tmpMat);
    };
    auto mask = _img(rect2i);
    cv::cvtColor(mask, mask, cv::COLOR_BGR2GRAY);
    auto getFuckCount = [&](const cv::Mat &_m1, const cv::Mat &_m2) -> double {
        double fuck = 0;
        for (int i = 0; i < _m1.rows; i++) {
            for (int j = 0; j < _m1.cols; j++) {
                if (_m1.at<uchar>(i, j) == 0) {
                    if (_m2.at<uchar>(i, j) == 0) {
                        fuck += 1;
                    } else {
                        fuck -= 1;
                    }
                }
            }
        }
        return fuck;
    };

    double d1 = getFuckCount(mask, get_template(
            rect2i.width, rect2i.height, 1));
    double d2 = getFuckCount(mask, get_template(
            rect2i.width, rect2i.height, 2));
    double d3 = getFuckCount(mask, get_template(
            rect2i.width, rect2i.height, 3));
    double d4 = getFuckCount(mask, get_template(
            rect2i.width, rect2i.height, 4));

//    cv::matchTemplate(_img(rect2i),get_template(rect2i.width,rect2i.height,1),)
    if (debug && isDirNeeded) {
//        cv::imshow("fuck", mask);
//        cv::waitKey(0);
        std::cout << sum1 << ", " << sum2 << std::endl;
        std::cout << sum3 << ", " << sum4 << std::endl;
        std::cout << "----------------\n";
        std::cout << d1 << ", " << d2 << std::endl;
        std::cout << d3 << ", " << d4 << std::endl;
        std::cout << "----------------\n";
        auto tmpMat = _img.clone();
        cv::putText(tmpMat, "[0]", from, 1, 1.2,
                    getScalar(ColorName::rgbOrangeRed), 2,
                    cv::LINE_AA, false);
        cv::putText(tmpMat, "[1]", to, 1, 1.2,
                    getScalar(ColorName::rgbDarkRed), 2,
                    cv::LINE_AA, false);
        cv::imshow("BoxGraphConverter-getFromTo4LineBond", tmpMat);
        cv::waitKey(0);
    }
    return {from, to};
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
