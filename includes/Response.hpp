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
#include <fcntl.h>

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
            
            void _create_file();
            std::string _constructUri(std::string path);
            Location _getLocation();
            std::string   _getStatusdescription(int statusCode);
            bool _checkBodySize();
            bool _checkUri();
            bool _checkAllowedMethod();
            void    _constructError(int status);
            void    _loadError(int status, std::string path);
            void    _redirectTo(int status, std::string location);
            void    _conflict();
            void    _timeout();
            void    _badRequest();
            void    _notFound();
            void    _internalError();
            void    _payloadTooLarge();
            void    _forbidden();
            void    _unallowedMethod();
            void    _httpVersionNotSupported();
            void    _notImplemented();
            void    _requestUriTooLong();
            bool _hasAutoIndex();
            std::string _getTimeForAutoIndex(time_t * time);
            std::string _getfileInfoForAutoIndex(std::string filename, std::string path);
            void    _autoInedxCreate();
            bool _hasUpload();
            bool _hasIndexFiles(std::string path);
            std::string _getIndexFiles(std::string path);
            void    _loadResource(std::string path);
            void    _upload();
            void    _deleteFile(std::string path);
            void    _deleteDirectoryContent(std::string path);
            bool _locationHasCGI();
            std::string _convertHTTPFiled(std::string filed);
            char** _setCGIMetaVariables(std::string path);
            void    _cgi(std::string path);
            void    _proccessGET(std::string uri);
            void    _proccessPOST(std::string uri);
            void    _proccessDELETE(std::string uri);
        public:
            Response();
            Response(Request const& req,  Server server);
            Response(Response const& res);
            ~Response();
            void    process();
            int getFd() const;
            std::string getFilepath() const;
            Request& getRequest();
            void update_size_sended(long send);
            void    reset();
    };
    
}


#endif