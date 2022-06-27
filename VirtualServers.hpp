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

# define BUFFER 1024
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
            std::vector<webserve::Request> _request;
            std::vector<int> _fdsize;
            fd_set _current_read_socket, _read_socket;
            fd_set _current_write_socket, _write_socket;
            int  _fd_max_size;
            int  _fd_min_size;

            int accept_new_connection(int socket_index)
            {
                int new_socket;
                new_socket = accept(socket_index,
                                (struct sockaddr *)_sockets.find(socket_index)->second->getAddress(),
                                (socklen_t*)_sockets.find(socket_index)->second->getSizeofAddress());
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

            void    print_request(std::string const& str)
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
                _request.insert(_request.begin(), MAX_REQ, webserve::Request());
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
                        _fd_min_size = std::max(tmp->getSocketFd(), _fd_min_size);
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
                            if (_sockets.find(i) == _sockets.end())
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
                            if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1){
                                continue;
                            }
                            valread = read(new_socket, buffer, BUFFER);
                            if (valread > 0){
                                s = std::string(buffer, valread);
                                fd_with_time[new_socket] = get_current_time();
                            }
                            else if (valread == 0){
                                s = "";
                            }
                            else{ //? remove client in error
                                //_request[new_socket].clear();
                                //fd_with_response_object[new_socket].get_request().clear();
                                if (FD_ISSET(new_socket, &_read_socket)){
                                    FD_CLR(new_socket, &_current_read_socket);
                                }
                                else if (FD_ISSET(new_socket, &_write_socket)){
                                    FD_CLR(new_socket, &_current_write_socket);
                                }
                                fd_with_time.erase(i);
                                continue;
                            }
                            _request[new_socket].append(s, valread);
                            // if (v_of_request_object[new_socket].isRequestCompleted() && valread != -1){
                            //     // fd_with_response_object[new_socket] = Response(v_of_request_object[new_socket]);
                            //     // fd_with_response_object[new_socket].handleRequest(v_of_request_object[new_socket].setServer(all_servers));
                            //     FD_CLR(new_socket, &_fd_set_read);
                            //     FD_SET(new_socket, &_fd_set_write);
                            }
                            
                        }
                        // if (FD_ISSET(i, &_write_socket)){
                        // //? for first time save length and size of the file;
                        //     new_socket = i;
                        //     bzero(buffer, BUFFER);
                        //     int fd = fd_with_response_object[new_socket].get_fd();
                        //     valread =  read(fd, buffer, BUFFER);
                        //     int sended = 0;
                        //     if (valread != -1)
                        //         sended = send(new_socket, buffer, valread, 0);
                        //     if (sended > 0){
                        //         fd_with_time[new_socket] = get_current_time();
                        //         fd_with_response_object[new_socket].update_size_sended(sended);
                        //     }
                        //     //? remove client in error in sendig
                        //     if (sended  == -1 ){
                        //         v_of_request_object[new_socket].clear();
                        //         fd_with_response_object[new_socket].get_request().clear();
                        //         if (FD_ISSET(new_socket, &_fd_set_read_temp)){
                        //             FD_CLR(new_socket, &_fd_set_read);
                        //         }
                        //         else if (FD_ISSET(new_socket, &_fd_set_write_temp)){
                        //             FD_CLR(new_socket, &_fd_set_write);
                        //         }
                        //         fd_with_time.erase(i);
                        //         continue;
                        //     }
                        //     //? move the pointer of the file to send all the file 
                        //     if (valread != sended && fd != -1 && valread > 0){
                        //         int defferent = valread - sended;
                        //         if (defferent > 0)
                        //             lseek(fd, -defferent ,SEEK_CUR);
                        //     }



                        //     if (valread <= 0 && sended == 0){ //? after finish sending all responce
                        //         close(fd_with_response_object[new_socket].get_fd());
                        //         unlink(fd_with_response_object[new_socket].get_file_path().c_str());

                        //         fd_with_response_object[new_socket].reset();
                        //         if (v_of_request_object[new_socket]._isKeepAlive() == false){
                        //             FD_CLR(new_socket, &_fd_set_write);
                        //             close(new_socket);
                        //             fd_max = fds[0];
                        //             int index = 0;
                        //             for (size_t i = 0; i < fds.size(); i++){
                        //                 if (fds[i] == new_socket){
                        //                     index = i;
                        //                     continue;
                        //                 }
                        //                 fd_max = std::max(fds[i], fd_max);
                        //             }
                        //             if (index != 0)
                        //                 fds.erase(fds.begin() + index);
                        //         }
                        //         else{
                        //             FD_CLR(new_socket, &_fd_set_write);
                        //             FD_SET(new_socket, &_fd_set_read);
                        //         }
                        //         v_of_request_object[new_socket].clear();
                        //         fd_with_response_object[new_socket].get_request().clear();
                        //         fd_with_time.erase(new_socket); //? check if true
                        //     }
                        // }
                    }
            }
    };
    
} // namespace webserve

#endif