#ifndef BOOST_FILE_SYSTEM_H
#define BOOST_FILE_SYSTEM_H

#include "file_system.h"

// file system impelementation using Boost file system library
class BoostFileSystem : public FileSystem
{
public:
    std::shared_ptr<FileSystem> get_file_system() override;
    status_f write_file(std::string& path, std::string& content) override;
    status_f read_file(std::string& path, std::string& content) override;
    status_f delete_file(std::string& path) override;
    status_f create_directory(std::string& path) override;
    status_f delete_directory(std::string& path) override;
    bool exist(std::string& path) override;
};

#endif