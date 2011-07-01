/*
 *      irc_connection.cxx
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

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "irc_connection.hxx"
#include "string.hxx"

using namespace std;

const int STR = -1, ARR = 0;

IRC_Connection::IRC_Connection(ISocketHandler& h, std::map<std::string,std::string> cfg) : IRC_Base(h) {
    config = cfg;
    plugins = new PluginManager(this);
    plugins->load_start_plugins();
}

void IRC_Connection::OnConnect() {
    IRC_Base::OnConnect();
    config["ident"] = str::trim(config["ident"], " ") == "" ? config["nick"] : config["ident"];
    sendLine("USER "+config["ident"]+" * * *");
    sendLine("NICK "+config["nick"]);
    plugins->fire("connect", 0);
}

void IRC_Connection::OnDisconnect() {
    plugins->fire("disconnect", 0);
}

void IRC_Connection::signedOn() {
    vector<string> channels = str::split(config["channels"], ",");
    for (vector<string>::iterator i = channels.begin(); i != channels.end(); ++i)
        join(*i);
    
    plugins->fire("connected", 0);
}

map<string,string> IRC_Connection::getConfig() const {
    return config;
}

PluginManager* IRC_Connection::pluginManager() {
    return plugins;
}

void IRC_Connection::process_line(vector<string> words) {
    string line = str::join(words.begin(), words.end(), " ");

    string user[3]; 
    // this end up in the sequence [0] = nick, [1] = ident, [2] = host
    user[0] = str::split(str::trim(words[0], ":"), "!")[0];
    if (words[0].find("!") != string::npos && words[0].find("@") != string::npos) {
        user[1] = str::split(str::split(str::trim(words[0], ":"), "!")[1], "@")[0];
        user[2] = str::split(str::split(str::trim(words[0], ":"), "!")[1], "@")[1];
    } else {
        user[1] = "";
        user[2] = "";
    }
    
    string event = str::upper(words[1]);
    
    if (event == "376" || event == "422") {
        signedOn();
    } else if (event == "PRIVMSG" /* msg, action, and cmd */) {
        if (words[3][0] == ':') words[3] = words[3].substr(1);
        string message = str::join(words.begin()+3, words.end(), " ");
        string channel = (words[2] == config["nick"] ? str::split(str::trim(words[0], ":"), "!")[0] : words[2]);
        
        if (message.substr(0, 7) == "\01ACTION") {
            message = message.substr(8, message.size()-9);
            plugins->fire("action", 3, STR, &channel, ARR+3, &user, STR, &message);
            return;
        }
        
        plugins->fire("message", 3, STR, &channel, ARR+3, &user, STR, &message);
        
        if (message.find(config["comchar"]) == 0) {
            vector<string> tmp = str::split(message, " ");
            string *args = new string[tmp.size()];
            int k = 0;
            vector<string>::iterator i = tmp.begin();
            i++;
            while (i != tmp.end()) {
                args[k] = *i;
                k++, i++;
            }
            string command = tmp[0].substr(config["comchar"].size());
            if (command != "")
                if (k > 0) 
                    plugins->fire("command", 4, STR, &channel, ARR+3, &user, STR, &command, ARR+k, args);
                else
                    plugins->fire("command", 4, STR, &channel, ARR+3, &user, STR, &command, ARR);
                    
        } else if (config["address_is_command"] == "true" && message.find(config["nick"]) == 0) {
            vector<string> tmp = str::split(message, " ");
            string *args = new string[tmp.size()];
            int k = 0;
            vector<string>::iterator i = tmp.begin();
            i += 2;
            while (i != tmp.end()) {
                args[k] = *i;
                k++, i++;
            }
            string command = tmp[1];
            if (k > 0) 
                plugins->fire("command", 4, STR, &channel, ARR+3, &user, STR, &command, ARR+k, args);
            else
                plugins->fire("command", 4, STR, &channel, ARR+3, &user, STR, &command, ARR);
        }
    }  else if (event == "JOIN") {
        string channel = str::trim(words[2], ":");
        plugins->fire("join", 2, STR, &channel, ARR+3, &user);
    } else if (event == "PART") {
        string channel = str::trim(words[2], ":");
        string message = str::join(words.begin() + 3, words.end(), " ");
        if (message != "")
            plugins->fire("part", 2, STR, &channel, ARR+3, &user, STR, &message);
        else
            plugins->fire("part", 2, STR, &channel, ARR+3, &user);
    } else if (event == "NOTICE") {
    //:bla!Bla@127.0.0.1 NOTICE bla :hi!
        string dest = words[2];
        string message = line.substr(line.find(dest)+dest.size()+1);
        if (message[0] == ':') message = message.substr(1);
        plugins->fire("notice", 2, STR, &dest, ARR+3, &user, STR, &message);
    } else if (event == "QUIT") {
        string message = line.substr(line.find(words[1])+event.size()+1);
        if (message[0] == ':') message = message.substr(1);
        if (message == "")
            plugins->fire("quit", 1, ARR+3, &user);
        else
            plugins->fire("quit", 2, ARR+3, &user, STR, &message);
        
    } else if (event == "KICK") {
        string channel = words[2];
        string otheruser = words[3];
        line = line.substr(line.find(otheruser)+otheruser.size()+1);
        if (line[0] == ':') line = line.substr(1);
        if (line == "")
            plugins->fire("kick", 3, STR, &channel, ARR+3, &user, STR, &otheruser);
        else
            plugins->fire("kick", 4, STR, &channel, ARR+3, &user, STR, &otheruser, STR, &line);
    } else if (event == "MODE") {
        string mode = words[3];
        string channel = words[2];
        if (words.size() > 4) {
            string otheruser = words[4];
            plugins->fire("mode", 4, STR, &channel, ARR+3, &user, STR, &mode, STR, &otheruser);
        } else
            plugins->fire("mode", 3, STR, &channel, ARR+3, &user, STR, &mode);
    } else if (event == "NICK") {
        string nick = words[2];
        plugins->fire("nick", 2, ARR+3, &user, STR, &nick);
    } else if (event == "353") {
        //:penguin.omega.org.za 353 Test = #plugbot :TestUser42
        string channel = words[4];
        string tmp = line.substr(line.find(":", line.find(channel))+1);
        vector<string> t = str::split(tmp, " ");
        string *users = new string[t.size()+1];
        int j = 0;
        for (vector<string>::iterator i = t.begin(); i != t.end(); ++i, j++)
            users[j] = *i;
        plugins->fire("353", 2, STR, &channel, ARR+j, users);
    } else if (event == "366") {
        string dest = words[3];
        plugins->fire("366", 1, STR, &dest);
    } else {
        /* fires as source, target, message (message can be nil) */
        int ind = line.find(words[1])+event.size()+1;
        string target = line.substr(ind);
        int inde = target.find(":", ind);
        if (inde != string::npos)
            target = target.substr(0, inde-1);
        
        line = line.substr(ind); 
        if (line[0] == ':') line = line.substr(1);
        
        if (line != "")
            plugins->fire(str::lower(event), 3, ARR+3, &user, STR, &target, STR, &line);
        else
            plugins->fire(str::lower(event), 2, ARR+3, &user, STR, &target);
    }
}

void IRC_Connection::quit(string reason) {
    sendLine("QUIT "+reason);
    SetCloseAndDelete();
    OnDisconnect();
    plugins->unload_all_plugins();
}

void IRC_Connection::join(string channel) {
    sendLine("JOIN "+channel);
}

void IRC_Connection::part(string channel, string reason) {
    sendLine("PART "+channel+" :"+reason);
}

void IRC_Connection::msg(string dest, string message) {
    sendLine("PRIVMSG "+dest+" :"+message);
}

void IRC_Connection::action(string dest, string action) {
    msg(dest, "\01ACTION "+action+"\01");
}

void IRC_Connection::kick(string channel, string user, string reason) {
    sendLine("KICK "+channel+" "+user+" :"+reason);
}

void IRC_Connection::nick(string newnick) {
    sendLine("NICK "+newnick);
}

void IRC_Connection::notice(string dest, string message) {
    sendLine("NOTICE "+dest+" :"+message);
}

void IRC_Connection::mode(string channel, string mode, string user) {
    sendLine("MODE "+channel+" "+mode+" "+user);
}
