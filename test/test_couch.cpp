#include "gtest/gtest.h"
#include "couch.h"
#include <vector>
#include <set>
#include <string>

TEST(CouchTestSuite, CouchTest) {
    using namespace std;
    vector<cocr::SampleData> samples;
    bool success = cocr::loadCouchFromFile("../../backup/dls", samples);
    EXPECT_TRUE(success);
    set<string> labels;
    for (auto &s : samples) { labels.insert(s.getLabel()); }
    for (auto &sample:samples) {
        EXPECT_LE(sample.getWidth(), 2000);
        EXPECT_LE(sample.getHeight(), 2000);
    }
    EXPECT_EQ(labels.size(), 26 + 26 + 10 + 121);
}