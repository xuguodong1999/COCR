#include "box2graph.hpp"
#include "opencv_util.hpp"
#include "std_util.hpp"
#include "colors.hpp"
#include "mol_graph.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <vector>
#include <variant>
#include <unordered_map>

extern std::vector<std::string> CLASSES;

std::vector<std::shared_ptr<JMol>> BoxGraphConverter::handleNoBond() {
    if (!bondBoxes.empty()) {
        std::cerr << "you must confirm bonds empty before calling BoxGraphConverter::handleNoBond"
                  << std::endl;
        exit(-1);
    }
    std::vector<std::shared_ptr<JMol>> mols;
    for (auto&[aLabel, center, width, height]:eAtomBoxes) {
        auto mol = std::make_shared<JMol>();
        auto atom = mol->addAtom(0);
        // FIXME: add pos3d here
//        mol->setPos2D(atom->getId(), true, center);
        atom->setElementType(getElementTyoeFromLabelIdx(aLabel));
        mols.push_back(std::move(mol));
    }
    for (auto&[center, r]:circleBoxes) {
        auto mol = std::make_shared<JMol>();
        auto atom = mol->addAtom(0);
        atom->setElementType(ElementType::O);
        //FIXME: circle bond considered as Oxygen here
        mols.push_back(std::move(mol));
    }
    return std::move(mols);
}

