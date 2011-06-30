/*
 *      irc_plugin_manager.cxx
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

#include <fstream>
#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <cstdarg>
#include <sstream>
#include "string.hxx"
#include "configparser.hxx"
#include "irc_plugin_manager.hxx"
using namespace std;

bool check_stack(lua_State* L) {
    int n = lua_gettop(L); 
    for (int i = 1; i <= n; i++) { 
        if (!lua_isstring(L, i)) { 
            stringstream err;
            err<<"Bad argument # "<<i<<": expected a string, but got type '"<<lua_typename(L, lua_type(L, i))<<"'";
            lua_pushstring(L, err.str().c_str()); 
            lua_error(L); 
            return false;
        } 
    }
    return true;
}

class IRC_Wrapper {
    IRC_Connection* real_obj;
    
    public:
    IRC_Wrapper(lua_State* L) {
         real_obj = (IRC_Connection*)lua_touserdata(L, 1);
    }
    
    int config(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) < 1)
            return 0;
        
        map<string,string> cfg = real_obj->getConfig();
        string key = (lua_isstring(L, -1) ? lua_tostring(L, -1) : "");
        if (key == "")  
            lua_pushnil(L);
        else
            lua_pushstring(L, cfg[key].c_str());
            
        return 1;
    }
    
    int plugin(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        int n = lua_gettop(L);
        if (n < 1) {
            return 0;
        }
            
        PluginManager* mgr = real_obj->pluginManager();
        lua_getglobal(L, "plugins");
        if (!lua_istable(L, -1)) {
            lua_pop(L,1);
            lua_newtable(L);
            lua_setglobal(L, "plugins");
            return 0;
        }
        
        if (!lua_isstring(L, -2)) {
            lua_pop(L,n+1);
            return 0;
        }
        
        string name = lua_tostring(L, -2);
        lua_getfield(L, -1, name.c_str());
        if (!lua_istable(L, -1)) {
            lua_pop(L, n+2);
            return 0;
        }
        return 1;
    }
    
    int sendLine(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0)
            real_obj->sendLine(lua_tostring(L, 1));
        return 0;
    }
    
    int quit(lua_State*  L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0)
            real_obj->quit(lua_tostring(L, 1));
        else
            real_obj->quit();
            
        return 0;
    }  
    
    int join(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0)
            real_obj->join(lua_tostring(L, 1));
        return 0;
    }
    
    int part(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 1)
            real_obj->part(lua_tostring(L, 1), lua_tostring(L, 2));
        else if (lua_gettop(L) > 0)
            real_obj->part(lua_tostring(L, 1));
        return 0;
    }
    
    int msg(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 1)
            real_obj->msg(lua_tostring(L, 1), lua_tostring(L, 2));
        return 0;
    }
    
    int notice(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 1)
            real_obj->notice(lua_tostring(L, 1), lua_tostring(L, 2));
        return 0;
    }
    
    int action(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 1)
            real_obj->action(lua_tostring(L, 1), lua_tostring(L, 2));
        return 0;
    }
    
    int kick(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 2)
            real_obj->kick(lua_tostring(L, 1), lua_tostring(L, 2), lua_tostring(L, 3));
        else if (lua_gettop(L) > 1) 
            real_obj->kick(lua_tostring(L, 1), lua_tostring(L, 2));
        return 0;
    }
    
    int mode(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 2)
            real_obj->mode(lua_tostring(L, 1), lua_tostring(L, 2), lua_tostring(L, 3));
        else if (lua_gettop(L) > 1) 
            real_obj->mode(lua_tostring(L, 1), lua_tostring(L, 2));
        return 0;
    }
    
    int nick(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0)
            real_obj->nick(lua_tostring(L, 1));
        return 0;
    }
    
    int load_plugin(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0) {
            bool success = real_obj->pluginManager()->load_plugin(lua_tostring(L, 1));
            lua_pop(L, 1);
            lua_pushboolean(L, success ? 1 : 0);
            return 1;
        }
        return 0;
    }
    
    int unload_plugin(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0) {
            bool success = real_obj->pluginManager()->unload_plugin(lua_tostring(L, 1));
            lua_pop(L, 1);
            lua_pushboolean(L, success ? 1 : 0);
            return 1;
        }
        return 0;
    }
    
    int plugin_exists(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0) {
            bool exists = real_obj->pluginManager()->plugin_exists(lua_tostring(L, 1));
            lua_pop(L, 1);
            lua_pushboolean(L, exists ? 1 : 0);
            return 1;
        }
        return 0;
    }
    
    int loaded_plugin(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0) {
            bool loaded = real_obj->pluginManager()->plugin_exists(lua_tostring(L, 1));
            lua_pop(L, 1);
            lua_pushboolean(L, loaded ? 1 : 0);
            return 1;
        }
        return 0;
    }
    
    static const char className[];
    static Lunar<IRC_Wrapper>::RegType methods[];
};

const char IRC_Wrapper::className[] = "IRC";
#define ircmethod(name) {#name, &IRC_Wrapper::name}
Lunar<IRC_Wrapper>::RegType IRC_Wrapper::methods[] = {
    ircmethod(config),
    ircmethod(plugin),
    ircmethod(sendLine),
    ircmethod(quit),
    ircmethod(join),
    ircmethod(part),
    ircmethod(msg),
    ircmethod(action),
    ircmethod(kick),
    ircmethod(nick),
    ircmethod(notice),
    ircmethod(mode),
    ircmethod(load_plugin),
    ircmethod(unload_plugin),
    ircmethod(plugin_exists),
    ircmethod(loaded_plugin),
    {0,0}
};

class ConfigParser_Wrapper {
    ConfigParser *real_obj;
    
    public:
    ConfigParser_Wrapper(lua_State* L) {
        real_obj = new ConfigParser();
    }
    
    int read(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0) {
            try {
                real_obj->read(lua_tostring(L, 1));
                lua_pop(L, lua_gettop(L));
                return 0;
            } catch(SyntaxError& e) {
                lua_pop(L, lua_gettop(L));
                lua_pushstring(L, e.what().c_str());
                return 1;
            }
        } else
            return 0;
    }
    
    int write(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0) {
            try {
                real_obj->write(lua_tostring(L, 1));
                lua_pop(L, lua_gettop(L));
                return 0;
            } catch(SyntaxError& e) {
                lua_pop(L, lua_gettop(L));
                lua_pushstring(L, e.what().c_str());
                return 1;
            }
        } else
            return 0;
    }
    
    int sections(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        lua_pop(L, lua_gettop(L));
        vector<string> s = real_obj->sections();
        lua_newtable(L);
        int j = 1;
        for (vector<string>::iterator i = s.begin(); i != s.end(); ++i, j++) {
            lua_pushstring(L, i->c_str());
            lua_rawseti(L, -2, j);
        }
        return 1;
    }
    
    int options(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0) {
            try {
                vector<string> opts = real_obj->options(lua_tostring(L, 1));
                lua_pop(L, lua_gettop(L));
                int j = 1;
                lua_newtable(L);
                for (vector<string>::iterator i = opts.begin(); i != opts.end(); ++i, j++) {
                    lua_pushstring(L, i->c_str());
                    lua_rawseti(L, -2, j);
                }
                return 1;
            } catch(SyntaxError& e) {
                lua_pop(L, lua_gettop(L));
                lua_pushnil(L);
                return 1;
            }
        } else {
            lua_pushnil(L);
            return 1;
        }
    }
    
    int get(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 1) {
            try {
                string tmp = real_obj->get(lua_tostring(L, 1), lua_tostring(L, 2));
                lua_pop(L, lua_gettop(L));
                lua_pushstring(L, tmp.c_str());
                return 1;
            } catch(SyntaxError& e) {
                lua_pop(L, lua_gettop(L));
                return 0;
            }
        } else {
            if (lua_gettop(L) > 0)
                lua_pop(L, lua_gettop(L));
            return 0;
        }
    }
    
    int set(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 2) {
            try {
                real_obj->set(lua_tostring(L, 1), lua_tostring(L, 2), lua_tostring(L, 3));
            } catch(SyntaxError& e) {
                lua_pop(L, lua_gettop(L));
                return 0;
            }
        } else {
            if (lua_gettop(L) > 0)
                lua_pop(L, lua_gettop(L));
            
            return 0;
        }
    }
    
    int items(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0) {
            try {
                map<string,string> s = real_obj->items(lua_tostring(L, 1));
                lua_pop(L, lua_gettop(L));
                lua_newtable(L);
                int ind = lua_gettop(L);
                for (map<string,string>::iterator i = s.begin(); i != s.end(); ++i) {
                    lua_pushstring(L, i->second.c_str());
                    lua_setfield(L, ind, i->first.c_str());
                }
                return 1;
            } catch(SyntaxError& e) {
                lua_pop(L, lua_gettop(L));
                return 0;
            }
        } else
            return 0;
    }
    
    int has_section(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0) {
            bool has = real_obj->has_section(lua_tostring(L, 1));
            lua_pop(L, lua_gettop(L));
            lua_pushboolean(L, has ? 1 : 0);
            return 1;
        } else 
            return 0;
    }
    
    int has_option(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 1) {
            bool has = real_obj->has_option(lua_tostring(L, 1), lua_tostring(L, 2));
            lua_pop(L, lua_gettop(L));
            lua_pushboolean(L, has ? 1 : 0);
            return 1;
        } else {
            if (lua_gettop(L) > 0)
                lua_pop(L, lua_gettop(L));
            
            return 0;
        }
    }
    
    int add_section(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0) {
            try {
                real_obj->add_section(lua_tostring(L, 1));
            } catch(SyntaxError& e) {
                lua_pop(L, lua_gettop(L));
                return 0;
            }
        }
        return 0;
    }
    
    int  remove_section(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 0) {
            try {
                real_obj->remove_section(lua_tostring(L, 1));
            } catch(SyntaxError& e) {
                lua_pop(L, lua_gettop(L));
                return 0;
            }
        } 
        return 0;
    }
    
    int  remove_option(lua_State* L) {
        if (!check_stack(L)) 
            return 0;
            
        if (lua_gettop(L) > 1) {
            try {
                real_obj->remove_option(lua_tostring(L, 1), lua_tostring(L, 1));
            } catch(SyntaxError& e) {
                lua_pop(L, lua_gettop(L));
                return 0;
            }
        } else {
            if (lua_gettop(L) > 0)
                lua_pop(L, lua_gettop(L));
        }
        return 0;
    }
    
    static const char className[];
    static Lunar<ConfigParser_Wrapper>::RegType methods[];
};

const char ConfigParser_Wrapper::className[] = "ConfigParser";
#define cpmethod(name) {#name, &ConfigParser_Wrapper::name}
Lunar<ConfigParser_Wrapper>::RegType ConfigParser_Wrapper::methods[] = {
    cpmethod(read),
    cpmethod(write),
    cpmethod(sections),
    cpmethod(options),
    cpmethod(get),
    cpmethod(set),
    cpmethod(items),
    cpmethod(has_section),
    cpmethod(has_option),
    cpmethod(add_section),
    cpmethod(remove_section),
    cpmethod(remove_option),
    {0,0}
};

IRC_Connection* conn;
PluginManager::PluginManager(IRC_Connection* serv) {
    server = conn = serv;
    L = luaL_newstate();
    luaL_openlibs(L);
    lua_getglobal(L, "print");
    lua_setglobal(L, "print_bak");
    lua_pushcfunction(L, &PluginManager::print);
    lua_setglobal(L, "print");
    
    Lunar<ConfigParser_Wrapper>::Register(L);
    
    Lunar<IRC_Wrapper>::Register(L);
    lua_getglobal(L, IRC_Wrapper::className);
    lua_pushlightuserdata(L, (void*)serv);
    lua_call(L, 1, 1);
    lua_setglobal(L, "server");
    
    lua_newtable(L);
    int table_index = lua_gettop(L);
    string name;
    
    lua_pushnil(L);
    while (lua_next(L, LUA_GLOBALSINDEX) != 0) {
        /* key is at -2, value at -1 */
        int value = lua_gettop(L), key = value-1;
        if (lua_isstring(L, key)) {
            name = lua_tostring(L, key);
            if (name != "IRC") {
                lua_setfield(L, table_index, name.c_str());
                continue;
            }
        }
        lua_pop(L, 1);
    }
    lua_setglobal(L, "new_env_items");
}

