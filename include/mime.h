#ifndef MIME_H
#define MIME_H

#include <string>


class MIME {
public:
    static std::string get_mime_type(const std::string& fileName);
};
#endif // MIME_H