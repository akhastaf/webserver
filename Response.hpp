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
#include <arpa/inet.h>
# include "VirtualServers.hpp"
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
            std::string _cgi_path;
            size_t _size_sended;
            bool _locationFound;
            std::string _contentFile;

            
            
            void _create_file()
            {
                time_t t;
                time(&t);
                std::string file_name = "response_" + toString(t);
                std::string filepath = "/tmp/" + file_name;
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
                std::cout << _request.getHost() + ":" + toString(_request.getPort()) + path << std::endl;
                return _request.getHost() + ":" + toString(_request.getPort()) + path;
            }

            Location _getLocation()
            {
                std::string uriCopy(_request.getUri());
                std::string locationUri;
                size_t pos;
                bool notFound = true;
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
                body = "<html>\n<head>\n<title>" + toString(status) + " " + _getStatusdescription(status) + "</title>\n</head>\n<body>\n";
                body += "<center>\n<h1>" + toString(status) + " " + _getStatusdescription(status);
                body += "</center>\n<hr>\n<center>webserver</center>\n</body>\n</html>";
                _response = "HTTP/1.1 " + toString(status) + " " + _getStatusdescription(status).c_str() + "\r\n";
                _response += "Server: webserver\r\n";
                _response += "Content-Length: " + toString(body.length()) + "\r\n";
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
                _response = "HTTP/1.1 " + toString(status) + " " + _getStatusdescription(status).c_str() + "\r\n";
                _response += "Server: webserver\r\n";
                _response += "Content-Length: " + toString(s.st_size) + "\r\n";
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
                _response = "HTTP/1.1 " + toString(status) + " " + _getStatusdescription(status).c_str() + "\r\n";
                _response += "Server: webserver\r\n";
                _response += "Location: "+ location +"\r\n";
	            _response += "Connection: close\r\n\r\n";
                _response += "\r\n\r\n";
                _create_file();
            }


            void    _conflict()
            {
                int fd;
                std::string path;
                if (_locationFound && _location._error_pages.count(409))
                    path = _location._error_pages[409];
                else if (_server._error_pages.count(409))
                    path = _server._error_pages[409];
                fd = open(path.c_str(), O_RDONLY);
                fcntl(fd, F_SETFL, O_NONBLOCK);
	            close(fd);
                if (fd > 0)
                    _loadError(409, path);
                else
                    _constructError(409);
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
                char buffer[20];
                struct tm* t;
                std::string timeString;
                std::string hours = std::ctime(time);
                t = std::gmtime(time);
                std::strftime(buffer, 20, "%d-%b-%Y ", t);
                timeString = buffer;
                timeString += hours.substr(10, 6);
                return timeString;
            }

            std::string _getfileInfoForAutoIndex(std::string filename, std::string path)
            {
                struct stat s;
                stat(path.c_str(), &s);
                std::string filepath = _request.getUri() + filename;
                std::string fileinfo = "<div><a href=\"" + filepath + "\">" + filename + "</a></div>";
                path += filename;
                fileinfo += "                 " + _getTimeForAutoIndex(&s.st_mtimespec.tv_sec);
                if (s.st_mode & S_IFREG)
                    fileinfo += "                 " + toString(s.st_size);
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
                        if (strcmp(dirIterator->d_name, "."))
                            body += _getfileInfoForAutoIndex(dirIterator->d_name, path);
                    body += "</pre><hr>\n</body>\n</html>";
                    _response = "HTTP/1.1 200 " + _getStatusdescription(200) + "\r\n";
                    _response += "Server: webserver\r\n";
                    _response += "Content-Length: " + toString(body.length()) + "\r\n";
                    _response += "Connection: " + _request.get("Connection").front() + "\r\n\r\n";
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
                if (type !="CGI/Content")
                    _response = "HTTP/1.1 200 " + _getStatusdescription(200) + "\r\n";
                _response += "Server: webserver\r\n";
                if (S_ISREG(s.st_mode) && !type.length())
                {
                    std::string file_name = path.substr(path.find_last_of('/') + 1, path.size());
                    _response += "Content-Disposition: attachement; filename=" + file_name + "\r\n";
                }
                if (type.length() && type !="CGI/Content")
			        _response += "Content-Type: " + type + "\r\n";
                _response += "Connection: close\r\n";	
                if (type !="CGI/Content")
                    _response += "Content-Length: " + toString(s.st_size) + "\r\n\r\n";
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
                // mv = (path + filename);

                int r = system(mv.c_str());
                perror("errr");
                if (r != -1)
                    _response += "HTTP/1.1 201 " + _getStatusdescription(201) + "\r\n";
                else
                    _response += "HTTP/1.1 409 " + _getStatusdescription(409) + "\r\n";
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

            bool _locationHasCGI()
            {
                size_t pos;
                std::string extention; 
                if ((pos = _request.getUri().find_last_of(".")) != std::string::npos)
                    extention = _request.getUri().substr(pos + 1);
                if (_locationFound && extention.size() && _location._cgi_pass.count(extention))
                {
                   _cgi_path = _location._cgi_pass.find(extention)->second;
                   return true;
                }
                return false;

            }

            std::string _convertHTTPFiled(std::string filed)
            {
                size_t pos;
                for (int i = 0; i < filed.size(); i++)
                {
                    filed[i] = (char)std::toupper(filed[i]);
                    if (filed[i] == '-')
                        filed[i] = '_';
                }
                return filed;
            }

            char** _setCGIMetaVariables(std::string path)
            {
                struct sockaddr_in* addr = (struct sockaddr_in*)_request.getAddress();
                std::vector<std::string> variables;
                char ip[INET_ADDRSTRLEN];
                inet_ntop( AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN );
                std::string remoteAddr = "REMOTE_ADDR=";
                remoteAddr += ip;
                variables.push_back("AUTH_TYPE=");
                variables.push_back("CONTENT_LENGTH=" + _request.getContentLength());
                variables.push_back("CONTENT_TYPE=" + _request.getContentType());
                variables.push_back("GATEWAY_INTERFACE=CGI/1.1");
                variables.push_back("PATH_INFO=" + _request.getUri());
                variables.push_back("PATH_TRANSLATED=");
                variables.push_back("QUERY_STRING=" + _request.getQuery());
                variables.push_back(remoteAddr);
                variables.push_back("REMOTE_HOST=");
                variables.push_back("REMOTE_IDENT=");
                variables.push_back("REMOTE_USER=" + _request.getUserAgent());
                variables.push_back("REQUEST_METHOD=" + _request.getMethod());
                variables.push_back("SCRIPT_NAME=");
                variables.push_back("SCRIPT_FILENAME=" + path);
                variables.push_back("SERVER_NAME=Webserver");
                variables.push_back("SERVER_PORT=" + toString(_request.getPort()));
                variables.push_back("SERVER_PROTOCOL=HTTP/1.1");
                variables.push_back("SERVER_SOFTWARE=" + _cgi_path);
                variables.push_back("REDIRECT_STATUS=200");
                variables.push_back("DOCUMENT_URI=" + _request.getUri());
                std::map<std::string, std::vector<std::string> >::iterator it = _request.getRequest().begin();
                std::string str;
                int pos;
                for (; it != _request.getRequest().end(); it++)
                {
                    str = "HTTP_" + _convertHTTPFiled(it->first) + "= ";
                    for (int i = 0; i < it->second.size(); i++)
                    {
                        str += it->second[i];
                        if (i + 1 < it->second.size() && it->second[i].back() != ';')
                            str += ", ";
                        else if (i + 1 < it->second.size())
                            str += " ";
                    }
                    variables.push_back(str);
                }
                for (int i = 0; i < variables.size(); i++)
                    std::cout << variables[i] << std::endl;
                char ** vars = new char*[variables.size() + 1];
                int i;
                for (i = 0; i < variables.size(); i++)
                    vars[i] = strdup(variables[i].c_str());
                vars[i] = NULL;
                return vars;
            }

            void    _cgi(std::string path)
            {
                std::cout << "cgi founded : " << _cgi_path << " " << path << std::endl;
                char **meta_variables = _setCGIMetaVariables(path);
                time_t t;
                time(&t);
                std::string filename = "/tmp/cgi_" + toString(t) + ".cgi";
                std::cout << filename << std::endl;
                char *args[3];
                args[0] = (char *)_cgi_path.c_str();
                args[1] = (char *)_request.getUri().c_str();
                args[2] = NULL;
                int fdout = open(filename.c_str(), O_RDWR | O_CREAT , S_IRWXU | S_IRWXG);
                pid_t pid = fork();
                std::string path_c = _request.getPath();
                if (pid == 0)
                {
                    int fdin = open(path_c.c_str(), O_RDONLY);
                    if (fdin == -1)
                        perror("error");
                    //fcntl(fdin, F_SETFL, O_NONBLOCK);
                    // int r;
                    // char b[1001];
                    // while ((r = read(fdin, b, 1000)) > 0)
                    // {
                    //     b[r] = 0;
                    //     std::cout << b << std::endl;
                    // }
                    // close(fdin);
                    // fdin = open(path_c.c_str(), O_RDONLY);
                    if (dup2(fdout, STDOUT_FILENO) < 0)
                        perror("dup1");
                    if (dup2(fdin, STDIN_FILENO) < 0)
                        perror("dup2");
                    execve(args[0], args, meta_variables);
                    std::cerr << "execve is faild" << std::endl;
                    return _internalError();
                }
                else
                {
                    while (waitpid(pid, NULL, WNOHANG) == 0)
                        ;
                    close(fdout);
                    // return _badRequest();
                }
                char buffer[1000];
                int r;
                std::string out;
                size_t pos, end;
                fdout = open(filename.c_str(), O_RDONLY);
                fcntl(fdout, F_SETFL, O_NONBLOCK);
                while ((r = read(fdout, buffer, 1000)) > 0)
                {
                    out.append(buffer, r);
                    if ((pos = out.find("\r\n\r\n", 0)) != std::string::npos)
                        break;
                }
                struct stat s;
                stat(filename.c_str(), &s);
                int size = s.st_size - (pos + 4);
                if ((pos = out.find("Status:")) == std::string::npos)
                        _response = "HTTP/1.1 200 " + _getStatusdescription(200) + "\r\n";
                else
                {
                    end = out.find("\r\n");
                    std::string status_code = out.substr(pos + 7, end);
                    _response = "HTTP/1.1 " + status_code + " " + _getStatusdescription(std::stoi(status_code)) + "\r\n";
                }
                _response += "Content-Length: " + toString(size) + "\r\n";
                close(fdout);
                _loadResource(filename);
            }


            void    _proccessGET(std::string uri)
            {
                std::cout << "ProccessGet" << std::endl;
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
                    if (_locationHasCGI())
                        return _cgi(path);
                    return _loadResource(path);
                }
                else if (S_ISDIR(s.st_mode))
                {
                    std::cout << "is a dir" << std::endl;
                    if (path.back() != '/')
                        return _redirectTo(301, _request.getUri() + "/");
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
                std::cout << "ProccessPOST" << std::endl;
                struct  stat s;
                std::string path;
                if (_locationFound && _location._root.length())
                    path = _location._root;
                else
                    path = _server._root;
                path += uri;
                if (_hasUpload() && !_locationHasCGI())
                    return _upload();
                if (stat(path.c_str(), &s) < 0)
                    return _notFound();
                if (S_ISREG(s.st_mode))
                {
                    std::cout << "is a file" << std::endl;
                    if (_locationHasCGI())
                        return _cgi(path);
                    return _forbidden();
                }
                else if (S_ISDIR(s.st_mode))
                {
                    std::cout << "is a dir" << std::endl;
                    if (path.back() != '/')
                        return _redirectTo(301, _constructUri(_request.getUri() + "/"));
                    else if (_hasIndexFiles(path))
                    {
                        if (_locationHasCGI())
                            return _cgi(path);
                    }
                    else
                        return _forbidden();
                }
            }

            void    _proccessDELETE(std::string uri)
            {
                std::cout << "ProccessDELETE" << std::endl;
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
                    if (_locationHasCGI())
                        return _cgi(path);
                    return _deleteFile(path);
                }
                else if (S_ISDIR(s.st_mode))
                {
                    std::cout << "is a dir" << std::endl;
                    if (path.back() != '/')
                        return _conflict();
                    if (_locationHasCGI())
                    {
                        if (_hasIndexFiles(path))
                            return _forbidden();
                        return _cgi(path);
                    }
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
                if (_request.getUri().length() > 2048)
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
            void    reset()
            {
                _response.clear();
                _request.clear();
                _fd = -1;
                _filepath.clear();
                _cgi_path.clear();
                _size_sended = 0;
                _locationFound = false;
            }

    };
    
}


#endif