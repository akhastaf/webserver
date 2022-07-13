#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <vector>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>


namespace webserve
{
    struct Location
    {
        Location();
        ~Location();
        int _max_client_body_size;
        std::map<std::string,std::string> _cgi_pass;
        std::vector<std::string> _index;
        std::map<int, std::string> _error_pages;
        bool _autoindex;
        std::string _root;
        std::vector<std::string> _allowed_methods;
        std::pair<int, std::string> _redirection;
        std::string _upload;
        // friend std::ostream& operator<< (std::ostream& o, webserve::Location const& location);
    };
    struct Server
    {
            Server();
            ~Server();
            std::string _ip;
            int _port;
            std::vector<std::string> _server_names;
            std::vector<std::string> _index;
            std::map<int, std::string> _error_pages;
            int _max_client_body_size;
            bool _autoindex;
            std::string _root;
            std::pair<int, std::string> _redirection;
            std::string _upload;
            std::vector<std::string> _allowed_methods;
            std::vector<std::pair<std::string, webserve::Location> > _locations;
    };
    std::ostream& operator<< (std::ostream& o, webserve::Location const& location);
};




#endif