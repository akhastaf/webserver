#include "../includes/VirtualServers.hpp"

int webserve::VirtualServers::accept_new_connection(int socket_index)
{
    int new_socket;
    new_socket = accept(socket_index,
                    (struct sockaddr *)_requests[socket_index].getAddress(),
                    (socklen_t*)_requests[socket_index].getLengthAddress());
    return new_socket;
}

bool webserve::VirtualServers::_alredy_binded(int index, int port)
{
    for (int i = 0; i < index; i++)
        if (_servers[i]._port == port)
            return true;
    return false;
}

void    webserve::VirtualServers::_init_fd_set()
{
    FD_ZERO(&_read_socket);
    FD_ZERO(&_current_read_socket);
    FD_ZERO(&_write_socket);
    FD_ZERO(&_current_write_socket);
    
}

webserve::Server webserve::VirtualServers::_match_server(int new_socket)
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

webserve::VirtualServers::VirtualServers(std::string const& filename)
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
    _requests.insert(_requests.begin(), MAX_REQ, webserve::Request());
    try
    {
        _init_fd_set();
        for (size_t i = 0; i < _servers.size(); i++)
        {
            if (_alredy_binded(i, _servers[i]._port))
                continue;
            tmp = new Socket(_servers[i]._port);
            _sockets.insert(std::make_pair(tmp->getSocketFd(), tmp));
            FD_SET(tmp->getSocketFd(), &_current_read_socket);
            _fdsize.insert(tmp->getSocketFd());
            _fdserver.insert(tmp->getSocketFd());
        }
    }
    catch(const std::string& e)
    {
        std::cerr << e << std::endl;
    }
}

webserve::VirtualServers::~VirtualServers() 
{
    std::map<int, webserve::Socket*>::iterator it = _sockets.begin();
    for (; it != _sockets.end(); it++)
    {
        delete it->second;
    }
}
void    webserve::VirtualServers::connect()
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
        _fd_max_size = *_fdsize.rbegin();
        if (select(_fd_max_size + 1, &_read_socket, &_write_socket, NULL, &select_time) < 0)
            continue;
        for (std::set<int>::iterator it = _fdsize.begin(); it != _fdsize.end();)
        {
            int i = *it;
            ++it;
            
            if (FD_ISSET(i, &_read_socket))
            {
                if (_fdserver.count(i))
                {
                    if ((new_socket = accept_new_connection(i)) < 0 || new_socket > FD_SETSIZE)
                    {
                        if (new_socket > FD_SETSIZE)
                            close(new_socket);
                        continue;
                    }
                    FD_SET(new_socket, &_current_read_socket);
                    _fdsize.insert(new_socket);
                    if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1)
                        continue;
                }
                else
                {
                    new_socket = i;
                    long int now = get_current_time();
                    
                    if (fd_with_time[new_socket] && now - fd_with_time[new_socket] >= REQUEST_TIME_OUT)
                    {
                        _requests[new_socket].setTimeOUT(true);
                        _requests[new_socket].setRequestComplete(true);
                        _responses[new_socket] = Response(_requests[new_socket], _match_server(new_socket));
                        _responses[new_socket].process();
                        FD_CLR(new_socket, &_current_read_socket);
                        FD_SET(new_socket, &_current_write_socket);
                    }
                    else {
                        valread = read(new_socket, buffer, BUFFER);
                        if (valread > 0){
                            s.assign(buffer, valread);
                            fd_with_time[new_socket] = get_current_time();
                            _requests[new_socket].append(s, valread);
                            if (_requests[new_socket].isRequestComplete())
                            {
                                _responses[new_socket] = Response(_requests[new_socket], _match_server(new_socket));
                                _responses[new_socket].process();
                                FD_CLR(new_socket, &_current_read_socket);
                                FD_SET(new_socket, &_current_write_socket);
                            }
                        }
                        else if (valread == -1)
                        { 
                            _requests[new_socket].clear();
                            _responses[new_socket].getRequest().clear();
                            FD_CLR(new_socket, &_current_read_socket);
                            close(new_socket);
                            fd_with_time.erase(i);
                            _fdsize.erase(new_socket);
                            continue;
                        }
                    }
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
                if (sended >= 0){
                    fd_with_time[new_socket] = get_current_time();
                    _responses[new_socket].update_size_sended(sended);
                }
                if (sended  == -1 ){
                    close(_responses[new_socket].getFd());
                    unlink(_responses[new_socket].getFilepath().c_str());
                    _requests[new_socket].clear();
                    _responses[new_socket].getRequest().clear();
                    _responses[new_socket].reset();
                    FD_CLR(new_socket, &_current_write_socket);
                    fd_with_time.erase(i);
                    _fdsize.erase(new_socket);
                    close(new_socket);
                    continue;
                }
                if (valread != sended && fd != -1 && valread > 0){
                    int defferent = valread - sended;
                    if (defferent > 0)
                        lseek(fd, -defferent ,SEEK_CUR);
                }
                if (!valread && !sended){
                    close(_responses[new_socket].getFd());
                    unlink(_responses[new_socket].getFilepath().c_str());

                    _responses[new_socket].reset();
                    if (!_requests[new_socket].isKeepAlive()){
                        FD_CLR(new_socket, &_current_write_socket);
                        close(new_socket);
                        _fdsize.erase(new_socket);
                        fd_with_time.erase(i);
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

