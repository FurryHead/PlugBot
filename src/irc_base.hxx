/*
 *      irc_base.hxx
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

#ifndef _IRC_BASE_H_
#define _IRC_BASE_H_

#include <Sockets/TcpSocket.h>
#include <string>
#include <vector>
 
class IRC_Base : public TcpSocket {
    protected:
    void OnLine(const std::string& line);
    void OnConnect();
    virtual void process_line(std::vector<std::string> words);
    
    public:
    void sendLine(std::string line);
    IRC_Base(ISocketHandler& h);
};

#endif // _IRC_BASE_H_
