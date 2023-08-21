#pragma once

#include "spdlog_stream_export.h"

#include <sstream>

namespace xgd {
    enum SpdlogStreamTag {
        COUT, CLOG, CERR
    };

    class SpdlogStreamBuf : public std::stringbuf {
        std::ostringstream &osm;
        SpdlogStreamTag tag;
    public:
        explicit SpdlogStreamBuf(std::ostringstream &osm, const SpdlogStreamTag &tag);

        ~SpdlogStreamBuf() override;

        int sync() override;

        void flush();
    };

    class SPDLOG_STREAM_EXPORT SpdlogStream : public std::ostream {
        std::ostringstream osm;
        SpdlogStreamBuf buf;
    public:
        explicit SpdlogStream(const SpdlogStreamTag &tag);
    };

    extern SPDLOG_STREAM_EXPORT SpdlogStream sout;
    extern SPDLOG_STREAM_EXPORT SpdlogStream serr;
    extern SPDLOG_STREAM_EXPORT SpdlogStream slog;
}
