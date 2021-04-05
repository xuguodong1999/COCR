#include "jresidue.hpp"


xgd::JResidue::JResidue(const size_t &_id, std::string _text, bool _isLeftToRight, const float &_x, const float &_y)
        : JAtom(_id, ElementType::None, _x, _y), rawText(std::move(_text)), mIsLeftToRight(_isLeftToRight) {

}

const std::string &xgd::JResidue::getRawText() const {
    return rawText;
}

bool xgd::JResidue::isLeftToRight() const {
    return mIsLeftToRight;
}

