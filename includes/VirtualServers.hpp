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
#include <set>
#include <sys/types.h>
#include <sys/stat.h>

# define BUFFER 1000
# define MAX_REQ 1024

#define CGI_TIME_OUT 2000000
#define REQUEST_TIME_OUT 1000000
namespace webserve
{
    class VirtualServers
    {
        private:
            std::vector<Server> _servers;
            std::map<int, webserve::Socket*> _sockets;
            std::vector<webserve::Request> _requests;
            std::map<int, webserve::Response> _responses;
            std::set<int> _fdsize;
            std::set<int> _fdserver;
            fd_set _current_read_socket, _read_socket;
            fd_set _current_write_socket, _write_socket;
            int  _fd_max_size;

            int accept_new_connection(int socket_index);
            bool _alredy_binded(int index, int port);
            void    _init_fd_set();
            Server _match_server(int new_socket);
        public:
            VirtualServers(std::string const& filename);
            ~VirtualServers();
            void    connect();
    };
    
}

#endif