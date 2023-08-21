#pragma once

#include "XGD/QtHybrid/Config.h"

#include <memory>
#include <string>
#include <string_view>

namespace xgd::js {
    std::string XGD_QT_HYBRID_EXPORT version();

    double XGD_QT_HYBRID_EXPORT add(double a, double b);

    std::string XGD_QT_HYBRID_EXPORT eval(const std::string_view &buffer);

    namespace impl {
        class NativeBridgeImpl;
    }

    class XGD_QT_HYBRID_EXPORT NativeBridge {
        std::shared_ptr<impl::NativeBridgeImpl> mImpl;
    public:
        NativeBridge();

        void init();

        void runScript(const char *buffer);
    };
}