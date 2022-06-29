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

        Token _eat(std::string const &token_type)
        {
            Token t = _lookahead;
            try
            {
                if (t.type == "NULL")
                    throw ("Unexpected end of input, expected : " + token_type);
                if (t.type != token_type)
                    throw ("Unexpected token : " + t.type + ", expected : " + token_type);
                _lookahead = _tokenizer.get_next_token();
            }
            catch (std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
            return t;
        }

    public:
        Parser(std::string const &text) : _tokenizer(text)
        {
            try
            {
                _lookahead = _tokenizer.get_next_token();
            }
            catch (std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
        }
        ~Parser() {}

        std::vector<webserve::Server> parsing()
        {
            std::vector<webserve::Server> config;
            // while (_lookahead.type != "NULL")
            // {
            //     std::cout << _lookahead;
            //     _lookahead = _tokenizer.get_next_token();
            // }
            do
            {
                config.push_back(Server_block());
            } while (_lookahead.type == "SERVER");
            for (size_t i = 0; i < config.size(); i++)
            {
                std::cout << config[i] << std::endl;
            }
            return config;
        }

        webserve::Server Server_block()
        {
            webserve::Server server;
            try 
            {
                _eat("SERVER");
                _eat("{");
                Statmentlist_server(server);
                _eat("}");
            }
            catch (std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
            return server;
        }
       
        void Statmentlist_server(webserve::Server &server)
        {

            while (_lookahead.type != "}")
                Statment_server(server);
        }
        void Statmentlist_location(webserve::Server &server)
        {

            std::pair<std::string, webserve::Location> newlocation;
            try
            {
                _eat("LOCATION");
                newlocation.first = _eat("STRING").value;
                _eat("{");
                while (_lookahead.type != "}")
                    Statment_location(newlocation.second);
                _eat("}");
            }
            catch (std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
            server._locations.push_back(newlocation);
        }
        
        void Statment_server(webserve::Server &server)
        {
            if (_lookahead.type == "LISTEN")
            {
                std::pair<std::string, int> t = Listen_statment();
                server._ip = t.first.empty() ? server._ip : t.first;
                server._port = t.second == 0 ? server._port : t.second;
            }
            else if (_lookahead.type == "SERVER_NAME")
                Server_name_statment(server._server_names);
            else if (_lookahead.type == "CLINET_BODY_MAX_SIZE")
                Client_max_body_size_statment(server._max_client_body_size);
            else if (_lookahead.type == "ERROR_PAGE")
                Error_page_statment(server._error_pages);
            else if (_lookahead.type == "INDEX")
                Index_statment(server._index);
            else if (_lookahead.type == "AUTOINDEX")
                Autoindex_statment(server._autoindex);
            else if (_lookahead.type == "ROOT")
                Root_statment(server._root);
            else if (_lookahead.type == "RETURN")
                Return_statment(server._redirection);
            else if (_lookahead.type == "UPLOAD")
                Upload_statment(server._upload);
            else if (_lookahead.type == "ALLOWED_METHOD")
                Allowed_methods_statment(server._allowed_methods);
            else if (_lookahead.type == "LOCATION")
                Statmentlist_location(server);
            else
                throw ("Unexpected token here : " + _lookahead.type);
        }
        
        void    Statment_location(webserve::Location& location)
        {
            // if (_lookahead.type == "CLINET_BODY_MAX_SIZE")
            //     Client_max_body_size_statment(location._max_client_body_size);
            // else 
            if (_lookahead.type == "ERROR_PAGE")
                Error_page_statment(location._error_pages);
            else if (_lookahead.type == "INDEX")
                Index_statment(location._index);
            else if (_lookahead.type == "AUTOINDEX")
                Autoindex_statment(location._autoindex);
            else if (_lookahead.type == "ROOT")
                Root_statment(location._root);
            else if (_lookahead.type == "RETURN")
                Return_statment(location._redirection);
            else if (_lookahead.type == "UPLOAD")
                Upload_statment(location._upload);
            else if (_lookahead.type == "ALLOWED_METHODS")
                Allowed_methods_statment(location._allowed_methods);
            else if (_lookahead.type == "CGI_PASS")
                Cgi_pass_statment(location._cgi_pass);
            else if (_lookahead.type == "CGI_EXT")
                Cgi_ext_statment(location._cgi_ext);
            else
                throw ("Unexpected token here : " + _lookahead.type);
        }
        
        std::pair<std::string, int> Listen_statment()
        {
            std::pair<std::string, int> ip_port;
            Token tmp;
            std::string str;
            try
            {
                _eat("LISTEN");
                if (_lookahead.type == "STRING")
                    ip_port.first = _eat("STRING").value;
                else if (_lookahead.type == "NUMBER")
                {
                    tmp = _eat("NUMBER");
                    if (_lookahead.type == ".")
                    {
                        ip_port.first += tmp.value;
                        ip_port.first += _eat(".").value;
                        ip_port.first += _eat("NUMBER").value;
                        ip_port.first += _eat(".").value;
                        ip_port.first += _eat("NUMBER").value;
                        ip_port.first += _eat(".").value;
                        ip_port.first += _eat("NUMBER").value;
                    }
                    else
                        ip_port.second = std::stoi(tmp.value);
                }
                if (_lookahead.type == ":" && !ip_port.second)
                {
                    _eat(":");
                    ip_port.second = std::stoi(_eat("NUMBER").value);
                }
                _eat(";");
            }
            catch(const std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
            
            return ip_port;
        }
       
        void Server_name_statment(std::vector<std::string> &names)
        {
            try
            {
                _eat("SERVER_NAME");
                names.clear();
                while (_lookahead.type == "STRING")
                    names.push_back(_eat("STRING").value);
                _eat(";");
            }
            catch(const std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
            
        }
      
        void Client_max_body_size_statment(int& cmbs)
        {
            try
            {
                _eat("CLINET_BODY_MAX_SIZE");
                cmbs = std::stoi(_eat("NUMBER").value);
                // _eat("STRING").value;
                _eat(";");
            }
            catch(const std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
        }

        
        void Error_page_statment(std::map<int, std::string> &error_page)
        {
            std::vector<int> numbers;
            std::string path;
            try
            {
                _eat("ERROR_PAGE");
                while (_lookahead.type == "NUMBER")
                {
                    numbers.push_back(std::stoi(_eat("NUMBER").value));
                }
                path = _eat("STRING").value;
                for (size_t i = 0; i < numbers.size(); i++)
                {
                    error_page.insert(std::make_pair(numbers[i], path));
                }
                _eat(";");
            }
            catch(const std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
            
        }
       
        void    Index_statment(std::vector<std::string>& index)
        {
            try
            {
                _eat("INDEX");
                index.clear();
                while (_lookahead.type == "STRING")
                    index.push_back(_eat("STRING").value);
                _eat(";");
            }
            catch(const std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
        }

        void    Autoindex_statment(bool& autoindex)
        {
            try
            {
                _eat("AUTOINDEX");
                if (_lookahead.type == "STRING" && _lookahead.value == "on")
                    autoindex = true;
                else if (_lookahead.type == "STRING" && _lookahead.value == "off")
                    autoindex = false;
                else
                    throw ("unexpected : on | off");
                _eat("STRING");
                _eat(";");
            }
            catch(const std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
        }
       
        void    Root_statment(std::string& root)
        {
            try
            {
                _eat("ROOT");
                root = _eat("STRING").value;
                _eat(";");
            }
            catch(const std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
            
        }

       
        void    Return_statment(std::pair<int, std::string>& redirection)
        {
            try
            {
                _eat("RETURN");
                redirection.first = std::stoi(_eat("NUMBER").value);
                redirection.second = _eat("STRING").value;
                _eat(";");
            }
            catch(const std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
            
        }
       
        void    Upload_statment(std::string& upload_path)
        {
            try
            {
                _eat("UPLOAD");
                upload_path = _eat("STRING").value;
                _eat(";");
            }
            catch(const std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
            
        }
        void    Allowed_methods_statment(std::vector<std::string>& allowed_methods)
        {
            try
            {
                _eat("ALLOWED_METHOD");
                while (_lookahead.type == "METHOD")
                {
                    allowed_methods.push_back(_eat("METHOD").value);
                }
                _eat(";");
            }
            catch(const std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
        }

        void    Cgi_pass_statment(std::string& cgi_pass)
        {
            try
            {
                _eat("CGI_PASS");
                cgi_pass = _eat("STRING").value;
                _eat(";");
            }
            catch(const std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
            
        }
        void    Cgi_ext_statment(std::vector<std::string>& cgi_ext)
        {
            try
            {
                _eat("CGI_EXT");
                while (_lookahead.type == "STRING")
                    cgi_ext.push_back(_eat("STRING").value);
                _eat(";");
            }
            catch(const std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
            
        }
    };
}

#endif