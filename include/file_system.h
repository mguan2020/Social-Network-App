#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <shared_mutex>

enum status_f { success, not_found, conflict, error };

// file system interface
// manages mutex for each file instance
class FileSystem
{
public:
    virtual std::shared_ptr<FileSystem> get_file_system() = 0;
    virtual status_f write_file(std::string& path, std::string& content) = 0;
    virtual status_f read_file(std::string& path, std::string& content) = 0;
    virtual status_f delete_file(std::string& path) = 0;
    virtual status_f delete_directory(std::string& path) = 0;
    virtual bool exist(std::string& path) = 0;
    virtual status_f create_directory(std::string& path) = 0;
protected:
    std::shared_ptr<std::shared_timed_mutex> get_mutex(const std::string& path);
    void cleanup_mutexes();
    std::shared_ptr<FileSystem> fs_;
    std::shared_timed_mutex fs_mutex_;
    std::unordered_map<std::string, std::shared_ptr<std::shared_timed_mutex>> file_mutexes_;
};

#endif