#include "../includes/MimeTypes.hpp"

std::map<std::string, std::string> webserve::MimeTypes::_types = std::map<std::string, std::string>();


std::string webserve::MimeTypes::_Tolower(std::string const & str)
{
    std::string ret;
    for (size_t i = 0; i < str.length(); i++)
        ret += (char)std::tolower(str[i]);
    return ret;
}

std::string webserve::MimeTypes::getType(std::string path)
{
    size_t pos;
    std::string extention;
    std::map<std::string, std::string>::iterator found;
    if ((pos = path.find_last_of(".")) != std::string::npos)
        extention = path.substr(pos + 1);//_Tolower(path.substr(pos + 1));
    found = _types.find(extention);
    if (found != _types.end())
        return found->second;
    return std::string();
}

std::string webserve::MimeTypes::getExtention(std::string type)
{
    std::map<std::string, std::string>::iterator it = _types.begin();
    if (type.empty())
        return std::string();
    for (; it != _types.end(); it++)
    {
        if (it->second == type)
            return it->first;
    }
    return std::string();
}

void webserve::MimeTypes::set()
{
    std::ifstream input("./mime.csv");
    std::string     line;
    std::string buffer, key, value;
    while (std::getline(input, line))
    {
        std::istringstream line_strem(line);
        int i = 0;
        while(std::getline(line_strem, buffer, ',' ) && i < 2){
            if (i == 0)
                key = buffer;//_Tolower(buffer);
            else if (i == 1)
                value = buffer;
            i++;
        }
        _types.insert(std::make_pair(key, value));
        key.clear();
        value.clear();
        buffer.clear();
        line_strem.clear();
    }
    input.close();
}