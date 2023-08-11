#include "gtest/gtest.h"
#include "mime.h"

class MimeTest : public :: testing::Test{ // test fixture

};

// This test is to test http_text() function in get_response.cc
TEST_F(MimeTest, testhtml) {
    std::string text = "mytest.html";
    std::string response = MIME::get_mime_type(text);
    EXPECT_EQ(response,"text/html");
}

TEST_F(MimeTest, testzip) {
    std::string text = "myfiles.zip";
    std::string response = MIME::get_mime_type(text);
    EXPECT_EQ(response,"application/zip");
}

TEST_F(MimeTest, testimage) {
    std::string text = "myimage.jpeg";
    std::string response = MIME::get_mime_type(text);
    EXPECT_EQ(response, "image/jpeg");
}
TEST_F(MimeTest, testnoextension) {
    std::string text = "filewithoutextension";
    std::string response = MIME::get_mime_type(text);
    EXPECT_EQ(response, "application/octet-stream");
}