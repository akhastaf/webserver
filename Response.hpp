#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <map>
#include <vector>
#include <unistd.h>
#include "Request.hpp"
#include "Server.hpp"
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CRLF "\r\n"

namespace webserve 
{
    class Response
    {
        private:
            std::string _response;
            webserve::Request _request;
            webserve::Server _server;
            webserve::Location _location;
            int _fd;
            std::string _filepath;
            size_t _size_sended;
            
            void _create_file()
            {
                struct timeval tp;
                gettimeofday(&tp, NULL);
                long int us = tp.tv_sec * 1000000 + tp.tv_usec;
                std::string file_name =   std::to_string(us);
                std::string filepath = "/tmp/autoindex_" + file_name;
                std::ofstream out(filepath);
                _filepath = filepath;
                out << _response;
                out.close();
                _fd = open(filepath.c_str(), O_RDONLY);
                fcntl(_fd, F_SETFL, O_NONBLOCK);
                _size_sended = 0;
            }

            std::string   _getStatusdescription(int statusCode)
            {
                switch (statusCode)
                {
                    case 0: return "None";
                    case 100: return "Continue";
                    case 101: return "Switching Protocols";
                    case 102: return "Processing";
                    case 103: return "Early Hints";
                    case 200: return "OK";
                    case 201: return "Created";
                    case 202: return "Accepted";
                    case 203: return "Non-Authoritative Information";
                    case 204: return "No Content";
                    case 205: return "Reset Content";
                    case 206: return "Partial Content";
                    case 207: return "Multi-Status";
                    case 208: return "Already Reported";
                    case 226: return "IM Used";
                    case 300: return "Multiple Choices";
                    case 301: return "Moved Permanently";
                    case 302: return "Found";
                    case 303: return "See Other";
                    case 304: return "Not Modified";
                    case 305: return "Use Proxy";
                    case 307: return "Temporary Redirect";
                    case 308: return "Permanent Redirect";
                    case 400: return "Bad Request";
                    case 401: return "Unauthorized";
                    case 402: return "Payment Required";
                    case 403: return "Forbidden";
                    case 404: return "Not Found";
                    case 405: return "Method Not Allowed";
                    case 406: return "Not Acceptable";
                    case 407: return "Proxy Authentication Required";
                    case 408: return "Request Timeout";
                    case 409: return "Conflict";
                    case 410: return "Gone";
                    case 411: return "Length Required";
                    case 412: return "Precondition Failed";
                    case 413: return "Payload Too Large";
                    case 414: return "URI Too Long";
                    case 415: return "Unsupported Media Type";
                    case 416: return "Range Not Satisfiable";
                    case 417: return "Expectation Failed";
                    case 418: return "I'm a teapot";
                    case 422: return "Unprocessable Entity";
                    case 423: return "Locked";
                    case 424: return "Failed Dependency";
                    case 426: return "Upgrade Required";
                    case 428: return "Precondition Required";
                    case 429: return "Too Many Requests";
                    case 431: return "Request Header Fields Too Large";
                    case 451: return "Unavailable For Legal Reasons";
                    case 500: return "Internal Server Error";
                    case 501: return "Not Implemented";
                    case 502: return "Bad Gateway";
                    case 503: return "Service Unavailable";
                    case 504: return "Gateway Time-out";
                    case 505: return "HTTP Version Not Supported";
                    case 506: return "Variant Also Negotiates";
                    case 507: return "Insufficient Storage";
                    case 508: return "Loop Detected";
                    case 510: return "Not Extended";
                    case 511: return "Network Authentication Required";
                    default: return "";
                }
            }
            bool _checkBodySize()
            {
                // check if location has max_client_body_size
                if (_request.getBodysize() > _server._max_client_body_size)
                    return true;
                return false;
            }
            void    _constructError(int status, std::string path)
            {
                struct stat s;
                int fd;
	            stat(path.c_str(), &s);
                _response = "HTTP/1.1 " + std::to_string(status) + " " + _getStatusdescription(status).c_str() + "\r\n";
                _response += "Server: webserver\r\n";
                _response += "Content-Length: " + std::to_string(s.st_size) + "\r\n";
	            _response += "Connection: close\r\n\r\n";
                fd = open(path.c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
                char buffer[s.st_size];
                int r = read(fd, buffer, s.st_size);
                close(fd);
                if (r)
                    _response += std::string(buffer, r);
                _response += "\r\n\r\n";
            }

            void    _badRequest()
            {
                int fd;
                std::string path;
                if (_location._error_pages.count(400))
                    path = _location._error_pages[400];
                else if (_server._error_pages.count(400))
                    path = _server._error_pages[400];
                fd = open(_location._error_pages[400].c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    return _constructError(400, path);
                _constructError(400, "./error_pages/400.html");
                _create_file();
            }
            
            void    _notFound()
            {
                int fd;
                std::string path;
                if (_location._error_pages.count(404))
                    path = _location._error_pages[404];
                else if (_server._error_pages.count(404))
                    path = _server._error_pages[404];
                fd = open(_location._error_pages[404].c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    return _constructError(404, path);
                _constructError(404, "./error_pages/404.html");
                _create_file();
            }

            void    _internalError()
            {
                int fd;
                std::string path;
                if (_location._error_pages.count(500))
                    path = _location._error_pages[500];
                else if (_server._error_pages.count(500))
                    path = _server._error_pages[500];
                fd = open(_location._error_pages[500].c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    return _constructError(500, path);
                _constructError(500, "./error_pages/500.html");
                _create_file();
            }

            void    _payloadTooLarge()
            {
                int fd;
                std::string path;
                if (_location._error_pages.count(413))
                    path = _location._error_pages[413];
                else if (_server._error_pages.count(413))
                    path = _server._error_pages[413];
                fd = open(_location._error_pages[413].c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    return _constructError(413, path);
                _constructError(413, "./error_pages/413.html");
                _create_file();
            }

            void    _forbidden()
            {
                int fd;
                std::string path;
                if (_location._error_pages.count(403))
                    path = _location._error_pages[403];
                else if (_server._error_pages.count(403))
                    path = _server._error_pages[403];
                fd = open(_location._error_pages[403].c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    return _constructError(403, path);
                _constructError(403, "./error_pages/403.html");
                _create_file();
            }

            void    _unallowedMethod()
            {
                int fd;
                std::string path;
                if (_location._error_pages.count(405))
                    path = _location._error_pages[405];
                else if (_server._error_pages.count(405))
                    path = _server._error_pages[405];
                fd = open(_location._error_pages[405].c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    return _constructError(405, path);
                _constructError(405, "./error_pages/405.html");
                _create_file();
            }
            
            void    _httpVersionNotSupported()
            {
                int fd;
                std::string path;
                if (_location._error_pages.count(505))
                    path = _location._error_pages[505];
                else if (_server._error_pages.count(505))
                    path = _server._error_pages[505];
                fd = open(_location._error_pages[505].c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    return _constructError(505, path);
                _constructError(505, "./error_pages/505.html");
                _create_file();
            }
            
            void    _notImplemented()
            {
                int fd;
                std::string path;
                if (_location._error_pages.count(501))
                    path = _location._error_pages[501];
                else if (_server._error_pages.count(501))
                    path = _server._error_pages[501];
                fd = open(_location._error_pages[501].c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    return _constructError(501, path);
                _constructError(501, "./error_pages/501.html");
                _create_file();
            }
            
            void    _requestUriTooLong()
            {
                int fd;
                std::string path;
                if (_location._error_pages.count(414))
                    path = _location._error_pages[414];
                else if (_server._error_pages.count(414))
                    path = _server._error_pages[414];
                fd = open(_location._error_pages[414].c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    return _constructError(414, path);
                _constructError(414, "./error_pages/414.html");
                _create_file();
            }

        public:
            Response() {}
            Response(Request const& req,  Server server) :  _request(req), _server(server)
            {

                std::cout << "##############################################" << std::endl;   
                std::cout << "server found " << std::endl;
                std::cout << server << std::endl;
                std::cout << "##############################################" << std::endl;   
            }
            Response(Response const& res)
            {
                _request = res._request;
                _server = res._server;
                _location = res._location;
            }
            ~Response() {}

            void    process()
            {
                if (!_request.getTransferEncoding().empty() && _request.getTransferEncoding() != "chunked")
                    return _notImplemented();
                if (_checkBodySize())
                    return _payloadTooLarge();
                if (_request.get("Method")[0] == "POST" && !_request.has("Transfer-Encoding") && !_request.has("Content-Length"))
                    return _badRequest();
                if (_request.get("Uri")[0].length() > 2048)
                    return _requestUriTooLong();
                if (_request.get("Method")[0] != "GET" && _request.get("Method")[0] != "POST" && _request.get("Method")[0] != "DELETE")
                    return _unallowedMethod();



                std::cout << _request.get("Version")[0] << std::endl;
                std::cout << _request.get("Method")[0] << std::endl;
                std::cout << _request.get("Uri")[0] << std::endl;
                std::cout << _request.get("Query")[0] << std::endl;
                // _response = "HTTP/1.1 200 OK\nServer: Hello\nContent-Length: 100\nContent-Type: text/html\n\n<html><head></head><body><h1>Hello, world</h1></body></html>\r\n\r\n";
                // _create_file();
                _notFound();
            }

            int getFd() const { return _fd; }
            std::string getFilepath() const { return _filepath; }
            Request& getRequest() { return _request; }

            void update_size_sended(long send) { _size_sended += send; }

    };
    
}


#endif