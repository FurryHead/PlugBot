/*
 *      irc_connection.hxx
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
#ifndef _IRC_CONNECTION_H_
#define _IRC_CONNECTION_H_

#include <string>
#include <vector>
#include <map>
#include "irc_base.hxx"
#include "irc_plugin_manager.hxx"

class PluginManager;

class IRC_Connection : public IRC_Base {
    protected:
    std::map<std::string,std::string> config;
    PluginManager *plugins;
    void OnConnect();
    void signedOn();
    void OnDisconnect();
    
    public:
    IRC_Connection(ISocketHandler& h, std::map<std::string,std::string> cfg);
    void process_line(std::vector<std::string> words);
    std::map<std::string,std::string> getConfig() const;
    PluginManager* pluginManager();
    
    void quit(std::string reason="Quitting.");
    void join(std::string channel);
    void part(std::string channel, std::string reason="Leaving.");
    void msg(std::string dest, std::string message);
    void action(std::string dest, std::string action);
    void kick(std::string channel, std::string user, std::string reason="Kicked!");
    void nick(std::string newnick);
    void notice(std::string dest, std::string message);
    void mode(std::string channel, std::string mode, std::string user = "");
};

#endif // _IRC_CONNECTION_H_
