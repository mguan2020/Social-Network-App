#include "meme_classes/meme.h"

Meme::Meme(std::string content) : content_(content), view_count_(0), rating_(0.0)
{
    init();
}

Meme::Meme(std::string content, int view_count, float rating) : content_(content), view_count_(view_count), rating_(rating)
{
    init();
}

void Meme::init()
{
    boost::property_tree::ptree tree;
    std::stringstream ss = std::stringstream(content_);
    boost::property_tree::json_parser::read_json(ss, tree);
    top_ = tree.get<std::string>("top");
    bottom_ = tree.get<std::string>("bottom");
    template_ = tree.get<std::string>("template");
}

