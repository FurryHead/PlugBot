/*
 *      string.cxx
 *      
 *      Copyright 2011 FurryHead <furryhead14@yahoo.com>
 *      
 *      PlugBot is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      PlugBot is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with PlugBot; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 *      
 *      
 */
 
#include "string.hxx"
#include <string>
#include <cstring>
#include <vector>
using namespace std;

namespace str {
    
    vector<string> split(string line, string delim) {
        vector<string> items;
        int cutAt;
        while((cutAt = line.find_first_of(delim)) != string::npos) {
            if(cutAt > 0) {
                items.push_back(line.substr(0,cutAt));
            }
            line = line.substr(cutAt+1);
        }
        if(line.length() > 0) {
            items.push_back(line);
        }
        return items;
    }

    string join(vector<string>::iterator begin, vector<string>::iterator end, string delim) {
        vector<string>::iterator i = begin;
        if (begin == end) return "";
        
        string result = *i;
        while (++i != end) {
            result += delim;
            result += *i;
        }
        return result;
    }

    string trim(string line, string delim) {
        return rtrim(ltrim(line, delim), delim);
    }

    string ltrim(string line, string delim) {
        size_t start = line.find_first_not_of(delim);
        if (start == string::npos)
            return "";
        else
            return line.substr(start);
    }

    string rtrim(string line, string delim) {
        size_t end= line.find_last_not_of(delim);
        if (end == string::npos)
            return "";
        else
            return line.substr(0, end+1);
    }

    string upper(string line) {
        string ret = "";
        for (int i = 0; i < line.size(); i++) 
            ret += toupper(line[i]);
        
        return ret;
    }

    string lower(string line) {
        string ret = "";
        for (int i = 0; i < line.size(); i++) 
            ret += tolower(line[i]);
        
        return ret;
    }
}
