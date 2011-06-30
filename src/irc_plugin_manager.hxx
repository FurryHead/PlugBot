/*
 *      irc_plugin_manager.hxx
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

#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include <string>
#include <vector>
#include <map>
#include "irc_connection.hxx"
#include "lunar.h"

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

class IRC_Connection;

class PluginManager {
    IRC_Connection* server;
    lua_State* L;
    static int print(lua_State* L);
    void c_print(std::string line);
    
    public:
    PluginManager(IRC_Connection* server);
    ~PluginManager();
    void load_start_plugins();
    void unload_all_plugins();
    void fire(std::string event, int numargs, ...);
    bool load_plugin(std::string name, bool recursing = false);
    bool loaded_plugin(std::string name);
    bool unload_plugin(std::string name, bool cleanup = true, bool recursing = false);
    bool plugin_exists(std::string name);
};

#endif // _PLUGINMANAGER_H_
