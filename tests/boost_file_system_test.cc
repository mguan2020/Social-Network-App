#include "boost_file_system.h"
#include "gtest/gtest.h"
#include <string>

class BoostFileSystemTest : public :: testing::Test
{ // test fixture
protected:
  BoostFileSystem fs;
};


TEST_F(BoostFileSystemTest, SingletonTest) {
  std::shared_ptr<FileSystem> ptr_1 = fs.get_file_system();
  std::shared_ptr<FileSystem> ptr_2 = fs.get_file_system();
  EXPECT_EQ(ptr_1, ptr_2);
}

TEST_F(BoostFileSystemTest, CreateDirTest) {
  std::shared_ptr<FileSystem> fs_ = fs.get_file_system();
  std::string dir = "./FS_TEST";
  fs_->create_directory(dir);
  EXPECT_TRUE(fs_->exist(dir));
}

TEST_F(BoostFileSystemTest, ReadWriteTest) {
  std::shared_ptr<FileSystem> fs_ = fs.get_file_system();

  std::string path = "./FS_TEST/test";

  std::string write_content = "test";
  fs_->write_file(path, write_content);

  std::string read_content;
  fs_->read_file(path, read_content);

  EXPECT_EQ(read_content, write_content);
}

TEST_F(BoostFileSystemTest, ExistTest) {
  std::shared_ptr<FileSystem> fs_ = fs.get_file_system();

  std::string path = "./FS_TEST/test";
  std::string random_path = "./FS_TEST/random";

  EXPECT_TRUE(fs_->exist(path));
  EXPECT_FALSE(fs_->exist(random_path));
}

TEST_F(BoostFileSystemTest, DeleteTest) {
  std::shared_ptr<FileSystem> fs_ = fs.get_file_system();

  std::string path = "./FS_TEST/test";

  fs_->delete_file(path);

  EXPECT_FALSE(fs_->exist(path));
}