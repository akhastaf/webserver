#ifndef VIRTUALSERVER
# define VIRTUALSERVER
#include <sys/select.h>
#include <fcntl.h>
#include <vector>
#include "Socket.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "Parser.hpp"
#include "utils.hpp"
#include <errno.h>
#include <algorithm>

# define BUFFER 1000
# define MAX_REQ 1024

#define CGI_TIME_OUT 2000000
#define REQUEST_TIME_OUT 1000000
namespace webserve
{
    class VirtualServers
    {
        private:
            /* data */
            std::vector<Server> _servers;
            std::map<int, webserve::Socket*> _sockets;
            std::vector<webserve::Request> _requests;
            std::map<int, webserve::Response> _responses;
            std::vector<int> _fdsize;
            fd_set _current_read_socket, _read_socket;
            fd_set _current_write_socket, _write_socket;
            int  _fd_max_size;
            int  _fd_min_size;

            int accept_new_connection(int socket_index)
            {
                int new_socket;
                if ((new_socket = accept(socket_index,
                                (struct sockaddr *)_requests[socket_index].getAddress(),
                                (socklen_t*)_requests[socket_index].getLengthAddress())) < 0)
                        std::cout << "accept error" << std::endl;
                std::cout << "accepte is done" << std::endl;
                return new_socket;
            }

            bool _alredy_binded(int index, int port)
            {
                for (int i = 0; i < index; i++)
                    if (_servers[i]._port == port)
                        return true;
                return false;
            }

            void    _init_fd_set()
            {
                FD_ZERO(&_read_socket);
                FD_ZERO(&_current_read_socket);
                FD_ZERO(&_write_socket);
                FD_ZERO(&_current_write_socket);
                
            }

            Server _match_server(int new_socket)
            {
                std::vector<Server> match;
                for (size_t i = 0; i < _servers.size(); i++ ) {
                    for (size_t j = 0; j < _servers[i]._server_names.size() ; j++) {
                        if (_requests[new_socket].getHost() == _servers[i]._server_names[j]) {
                            match.push_back(_servers[i]);
                        }
                    }
                }
                if (match.size() == 1) {
                    return match[0];
                } 
                else if (match.size() == 0) {
                    for (size_t i = 0; i < _servers.size(); i++) {
                        if (_requests[new_socket].getPort() == _servers[i]._port) {
                            return _servers[i];
                        }
                    }
                    return _servers[0];
                }
                for (size_t i = 0; i < match.size(); i++) {
                    if (_requests[new_socket].getPort() == match[i]._port) {
                        return match[i];
                    }
                }
                return match[0];
            }

            void    print_requests(std::string const& str)
            {
                for (int i = 0; i < str.length(); i++)
                {
                    switch (str[i])
                    {
                    case 10:
                        std::cout << "\\n";
                        break;
                    case 13:
                        std::cout << "\\r";
                        break;
                    default:
                        std::cout << (char)str[i];
                        break;
                    }
                }
                std::cout << std::endl;
            }

        public:
            VirtualServers(std::string const& filename)
            {
                std::string text;
                std::string buffer;
                webserve::Socket* tmp;
                std::ifstream file;
                file.open(filename);
                if (!file.is_open())
                    throw "file can't be open";
                while (getline(file, buffer))
                    text += buffer + "\n";
                file.close();
                Parser p(text);
                _servers = p.parsing();
                _fd_max_size = 0;
                _fd_min_size = INT32_MAX;
                _requests.insert(_requests.begin(), MAX_REQ, webserve::Request());
                try
                {
                    _init_fd_set();
                    for (size_t i = 0; i < _servers.size(); i++)
                    {
                        if (_alredy_binded(i, _servers[i]._port))
                            continue;
                        tmp = new webserve::Socket(_servers[i]._port);
                        _sockets.insert(std::make_pair(tmp->getSocketFd(), tmp));
                        fcntl(tmp->getSocketFd(), F_SETFL, O_NONBLOCK);
                        FD_SET(tmp->getSocketFd(), &_current_read_socket);
                        _fd_max_size = std::max(tmp->getSocketFd(), _fd_max_size);
                        _fd_min_size = std::min(tmp->getSocketFd(), _fd_min_size);
                        _fdsize.push_back(tmp->getSocketFd());
                    }
                }
                catch(const std::string& e)
                {
                    std::cerr << e << std::endl;
                }
            }

