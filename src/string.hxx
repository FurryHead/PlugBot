/*
 *      string.hxx
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

#include <string>
#include <vector>

namespace str {
    std::vector<std::string> split(std::string line, std::string delim);
    std::string join(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end, std::string delim);
    std::string trim(std::string line, std::string delim);
    std::string ltrim(std::string line, std::string delim);
    std::string rtrim(std::string line, std::string delim);
    std::string upper(std::string line);
    std::string lower(std::string line);
}
