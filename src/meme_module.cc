#include "meme_module.h"
#include "logger.h"
#include "boost/filesystem.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

MemeModule::MemeModule(std::string root, std::shared_ptr<FileSystem> fs) 
    : root_(root), fs_(fs), id_count_(0)
{
    Logger* logger = Logger::getLogger();

    // load meme meta data
    if (boost::filesystem::exists(root_) && boost::filesystem::is_directory(root_)) {
        boost::filesystem::directory_iterator end_iter; // default construction yields past-the-end
        for (boost::filesystem::directory_iterator dir_itr(root_); dir_itr != end_iter; ++dir_itr) {
            try {
                if (boost::filesystem::is_directory(dir_itr->status())) {
                    // get id
                    logger->log_debug(dir_itr->path().string().substr(root_.length() + 1, std::string::npos));
                    unsigned int id;
                    try {
                        id = std::stoul(dir_itr->path().string().substr(root_.length() + 1, std::string::npos));
                    } catch(...) {
                        logger->log_error("Can't parse id: " + dir_itr->path().string());
                    }
                    if (id > id_count_) id_count_ = id;
                    std::string meme_path = dir_itr->path().string();
                    std::string view_path = meme_path + "/views";
                    std::string ratings_path = meme_path + "/ratings";
                    // get views
                    std::string view_data;
                    status_f view_status = fs_->read_file(view_path, view_data);
                    if (view_status == status_f::success)
                    {
                        int view_count;
                        std::stringstream ss_rating(view_data);
                        ss_rating >> view_count;
                        view_counts_[id] = view_count;

                    }
                    // get ratings
                    std::string rating_data;
                    status_f rating_status = fs_->read_file(ratings_path, rating_data);
                    if (rating_status == status_f::success) 
                    {
                        int value_ratings_count;
                        float value_ratings;
                        std::stringstream ss_rating(rating_data);
                        std::string int_str;
                        std::getline(ss_rating, int_str, ',');
                        value_ratings_count = std::stoi(int_str);
                        std::string float_str;
                        std::getline(ss_rating, float_str);
                        value_ratings = std::stof(float_str);
                        ratings_[id] = std::make_tuple(value_ratings_count, value_ratings);
                    }   
                    logger->log_info("Loaded meta data for " + std::to_string(id));
                }
            }
            catch (const boost::filesystem::filesystem_error& ex) {
                logger->log_error("fail to load meta data");
            }
        }
    }
}


// generate id
unsigned int MemeModule::generate_id()
{
    const std::lock_guard<std::mutex> lock(id_mutex_);
    id_count_++;
    Logger* logger = Logger::getLogger();
    logger->log_info("Generate id: " + std::to_string(id_count_)); 
    return id_count_;
}

// basics
bool MemeModule::create(unsigned int& id, Meme& meme)
{
    Logger* logger = Logger::getLogger();
    // get an unique id
    unsigned int new_id = generate_id();

    // new file path
    std::string dir_path = root_ + "/" + std::to_string(new_id);

    // create meme directory
    status_f status_dir = fs_->create_directory(dir_path);
    if (status_dir != status_f::success)
    {
        // log error
        logger->log_error("Can't create meme: " + std::to_string(new_id)); 
        return false; 
    }

    // write to file
    std::string content_path = dir_path + "/content";
    status_f status_content = fs_->write_file(content_path, meme.content_);
    if (status_content != status_f::success)
    {
        // log error
        logger->log_error("Can't create meme: " + std::to_string(new_id)); 
        return false; 
    }
    logger->log_info("Create meme: " + std::to_string(new_id)); 
    
    // set id to new id
    id = new_id;
    return true;
}

std::shared_ptr<Meme> MemeModule::get(unsigned int id)
{
    Logger* logger = Logger::getLogger();

    // content file path
    std::string meme_path = root_ + "/" + std::to_string(id);

    logger->log_info("Get meme: " + std::to_string(id));

    // check id dir exist
    if (!fs_->exist(meme_path) || !boost::filesystem::is_directory(meme_path)) {
        logger->log_error("Id does not exist: " + std::to_string(id));
        return nullptr;
    }
    
    // update status
    increment_view_count(id);

    // get meme
    std::string content = get_content(id);
    if (content == "") return nullptr;
    int views = get_view_count(id);
    float ratings = get_rating(id);
    std::shared_ptr<Meme> meme = std::make_shared<Meme>(content, views, ratings);
    
    logger->log_info("Get meme content: " + content); 
    
    return meme;
}

void MemeModule::remove(unsigned int id)
{
    Logger* logger = Logger::getLogger();

    // file path
    std::string path = root_ + "/" + std::to_string(id);

    // delete file
    status_f status = fs_->delete_directory(path);
    if (status != status_f::success)
    {
        // log error
        logger->log_error("Can't delete file: " + std::to_string(id));  
    }

    // delete table entries
    // view counts
    views_mutex_.lock();
    view_counts_.erase(id);
    views_mutex_.unlock();
    // ratings
    ratings_mutex_.lock();
    ratings_.erase(id);
    ratings_mutex_.unlock();
}

