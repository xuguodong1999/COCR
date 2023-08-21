#pragma once

#include "XGD/QtHybrid/Config.h"
#include <memory>

namespace xgd::js {
    namespace impl {
        class VueWindowImpl;
    }
    class XGD_QT_HYBRID_EXPORT VueWindow {
        std::shared_ptr<impl::VueWindowImpl> mImpl;
    public:
        explicit VueWindow();

        void show();

        void resize(int w, int h);
    };
}