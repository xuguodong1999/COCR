#include "XGD/SpdlogStream/Stream.h"

#include <spdlog/spdlog.h>

#include <string_view>
#include <iostream>

using namespace xgd;

SpdlogStreamBuf::SpdlogStreamBuf(std::ostringstream &osm, const SpdlogStreamTag &tag) : osm(osm), tag(tag) {

}

SpdlogStreamBuf::~SpdlogStreamBuf() {
#ifdef __clang__
    std::string buf = this->str();
    std::string_view view = buf;
#else
    std::string_view view = this->view();
#endif
    // FIXME: do not use spdlog in destructor of any global/static variable
    //  https://github.com/gabime/spdlog/issues/2113
    if (!view.empty()) {
        std::cerr << view << std::endl;
    }
    flush();
}

int SpdlogStreamBuf::sync() {
#ifdef __clang__
    std::string buf = this->str();
    std::string_view view = buf;
#else
    std::string_view view = this->view();
#endif
    while (!view.empty() && (view.back() == '\n' || view.back() == '\r')) {
        view = view.substr(0, view.size() - 1);
    }
    while (!view.empty() && (view.front() == '\n' || view.front() == '\r')) {
        view = view.substr(1, view.size() - 1);
    }
    if (!view.empty()) {
        switch (tag) {
            case SpdlogStreamTag::CERR:
            case SpdlogStreamTag::CLOG:
                spdlog::warn(view);
                break;
            case SpdlogStreamTag::COUT:
            default:
                spdlog::info(view);
                break;
        }
    }
    flush();
    return 0;
}

void SpdlogStreamBuf::flush() {
    str("");
    osm.flush();
}

SpdlogStream::SpdlogStream(const SpdlogStreamTag &tag) : buf(osm, tag), std::ostream(&buf) {
}

SpdlogStream xgd::sout{SpdlogStreamTag::COUT};
SpdlogStream xgd::serr{SpdlogStreamTag::CERR};
SpdlogStream xgd::slog{SpdlogStreamTag::CLOG};
