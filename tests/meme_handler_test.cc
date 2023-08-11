#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include "handler/meme_handler.h"
#include <fstream>
#include <filesystem>
#include <boost/filesystem.hpp>
#include <memory>
#include "boost_file_system.h"
class MemeHandlerTest : public::testing::Test{
 // test fixture

    public:
    std::string root = "./meme_dir";
    std::string location = "/meme";
    std::shared_ptr<MemeModule> module; // reuse the same filesystem and module for all tests

    void SetUp() override {
        BoostFileSystem system = BoostFileSystem();
        std::shared_ptr<FileSystem> fs = system.get_file_system();
        fs->create_directory(root);
        module = std::make_shared<MemeModule>(root,fs);
    }
};

TEST_F(MemeHandlerTest, HandleGetCreationForm) {
    MemeHandler handler = MemeHandler(location, root, module); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::get);
    req.target("/meme/creation_form");
    status s = handler.handle_request(req, res);
    EXPECT_EQ(s, http::status::ok);
}

TEST_F(MemeHandlerTest, HandleGetCommentForm) {
    MemeHandler handler = MemeHandler(location, root, module); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::get);
    req.target("/meme/comment_form");
    status s = handler.handle_post_comment_form(req, res);
    EXPECT_EQ(s, http::status::ok);
}

TEST_F(MemeHandlerTest, HandlePostMeme) {
    MemeHandler handler = MemeHandler(location, root, module); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::post);
    req.target("/meme");
    std::string content = "{\"top\":\"John Doe\",\"bottom\":\"Doe John\", \"template\":\"template1\"}";
    req.body() = content;
    status s = handler.handle_request(req, res);
    EXPECT_EQ(s, http::status::created);
    EXPECT_THAT(res.body(), testing::HasSubstr("Successfully created meme"));
}



TEST_F(MemeHandlerTest, HandleGetAll) {
    MemeHandler handler = MemeHandler(location, root, module); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::get);
    req.target("/meme/all");
    status s = handler.handle_request(req, res);
    EXPECT_EQ(s, http::status::ok);
}

TEST_F(MemeHandlerTest, HandleGetRandom) {
    MemeHandler handler = MemeHandler(location, root, module); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::get);
    req.target("/meme/random");
    status s = handler.handle_request(req, res);
    EXPECT_EQ(s, http::status::ok);
}

TEST_F(MemeHandlerTest, HandleGetById) {
    MemeHandler handler = MemeHandler(location, root, module); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::get);
    req.target("/meme/1");
    status s = handler.handle_request(req, res);
    EXPECT_EQ(s, http::status::ok);
}

TEST_F(MemeHandlerTest, HandleGetInfoId) {
    MemeHandler handler = MemeHandler(location, root, module); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::get);
    req.target("/meme/info/1");
    status s = handler.handle_request(req, res);
    EXPECT_EQ(s, http::status::ok);
}

TEST_F(MemeHandlerTest, HandleGetViewsId) {
    MemeHandler handler = MemeHandler(location, root, module); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::get);
    req.target("/meme/get_by_views/1");
    status s = handler.handle_request(req, res);
    EXPECT_EQ(s, http::status::ok);
}

TEST_F(MemeHandlerTest, HandleGetBad) {
    MemeHandler handler = MemeHandler(location, root, module); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::get);
    req.target("/badpath");
    status s = handler.handle_request(req, res);
    EXPECT_EQ(s, http::status::not_found);
}




TEST_F(MemeHandlerTest, HandlePostComment) {
    MemeHandler handler = MemeHandler(location, root, module); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::post);
    req.target("/meme/comment");
    std::string content = "{\"comment\":\"Meh\",\"id\":\"1\", \"rating\":\"5\"}";
    req.body() = content;
    status s = handler.handle_request(req, res);
    EXPECT_EQ(s, http::status::created);
    EXPECT_EQ(res.body(), "Successfully posted comment.");
    MemeHandler second_handler = MemeHandler(location, root, module); 
    http::request<http::string_body> req2;
    http::response<http::string_body> res2;
    req2.method(http::verb::get);
    req2.target("/meme/info/1");
    s = second_handler.handle_request(req2, res2);
    s = second_handler.handle_request(req2, res2); // post second comment
    EXPECT_EQ(s, http::status::ok);
}
