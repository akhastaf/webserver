#ifndef SOCKET_HPP
# define SOCKET_HPP
# include <sys/socket.h>
# include <unistd.h>
#include <netinet/in.h>
# include <iostream>
#include <fcntl.h>
namespace webserve
{
    class Socket
    {
        public:
            typedef struct sockaddr_in address;
            Socket(int port, u_long interface = INADDR_ANY);
            ~Socket();
            int getSocketFd() const;
            address* getAddress();
            int* getSizeofAddress();
        private:
            int _socket_fd;
            address _address;
            int _len_address;
    };
    
}



#endif