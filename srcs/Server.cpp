#include "../includes/Server.hpp"
#include <ostream>



webserve::Location::Location() 
{
    _autoindex = false;
    _allowed_methods.push_back("GET");
    _allowed_methods.push_back("POST");
    _allowed_methods.push_back("DELETE");
    _root = "html";
    _max_client_body_size = -1;
    _index.push_back("index.html");
}
webserve::Location::~Location() {}



webserve::Server::Server() 
{
    _port = 8080;
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

std::ostream& webserve::operator<< (std::ostream& o, webserve::Location const& location)
{
    o << "cgi_pass : ";
    std::map<std::string, std::string>::const_iterator it = location._cgi_pass.begin();
    for (; it != location._cgi_pass.end(); it++)
        o << "cgi :" << it->second << " " << it->second << std::endl;
    o << "index : ";
    for (size_t j = 0; j < location._index.size(); j++)
        o << location._index[j] << " ";
    o << std::endl;
    o << "error_pages : " << std::endl;
    std::map<int, std::string>::const_iterator it1 = location._error_pages.begin();
    for (; it1 != location._error_pages.end(); it1++)
        o << "number " << it1->first << " page " << it1->second << std::endl;
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
webserve::Server::~Server() {}