#include "hw_str.hpp"
#include "hw_data.hpp"
#include <iostream>

HwStr::HwStr(const std::string &_plainText) {
    for (auto& character:_plainText) {
        push_char(std::string(1,character));
    }
}

void HwStr::push_char(const std::string &_charStr, const HwCharType &_hwCharType) {
    richText.emplace_back(_charStr, _hwCharType);
    auto &dataLoader = HwDataLoader::getInstance();
    auto sym = dataLoader.GetByStrLabel(_charStr);
    // TODO: 分离随机要素
    float sNormSizeK = 1.0;// (-1,1)*100
    float sSubSizeK = 1.0;// (-1,1)*50
    float sNormOffsetKx = 0.2;// (-1,1)*100
    float sNormOffsetKy = 0.2;// (-1,1)*100
    float sSubOffsetKx = 0.1;// (-1,1)*50
    float sSubOffsetKy = 0.1;// (-1,1)*50
    float sAngleK = 0.0;// [0,1)*30
    sym.rotate(30 * sAngleK);
    float s = 100 * sNormSizeK;
    float sx = 100 * sNormOffsetKx;
    float sy = 100 * sNormOffsetKy;
    float ss = 50 * sSubSizeK;
    float ssx = 50 * sSubOffsetKx;
    float ssy = 50 * sSubOffsetKy;
    if (mData.empty()) {
        sym.resizeTo(s, s, true);
        sym.moveLeftTopTo(cv::Point2f(0, 0));
    } else {
        auto rect = getBoundingBox();
        if (!rect) {
            std::cerr << "found empty shapes in HwStr::push_char" << std::endl;
            exit(-1);
        }
        if (rect->height < 100) {
            rect->height = 100;
        }
        switch (_hwCharType) {
            case HwCharType::RightButtom: {
                sym.resizeTo(ss, ss);
                sym.moveLeftTopTo(cv::Point2f(
                        rect->x + rect->width + ssx, 50 + ssy));
                break;
            }
            case HwCharType::RightTop: {
                sym.resizeTo(ss, ss);
                sym.moveLeftTopTo(cv::Point2f(
                        rect->x + rect->width + ssx, ssy));
                break;
            }
            case HwCharType::Up: {
                rect = mData.back().getBoundingBox();
                if (!rect) {
                    std::cerr << "found empty shapes in HwStr::push_char" << std::endl;
                    exit(-1);
                }
                sym.resizeTo(ss, ss);
                sym.moveLeftTopTo(cv::Point2f(
                        rect->x + ssx, ssy));
                break;
            }
            case HwCharType::Down: {
                break;
            }
            case HwCharType::UpRand: {
                break;
            }
            case HwCharType::DownRand: {
                break;
            }
            default: {
                sym.resizeTo(s, s);
                sym.moveLeftTopTo(cv::Point2f(
                        rect->x + rect->width + sx, sy));
            }
        }
    }
//    if (!shapes.empty()) {
//        auto rect = shapes.back().getBoundingBox();
//        // 左上角对齐右上角
//        sym.resizeTo(std::numeric_limits<float>::max(), rect.height);
//        sym.moveLeftTopTo(cv::Point2f(
//                rect.x + 1.2 * rect.width, rect.y));
//    }
    push_back(sym);
}

std::string HwStr::getShownText() const {
    if (richText.empty())return "[]";
    std::string result = "[";
    auto[charData, lastCharType]=richText[0];
    result += charData;
    for (size_t i = 1; i < richText.size(); ++i) {
        result += richText[i].first;
        if (richText[i].second != lastCharType) {
            lastCharType = richText[i].second;
            result += "_";
        }
    }
    result += "]";
    return std::move(result);
}

std::shared_ptr<HwItem> HwStr::GetSpecText(const HwSpecText &_specText) {
    auto specText = std::make_shared<HwStr>();
    auto &dataLoader = HwDataLoader::getInstance();
    switch (_specText) {
        case HwSpecText::PositiveElec: {
            auto sym = dataLoader.GetByStrLabel("⊕");
            specText->push_back(sym);
            break;
        }
        case HwSpecText::NegativeElec: {
            auto sym1 = dataLoader.GetShape(ShapeType::Circle);
            sym1.resizeTo(100, 100);
            sym1.moveCenterTo(cv::Point2f(50, 50));
            auto sym2 = dataLoader.GetByStrLabel("-");
            // TODO: 分离随机要素
            sym2.resizeTo(80, 80);
            sym2.moveCenterTo(cv::Point2f(50, 50));
            specText->push_back(sym1);
            specText->push_back(sym2);
            break;
        }
        default: {
            std::cerr << "HwSpecText:" << (int) _specText
                      << "not implemented in HwStr::GetSpecText" << std::endl;
            exit(-1);
        }
    }
    return specText;
}
