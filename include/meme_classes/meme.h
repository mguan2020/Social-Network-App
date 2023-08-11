#ifndef MEME_H
#define MEME_H

#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// meme data object
class Meme
{
public:
    Meme(std::string content);
    Meme(std::string content, int view_count, float rating);
    // TODO: add more attributes
    std::string content_;
    std::string top_;
    std::string bottom_;
    std::string template_;
    int view_count_;
    float rating_;
private:
    void init();
};

#endif