#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <string.h>
# include <iostream>
# include <vector>
# include <map>
# include <ctime>
# include <fstream>
# include "RequestParser.hpp"


namespace webserve
{
    class Request
    {
        private:
            std::map<std::string, std::vector<std::string> > _request;
            std::string _rawRequest;
            int _done;
            size_t _bodysize;
            std::string _filename;

        public:
            Request() : _rawRequest(""), _done(0), _bodysize(0), _filename("") {}
            ~Request() {}
            Request& operator= (Request const& req)
            {
                _rawRequest = req._rawRequest;
                _done = req._done;
                _bodysize = req._bodysize;
                _filename =  req._filename;
                return *this;
            }
            // int parse() 
            // int getPort() { return std::stoi(_request["port"]); }
            int append(std::string str, size_t len) 
            {
                int r;
                char *dst;
                if (!len)
                    return 1;
                if (_done && _request.count("Content-Length") && len)
                {

                    std::cout << "str : " << str << std::endl;
                    return body(str, len);
                }
                for (int i = 0; i < len; i++)
                    _rawRequest.push_back(str[i]);
                //_rawRequest.append(str, len);
                if ((r = _rawRequest.find("\r\n\r\n")) != std::string::npos)
                {
                    r += 4;
                    dst = new char[_rawRequest.length() - r + 1];
                    _done = 1;
                    _rawRequest.copy(dst, _rawRequest.length() - r, r);
                    dst[_rawRequest.length() - r] = '\0';
                    _rawRequest.erase(r);
                    parse();
                    if (!_request.count("Content-Length"))
                    {
                        std::cout << "no Content-Length" << std::endl;
                        delete dst;
                        return 1;
                    }
                    std::cout << "Content lenght : " << _request["Content-Length"].front() << std::endl;
                    std::cout << _rawRequest << std::endl;
                    body(dst, len - r + 1);
                    delete dst;
                    return 0;
                }
                return 0;
            }
            int body(std::string str, size_t len)
            {
                time_t t;
                std::ofstream file;
                if (_filename.empty())
                {
                    time(&t);
                    _filename = "/tmp/" + std::to_string(t);
                    std::cout << "filename : " << _filename << std::endl;
                }
                file.open(_filename, std::ofstream::app);
                for (int i = 0; i < len; i++)
                    file << str[i];
                file.close();
                _bodysize += str.length();
                if (_request.count("Content-Length"))
                {
                    if (_bodysize == std::stol(_request["Content-Length"].front()))
                        return 1;
                }
                return 0;
            }
            void    parse()
            {
                RequestParser parser(_rawRequest);
                _request = parser.parsing();
                //print();
            }
            // std::vector<std::string>* getFiled(std::string const& filedName)
            // {
            //     if (_request.count(filedName))
            //         return &(_request.find(filedName)->second);
            //     return NULL;
            // }
            bool    has(std::string const& field)
            {
                if (_request.count(field))
                    return true;
                return false;
            }
            std::vector<std::string>&    get(std::string const& field)
            {
                return _request[field];
            }
            void print()
            {
                std::cout << _rawRequest << std::endl;
                // std::vector<std::string> v;
                // std::map<std::string, std::vector<std::string> >::iterator it = _request.begin();
                // for (; it != _request.end(); it++)
                // {
                //     std::cout << it->first << " : ";
                //     v = it->second;
                //     for (std::vector<std::string>::iterator vit = v.begin(); vit != v.end(); vit++)
                //     {
                //         std::cout << *vit;
                //         if (vit + 1 != v.end())
                //             std::cout << ", ";
                //     }
                //     std::cout << std::endl;
                // }
            }

            std::string getFilename() const
            {
                return _filename;
            }
            
    };
} // namespace webserve

#endif