            ~VirtualServers() 
            {
                std::map<int, webserve::Socket*>::iterator it = _sockets.begin();
                for (; it != _sockets.end(); it++)
                {
                    delete it->second;
                }
            }
            void    connect()
            {
                int new_socket;
                std::map<int, long int> fd_with_time;
                std::map<int, std::string>::iterator pos;
                struct timeval select_time;
                select_time.tv_sec = 1;
                select_time.tv_usec = 0;
                std::string s;
                char buffer[BUFFER] = {0};
                int valread;
                int max_servers_fd = _fd_max_size;
                while (true)
                {
                    bzero(buffer, BUFFER);
                    _read_socket = _current_read_socket;
                    _write_socket = _current_write_socket;
                    if (select(_fd_max_size + 1, &_read_socket, &_write_socket, NULL, &select_time) < 0)
                        continue;
                    // check for Time out
                    // for (int i = 0; i < _fd_max_size + 1; i++)
                    // {
                    //     long int now = get_current_time();
                    // }

                    // check for ready socket
                    for (int i = 0; i < _fd_max_size + 1; i++)
                    {
                        // check for reading sockets
                        if (FD_ISSET(i, &_read_socket))
                        {
                            if (i <= max_servers_fd)
                            {
                                if ((new_socket = accept_new_connection(i)) < 0)
                                    continue;
                                if (std::find(_fdsize.begin(), _fdsize.end(), new_socket) == _fdsize.end())
                                {
                                    FD_SET(new_socket, &_current_read_socket);
                                    _fdsize.push_back(new_socket);
                                    _fd_max_size = new_socket;
					            }
                            }
                            else 
                                new_socket = i;
                            if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1)
                                continue;
                            valread = read(new_socket, buffer, BUFFER);
                            if (valread > 0){
                                s = std::string(buffer, valread);
                                fd_with_time[new_socket] = get_current_time();
                            }
                            else if (valread == 0)
                                s = "";
                            else 
                            { 
                                _requests[new_socket].clear();
                                _responses[new_socket].getRequest().clear();
                                if (FD_ISSET(new_socket, &_read_socket))
                                    FD_CLR(new_socket, &_current_read_socket);
                                else if (FD_ISSET(new_socket, &_write_socket))
                                    FD_CLR(new_socket, &_current_write_socket);
                                fd_with_time.erase(i);
                                continue;
                            }
                            _requests[new_socket].append(s, valread);
                            if (_requests[new_socket].isRequestComplete() && valread != -1)
                            {
                                _responses[new_socket] = Response(_requests[new_socket], _match_server(new_socket));
                                _responses[new_socket].process();
                                FD_CLR(new_socket, &_current_read_socket);
                                FD_SET(new_socket, &_current_write_socket);
                            }
                            
                        }
                        if (FD_ISSET(i, &_write_socket))
                        {
                            new_socket = i;
                            bzero(buffer, BUFFER);
                            int fd = _responses[new_socket].getFd();
                            valread =  read(fd, buffer, BUFFER);
                            int sended = 0;
                            if (valread != -1)
                                sended = send(new_socket, buffer, valread, 0);
                            if (sended > 0){
                                fd_with_time[new_socket] = get_current_time();
                                _responses[new_socket].update_size_sended(sended);
                            }
                            if (sended  == -1 ){
                                _requests[new_socket].clear();
                                _responses[new_socket].getRequest().clear();
                                if (FD_ISSET(new_socket, &_read_socket)){
                                    FD_CLR(new_socket, &_current_read_socket);
                                }
                                else if (FD_ISSET(new_socket, &_write_socket)){
                                    FD_CLR(new_socket, &_current_write_socket);
                                }
                                fd_with_time.erase(i);
                                continue;
                            }
                            if (valread != sended && fd != -1 && valread > 0){
                                int defferent = valread - sended;
                                if (defferent > 0)
                                    lseek(fd, -defferent ,SEEK_CUR);
                            }



                            if (valread <= 0 && sended == 0){
                                close(_responses[new_socket].getFd());
                                unlink(_responses[new_socket].getFilepath().c_str());

                                // _responses[new_socket].reset();
                                if (_requests[new_socket].isKeepAlive() == false){
                                    FD_CLR(new_socket, &_current_write_socket);
                                    close(new_socket);
                                    _fd_max_size = _fdsize[0];
                                    int index = 0;
                                    for (size_t i = 0; i < _fdsize.size(); i++){
                                        if (_fdsize[i] == new_socket){
                                            index = i;
                                            continue;
                                        }
                                        _fd_max_size = std::max(_fdsize[i], _fd_max_size);
                                    }
                                    if (index != 0)
                                        _fdsize.erase(_fdsize.begin() + index);
                                }
                                else{
                                    FD_CLR(new_socket, &_current_write_socket);
                                    FD_SET(new_socket, &_current_read_socket);
                                }
                                _requests[new_socket].clear();
                                _responses[new_socket].getRequest().clear();
                                fd_with_time.erase(new_socket);
                            }
                        }
                    }
                }
            }
    };
    
} // namespace webserve

#endif