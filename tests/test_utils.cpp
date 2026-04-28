//
//  test_utils.cpp
//  
//
//  Created by Кудинова Елизавета on 28.04.2026.
//  Группа 12

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/common.h"

using ::testing::Eq;

class UtilsTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(UtilsTest, SleepMsDoesNotCrash) {
    EXPECT_NO_THROW({
        sleepMs(1);
        sleepMs(5);
        sleepMs(10);
    });
}

TEST_F(UtilsTest, ConsolePrintWorks) {
    EXPECT_NO_THROW({
        consolePrint("Test message\n");
    });
}

TEST_F(UtilsTest, GetPipeNameReturnsNonEmptyString) {
    std::string pipeName = getPipeName();
    EXPECT_FALSE(pipeName.empty());
}

#ifdef _WIN32
TEST_F(UtilsTest, PipeNameHasCorrectFormat) {
    std::string pipeName = getPipeName();
    EXPECT_TRUE(pipeName.find("\\\\.\\pipe\\") == 0);
}
#else
TEST_F(UtilsTest, PipeNameHasCorrectFormat) {
    std::string pipeName = getPipeName();
    EXPECT_TRUE(pipeName.find("/tmp/") == 0);
}
#endif
