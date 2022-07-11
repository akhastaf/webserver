#ifndef PARSER_HPP
#define PARSER_HPP

#include <map>
#include <vector>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include "Tokenizer.hpp"
#include "Server.hpp"

namespace webserve
{
    class Parser
    {
        private:
            typedef struct token Token;
            Tokenizer _tokenizer;
            Token _lookahead;
            Token _eat(std::string const &token_type);
        public:
            Parser(std::string const &text);
            ~Parser();
            std::vector<webserve::Server>   parsing();
            webserve::Server    Server_block();
            void    Statmentlist_server(webserve::Server &server);
            void    Statmentlist_location(webserve::Server &server);
            void    Statment_server(webserve::Server &server);
            void    Statment_location(webserve::Location& location);
            std::pair<std::string, int> Listen_statment();
            void    Server_name_statment(std::vector<std::string> &names);
            void    Client_max_body_size_statment(int& cmbs);
            void    Error_page_statment(std::map<int, std::string> &error_page);
            void    Index_statment(std::vector<std::string>& index);
            void    Autoindex_statment(bool& autoindex);
            void    Root_statment(std::string& root);
            void    Return_statment(std::pair<int, std::string>& redirection);
            void    Upload_statment(std::string& upload_path);
            void    Allowed_methods_statment(std::vector<std::string>& allowed_methods);
            void    Cgi_pass_statment(std::map<std::string,std::string>& cgi_pass);
    };
}
#endif