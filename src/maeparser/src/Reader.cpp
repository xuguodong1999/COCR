#include "maeparser/Reader.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/iostreams/device/file.hpp>

#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include <iostream>
#include <utility>

using boost::algorithm::ends_with;
using boost::iostreams::file_source;
using boost::iostreams::filtering_istream;

namespace schrodinger::mae {

    Reader::Reader(FILE *file, size_t buffer_size) {
        m_mae_parser.reset(new MaeParser(file, buffer_size));
    }

    Reader::Reader(const std::shared_ptr<std::istream> &stream, size_t buffer_size) {
        m_mae_parser.reset(new MaeParser(stream, buffer_size));
    }

    Reader::Reader(const std::string &fname, size_t buffer_size) {
        const auto ios_mode = std::ios_base::in | std::ios_base::binary;

        std::shared_ptr<std::istream> stream;
        if (ends_with(fname, ".maegz") || ends_with(fname, ".mae.gz")) {
            auto *gzip_stream = new filtering_istream();
            gzip_stream->push(boost::iostreams::gzip_decompressor());
            gzip_stream->push(file_source(fname, ios_mode));
            stream.reset(static_cast<std::istream *>(gzip_stream));
//            throw std::runtime_error("gz not supported for no boost integration");
        } else {
            auto *file_stream = new std::ifstream(fname, ios_mode);
            stream.reset(static_cast<std::istream *>(file_stream));
        }

        if (stream->fail()) {
            std::stringstream ss;
            ss << "Failed to open file \"" << fname << "\" for reading operation.";
            throw std::runtime_error(ss.str());
        }

        m_mae_parser.reset(new MaeParser(stream, buffer_size));
    }

    Reader::Reader(std::shared_ptr<MaeParser> mae_parser)
            : m_mae_parser(std::move(mae_parser)) {
    }

    std::shared_ptr<Block> Reader::next(const std::string &outer_block_name) {
        std::shared_ptr<Block> block;
        do {
            m_mae_parser->whitespace();
            block = m_mae_parser->outerBlock();
        } while (block != nullptr && block->getName() != outer_block_name);
        return block;
    }
} // namespace schrodinger
