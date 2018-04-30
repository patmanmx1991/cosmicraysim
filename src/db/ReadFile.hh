#ifndef __DB_ReadFile__
#define __DB_ReadFile__

#include <string>
#include <stdio.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <sstream>
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace COSMIC {

/// Utils namespace for any simple I/O, like parsing strings/files.
namespace DBUtils {

/// Reads a text file into a string. Returns 0 if success, negative if file cannot be opened
int ReadFile(const std::string &filename, std::string &filecontents);

/// Uses glob to expand wildcards in files and return vector of strings
std::vector<std::string> Expand(std::string);

} // - namespace DBUtils
} // - namespace COSMIC
#endif
