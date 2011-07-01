/*
 *      irc_base.cxx
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
 
#include "irc_base.hxx"
#include "string.hxx"
#include <string>
#include <iostream>
using namespace std;

IRC_Base::IRC_Base(ISocketHandler& h) : TcpSocket(h) {
    
}

void IRC_Base::OnConnect() {
    SetLineProtocol();
}

void IRC_Base::sendLine(string line) {
    if (line.find("\r\n") == line.size()-2)
        Send(line);
    else
        Send(line+"\r\n");
}

void IRC_Base::OnLine(const string& line) {
    vector<string> data = str::split(line, " ");
    if (data[0] == "PING") {
        sendLine("PONG "+line.substr(5));
    } else if (data[0] == "ERROR") {
        
    } else {
        process_line(data);
    }
}

void IRC_Base::process_line(vector<string> words) {
    
}
