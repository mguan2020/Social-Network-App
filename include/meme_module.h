#ifndef MEME_MODULE_H
#define MEME_MODULE_H

#include <unordered_map>
#include <vector>
#include <memory>
#include "meme_classes/meme.h"
#include "meme_classes/comment.h"
#include "file_system.h"

class MemeModule
{
public:
    MemeModule(std::string root, std::shared_ptr<FileSystem> fs);
    // basics
    bool create(unsigned int& id, Meme& meme);
    std::shared_ptr<Meme> get(unsigned int id);
    void remove(unsigned int id);
    std::string get_content(unsigned int id);

    // status
    int get_view_count(unsigned int id);
    int get_meme_count();
    float get_rating(unsigned int id);
    void add_rating(unsigned int id, int rating);

    // commenting
    void add_comment(unsigned int id, Comment& comment);
    std::vector<Comment> get_comments(unsigned int id);

private:

    // status update
    void increment_view_count(unsigned int id);

    // id
    std::mutex id_mutex_;
    unsigned int generate_id();
    unsigned int id_count_;
    
    // views
    std::mutex views_mutex_;
    std::unordered_map<int, int> view_counts_;
    
    // ratings
    std::mutex ratings_mutex_;
    std::unordered_map<int, std::tuple<int, float>> ratings_;


    // File System
    std::shared_ptr<FileSystem> fs_;
    std::string root_;
};

#endif