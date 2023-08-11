#ifndef COMMENT_H
#define COMMENT_H

#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// comment data object
class Comment
{
public:
    Comment(std::string content);
    std::string content_;
    // json data of format {comment:comment, id:id, rating:rating}
    std::string comment_;
    std::string id_;
    std::string rating_;
private:
    void init();
};

#endif