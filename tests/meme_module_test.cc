#include "meme_module.h"
#include "gtest/gtest.h"
#include "boost_file_system.h"
#include <memory>
#include "meme_classes/meme.h"
#include "meme_classes/comment.h"

class MemeModuleTest : public :: testing::Test
{ // test fixture
public:
    std::string root = "./meme_mod";
};

// test constructor load meta data
TEST_F(MemeModuleTest, MemeModuleConstructorEmptyMetaFileTest) {
    BoostFileSystem bfs = BoostFileSystem();
    std::shared_ptr<FileSystem> fs = bfs.get_file_system();
    fs->create_directory(root);
    MemeModule(root, fs);
}

// test create new meme
TEST_F(MemeModuleTest, MemeModuleCreateTest) {
    BoostFileSystem bfs = BoostFileSystem();
    std::shared_ptr<FileSystem> fs = bfs.get_file_system();
    MemeModule memes = MemeModule(root, fs);
    std::string content = "{\"top\":\"John Doe\",\"bottom\":\"Doe John\", \"template\":\"template1\"}";
    Meme new_meme = Meme(content);
    unsigned int new_id;
    ASSERT_TRUE(memes.create(new_id, new_meme)); // create new meme
    std::shared_ptr<Meme> meme_get = memes.get(new_id); // get meme
    ASSERT_TRUE(meme_get != nullptr);
    
    EXPECT_EQ(meme_get->content_, content);
}

// test view count
TEST_F(MemeModuleTest, MemeModuleViewCountTest) {
    BoostFileSystem bfs = BoostFileSystem();
    std::shared_ptr<FileSystem> fs = bfs.get_file_system();
    MemeModule memes = MemeModule(root, fs);
    std::string content = "{\"top\":\"John Doe\",\"bottom\":\"Doe John\", \"template\":\"template1\"}";
    Meme new_meme = Meme(content);
    unsigned int new_id;
    memes.create(new_id, new_meme); // create new meme
    memes.get(new_id); // view count +1
    memes.get(new_id); // view count +1

    EXPECT_EQ(memes.get_view_count(new_id), 2);
}

// test ratings
TEST_F(MemeModuleTest, MemeModuleRatingsTest) {
    BoostFileSystem bfs = BoostFileSystem();
    std::shared_ptr<FileSystem> fs = bfs.get_file_system();
    MemeModule memes = MemeModule(root, fs);
    std::string content = "{\"top\":\"John Doe\",\"bottom\":\"Doe John\", \"template\":\"template1\"}";
    
    Meme new_meme = Meme(content);
    unsigned int new_id;

    memes.create(new_id, new_meme); // create new meme

    memes.add_rating(new_id, 0); // ratings = 0
    EXPECT_EQ(memes.get_rating(new_id), 0.0);

    memes.add_rating(new_id, 4); // ratings = 4
    EXPECT_EQ(memes.get_rating(new_id), 2.0);
}


// test comment
TEST_F(MemeModuleTest, MemeModuleCommentTest) {
    BoostFileSystem bfs = BoostFileSystem();
    std::shared_ptr<FileSystem> fs = bfs.get_file_system();
    MemeModule memes = MemeModule(root, fs);
    std::string content = "{\"top\":\"John Doe\",\"bottom\":\"Doe John\", \"template\":\"template1\"}";
    
    Meme new_meme = Meme(content);
    unsigned int new_id;

    memes.create(new_id, new_meme); // create new meme
    std::string comment_content = "{\"id\":\"5\",\"rating\":\"5\",\"comment\":\"heyheyhey\"}";
    Comment new_comment = Comment(comment_content);
    memes.add_comment(new_id, new_comment);
    memes.add_comment(new_id, new_comment);
    std::vector<Comment> comments = memes.get_comments(new_id);
    ASSERT_TRUE(!comments.empty());
    EXPECT_EQ(comments[0].content_, comment_content);
    EXPECT_EQ(comments[1].content_, comment_content);
}


// test id dne
TEST_F(MemeModuleTest, MemeModuleInvalidID) {
    BoostFileSystem bfs = BoostFileSystem();
    std::shared_ptr<FileSystem> fs = bfs.get_file_system();
    MemeModule memes = MemeModule(root, fs);
    unsigned int invalid_id = -1; // max int
    std::shared_ptr<Meme> meme = memes.get(invalid_id);
    ASSERT_TRUE(meme == nullptr);
}
