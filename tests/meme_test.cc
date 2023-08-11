#include "gtest/gtest.h"
#include "meme_classes/meme.h"

class MemeTest : public :: testing::Test
{ // test fixture

};

TEST_F(MemeTest, MemeConstructorTest) {
    std::string content = "{\"top\":\"John Doe\",\"bottom\":\"Doe John\", \"template\":\"template1\"}";
    Meme meme = Meme(content);
    EXPECT_EQ(meme.content_, content);
    EXPECT_EQ(meme.top_, "John Doe");
    EXPECT_EQ(meme.bottom_, "Doe John");
    EXPECT_EQ(meme.template_, "template1");
}
