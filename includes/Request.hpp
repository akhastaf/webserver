#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <string.h>
# include <iostream>
# include <sys/_types/_size_t.h>
# include <vector>
# include <map>
# include <ctime>
# include <fstream>
# include <sstream>
# include "RequestParser.hpp"
# include "utils.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


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
            bool _isTimeOUt;
            size_t  _getHex(std::string n);

        public:
            Request();
            Request(Request const& req);
            ~Request();
            Request& operator= (Request const& req);
            void    clear();
            std::map<std::string, std::vector<std::string> >& getRequest();
            void append(std::string str, size_t len);
            bool    _getChunkedBody(std::string str);
            void body(std::string str);
            void    parse();
            bool    has(std::string const& field) const;
            std::vector<std::string>&   get(std::string const& field);
            void print();
            std::string getFilename() const;
            std::string& getPath();
            std::string getHost() const;
            int getPort() const;
            bool isRequestComplete() const;
            void setRequestComplete(bool state);
            size_t getBodysize() const;
            bool isKeepAlive();
            std::string getTransferEncoding();
            std::string getContentType();
            std::string getUri();
            std::string getMethod();
            std::string getContentLength();
            std::string getQuery();
            std::string getCookies();
            std::string getUserAgent();
            struct sockaddr* getAddress();
            int* getLengthAddress();
            void    setTimeOUT(bool state);
            bool    getTimeOUT() const;
            
    };
}

#endif