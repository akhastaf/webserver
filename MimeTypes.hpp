#ifndef MIMETYPES_HPP
# define MIMETYPES_HPP
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
namespace webserve
{
    class MimeTypes
    {
        private:
            static std::map<std::string, std::string> _types;
            static std::string _Tolower(std::string const & str);
        public:
            static std::string getType(std::string path);
            static std::string getExtention(std::string type);
            static void set();
    };
    
}
#endif