#ifndef _XGD_COMPOSER_HANDLER_HPP_
#define _XGD_COMPOSER_HANDLER_HPP_

#include "ocr_item.hpp"

namespace xgd {
    namespace _p {
        class A;

        class B;

        class G;

        class C;

    }
    class ComposerHelper {

    public:
        ComposerHelper();

        std::shared_ptr<JMol> compose(const std::vector<OCRItem> &_items);

    private:
        size_t mIdBase;
        std::vector<std::shared_ptr<_p::A>> mAtoms;
        std::vector<std::shared_ptr<_p::B>> mBonds;
        std::vector<std::shared_ptr<_p::G>> mGroups;
        std::vector<std::shared_ptr<_p::C>> mCircles;
        std::vector<int> mId2typeVec;
    };
}
#endif//_XGD_COMPOSER_HANDLER_HPP_
