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
            int _isHeaderComplete;
            size_t _bodysize;
            std::string _filename;
            std::string _path;
            std::string _host;
            int _port;
            bool _isComplete;

        public:
            Request() : _rawRequest(""), _isHeaderComplete(false), _bodysize(0), _filename("") {}
            Request(Request const& req) 
            {
                *this = req;
            }
            ~Request() 
            {
                // unlink(_path.c_str());
            }
            Request& operator= (Request const& req)
            {
                _rawRequest = req._rawRequest;
                _isHeaderComplete = req._isHeaderComplete;
                _bodysize = req._bodysize;
                _filename =  req._filename;
                _request = req._request;
                _host = req._host;
                _port = req._port;
                _isComplete = req._isComplete;
                _path = req._path;
                return *this;
            }

            void    clear()
            {
                _request.erase(_request.begin(), _request.end());
                _rawRequest.clear();
                _isHeaderComplete = false;
                _isComplete = false;
                _port = 0;
                _host.clear();
                _bodysize = 0;
                _filename.clear();
                _path.clear();
                unlink(_path.c_str());
            }

            void append(std::string str, size_t len) 
            {
                int r;
                std::string dst;
                if (!len || _isComplete)
                    return;
                if (_isHeaderComplete && _request.count("Content-Length") && len)
                {
                    body(str);
                    return;
                }
                _rawRequest.append(str);
                if ((r = _rawRequest.find("\r\n\r\n")) != std::string::npos)
                {
                    r += 4;
                    dst = _rawRequest.substr(r);// std::string(_rawRequest, _rawRequest.length() - r);
                    _isHeaderComplete = true;
                    _rawRequest.erase(r);
                    parse();
                    print();
                    if (!_request.count("Content-Length") || (has("Content-Length") && !std::stol(_request["Content-Length"][0])))
                    {
                        std::cout << "no Content-Length" << std::endl;
                        _isComplete = true;
                        _isHeaderComplete = true;
                        return;
                    }
                    body(dst);
                }
            }
            void body(std::string str)
            {
                time_t t;
                std::ofstream file;
                if (_filename.empty())
                {
                    time(&t);
                    _filename = std::to_string(t);
                    _path = "/tmp/" + _filename;
                    std::cout << "filename : " << _filename << std::endl;
                }
                file.open(_path, std::ofstream::app);
                file << str;
                file.close();
                _bodysize += str.length();
                if (_request.count("Content-Length"))
                {
                    if (_bodysize == std::stol(_request["Content-Length"][0]))
                    {
                        _isComplete = true;
                        return;
                    }
                }
            }

            void    parse()
            {
                RequestParser parser(_rawRequest);
                _request = parser.parsing();
                _port = 80;
                size_t pos; 
                _host = get("Host").front();
                if ((pos = _host.find(":")) != std::string::npos)
                {
                    _port = std::stoi(_host.substr(pos + 1));
                    _host = _host.substr(0, pos);
                }
                std::cout << "Host " << _host << " port " << _port << std::endl;
                //print();
            }
            // std::vector<std::string>* getFiled(std::string const& filedName)
            // {
            //     if (_request.count(filedName))
            //         return &(_request.find(filedName)->second);
            //     return NULL;
            // }
            bool    has(std::string const& field) const
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
            std::string getPath() const
            {
                return _path;
            }
            std::string getHost() const
            {
                return _host;
            }
            int getPort() const
            {
                return _port;
            }
            bool isRequestComplete() const { return _isComplete; }
            size_t getBodysize() const { return _bodysize; }
            bool isKeepAlive() 
            {
                if (_request.count("Connection") && get("Connection").front() == "keep-alive")
                    return true;
                return false;
            }

            std::string getTransferEncoding()
            {
                if (has("Transfer-Encoding"))
                    return get("Transfer-Encoding").front();
                return std::string();
            }
            std::string getContentType()
            {
                if (has("Content-Type"))
                    return get("Content-Type").front();
                return std::string();
            }
            std::string getUri()
            {
                if (has("Uri"))
                    return get("Uri").front();
                return std::string();
            }
            
            std::string getMethod()
            {
                if (has("Method"))
                    return get("Method").front();
                return std::string();
            }
            
            
    };
} // namespace webserve

#endif