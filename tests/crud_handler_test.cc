#include "gtest/gtest.h"
#include "handler/crud_handler.h"
#include <fstream>
#include <filesystem>
#include <boost/filesystem.hpp>

class CrudHandlerTest : public ::testing::Test {
public:
    void initialize_entity_map(std::string root, std::map<std::string, std::vector<int>>& entity_ids)
    {
        if (!boost::filesystem::exists(root) || !boost::filesystem::is_directory(root)) {
            return;
        }
        else {
            boost::filesystem::directory_iterator directory_it(root);
            // default constructed iterator represents the end interator
            boost::filesystem::directory_iterator end_it;
            // loop for the root directory to find all entity types
            for (;directory_it != end_it; directory_it++) {
                boost::filesystem::path directory_path = directory_it->path();

                if (boost::filesystem::is_directory(directory_path)) {

                    boost::filesystem::directory_iterator entity_it(directory_path);
                    // loop for each entity type to find all related ids
                    for (;entity_it != end_it; entity_it++) {
                        boost::filesystem::path entity_path = entity_it->path();
                        int id;
                        try {
                            id = std::stoi(entity_path.filename().string());
                        } catch (const std::invalid_argument& e) {
                            return;
                        }
                        std::string dir = directory_path.filename().string();
                        if (entity_ids.find(dir) == entity_ids.end()) {
                            // it doesnt
                            entity_ids[dir] = {id};
                        } else {
                            entity_ids[dir].push_back(id);
                            std::sort(entity_ids[dir].begin(), entity_ids[dir].end());
                        }
                    }
                }
            }
        }
    }
};

TEST_F(CrudHandlerTest, handlePOSTGoodRequestTest) {
    // clear the test directory before first test
    for (const auto& entry : std::filesystem::directory_iterator("./crud_test_files/testing")) {
        std::filesystem::remove_all(entry.path());
    }
    std::string root = "crud_test_files";
    std::map<std::string, std::vector<int>> entity_ids;
    initialize_entity_map(root, entity_ids);
    std::mutex mtx;
    CrudHandler handler = CrudHandler(root, entity_ids, mtx); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::post);
    req.target("api/testing");
    handler.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::created);
}

TEST_F(CrudHandlerTest, handlePOSTBadRequestTest) {
    std::string root = "crud_test_files";
    std::map<std::string, std::vector<int>> entity_ids;
    initialize_entity_map(root, entity_ids);
    std::mutex mtx;
    CrudHandler handler = CrudHandler(root, entity_ids, mtx); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::post);
    req.target("/");
    handler.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::bad_request);
}

TEST_F(CrudHandlerTest, handlePUTExistRequestTest) {
    std::string root = "crud_test_files";
    std::map<std::string, std::vector<int>> entity_ids;
    initialize_entity_map(root, entity_ids);
    std::mutex mtx;
    CrudHandler handler = CrudHandler(root, entity_ids, mtx); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::put);
    req.target("api/testing/1");
    req.body() = "{\"key1\":\"value1\", \"key2\":\"value2\"}";
    handler.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::ok);
    EXPECT_EQ(res.body(), "Update Entity {\"id\":1}\n");
}

TEST_F(CrudHandlerTest, handlePUTNonexistRequestTest) {
    std::string root = "crud_test_files";
    std::map<std::string, std::vector<int>> entity_ids;
    initialize_entity_map(root, entity_ids);
    std::mutex mtx;
    CrudHandler handler = CrudHandler(root, entity_ids, mtx); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::put);
    req.target("api/testing/2");
    req.body() = "testtest123456789";
    handler.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::created);
}

TEST_F(CrudHandlerTest, RetrieveEntityTest) {
    std::string root = "crud_test_files";
    std::map<std::string, std::vector<int>> entity_ids;
    initialize_entity_map(root, entity_ids);
    std::mutex mtx;
    CrudHandler handler = CrudHandler(root, entity_ids, mtx); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.target("api/testing/1");
    handler.handle_get(req, res);
    EXPECT_EQ(res.result(), http::status::ok);
    EXPECT_EQ(res.body(), "{\"key1\":\"value1\", \"key2\":\"value2\"}");
   
}

TEST_F(CrudHandlerTest, ListEntityTest) {
    std::string root = "crud_test_files";
    std::map<std::string, std::vector<int>> entity_ids;
    initialize_entity_map(root, entity_ids);
    std::mutex mtx;
    CrudHandler handler = CrudHandler(root, entity_ids, mtx); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.target("api/testing");
    handler.handle_get(req, res);
    EXPECT_EQ(res.result(), http::status::ok);
    EXPECT_EQ(res.body(), "[1,2]");
    
}

TEST_F(CrudHandlerTest, handleDELETERequestTest) {
    std::string root = "crud_test_files";
    std::map<std::string, std::vector<int>> entity_ids;
    initialize_entity_map(root, entity_ids);
    std::mutex mtx;
    CrudHandler handler = CrudHandler(root, entity_ids, mtx); 
    http::request<http::string_body> req;
    http::response<http::string_body> res;
    req.method(http::verb::delete_);
    req.target("api/testing/2");
    handler.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::ok);
}
