#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <map>
#include <vector>
#include <unistd.h>
#include "Request.hpp"
#include "Server.hpp"
#include "MimeTypes.hpp"
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include <dirent.h>

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
            bool _locationFound;
            
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

            std::string _constructUri(std::string path)
            {
                std::cout << _request.getHost() + ":" + std::to_string(_request.getPort()) + path << std::endl;
                return _request.getHost() + ":" + std::to_string(_request.getPort()) + path;
            }

            Location _getLocation()
            {
                std::string uriCopy(_request.getUri());
                std::string locationUri;
                size_t pos;
                bool notFound = true;
                // std::vector<std::pair<std::string, webserve::Location> >::ite
                do {
                    pos = uriCopy.find("/");
                    if (pos != std::string::npos)
                    {
                        if (!pos)
                            locationUri += uriCopy.substr(0, pos + 1);
                        else
                            locationUri += uriCopy.substr(0, pos);
                        uriCopy.erase(0, pos + 1);
                    }
                    else
                        break;
                    for (int i = 0; i < _server._locations.size(); i++)
                    {
                        if (_server._locations[i].first == locationUri)
                        {
                            _locationFound = true;
                            return _server._locations[i].second;
                        }
                    }
                } while (notFound);
                locationUri = _request.getUri();
                for (int i = 0; i < _server._locations.size(); i++)
                {
                    if (_server._locations[i].first == locationUri)
                    {
                        _locationFound = true;
                        return _server._locations[i].second;
                    }
                }
                _locationFound = false;
                return Location();
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
                if (_locationFound && _location._max_client_body_size != -1)
                {
                    if (_request.getBodysize() > _location._max_client_body_size * 1e6)
                        return true;
                    return false;
                }
                if (_request.getBodysize() > _server._max_client_body_size * 1e6)
                    return true;
                return false;
            }
            bool _checkUri()
            {
                std::string uriCharset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=";
                std::string uri = _request.getUri();
                size_t pos;
                for (int i = 0; i < uri.length(); i++)
                {
                    if ((pos = uriCharset.find(uri[i])) == std::string::npos)
                        return false;
                }
                return true;
            }

            bool _checkAllowedMethod()
            {
                std::string method = _request.getMethod();
                if (_locationFound)
                {
                    if (std::find(_location._allowed_methods.begin(), _location._allowed_methods.end(), method) != _location._allowed_methods.end())
                        return true;
                    return false;
                }
                else if (std::find(_server._allowed_methods.begin(), _server._allowed_methods.end(), method) != _server._allowed_methods.end())
                    return true;
                return false;
            }

            void    _constructError(int status)
            {
                int fd;
                std::string body;
                body = "<html>\n<head>\n<title>" + std::to_string(status) + " " + _getStatusdescription(status) + "</title>\n</head>\n<body>\n";
                body += "<center>\n<h1>" + std::to_string(status) + " " + _getStatusdescription(status);
                body += "</center>\n<hr>\n<center>webserver</center>\n</body>\n</html>";
                _response = "HTTP/1.1 " + std::to_string(status) + " " + _getStatusdescription(status).c_str() + "\r\n";
                _response += "Server: webserver\r\n";
                _response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
	            _response += "Connection: close\r\n\r\n";
                _response += body;
                _response += "\r\n\r\n";
            }

            void    _loadError(int status, std::string path)
            {
                std::cout << "load file" << std::endl;
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
            void    _redirectTo(int status, std::string location)
            {
                std::cout << status << " redirect to " << location << std::endl;
                _response = "HTTP/1.1 " + std::to_string(status) + " " + _getStatusdescription(status).c_str() + "\r\n";
                _response += "Server: webserver\r\n";
                _response += "Location: "+ location +"\r\n";
	            _response += "Connection: close\r\n\r\n";
                _response += "\r\n\r\n";
                _create_file();
            }


            void    _badRequest()
            {
                int fd;
                std::string path;
                if (_locationFound && _location._error_pages.count(400))
                    path = _location._error_pages[400];
                else if (_server._error_pages.count(400))
                    path = _server._error_pages[400];
                fd = open(path.c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    _loadError(400, path);
                else
                    _constructError(400);
                _create_file();
            }
            
            void    _notFound()
            {
                int fd;
                std::string path;
                if (_locationFound && _location._error_pages.count(404))
                    path = _location._error_pages[404];
                else if (_server._error_pages.count(404))
                    path = _server._error_pages[404];
                fd = open(path.c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    _loadError(404, path);
                else
                    _constructError(404);
                _create_file();
            }

            void    _internalError()
            {
                int fd;
                std::string path;
                if (_locationFound && _location._error_pages.count(500))
                    path = _location._error_pages[500];
                else if (_server._error_pages.count(500))
                    path = _server._error_pages[500];
                fd = open(path.c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    _loadError(500, path);
                else
                    _constructError(500);
                _create_file();
            }

            void    _payloadTooLarge()
            {
                int fd;
                std::string path;
                if (_locationFound && _location._error_pages.count(413))
                    path = _location._error_pages[413];
                else if (_server._error_pages.count(413))
                    path = _server._error_pages[413];
                fd = open(path.c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    _loadError(413, path);
                else
                    _constructError(413);
                _create_file();
            }

            void    _forbidden()
            {
                int fd;
                std::string path;
                if (_locationFound && _location._error_pages.count(403))
                    path = _location._error_pages[403];
                else if (_server._error_pages.count(403))
                    path = _server._error_pages[403];
                fd = open(path.c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    _loadError(403, path);
                else
                    _constructError(403);
                _create_file();
            }

            void    _unallowedMethod()
            {
                int fd;
                std::string path;
                if (_locationFound && _location._error_pages.count(405))
                    path = _location._error_pages[405];
                else if (_server._error_pages.count(405))
                    path = _server._error_pages[405];
                fd = open(path.c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    _loadError(405, path);
                else
                    _constructError(405);
                _create_file();
            }
            
            void    _httpVersionNotSupported()
            {
                int fd;
                std::string path;
                if (_locationFound && _location._error_pages.count(505))
                    path = _location._error_pages[505];
                else if (_server._error_pages.count(505))
                    path = _server._error_pages[505];
                fd = open(path.c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    _loadError(505, path);
                else
                    _constructError(505);
                _create_file();
            }
            
            void    _notImplemented()
            {
                int fd;
                std::string path;
                if (_locationFound && _location._error_pages.count(501))
                    path = _location._error_pages[501];
                else if (_server._error_pages.count(501))
                    path = _server._error_pages[501];
                fd = open(path.c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    _loadError(501, path);
                else
                    _constructError(501);
                _create_file();
            }
            
            void    _requestUriTooLong()
            {
                int fd;
                std::string path;
                if (_locationFound && _location._error_pages.count(414))
                    path = _location._error_pages[414];
                else if (_server._error_pages.count(414))
                    path = _server._error_pages[414];
                fd = open(path.c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    _loadError(414, path);
                else
                    _constructError(414);
                _create_file();
            }

            bool _hasAutoIndex()
            {
                if (_locationFound)
                    return _location._autoindex;
                return _server._autoindex;
            }

            std::string _getTimeForAutoIndex(time_t * time)
            {
                char buffer[12];
                struct tm* t;
                std::string timeString;
                std::string hours = std::ctime(time);
                t = std::gmtime(time);
                std::strftime(buffer, 12, "%d-%b-%Y", t);
                timeString = buffer;
                timeString += hours.substr(10, 6);
                return timeString;
            }

            std::string _getfileInfoForAutoIndex(std::string filename, std::string path)
            {
                struct stat s;
                std::string filepath = _request.getUri() + filename;
                std::string fileinfo = "<div><a href=\"" + _request.getHost() + ":" + std::to_string(_server._port) + filepath + "\">" + filename + "</a></div>";
                path += filename;
                stat(path.c_str(), &s);
                fileinfo += "                 " + _getTimeForAutoIndex(&s.st_mtimespec.tv_sec);
                if (s.st_mode & S_IFREG)
                    fileinfo += "                 " + std::to_string(s.st_size);
                else
                    fileinfo += "                 --";
                
                return fileinfo;
            }

            void    _autoInedxCreate()
            {
                std::string body;
                std::string path;
                DIR *dir;
                struct dirent *dirIterator;
                if (_locationFound && _location._root.length())
                    path = _location._root;
                else
                    path = _server._root;
                path += _request.getUri();
                std::cout << path << std::endl;
                if ((dir = opendir(path.c_str())) != NULL)
                {
                    body = "<html>\n<head>\n<title>Index of " + _request.getUri() + "</title>\n</head>\n<body>";
                    body += "<h1>Index of " + _request.getUri() + "</h1><hr><pre>";
                    while ((dirIterator = readdir(dir)) != NULL)
                            body += _getfileInfoForAutoIndex(dirIterator->d_name, path);
                    body += "</pre><hr>\n</body>\n</html>";
                    _response = "HTTP/1.1 200 " + _getStatusdescription(200) + "\r\n";
                    _response += "Server: webserver\r\n";
                    _response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
                    _response += "Connection: close\r\n\r\n";
                    _response += body;
                    _response += "\r\n\r\n";
                    closedir(dir);
                    _create_file();
                }
                else
                    _notFound();
            }

            bool _hasUpload()
            {
                if (_locationFound)
                {
                    if (_location._upload.size())
                        return true;
                }
                else if (_server._upload.size())
                    return true;
                return false;
            }

            bool _hasIndexFiles(std::string path)
            {
                if (_getIndexFiles(path).length())
                    return true;
                return false;
            }

            std::string _getIndexFiles(std::string path)
            {
                std::string found;
                struct stat s;
                if (_locationFound && _location._index.size())
                {
                    for (int i = 0; i < _location._index.size(); i++)
                    {
                        found = path;
                        found += _location._index[i];
                        if (stat(found.c_str(), &s) == 0 && S_ISREG(s.st_mode))
                            return found;
                    }
                }
                else if (_server._index.size())
                {
                    for (int i = 0; i < _server._index.size(); i++)
                    {
                        found = path;
                        found += _server._index[i];
                        if (stat(found.c_str(), &s) == 0 && S_ISREG(s.st_mode))
                            return found;
                    }
                }
                return std::string();
            }

            void    _loadResource(std::string path)
            {
                std::string type = MimeTypes::getType(path);
                struct stat s;
                stat(path.c_str(), &s);
                int fd  = open(path.c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
                if (fd == -1)
                    return _notFound();
                _response = "HTTP/1.1 200 " + _getStatusdescription(200) + "\r\n";
                _response += "Server: webserver\r\n";
                if (S_ISREG(s.st_mode) && !type.length())
                {
                    std::string file_name = path.substr(path.find_last_of('/') + 1, path.size());
                    _response += "Content-Disposition: attachement; filename=" + file_name + "\r\n";
                }
                if (type.length())
			        _response += "Content-Type: " + type + "\r\n";	
                _response += "Content-Length: " + std::to_string(s.st_size) + "\r\n\r\n";
                char buffer[1000];
                int reading = 0;
                while((reading = read(fd, buffer, 1000)) > 0){
                    _response.append(buffer, reading);
                    bzero(buffer, 1000);
                }
                close(fd);
                if (reading == -1)
                    return _notFound();
                _create_file();
            }

            void    _upload()
            {
                std::string path;
                std::string extention;
                std::string realPath = _request.getPath();
                std::string filename = _request.getFilename();
                if (_locationFound && _location._upload.size())
                    path = _location._upload;
                else if (_server._upload.size())
                    path = _server._upload;
                if (path.back() != '/')
                    path.push_back('/');
                extention = MimeTypes::getExtention(_request.getContentType());
                std::string mv = "mv " + realPath + " " + (path + filename);
                if (extention.size())
                    mv += "." + extention;
                std::cout << "realpath " << realPath << " path " << path << std::endl;
                std::cout << mv << std::endl;
                int r = system(mv.c_str());
                if (r != -1)
                    _response += "HTTP/1.1 201 " + _getStatusdescription(201) + "\r\n";
                else
                    _response += "HTTP/1.1 409 " + _getStatusdescription(409) + "\r\n";
                // _response += "Date: " + std::string(ctime(&rawtime));
                _response += "Server: webserver\r\n";
                _response += "Content-Length: 0\r\n";
                _response +=  "Connection: close\r\n";
                _response +=  "Accept-Ranges: bytes\r\n\r\n";
                _create_file();
            }
            
            void    _deleteFile(std::string path)
            {
                unlink(path.c_str());
                _response += "HTTP/1.1 204 " + _getStatusdescription(204) + "\r\n";
                _response += "Server: webserver\r\n";
                _response += "Content-Length: 0\r\n";
                _response +=  "Connection: close\r\n";
                _create_file();
            }
            void    _deleteDirectoryContent(std::string path)
            {
                DIR *dir;
                struct dirent *dirIterator;
                std::string filePath;
                while ((dirIterator = readdir(dir)) != NULL)
                {
                    filePath = path + dirIterator->d_name;
                    if (unlink(filePath.c_str()) < 0)
                    {
                        if (errno == EACCES)
                            return _forbidden();
                        else if (errno == EEXIST)
                            return _internalError();
                    }
                }
                _response += "HTTP/1.1 204 " + _getStatusdescription(204) + "\r\n";
                _response += "Server: webserver\r\n";
                _response += "Content-Length: 0\r\n";
                _response +=  "Connection: close\r\n";
                _create_file();
            }

            void    _proccessGET(std::string uri)
            {
                struct stat s;
                std::string path;
                if (_locationFound && _location._root.length())
                    path = _location._root;
                else
                    path = _server._root;
                path += uri;
                if (stat(path.c_str(), &s) < 0)
                    return _notFound();
                if (S_ISREG(s.st_mode))
                {
                    std::cout << "is a file" << std::endl;
                    // if (_locationHasCGI())
                    //     _cgi();
                    // else
                    return _loadResource(path);
                }
                else if (S_ISDIR(s.st_mode))
                {
                    std::cout << "is a dir" << std::endl;
                    if (path.back() != '/')
                        return _redirectTo(301, _constructUri(_request.getUri() + "/"));
                    else if (_hasIndexFiles(path))
                        return _loadResource(_getIndexFiles(path));
                    else if (_hasAutoIndex())
                        return _autoInedxCreate();
                    else
                        return _forbidden();
                    
                }
                return _notFound();
            }


            void    _proccessPOST(std::string uri)
            {
                struct  stat s;
                std::string path;
                if (_locationFound && _location._root.length())
                    path = _location._root;
                else
                    path = _server._root;
                path += uri;
                if (_hasUpload())
                    return _upload();
                if (stat(path.c_str(), &s) < 0)
                    return _notFound();
                if (S_ISREG(s.st_mode))
                {
                    std::cout << "is a file" << std::endl;
                    // if (_locationHasCGI())
                    //     _cgi();
                    // else
                    return _forbidden();
                }
                else if (S_ISDIR(s.st_mode))
                {
                    std::cout << "is a dir" << std::endl;
                    if (path.back() != '/')
                        return _redirectTo(301, _constructUri(_request.getUri() + "/"));
                    else if (_hasIndexFiles(path))
                    {
                        // return _loadResource(_getIndexFiles(path));
                        return _forbidden();
                    }
                    // else if (_hasAutoIndex())
                    //     return _autoInedxCreate();
                    else
                        return _forbidden();
                }
            }

            void    _proccessDELETE(std::string uri)
            {
                struct  stat s;
                std::string path;
                if (_locationFound && _location._root.length())
                    path = _location._root;
                else
                    path = _server._root;
                path += uri;
                errno = 0;
                if (lstat(path.c_str(), &s) < 0)
                {
                    if (errno == ENOTDIR || errno == ENOENT)
                        return _notFound();
                    else if (errno == EACCES)
                        return _forbidden();
                    else if (errno == EEXIST)
                        return _internalError();
                }
                if (S_ISREG(s.st_mode))
                {
                    std::cout << "is a file" << std::endl;
                    // if (_locationHasCGI())
                    //     _cgi();
                    // else
                    return _deleteFile(path);
                }
                else if (S_ISDIR(s.st_mode))
                {
                    std::cout << "is a dir" << std::endl;
                    if (path.back() != '/')
                        return _redirectTo(301, _constructUri(_request.getUri() + "/"));
                    // if Location has CGI
                    return _deleteDirectoryContent(path);
                }
            }

        public:
            Response() {}
            Response(Request const& req,  Server server) :  _request(req), _server(server), _locationFound(false)
            {
                _location = _getLocation();
                if (_locationFound)
                {
                    std::cout << "##############################################" << std::endl;   
                    std::cout << "locatio found " << std::endl;
                    //std::cout << _server << std::endl;
                    std::cout << _location << std::endl;
                    std::cout << "##############################################" << std::endl;   
                }
            }
            Response(Response const& res)
            {
                _request = res._request;
                _server = res._server;
                _location = res._location;
                _locationFound = res._locationFound;
                _fd = res._fd;
                _filepath = res._filepath;
                _size_sended = res._size_sended;
            }
            ~Response() {}

            void    process()
            {
                if (!_request.getTransferEncoding().empty() && _request.getTransferEncoding() != "chunked")
                    return _notImplemented();
                if (_request.getMethod() == "POST" && _checkBodySize())
                    return _payloadTooLarge();
                if ((!_checkUri() || (_request.getMethod() == "POST" && !_request.has("Transfer-Encoding") && !_request.has("Content-Length"))))
                    return _badRequest();
                if (_request.get("Uri")[0].length() > 2048)
                    return _requestUriTooLong();
                // if (_request.getMethod() != "GET" && _request.getMethod() != "POST" && _request.getMethod() != "DELETE")
                //     return _notImplemented();
                if (_locationFound && (_location._redirection.first >= 300 && _location._redirection.first <= 307))
                    return _redirectTo(_location._redirection.first, _location._redirection.second);
                if (!_checkAllowedMethod())
                    _unallowedMethod();
                if (_request.getMethod() == "GET")
                    return _proccessGET(_request.getUri());
                else if (_request.getMethod() == "POST")
                    _proccessPOST(_request.getUri());
                else if (_request.getMethod() == "DELETE")
                    _proccessDELETE(_request.getUri());
                else
                    return _notImplemented();
                std::cout << _request.get("Version")[0] << std::endl;
                std::cout << _request.get("Method")[0] << std::endl;
                std::cout << _request.get("Uri")[0] << std::endl;
                std::cout << _request.get("Query")[0] << std::endl;
                // _response = "HTTP/1.1 200 OK\nServer: Hello\nContent-Length: 100\nContent-Type: text/html\n\n<html><head></head><body><h1>Hello, world</h1></body></html>\r\n\r\n";
                // _create_file();
                // _autoInedxCreate();
                // _notFound();
            }

            int getFd() const { return _fd; }
            std::string getFilepath() const { return _filepath; }
            Request& getRequest() { return _request; }

            void update_size_sended(long send) { _size_sended += send; }

    };
    
}


#endif