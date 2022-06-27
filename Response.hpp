#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <map>
#include <vector>
#include "Request.hpp"
#include "Server.hpp"

namespace webserve 
{
    class Response
    {
        private:
            Request _req;
            Server _server;
            int    _socket_fd;
            void    match_server(std::vector<Server> servers)
            {
                int pos;
                if (_req.has("Host"))
                {
                    std::string host = _req.get("Host").front();
                    int port = 80;
                    if ((pos = host.find(":")) != std::string::npos)
                    {
                        port = std::stoi(host.substr(pos+1));
                        host = host.substr(0, pos);
                        std::cout << "Host : " << host << " port : " << port << std::endl;
                    }
                    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
                    {
                        if (it->_port == port)
                        {
                            for (size_t i = 0; i < it->_server_names.size(); i++)
                            {
                                if (it->_server_names[i] == host)
                                {
                                    std::cout << "match" << std::endl;
                                    _server = *it;
                                    std::cout << "=================================" << std::endl;
                                    std::cout << *it << std::endl;
                                    std::cout << "=================================" << std::endl;
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        public:
            Response(Request const& req, std::vector<Server> const& servers, int socket_fd) : _req(req), _socket_fd(socket_fd)
            {
                match_server(servers);
                std::cout << "server match : ";
                std::cout << _server << std::endl;
            }
            ~Response() {}
    };
    
}


#endif