// FIXME: add mol->setPos2D for all atoms
std::vector<std::shared_ptr<JMol>> BoxGraphConverter::then() {
    // analysis features here
    // modify mol here
    if (bondBoxes.empty()) {
        return handleNoBond();
    }
    const float bondSideConThresh = 0.6;
    const float atomSideThresh = 0.5;
    //<bondIndex,atom> for fromSide and toSide
    auto mol = std::make_shared<JMol>();
    std::unordered_map<size_t, std::shared_ptr<JAtom>> aFrom, aTo;
    std::unordered_map<size_t, std::shared_ptr<JAtom>> aA;
    ///////////////// end data define
    // TODO: 将 a-b 和 b-b 关系放到一组概率图中进行对比
    // a-b 关系需要的信息：<a索引、b索引、b键端、ab浮点特征>
    // b-b 关系需要的信息：<b1索引、b1键端、b2索引、b2键端，bb浮点特征>
    // 整合为：<类型(ab或者bb)标记、索引1、索引2、
    using union_feature = std::tuple<
            bool,                   // 类型标记：false: ab, true: bb
            size_t,                 // 索引1
            size_t,                 // 索引2
            std::pair<bool, bool>,  // 键端1、键端2
            float                   // 浮点特征
    >;
    auto get_feature = [&](const union_feature &_a) -> const float & {
        return std::get<4>(_a);
    };
    auto sort_way = [&](const union_feature &_a, const union_feature &_b) -> bool {
        return get_feature(_a) < get_feature(_b);
    };
    auto bb_feature = [&](const size_t &_idx1, const size_t &_idx2,
                          bool _isFrom1, bool _isFrom2, const float &_feature) -> union_feature {
        return {true, _idx1, _idx2, {_isFrom1, _isFrom1}, _feature};
    };
    auto ab_feature = [&](const size_t &_aid, const size_t &_bid,
                          bool isFrom, const float &_feature) -> union_feature {
        return {false, _aid, _bid, {isFrom, false}, _feature};
    };
    std::vector<union_feature> uDistances;

    std::vector<std::tuple<size_t, bool, size_t, bool, float>> bbDistances;
    for (size_t j1 = 0; j1 < bondBoxes.size(); j1++) {
        const auto&[bLabel1, fromTo1, _]=bondBoxes[j1];
        const auto&[from1, to1]=fromTo1;
        for (size_t j2 = j1 + 1; j2 < bondBoxes.size(); j2++) {
            const auto&[bLabel2, fromTo2, __]=bondBoxes[j2];
            const auto&[from2, to2]=fromTo2;
            std::vector<union_feature> whoConnectsWho = {
                    bb_feature(j1, j2, true, true, getDistance2D(from1, from2)),
                    bb_feature(j1, j2, true, false, getDistance2D(from1, to2)),
                    bb_feature(j1, j2, false, true, getDistance2D(to1, from2)),
                    bb_feature(j1, j2, false, false, getDistance2D(to1, to2))
            };
            size_t minIdx = 0;
            for (size_t iw = 1; iw < whoConnectsWho.size(); iw++) {
                if (get_feature(whoConnectsWho[minIdx]) > get_feature(whoConnectsWho[iw]))
                    minIdx = iw;
            }
            if (get_feature(whoConnectsWho[minIdx]) < bondSideConThresh * (
                    std::get<2>(bondBoxes[j1]) + std::get<2>(bondBoxes[j2])))
                uDistances.push_back(whoConnectsWho[minIdx]);
        }
    }
    for (size_t i = 0; i < eAtomBoxes.size(); i++) {
        const auto&[aLabel, center, width, height]=eAtomBoxes[i];
        std::vector<std::tuple<size_t, size_t, bool, float>> abDistances;
        for (size_t j = 0; j < bondBoxes.size(); j++) {
            const auto&[bLabel, fromTo, bondLength]=bondBoxes[j];
            const auto&[from, to]=fromTo;
            float fromDis = getDistance2D(center, from);
            float toDis = getDistance2D(center, to);
            bool isFrom = fromDis < toDis;
            auto &minDis = (std::min)(fromDis, toDis);
            if (minDis < atomSideThresh * (std::get<2>(bondBoxes[j]) + (std::max)(
                    std::get<2>(eAtomBoxes[i]), std::get<3>(eAtomBoxes[i]))))
                uDistances.push_back(ab_feature(i, j, isFrom, minDis));
        }
    }
    std::sort(uDistances.begin(), uDistances.end(), sort_way);
    // show debug info here




    while (!bbDistances.empty()) {
        // distance 优先
        auto&[j1, isFrom1, j2, isFrom2, distance]=bbDistances.back();
        bbDistances.pop_back();
        if (distance > (std::get<2>(bondBoxes[j1]) +
                        std::get<2>(bondBoxes[j2])) * bondSideConThresh / 2.0)
            continue;
        // whether need to add a new Atom
        std::shared_ptr<JAtom> atom = nullptr;
        if (isFrom1 && !notExist(aFrom, j1)) {
            atom = aFrom[j1];
        } else if (isFrom2 && !notExist(aFrom, j2)) {
            atom = aFrom[j2];
        } else if (!isFrom1 && !notExist(aTo, j1)) {
            atom = aTo[j1];
        } else if (!isFrom2 && !notExist(aTo, j2)) {
            atom = aTo[j2];
        }
        if (atom == nullptr) {
            atom = mol->addAtom(6);
        }
        if (isFrom1) {
            aFrom[j1] = atom;
        } else {
            aTo[j1] = atom;
        }
        if (isFrom2) {
            aFrom[j2] = atom;
        } else {
            aTo[j2] = atom;
        }
    }
    /////////////// bond-bond end

    for (size_t i = 0; i < eAtomBoxes.size(); i++) {
        const auto&[aLabel, center, width, height]=eAtomBoxes[i];
        std::vector<std::tuple<size_t, size_t, bool, float>> abDistances;
//        for (auto&[bLabel, fromTo]:bondBoxes) {
        for (size_t j = 0; j < bondBoxes.size(); j++) {
            const auto&[bLabel, fromTo, bondLength]=bondBoxes[j];
            const auto&[from, to]=fromTo;
            float fromDis = getDistance2D(center, from);
            float toDis = getDistance2D(center, to);
            bool isFrom = fromDis < toDis;
            abDistances.emplace_back(i, j, isFrom, (std::min)(fromDis, toDis));
        }
        std::sort(abDistances.begin(), abDistances.end(), [&](
                const std::tuple<size_t, size_t, bool, float> &_a,
                const std::tuple<size_t, size_t, bool, float> &_b
        ) -> bool {
            return std::get<3>(_a) > std::get<3>(_b);
        });
        while (!abDistances.empty()) {
            // distance 优先
            auto&[i, j, isFrom, distance]=abDistances.back();
            abDistances.pop_back();
            if (distance > atomSideThresh * (std::get<2>(bondBoxes[j]) + (std::max)(
                    std::get<2>(eAtomBoxes[i]), std::get<3>(eAtomBoxes[i]))))
                continue;
            // whether need to add a new Atom
            std::shared_ptr<JAtom> atom = nullptr;
            if (notExist(aA, i)) {
                atom = mol->addAtom(0);
                atom->setElementType(getElementTyoeFromLabelIdx(
                        std::get<0>(eAtomBoxes[i])));
                aA[i] = atom;
            } else {
                atom = aA[i];
            }
            if (isFrom) {
                aFrom[j] = atom;
            } else {
                aTo[j] = atom;
            }
        }
    }
    ///////////////////// atom-bond end

    // 保证键端都有碳原子
    for (size_t j = 0; j < bondBoxes.size(); j++) {
        if (notExist(aFrom, j)) {
            aFrom[j] = mol->addAtom(6);
        }
        if (notExist(aTo, j)) {
            aTo[j] = mol->addAtom(6);
        }
    }
    // 加键
    for (size_t j = 0; j < bondBoxes.size(); j++) {
        mol->addBond(aFrom[j]->getId(), aTo[j]->getId(),
                     getBondTypeFromLabelIdx(std::get<0>(bondBoxes[j])));
    }
    if (debug) {
//        std::cout << mol->toSMILES(false) << std::endl;
//        std::cout << mol->getAtomsMap().size() << std::endl;
//        std::cout << mol->getBondsMap().size() << std::endl;
    }
    return MolGraph(mol).iSplit();
//    }
//    return std::move(mols);
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

    // 转换数据格式
    for (auto &box:_boxes) {
        ItemType itemType = getTypeFromLabelIdx(box.label);
        switch (itemType) {
            case ItemType::ExplicitAtom:
                eAtomBoxes.emplace_back(box.label, getRectCenter2D(box.bBox),
                                        box.bBox.width, box.bBox.height);
                break;
            case ItemType::LineBond: {
                auto[from, to]=getFromTo4LineBond(box, _img);
                bondBoxes.emplace_back(box.label, std::make_pair(from, to),
                                       getDistance2D(from, to));
                break;
            }
            case ItemType::CircleBond:
                circleBoxes.emplace_back(getRectCenter2D(box.bBox),
                                         (box.bBox.height + box.bBox.height) / 4.0);
                break;
            default:
                std::cerr << "unhandled type in BoxGraphConverter::accept" << std::endl;
                exit(-1);
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
        std::cout << "width/height=" << k << std::endl;
    }
    const float kThresh = 3;
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
    cv::Point2f from, to;
    if (isLineLike) {
        // 返回短边中点
        if (isVertical) {
            from.x = to.x = rect.x + rect.width / 2;
            from.y = rect.y;
            to.y = rect.y + rect.height;
        } else {
            from.x = rect.x;
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
    } else if (isDirNeeded) {
        auto mask = _img(rect2i);
        cv::cvtColor(mask, mask, cv::COLOR_BGR2GRAY);
        auto getWedgeTemplate = [&](
                const int &_w, const int &_h, const int &_direction, const float &_k = 2) {
            cv::Mat tmpMat(_h, _w, CV_8UC1,
                           getScalar(ColorName::rgbWhite));
            std::vector<cv::Point2i> pts(3);
            const float up = _k / (_k + 1);
            const float down = 1 - up;
            switch (_direction) {
                case 0:
                    pts[0] = {0, 0};
                    pts[1] = {_w - 1, (int) (_h * up)};
                    pts[2] = {(int) (_w * up), _h - 1};
                    break;
                case 1:
                    pts[0] = {_w - 1, 0};
                    pts[1] = {0, (int) (_h * up)};
                    pts[2] = {(int) (_w * down), _h - 1};
                    break;
                case 2:
                    pts[0] = {0, _h - 1};
                    pts[1] = {_w - 1, (int) (_h * down)};
                    pts[2] = {(int) (_w * up), 0};
                    break;
                case 3:
                    pts[0] = {_w - 1, _h - 1};
                    pts[1] = {0, (int) (_h * down)};
                    pts[2] = {(int) (_w * down), 0};
                    break;
            }
            cv::fillPoly(tmpMat, pts, getScalar(ColorName::rgbBlack), cv::LINE_AA);
//        cv::imshow("fuck",tmpMat);
//        cv::waitKey(0);
            return std::move(tmpMat);
        };
        auto getFuckCount = [&](const cv::Mat &_m1,
                                const cv::Mat &_m2) -> double {
            double fuck = 0;
            for (int i = 0; i < _m1.rows; i++) {
                for (int j = 0; j < _m1.cols; j++) {
                    if (_m2.at<uchar>(i, j) == 0) { // 黑
                        if (_m1.at<uchar>(i, j) == 0) {
                            fuck += 1;
                        } else {
                            fuck -= 1;
                        }
                    }
                }
            }
            return fuck;
        };
        double d0 = getFuckCount(mask, getWedgeTemplate(
                rect2i.width, rect2i.height, 0));
        double d1 = getFuckCount(mask, getWedgeTemplate(
                rect2i.width, rect2i.height, 1));
        double d2 = getFuckCount(mask, getWedgeTemplate(
                rect2i.width, rect2i.height, 2));
        double d3 = getFuckCount(mask, getWedgeTemplate(
                rect2i.width, rect2i.height, 3));
        cv::Point maxLoc;
        cv::minMaxLoc(std::vector<double>({d0, d1, d2, d3}),
                      nullptr, nullptr, nullptr, &maxLoc);
        from = to = rect.tl();
        switch (maxLoc.x) {
            case 0:
                to = rect.br();
                break;
            case 1:
                from.x += rect.width;
                to.y += rect.height;
                break;
            case 2:
                from.y += rect.height;
                to.x += rect.width;
                break;
            case 3:
                from = rect.br();
                break;
        }
        if (debug) {
            std::cout << d0 << ", " << d1 << std::endl;
            std::cout << d2 << ", " << d3 << std::endl;
            std::cout << "----------------\n";
        }
    } else {
        // 非扁平矩形，区域四分(1234左上右上左下右下)，统计交叉网格的特征像素计数
        auto mask0 = _img(cv::Rect2i(rect2i.x, rect2i.y,
                                     rect2i.width / 2, rect2i.height / 2));
        auto mask1 = _img(cv::Rect2i(rect2i.x + rect2i.width / 2, rect2i.y,
                                     rect2i.width / 2, rect2i.height / 2));
        auto mask2 = _img(cv::Rect2i(rect2i.x, rect2i.y + rect.height / 2,
                                     rect2i.width / 2, rect2i.height / 2));
        auto mask3 = _img(cv::Rect2i(rect2i.x + rect2i.width / 2,
                                     rect2i.y + rect2i.height / 2,
                                     rect2i.width / 2, rect2i.height / 2));
        auto sum0 = getScalarSum(cv::mean(std::move(mask0)));
        auto sum1 = getScalarSum(cv::mean(std::move(mask1)));
        auto sum2 = getScalarSum(cv::mean(std::move(mask2)));
        auto sum3 = getScalarSum(cv::mean(std::move(mask3)));
        if (sum0 + sum3 < sum1 + sum2) {
            from = rect.tl();
            to = rect.br();
        } else {
            from.x = rect.x;
            from.y = rect.y + rect.height;
            to.x = rect.x + rect.width;
            to.y = rect.y;
        }
        if (debug) {
            std::cout << sum0 << ", " << sum1 << std::endl;
            std::cout << sum2 << ", " << sum3 << std::endl;
            std::cout << "----------------\n";
        }
    }
    if (debug) {
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

//void BoxGraphConverter::featureTraverse(const callback_type &_func) {
//    for (const auto&[idx1, idx2, feature]:abDisGrid) {
//        _func(idx1, idx2, feature);
//    }
//    for (const auto&[idx1, idx2, feature]:bbDisGrid) {
//        _func(idx1, idx2, feature);
//    }
//    for (const auto&[idx1, idx2, feature]:bcDisGrid) {
//        _func(idx1, idx2, feature);
//    }
//    for (const auto&[idx1, idx2, feature]:acDisGrid) {
//        _func(idx1, idx2, feature);
//    }
//}

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

ElementType BoxGraphConverter::getElementTyoeFromLabelIdx(const int &_label) {
    //        "Br", "O", "I", "S", "H", "N", "C", "B",
    //        "-", "--", "-+", "=", "F", "#", "Cl", "P", "[o]"};
    switch (_label) {
        case 0:
            return ElementType::Br;
        case 1:
            return ElementType::O;
        case 2:
            return ElementType::I;
        case 3:
            return ElementType::S;
        case 4:
            return ElementType::H;
        case 5:
            return ElementType::N;
        case 6:
            return ElementType::C;
        case 7:
            return ElementType::B;
        case 12:
            return ElementType::F;
        case 14:
            return ElementType::Cl;
        case 15:
            return ElementType::P;
        default: {
            std::cerr << "consider label " << _label
                      << "as atom type" << std::endl;
            exit(-1);
        }
    }
}


