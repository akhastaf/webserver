#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <map>
#include <vector>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include "Server.hpp"
#include "Parser.hpp"


namespace   webserve
{
    class Config
    {
        private:
            std::string _fileConfig;
            std::string _content;
            std::fstream _file;
            std::vector<webserve::Server> _Servers;
        public:
            Config(std::string const& fileName);
            ~Config();
            void    load_file();
            void    read_file();
            std::string const& getContent() const;
            void getServers();

    };
    
    
    
}

#endif