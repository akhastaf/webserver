#ifndef SOCKET
# define SOCKET
# include <sys/socket.h>
# include <unistd.h>
#include <netinet/in.h>
# include <iostream>
namespace webserve
{
    class Socket
    {
        public:
            typedef struct sockaddr_in address;

            Socket(int port, u_long interface = INADDR_ANY)
            {
                _address.sin_family = AF_INET;
                _address.sin_port = htons(port);
                _address.sin_addr.s_addr = htonl(interface);
                _len_address = sizeof(_address);
                //memset(_address.sin_zero, '\0', sizeof _address.sin_zero);
                if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
                    throw ("socket  error");
                const int enable = 1;
                setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
                if (bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
                {
                    std::cout << _socket_fd << std::endl;
                    close(_socket_fd); 
                    perror("in bind");

                    exit(EXIT_FAILURE);
                    //throw ("bind  error");
                }
                if (listen(_socket_fd, 10) < 0)
                {
                    perror("in listen");
                    throw ("listen  error");
                }
                std::cout << "from Socket : " << _socket_fd << std::endl;
            }
            ~Socket()
            {
                close(_socket_fd);
            }
            int getSocketFd() const
            {
                return _socket_fd;
            }
            address* getAddress()
            {
                return &_address;
            }
            int* getSizeofAddress()
            {
                return &_len_address;
            }
        private:
            int _socket_fd;
            address _address;
            int _len_address;
    };
    
} // namespace webserve



#endif