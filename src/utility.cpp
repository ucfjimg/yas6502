#include "utility.h"

#include <algorithm>
#include <cctype>
#include <sstream>

using std::set;
using std::string;
using std::transform;

using ss = std::stringstream;

namespace yas6502
{
    /**
     * Concatenate the strings in a set, with a seperator
     * string between.
     */
    string concatSet(const set<string> &s, const string &sep)
    {
        ss concat{};

        size_t i = 0;
        for (const string &piece : s) {
            concat << piece;
            if (++i < s.size()) {
                concat << sep;
            }
        }

        return concat.str();
    }

    /**
     * Convert a std::string to upper case.
     */
    string toUpper(const string &s)
    {
        string upper = s;
        transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        return upper;
    }
}