// status
int MemeModule::get_view_count(unsigned int id)
{
    // check if id exist
    auto c = view_counts_.find(id);
    if (c != view_counts_.end())
    {
        // get counts
        return c->second;
    }
    else
    {
        return -1;
    }
}

int MemeModule::get_meme_count() {
    return id_count_;
}

float MemeModule::get_rating(unsigned int id)
{
    // check if id exist
    auto r = ratings_.find(id);
    if (r != ratings_.end())
    {
        // get ratings
        return std::get<float>(r->second);
    }
    else
    {
        return -1;
    }
}

// commenting
void MemeModule::add_comment(unsigned int id, Comment& comment)
{
    Logger* logger = Logger::getLogger();
    // check if id exist
    std::string meme_path = root_ + "/" + std::to_string(id);
    if (!fs_->exist(meme_path)) {
        logger->log_error("Invalid id: " + std::to_string(id));
        return;
    }

    // write back to file system
    // comment path
    std::string comment_path = meme_path + "/comments";

    // create comment dir
    if (!fs_->exist(comment_path)) {
        status_f comment_status = fs_->create_directory(comment_path);
        if (comment_status != status_f::success)
        {
            // log error
            logger->log_error("Can't create comment dir: " + std::to_string(id));  
        }
    }
    
    // add comment
    std::string comment_file_name = comment_path + "/" + boost::lexical_cast<std::string>((boost::uuids::random_generator())());
    status_f status = fs_->write_file(comment_file_name, comment.content_);

    if (status != status_f::success)
    {
        // log error
        logger->log_error("Can't update views: " + std::to_string(id));  
    }
}

std::vector<Comment> MemeModule::get_comments(unsigned int id)
{
    Logger* logger = Logger::getLogger();

    // check if id exist
    std::string meme_path = root_ + "/" + std::to_string(id);
    if (!fs_->exist(meme_path)) {
        logger->log_error("Invalid id: " + std::to_string(id));
        return std::vector<Comment>();
    }

    std::vector<Comment> comments;
    // content file path
    std::string comment_path = root_ + "/" + std::to_string(id) + "/comments";
    
    // check if comment exist
    if (!fs_->exist(comment_path) || !boost::filesystem::is_directory(comment_path)) {
        logger->log_info("No comment: " + std::to_string(id));
        return std::vector<Comment>();
    }

    // get comment
    boost::filesystem::directory_iterator end_iter; // default construction yields past-the-end
    for (boost::filesystem::directory_iterator dir_itr(comment_path); dir_itr != end_iter; ++dir_itr) {
        std::string content;
        std::string path = dir_itr->path().string();
        status_f status = fs_->read_file(path, content);
        if (status == status_f::success)
            comments.push_back(Comment(content));
        else
            logger->log_error("Can't open file: " + path);
    }
    return comments;
}

void MemeModule::increment_view_count(unsigned int id)
{
    const std::lock_guard<std::mutex> lock(views_mutex_);
    Logger* logger = Logger::getLogger();

    int view_count = 1;

    // check if id exist
    auto c = view_counts_.find(id);
    if (c != view_counts_.end())
    {
        // increment counts
        view_count = c->second + 1;
        c->second = view_count;
    }
    else
    {
        // initialize a view count instance
        view_counts_.insert({id, view_count});
    }

    // write back to file system
    // file path
    std::string view_path = root_ + "/" + std::to_string(id) + "/views";

    // write to views
    // update views
    std::string view_count_str = std::to_string(view_count);
    status_f status = fs_->write_file(view_path, view_count_str);

    if (status != status_f::success)
    {
        // log error
        logger->log_error("Can't update views: " + std::to_string(id));  
    }

}

void MemeModule::add_rating(unsigned int id, int rating)
{
    
    const std::lock_guard<std::mutex> lock(ratings_mutex_);
    Logger* logger = Logger::getLogger();

    int new_count = 1;
    float new_rating = rating;

    // check if id exist
    auto r = ratings_.find(id);
    if (r != ratings_.end())
    {
        // calculate new rating
        int old_count = std::get<int>(r->second);
        float old_rating = std::get<float>(r->second);
        new_count = old_count + 1;
        new_rating = ((old_rating * old_count) + rating) / new_count;
        r->second = std::make_tuple(new_count, new_rating);
    }
    else
    {
        // initialize a rating instance
        ratings_.insert({id, std::make_tuple(new_count, new_rating)});
    }

    // write back to file system
    // file path
    std::string ratings_path = root_ + "/" + std::to_string(id) + "/ratings";

    // update ratings
    std::string ratings_str = std::to_string(new_count) + "," + std::to_string(new_rating);
    status_f status = fs_->write_file(ratings_path, ratings_str);

    if (status != status_f::success)
    {
        // log error
        logger->log_error("Can't update views: " + std::to_string(id));  
    }
}

std::string MemeModule::get_content(unsigned int id)
{
    Logger* logger = Logger::getLogger();

    // content file path
    std::string content_path = root_ + "/" + std::to_string(id) + "/content";
    
    // read file
    std::string content;
    status_f status = fs_->read_file(content_path, content);
    if (status != status_f::success)
    {
        // log error
        logger->log_error("Can't read file: " + std::to_string(id));  
        return "";
    }

    return content;
}
