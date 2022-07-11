# include "../includes/Socket.hpp"

webserve::Socket::Socket(int port, u_long interface) // = INADDR_ANY
{
    _address.sin_family = AF_INET;
    _address.sin_port = htons(port);
    _address.sin_addr.s_addr = htonl(interface);
    _len_address = sizeof(_address);
    if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        throw ("socket  error");
    const int enable = 1;
    fcntl(_socket_fd, F_SETFL, O_NONBLOCK);
    setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    if (bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
    {
        close(_socket_fd); 
        throw ("bind  error");
    }
    if (listen(_socket_fd, 128) < 0)
    {
        perror("in listen");
        throw ("listen  error");
    }
}
webserve::Socket::~Socket()
{
    close(_socket_fd);
}
int webserve::Socket::getSocketFd() const
{
    return _socket_fd;
}
webserve::Socket::address* webserve::Socket::getAddress()
{
    return &_address;
}
int* webserve::Socket::getSizeofAddress()
{
    return &_len_address;
}