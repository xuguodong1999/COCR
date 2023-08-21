#include "XGD/Base/Util.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

using namespace std;

void xgd::printVersion() {
}

void xgd::recurseDirectory(
        const filesystem::path &root,
        const std::function<void(const std::filesystem::path &)> &callback
) {
    if (std::filesystem::is_directory(root)) {
        for (auto &it: std::filesystem::directory_iterator(root)) {
            xgd::recurseDirectory(it.path(), callback);
        }
    } else if (std::filesystem::is_regular_file(root)) {
        callback(root);
    } else {
        spdlog::warn("{} is neither a directory or regular file", root);
    }
}

void xgd::recurseDirectoryWithFilter(
        const filesystem::path &root,
        const unordered_set<std::string> &ignoredDirectoryName,
        const function<void(const std::filesystem::path &)> &callback
) {
    if (std::filesystem::is_directory(root)) {
        if (ignoredDirectoryName.contains(root.filename().string())) {
            return;
        }
        for (auto &it: std::filesystem::directory_iterator(root)) {
            xgd::recurseDirectoryWithFilter(it.path(), ignoredDirectoryName, callback);
        }
    } else if (std::filesystem::is_regular_file(root)) {
        callback(root);
    } else {
        spdlog::warn("{} is neither a directory or regular file", root);
    }
}

