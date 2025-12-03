#ifndef SHARED_FILE_UTILS_H
#define SHARED_FILE_UTILS_H

#include <string>

bool acquireLock(const std::string& filename);
void releaseLock(const std::string& filename);

#endif