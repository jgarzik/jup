#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__

#include <string>

extern bool readStringFd(int fd, std::string& rawBody);
extern bool writeStringFd(int fd, const std::string& rawBody);
extern bool readBinaryFile(const std::string& filename, std::string& body);
extern bool readTextFile(const std::string& filename, std::string& body);

#endif // __FILEUTIL_H__
