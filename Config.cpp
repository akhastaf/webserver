#include "Config.hpp"

webserve::Config::Config(std::string const& fileName) : _fileConfig(fileName)
{
    load_file();
    read_file();
    std::cout << _content;
    getServers();
}
    
webserve::Config::~Config()
{
}

void    webserve::Config::load_file()
{
    _file.open(_fileConfig, std::ios::in);
    if (!_file.is_open())
    {
        std::cerr << "file error" << std::endl;
        exit(1);
    }
}
void    webserve::Config::read_file()
{
    std::string tmp;
    while (getline(_file, tmp))
        _content += tmp + "\n";
}
std::string const& webserve::Config::getContent() const
{
    return _content;
}
void webserve::Config::getServers()
{
    webserve::Parser parser(_content);
    _Servers = parser.parsing();
    for (size_t i = 0; i < _Servers.size(); i++)
    {
        std::cout << _Servers[i] << std::endl;
    }
}