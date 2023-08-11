#include "meme_classes/comment.h"

Comment::Comment(std::string content) : content_(content)
{
    init();
}
void Comment::init()
{
    boost::property_tree::ptree tree;
    std::stringstream ss = std::stringstream(content_);
    boost::property_tree::json_parser::read_json(ss, tree);
    comment_ = tree.get<std::string>("comment");
    id_ = tree.get<std::string>("id");
    rating_ = tree.get<std::string>("rating");
}