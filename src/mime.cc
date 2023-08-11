#include "mime.h"
#include <unordered_map>

std::unordered_map<std::string, std::string> mimeTypes = {
    {".html", "text/html"},
    {".txt",  "text/plain"},
    {".jpg",  "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".png",  "image/png"},
    {".gif",  "image/gif"},
    {".zip",  "application/zip"}
};

std::string MIME::get_mime_type(const std::string& fileName) {
  std::string extension = fileName.substr(fileName.find_last_of('.'));
  if (mimeTypes.find(extension) != mimeTypes.end()) {
    return mimeTypes[extension]; 
  } else {
    return "application/octet-stream"; // this is default mime type if no extension found
  }
}