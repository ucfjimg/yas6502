/**
 * Copyright 2020 Jim Geist.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do 
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **/
#include "utility.h"

#include <algorithm>
#include <cctype>
#include <sstream>

using std::set;
using std::string;
using std::transform;

using ss = std::stringstream;

namespace 
{
#ifdef WIN32
    const char PATHSEP = '\\';
#else
    const char PATHSEP = '/';
#endif
}

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

    /**
     * Replace the file extension in `fn' with `ext', or append the new extension 
     * if there isn't one on `fn' already.
     */
    string replaceOrAppendExtension(const string &fn, const string &ext)
    {
        auto pathsep = fn.rfind(PATHSEP);        
        auto dot = fn.rfind('.');

        if (dot != string::npos && (pathsep == string::npos || pathsep < dot)) {
            return fn.substr(0, dot+1) + ext;
        }

        return fn + '.' + ext;
    }
}
