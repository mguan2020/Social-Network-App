#include "file_system.h"

std::shared_ptr<std::shared_timed_mutex> FileSystem::get_mutex(const std::string& path) {
    // lock global fs
    std::unique_lock<std::shared_timed_mutex> lock(fs_mutex_);
    // check if a mutex exists for the file
    auto it = file_mutexes_.find(path);
    if (it != file_mutexes_.end()) {
        return it->second;
    } else {
        // create new mutex for the file
        auto new_mutex = std::make_shared<std::shared_timed_mutex>();
        file_mutexes_[path] = new_mutex;
        return new_mutex;
    }
}

void FileSystem::cleanup_mutexes() {
    // lock global fs
    std::unique_lock<std::shared_timed_mutex> lock(fs_mutex_);
    // Delete unused mutexes
    for(auto it = file_mutexes_.begin(); it != file_mutexes_.end(); ) {
        // Delete the mutex if only this map is holding reference
        if(it->second.use_count() == 1) { 
            it = file_mutexes_.erase(it);
        } else {
            ++it;
        }
    }
}