void PluginManager::load_start_plugins() {
    vector<string> plugs = str::split(server->getConfig()["plugins"], ",");
    for (vector<string>::iterator i = plugs.begin(); i != plugs.end(); ++i)
        load_plugin(*i);
}

void PluginManager::unload_all_plugins() {
    luaL_dostring(L, "p = {} for k,v in plugins do table.insert(p, #p+1, k) end for _,v in ipairs(p) do server:unload_plugin(v) end");
}

//static
int PluginManager::print(lua_State* L) {
    cout << "[" << conn->getConfig()["network"] << "] ";
    lua_getglobal(L, "print_bak");
    lua_CFunction p = lua_tocfunction(L, -1);
    lua_pop(L, 1);
    p(L);
}

void PluginManager::c_print(string line) {
    cout << "[" << server->getConfig()["network"] << "] ";
    cout << line << endl;
}

void PluginManager::fire(string event, int numargs, ...) {
    lua_getglobal(L, "plugins");
    int table_index = lua_gettop(L);
    if(!lua_istable(L, table_index)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_setglobal(L, "plugins");
        return;
    }
    lua_pushnil(L);
    while (lua_next(L, table_index) != 0) {
        /* key is at -2, value at -1 */
        int value = lua_gettop(L), key = value-1;
        if (lua_isstring(L, key)) {
            lua_getfield(L, value, ("handle_"+event).c_str());
            if (lua_isfunction(L, -1)) {
                va_list args;
                va_start(args, numargs);
                int size;
                string *str;
                for (int i=0;i<numargs;i++) {
                    size = va_arg(args, int);
                    if (size >= 1) {
                        str = va_arg(args, string*);
                        lua_newtable(L);
                        int table_index = lua_gettop(L);
                        for (int j = 0; j < size; j++) {
                            lua_pushstring(L,str[j].c_str());
                            lua_rawseti(L,table_index,j+1);
                        }
                    } else if (size == 0) {
                        lua_newtable(L);
                    } else if (size == -1) {
                        str = va_arg(args, string*);
                        lua_pushstring(L, str->c_str());
                    }
                }
                int code = lua_pcall(L, numargs, 0, 0);
                if (code != 0) {
                    c_print(string("Plugin ")+lua_tostring(L, key)+string(" handle_"+event+" error: ")+lua_tostring(L, -1));
                    lua_pop(L, 1);
                }
                va_end(args);
            } else
                lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

bool PluginManager::load_plugin(string name, bool recursing) {
    if (!plugin_exists(name)) {
        c_print("Error loading plugin "+name+": No such plugin.");
        return false;
    } else if (loaded_plugin(name)) {
        if (!recursing) {
            c_print("Error loading plugin "+name+": It has already been loaded.");
            return false;
        } else
            return true;
    }
    
    int code = luaL_loadfile(L, ("plugins/"+name+".lua").c_str());
    if (code != 0) {
        cout << "Error loading plugin " << name << ": " << lua_tostring(L, 1) << endl;
        lua_pop(L, 1);
        return false;
    } 
    int code_index = lua_gettop(L);
        
    lua_newtable(L);
    lua_setglobal(L, "plugin_env");
    lua_getglobal(L, "plugin_env");
    int new_env = lua_gettop(L);
    lua_newtable(L);
    lua_newtable(L);
    lua_setfield(L, -2, "__index");
    lua_newtable(L);
    lua_setfield(L, -2, "__metatable");
    lua_setmetatable(L, new_env);
    
    lua_getglobal(L, "new_env_items");
    int env_items = lua_gettop(L);
    lua_pushnil(L);
    string pname;
    while (lua_next(L, env_items) != 0) {
        /* key is at -2, value at -1 */
        int value = lua_gettop(L), key = value-1;
        if (lua_isstring(L, key)) {
            pname = lua_tostring(L, key);
            if (pname != "IRC") {
                lua_setfield(L, new_env, pname.c_str());
                continue;
            }
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    
    lua_setfenv(L, code_index);
    code = lua_pcall(L, 0, 0, 0);
    if (code != 0) {
        c_print("Error initializing plugin "+name+": "+ lua_tostring(L, -1));
        lua_pop(L, 1);
        return false;
    }
    
    lua_getglobal(L, "plugins");
    if (!lua_istable(L, -1)) {
        lua_newtable(L);
        lua_setglobal(L, "plugins");
        lua_getglobal(L, "plugins");
    }
    lua_getglobal(L, "plugin_env");
    lua_setfield(L, -2, name.c_str());
    lua_pop(L, 1);
    
    lua_getglobal(L, "plugin_env");
    new_env = lua_gettop(L);
    
    lua_getfield(L, new_env, "depends");
    if (lua_istable(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            /* key is at -2, value at -1 */
            int value = lua_gettop(L), key = value-1;
            if (lua_isstring(L, value)) {
                load_plugin(lua_tostring(L, value), true);
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "startup");
    if (lua_isfunction(L, -1)) {
        code = lua_pcall(L, 0, 0, 0);
        if (code != 0) {
            c_print("Error starting plugin "+name+": "+ lua_tostring(L, -1));
            unload_plugin(name, false);
            lua_pop(L, 2);
            return false;
        }
    } else
        lua_pop(L, 1);
    lua_pop(L, 1);
    
    return true;
}

bool PluginManager::unload_plugin(string name, bool cleanup, bool recursing) {
    if (!plugin_exists(name)) {
        c_print("Error unloading plugin "+name+": No such plugin.");
        return false;
    } else if (!loaded_plugin(name)) {
        if (!recursing) {
            c_print("Error unloading plugin "+name+": It has not been loaded.");
            return false;
        } else
            return true;
    }
    
    lua_getglobal(L, "plugins");
    int table_index = lua_gettop(L);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_setglobal(L, "plugins");
        return false;
    }
    lua_pushnil(L);
    while (lua_next(L, table_index) != 0) {
        /* key is at -2, value at -1 */
        int value = lua_gettop(L), key = value-1;
        if (lua_isstring(L, key)) {
            if (name == lua_tostring(L, key)) {
                
                lua_pushnil(L);
                lua_setfield(L, table_index, name.c_str());
                cout << "Unloading: " << name << " (recursing: " << recursing << ")" << endl;
                //===================================================================
                lua_newtable(L); 
                lua_setglobal(L, "deps");
                luaL_dostring(L, ("for k,v in pairs(plugins) do if v.depends ~= nil then for _,dep in ipairs(v.depends) do if dep == \""+name+"\" then table.insert(deps, #deps+1, k) end end end end").c_str());
                lua_getglobal(L, "deps");
                int t_ind = lua_gettop(L);
                lua_pushnil(L);
                while (lua_next(L, t_ind) != 0) {
                    // key is at -2, value at -1 
                    int value1 = lua_gettop(L), key1 = value-1;
                    if (lua_isstring(L, value1)) {
                        unload_plugin(lua_tostring(L, value1), true, true);
                    }
                    lua_pop(L, 1);
                }
                lua_pop(L, 1); 
                //===================================================================
                
                if (cleanup) {
                    lua_getfield(L, value, "cleanup");
                    if (lua_isfunction(L, -1)) {
                        int code = lua_pcall(L, 0, 0, 0);
                        if (code != 0) {
                            c_print("Error cleaning up plugin "+name+": "+lua_tostring(L, -1));
                            lua_pop(L, 5);
                            return false;
                        }
                    } else
                        lua_pop(L, 1);
                } 
                
                lua_pop(L, 3);
                break;
            }
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return true;
}

bool PluginManager::loaded_plugin(string name) {
    lua_getglobal(L, "plugins");
    int table_index = lua_gettop(L);
    if(!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_setglobal(L, "plugins");
        return false;
    }
    lua_pushnil(L);
    string pname;
    while (lua_next(L, table_index) != 0) {
        /* key is at -2, value at -1 */
        int value = lua_gettop(L), key = value-1;
        if (lua_isstring(L, key)) {
            pname = lua_tostring(L, key);
            if (pname == name) {
                lua_pop(L, 2);
                return true;
            }
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return false;
}

bool PluginManager::plugin_exists(string name) {
    ifstream plugin(("plugins/"+name+".lua").c_str(), ios::in);
    bool open = plugin.is_open();
    if (open) plugin.close();
    return open;
}
