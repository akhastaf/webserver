#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <string.h>
# include <iostream>
#include <sys/_types/_size_t.h>
# include <vector>
# include <map>
# include <ctime>
# include <fstream>
# include <sstream>
# include "RequestParser.hpp"
# include "utils.hpp"



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
            bool _isChunked;
            struct sockaddr _address;
            int _len_address;
            bool _isKeepAlive;
            std::string _body;

        public:
            Request() : _rawRequest(""), _isHeaderComplete(false), _bodysize(0), _filename(""), _len_address(sizeof(_address)) {}
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
                _isChunked = req._isChunked;
                _isKeepAlive = req._isKeepAlive;
                _body = req._body;
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
                _body.clear();
                unlink(_path.c_str());
            }

            std::map<std::string, std::vector<std::string> >& getRequest()
            {
                return _request;
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
                    dst = _rawRequest.substr(r);
                    _isHeaderComplete = true;
                    _rawRequest.erase(r);
                    parse();
                    print();
                    if (!_request.count("Content-Length") || (has("Content-Length") && !std::stol(_request["Content-Length"][0])))
                    {
                        _isComplete = true;
                        _isHeaderComplete = true;
                        return;
                    }
                    if (has("Transfer-Encoding") && get("Transfer-Encoding").front() == "chunked")
                        _isChunked = true;
                    else
                        _isChunked = false;
                    body(dst);
                }
            }

            size_t  _getHex(std::string n)
            {
                size_t num;
                std::stringstream stream;
                stream >> std::hex >> num;
                stream.clear();
                return num;
            }

            bool    _getChunkedBody(std::string str)
            {
                size_t pos, tmp, len;

                if ((pos = _body.find("0\r\n\r\n")) == std::string::npos)
                    _body.append(str);
                else
                {
                    pos = 0;
                    while(true){
                        tmp = _body.find("\r\n", pos);
                        len = _getHex(_body.substr(pos, tmp - pos));
                        _body.erase(pos, (tmp + 2) - pos);
                        pos += len;
                        _body.erase(pos, 2);
                        
                        if (len == 0)
                            break;
                    }
                    _isComplete = true;
                }
                return false;
            }

            void body(std::string str)
            {
                time_t t;
                std::ofstream file;
                if (_filename.empty())
                {
                    time(&t);
                    _filename = toString(t);
                    _path = "/tmp/_body" + _filename;
                    std::cout << "filename : " << _path << std::endl;
                }
                if (_isChunked && !_getChunkedBody(str))
                    return ;
                if (_isChunked && _isComplete)
                    str = _body;
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
                if (has("Connection") && get("Connection").front() == "keep-alive")
                    _isKeepAlive = true;
                else
                    _isKeepAlive = false;
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
            std::string& getPath()
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
                return _isKeepAlive;
                // if (_request.count("Connection") && get("Connection").front() == "keep-alive")
                //     return true;
                // return false;
            }

            std::string getTransferEncoding()
            {
                if (has("Transfer-Encoding"))
                    return get("Transfer-Encoding").front();
                return std::string();
            }
            std::string getContentType()
            {
                std::string type;
                if (has("Content-Type"))
                {
                    for (int i = 0; i < get("Content-Type").size(); i++)
                    {
                        type += get("Content-Type")[i];
                        if (i + 1 < get("Content-Type").size() && get("Content-Type")[i].back() != ';')
                            type += ", ";
                        else if (i + 1 < get("Content-Type").size())
                            type += " ";
                    }
                    return type;
                }
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
            
            std::string getContentLength()
            {
                if (has("Content-Length"))
                    return get("Content-Length").front();
                return std::string();
            }
            std::string getQuery()
            {
                if (has("Query"))
                    return get("Query").front();
                return std::string();
            }
            std::string getCookies()
            {
                if (has("Cookie"))
                    return get("Cookie").front();
                return std::string();
            }
            
            std::string getUserAgent()
            {
                std::string userAgent;
                if (has("User-Agent"))
                {
                    for (int i = 0; i < get("User-Agent").size(); i++)
                    {
                        userAgent += get("User-Agent")[i];
                        if (i + 1 < get("User-Agent").size())
                            userAgent += ", ";
                    }
                    return userAgent;
                }
                return std::string();
            }
            
            struct sockaddr* getAddress()
            {
                return &_address;
            }
            int* getLengthAddress()
            {
                return &_len_address;
            }
            
    };
} // namespace webserve

#endif