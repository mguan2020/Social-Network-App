#include "boost_file_system.h"
#include <boost/filesystem.hpp>
#include <boost/beast/http.hpp>
#include <exception>
#include <sstream>

std::shared_ptr<FileSystem> BoostFileSystem::get_file_system()
{
    // lock global fs
    std::unique_lock<std::shared_timed_mutex> lock(fs_mutex_);
    if (!fs_) {
        fs_ = std::make_shared<BoostFileSystem>();
    } 
    return fs_;
}

status_f BoostFileSystem::write_file(std::string& path, std::string& content)
{
    // get the mutex for the file and locks it
    std::shared_ptr<std::shared_timed_mutex> f_mutex = get_mutex(path);
    std::unique_lock<std::shared_timed_mutex> lock(*f_mutex);

    // write the file
    // open file
    std::ofstream file(path);
    if (file) 
    {
        // write content to file
        file << content;
        file.close();
    }
    else 
    {
        // can't open the file
        return status_f::not_found;
    }
    return status_f::success;
}

status_f BoostFileSystem::read_file(std::string& path, std::string& content)
{
    // get the mutex for the file and locks it
    std::shared_ptr<std::shared_timed_mutex> f_mutex = get_mutex(path);
    std::lock_guard<std::shared_timed_mutex> lock(*f_mutex);

    // read the file
    // check if file exist
    if (!exist(path))
        return status_f::not_found;

    // open file
    std::ifstream file(path);
    if (file)
    {
        // read file
        std::string s((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        content = s;
    }
    else
    {
        // can't open the file
        return status_f::error;
    }
    return status_f::success;
}

status_f BoostFileSystem::delete_file(std::string& path)
{
    // get the mutex for the file and locks it
    std::shared_ptr<std::shared_timed_mutex> f_mutex = get_mutex(path);
    std::lock_guard<std::shared_timed_mutex> lock(*f_mutex);

    // check if file exist
    if (!exist(path))
        return status_f::not_found;

    try {
        // delete file
        boost::filesystem::remove(path);
    } catch (const boost::filesystem::filesystem_error& e) {
        // exception
        return status_f::error;
    }
    return status_f::success;
}


status_f BoostFileSystem::create_directory(std::string& path)
{
    // get the mutex for the file and locks it
    std::shared_ptr<std::shared_timed_mutex> f_mutex = get_mutex(path);
    std::lock_guard<std::shared_timed_mutex> lock(*f_mutex);
    
    // check if directory exists
    if (exist(path))
        return status_f::conflict;

    try
    {
        // create directory
        boost::filesystem::create_directory(path);
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        // exception
        return status_f::error;
    }
    return status_f::success;
}

status_f BoostFileSystem::delete_directory(std::string& path)
{
    // get the mutex for the file and locks it
    std::shared_ptr<std::shared_timed_mutex> f_mutex = get_mutex(path);
    std::lock_guard<std::shared_timed_mutex> lock(*f_mutex);
    
    // check if directory exists
    if (!exist(path) || !boost::filesystem::is_directory(path))
        return status_f::not_found;

    try
    {
        // create directory
        boost::filesystem::remove_all(path);
    }
    catch(const boost::filesystem::filesystem_error& e)
    {
        // exception
        return status_f::error;
    }
    return status_f::success;
}

bool BoostFileSystem::exist(std::string& path) 
{
    return boost::filesystem::exists(path);
}