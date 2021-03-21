#ifndef _JRESIDUE_HPP_
#define _JRESIDUE_HPP_

#include "jatom.hpp"
#include <string>
#include <vector>
#include <memory>

namespace xgd {
    class JResidue : public JAtom {
        std::string rawText;
        bool isLeftToRight;
    public:
        JResidue(const size_t &_id, std::string _text, bool _isLeftToRight, const float &_x = 0, const float &_y = 0);
    };
}
#endif//_JRESIDUE_HPP_