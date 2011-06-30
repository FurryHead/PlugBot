/*
 *      main.cxx
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

#include <Sockets/TcpSocket.h>
#include <Sockets/SocketHandler.h>
#include <Sockets/Exception.h>
#include <Sockets/socket_include.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <signal.h>
#include "configparser.hxx"
#include "irc_connection.hxx"
using namespace std;

typedef std::map<SOCKET,Socket *> socket_m;

bool running = true;

void sighandler(int sig) { running = false; }

void checkerr(lua_State* L, int code) {
    if (code != 0) {
        string err = (lua_isstring(L, -1) ? lua_tostring(L, -1) : "");
        cout << "Error: " << err << endl;
        lua_close(L);
        exit(1);
    } 
}

int main(int argc, char **argv) {
    try {
        SocketHandler h;
        ConfigParser cp("conf/bot.cfg");
        
        stringstream portss;
        int port;
        vector<string> networks = cp.sections();
        for (vector<string>::iterator i = networks.begin(); i != networks.end(); ++i) {
            map<string, string> cfg = cp.items(*i);
            cfg["network"] = *i;
            cfg["confdir"] = "conf/";
            IRC_Connection *p = new IRC_Connection(h, cfg);
            
            p->SetDeleteByHandler();
            portss << cfg["port"];
            portss >> port;
            if (cfg["use_ssl"] == "true")
                p->InitSSLClient();
                
            p->Open(cfg["host"], port);
            h.Add(p);
        }
        
        signal(SIGABRT, &sighandler);
        signal(SIGTERM, &sighandler);
        signal(SIGINT, &sighandler);
        
        h.Select(1,0);
        while (h.GetCount()) {
            if (!running) {
                socket_m socks = h.AllSockets();
                for (socket_m::iterator i = socks.begin(); i != socks.end(); ++i) {
                    IRC_Connection *conn = (IRC_Connection*)i->second;
                    conn->quit("Caught SIGINT");
                }
                break;
            }
            h.Select(1,0);
        }
        if (running)
            cout << "All connections closed, exiting." << endl;
        else
            cout << "Caught SIGINT, exiting." << endl;
    } catch(SyntaxError& e) {
        cout << "SyntaxError: " << e.what() << endl;
    } catch(Exception& e) {
        cout << "Exception: " << e.ToString() << endl;
    }
}
