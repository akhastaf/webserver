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
        Location() 
        {
            _autoindex = false;
            _allowed_methods.push_back("GET");
            _allowed_methods.push_back("POST");
            _allowed_methods.push_back("DELETE");
            _root = "html";
            _max_client_body_size = -1;
            _index.push_back("index.html");
        }
        ~Location() {}
        int _max_client_body_size;
        std::string _cgi_pass;
        std::vector<std::string> _cgi_ext;
        std::vector<std::string> _index;
        std::map<int, std::string> _error_pages;
        bool _autoindex;
        std::string _root;
        std::vector<std::string> _allowed_methods;
        std::pair<int, std::string> _redirection;
        std::string _upload;
    };
    struct Server
    {
            Server() 
            {
                _port = 80;
                _ip = "*";
                _server_names.push_back("");
                _autoindex = false;
                _root = "html";
                _max_client_body_size = 1;
                _allowed_methods.push_back("GET");
                _allowed_methods.push_back("POST");
                _allowed_methods.push_back("DELETE");
                _index.push_back("index.html");
                _locations.clear();
            }
            ~Server() {}
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
    std::ostream& operator<< (std::ostream& o, Server s)
    {
        o << "ip : " << s._ip << std::endl; 
        o << "port : " << s._port << std::endl;
        o << "server_names : ";
        for (size_t i = 0; i < s._server_names.size(); i++)
            o << s._server_names[i] << " ";
        o << std::endl;
        o << "index : ";
        for (size_t i = 0; i < s._index.size(); i++)
            o << s._index[i] << " ";
        o << std::endl;
        o << "error_pages : " << std::endl;
        for (std::map<int, std::string>::iterator it = s._error_pages.begin(); it != s._error_pages.end(); it++)
            o << "number " << it->first << " page " << it->second << std::endl;
        o << "max_client_body_size : " << s._max_client_body_size << std::endl;
        o << "autoindex : " << s._autoindex << std::endl;
        o << "root : " << s._root << std::endl;
        o << "return number : " << s._redirection.first << " location " << s._redirection.second << std::endl;
        o << "allowed_methods : ";
        for (size_t i = 0; i < s._allowed_methods.size(); i++)
            o << s._allowed_methods[i] << " ";
        o << std::endl;
        for (size_t i = 0; i < s._locations.size(); i++)
        {

            o << "Location : " << s._locations[i].first << std::endl;
            o << "cgi_pass : " << s._locations[i].second._cgi_pass << std::endl;
            o << "cgi_ext :" << s._locations[i].second._cgi_pass;
            for (size_t i = 0; i < s._locations[i].second._cgi_ext.size(); i++)
                o << " " << s._locations[i].second._cgi_pass[i] << std::endl;
            o << std::endl;
            o << "index : ";
            for (size_t j = 0; j < s._locations[i].second._index.size(); j++)
                o << s._locations[i].second._index[j] << " ";
            o << std::endl;
            o << "error_pages : " << std::endl;
            for (std::map<int, std::string>::iterator it = s._locations[i].second._error_pages.begin(); it != s._locations[i].second._error_pages.end(); it++)
                o << "number " << it->first << " page " << it->second << std::endl;
            o << "max_client_body_size : " << s._locations[i].second._max_client_body_size << std::endl;
            o << "autoindex : " << s._locations[i].second._autoindex << std::endl;
            o << "root : " << s._locations[i].second._root << std::endl;
            o << "return number : " << s._locations[i].second._redirection.first << " location " << s._locations[i].second._redirection.second << std::endl;
            o << "allowed_methods : ";
            for (size_t j = 0; j < s._locations[i].second._allowed_methods.size(); j++)
                o << s._locations[i].second._allowed_methods[j] << " ";
            o << std::endl;
        }
        return o;
    } 
    std::ostream& operator<< (std::ostream& o, Location location)
    {
        o << "cgi_pass : " << location._cgi_pass << std::endl;
        o << "cgi_ext :" << location._cgi_pass;
        for (size_t i = 0; i < location._cgi_ext.size(); i++)
            o << " " << location._cgi_pass << std::endl;
        o << std::endl;
        o << "index : ";
        for (size_t j = 0; j < location._index.size(); j++)
            o << location._index[j] << " ";
        o << std::endl;
        o << "error_pages : " << std::endl;
        for (std::map<int, std::string>::iterator it = location._error_pages.begin(); it != location._error_pages.end(); it++)
            o << "number " << it->first << " page " << it->second << std::endl;
        o << "max_client_body_size : " << location._max_client_body_size << std::endl;
        o << "autoindex : " << location._autoindex << std::endl;
        o << "root : " << location._root << std::endl;
        o << "return number : " << location._redirection.first << " location " << location._redirection.second << std::endl;
        o << "allowed_methods : ";
        for (size_t j = 0; j < location._allowed_methods.size(); j++)
            o << location._allowed_methods[j] << " ";
        o << std::endl;
        return o;
    }

    
};




#endif