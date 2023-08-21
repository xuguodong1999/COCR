#include <gtest/gtest.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>

#include <sstream>
#include <thread>
#include <charconv>

void extract_thread_id(const std::string &buffer, unsigned long &result) {
    // get the number from "[thread number ]"
    const std::string thread_token{"thread"};
    const std::string token_blank{" "};

    std::string_view sv(buffer);

    auto beg_index = sv.find(thread_token);
    ASSERT_NE(beg_index, std::string_view::npos);

    beg_index += thread_token.length();

    auto end_index = sv.find(token_blank, beg_index + 1);
    ASSERT_NE(end_index, std::string_view::npos);

    sv = sv.substr(beg_index, end_index - beg_index + 1);

    // trim
    sv.remove_prefix(std::min(sv.find_first_not_of(token_blank), sv.size()));
    sv.remove_suffix(std::max(sv.length() - sv.find_last_not_of(token_blank) - 1, std::string_view::size_type(0)));

    auto [ptr, ec]{std::from_chars(sv.data(), sv.data() + sv.length(), result)};
    ASSERT_EQ(ec, std::errc()) << ptr;
}

TEST(XgdLibTest, log_contain_thread_id) {
    std::ostringstream oss;
    auto oss_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    auto oss_logger = std::make_shared<spdlog::logger>("oss logger", oss_sink);
    oss_logger->set_pattern("[%Y-%m-%d %T.%e] [%l] [thread %-6t] %v");

    spdlog::set_default_logger(oss_logger);

    unsigned long id1, id2;
    {
        oss.clear();
        spdlog::info("haha");
        const std::string buffer = oss.str();
        extract_thread_id(buffer, id1);
    }
    {
        oss.clear();
        spdlog::info("hello");
        const std::string buffer = oss.str();
        extract_thread_id(buffer, id2);
    }

    EXPECT_EQ(id1, id2);
}
