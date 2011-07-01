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

#define VERSION "1.0"

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
#include "string.hxx"
#include "configparser.hxx"
#include "irc_connection.hxx"
using namespace std;

typedef std::map<SOCKET,Socket *> socket_m;

bool running = true;

void sighandler(int sig) { running = false; }

void run() {
    try {
        SocketHandler h;
        ConfigParser cp("conf/main.cfg");
        cout << "Configuration file: conf/main.cfg" << endl;
        
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

void print_intro() {
    cout << " ______ _            ______       _   " << endl;
    cout << " | ___ \\ |           | ___ \\     | |  " << endl;
    cout << " | |_/ / |_   _  __ _| |_/ / ___ | |_ " << endl;
    cout << " |  __/| | | | |/ _` | ___ \\/ _ \\| __| " << endl;
    cout << " | |   | | |_| | (_| | |_/ / (_) | |_ " << endl;
    cout << " \\_|   |_|\\__,_|\\__, \\____/ \\___/ \\__| " << endl;
    cout << "                 __/ |                " << endl;
    cout << "                |___/  " << endl;
    cout << endl;
    cout << "http://github.com/FurryHead/PlugBot ----- Version " << VERSION << endl;
    cout << endl;
}

void print_help() {
    cout << "Usage: ./plugbot [-h] [-c] [-v]" << endl;
    cout << endl;
    cout << "Start PlugBot - a modular C++ and Lua IRC bot." << endl;
    cout << endl;
    cout << "optional arguments:" << endl;
    cout << "  -h, --help      show this help message and exit" << endl;
    cout << "  -c, --makeconf  generate a new configuration file" << endl;
    cout << "  -v, --version   display version information and exit." << endl;
    cout << endl;
}

string my_raw(string prompt="", string _default="") {
    cout << prompt << " [" << _default << "]: ";
    cout.flush();
    string ret;
    getline(cin, ret);
    if (ret == "") 
        return _default;
    else
        return ret;
}
        
void set(ConfigParser cp, string section, string option, string text, string _default="") {
    if (cp.has_option(section, option))
        if (_default == "")
            _default = cp.get(section, option);
    
    string value = my_raw(text, _default);
    try{
    cp.set(section,option,value);
}catch(SyntaxError& e){cout<<e.what()<<endl;exit(1);}
}

void makeconf() {
    ConfigParser cp = ConfigParser();
    bool getting_servers = true;
    while (getting_servers) {
        
        string currentNetwork = my_raw("IRC network name");
        if (cp.has_section(currentNetwork)) {
            string overwrite = str::lower(my_raw("Server already exists! Overwrite? (yes/no)", "no"));
            while (overwrite != "yes" and overwrite != "y" and overwrite != "no" and overwrite != "n") 
                overwrite = str::lower(my_raw("Invalid option. Overwrite configuration for "+currentNetwork+"? (yes/no)", "no"));
                    
            if (str::lower(overwrite) == "no" || str::lower(overwrite) == "n")
                continue; //go back to top of "while (getting_servers)"
            //else continue out of try/except
        } else {
            cp.add_section(currentNetwork);
        }
            
        set(cp,currentNetwork,"host","The IRC network host to connect to");
        set(cp,currentNetwork,"channels","Channels to automatically join (comma-separated, no spaces)");
        string nick = my_raw("My nickname");
        cp.set(currentNetwork, "nick", nick);
        set(cp,currentNetwork,"ident","ident",nick);
        set(cp,currentNetwork,"owner_nick","Your username (use the auth plugin to set a password)");
        set(cp,currentNetwork,"ns_name","NickServ username (if there is none, press ENTER)");
        set(cp,currentNetwork,"ns_pwd","NickServ password (if there is none, press ENTER)");
        set(cp,currentNetwork,"port","Port", "6667");
        set(cp,currentNetwork,"use_ssl","Use ssl (yes/no)", "no");
        set(cp,currentNetwork,"comchar","My command char", "!");
        set(cp,currentNetwork,"plugins","Plugins to load on startup (comma-separated, no spaces)", "auth,logger");
            
        string another_server = str::lower(my_raw("All done with "+currentNetwork+". Add another server? (yes/no)", "no"));
        while (another_server != "yes" && another_server != "y" && another_server != "no" && another_server != "n")
            another_server = str::lower(my_raw("Invalid option. Do you want to add another server? (yes/no)", "no"));
        
        if (another_server == "no" || another_server == "n")
            break;
        //else no action needed
    }
    cp.write("conf/main.cfg");
    
    string run_now = str::lower(my_raw("Saved the configuration. Do you want to run PlugBot now? (yes/no)", "no"));
    while (run_now != "yes" && run_now != "y" && run_now != "no" && run_now != "n")
        run_now = str::lower(my_raw("Invalid option. Do you want to run PlugBot now? (yes/no)", "no"));
    
    if (run_now == "yes" || run_now == "y")
        run();
}

int main(int argc, char **argv) {
    print_intro();
    string arg;
    for (int i = 0; i < argc; i++) {
        arg = argv[i];
        if (arg == "--makeconf" || arg == "-c") {
            makeconf();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            return 0;
        } else if (arg == "--help" || arg == "-h") {
            print_help();
            return 0;
        }
    }
    
    run();
}
