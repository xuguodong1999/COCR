#pragma once

#include <maeparser_export.h>

#include <cstdio>
#include <memory>
#include <string>

namespace schrodinger::mae {

    class Block;

    class MAEPARSER_EXPORT Writer {
    private:
        std::shared_ptr<std::ostream> m_out = nullptr;

        void write_opening_block();

    public:
        Writer() = delete;

        explicit Writer(const std::string &fname);

        Writer(std::shared_ptr<std::ostream> stream);

        void write(const std::shared_ptr<Block> &block);
    };

} // namespace schrodinger