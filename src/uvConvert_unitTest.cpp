#include "uvConvert.h"
#include "gtest/gtest.h"

namespace
{
    using namespace LowMapped;

    TEST(LowMappedIndex2Local, forward) {
        EXPECT_FLOAT_EQ(0.0,index2local(0,1));
        EXPECT_FLOAT_EQ(0.333333333f,index2local(1,3));
        EXPECT_FLOAT_EQ(0.666666666f,index2local(2,3));
    }

    TEST(LowMappedLocal2index, oneway) {
        EXPECT_EQ(0, local2index(0,5));
        EXPECT_EQ(2, local2index(0.5,5));

        EXPECT_EQ(4, local2index(1.0,5));
        EXPECT_EQ(4, local2index(1.0f,5));
    }
}
// 0    0.2   0.4   0.6   0.8   1.0
// |-----|-----|-----|-----|-----|
//    0     1     2     3     4
TEST(MidMappedLocal2Index, size5)
{
    EXPECT_EQ(0, MidMapped::local2index(0,   5));
    EXPECT_EQ(4, MidMapped::local2index(1,   5));
    EXPECT_EQ(2, MidMapped::local2index(0.5, 5));
    EXPECT_EQ(2, MidMapped::local2index(0.4, 5));
    EXPECT_EQ(1, MidMapped::local2index(0.2, 5));
    EXPECT_EQ(0, MidMapped::local2index(0.1999, 5));
}
TEST(MidMappedLocal2Index, size2)
{
    EXPECT_EQ(0, MidMapped::local2index(0,      2));
    EXPECT_EQ(1, MidMapped::local2index(1,      2));
    EXPECT_EQ(1, MidMapped::local2index(0.5,    2));
    EXPECT_EQ(0, MidMapped::local2index(0.4,    2));
}

TEST(MidMappedIndex2Local, size5)
{
    EXPECT_FLOAT_EQ(0.1, MidMapped::index2local(0, 5));
    EXPECT_FLOAT_EQ(0.9, MidMapped::index2local(4, 5));
    EXPECT_FLOAT_EQ(0.5, MidMapped::index2local(2, 5));
}

