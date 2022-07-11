#include "../includes/Server.hpp"



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
webserve::Server::~Server() {}