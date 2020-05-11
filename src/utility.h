#ifndef UTILITY_H_
#define UTILITY_H_

#include <set>
#include <string>

namespace yas6502
{
    extern std::string concatSet(const std::set<std::string> &s, const std::string &sep);    
    extern std::string toUpper(const std::string &s);
}

#endif

