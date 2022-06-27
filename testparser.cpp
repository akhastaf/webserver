#include "Parser.hpp"
#include "Socket.hpp"

webserve::Socket* creat_socket(int port)
{
    std::cout << port << std::endl;
    try
    {
        return new webserve::Socket(port);
    }
    catch(const std::string& e)
    {
        std::cerr << e << std::endl;
    }
    return NULL;
}

int main()
{
    webserve::Parser p(" server \n \
 {\n \
    server_name test.com test.net;\n\
    error_page 404 /public/404.html;\n \
    error_page 500 502 503 504 /public/50x.html;\n \
    max_client_body_size 100M;\n \
    listen 127.0.0.1:8080; \n\
    upload_pass /upload\n; \
    autoindex on;\n \
    index index.html;\n \
    root /www/html;\n \
    allow_methods GET POST DELETE;\n \
    location /test {\n \
        return 301 'https://$host$request_uri';\n \
    }\n \
    location /test2  {\n \
        root /www/html/test2;\n \
        cgi_pass php_cgi;\n \
        cgi_ext php;\n \
    }\n \
}");
    std::vector<webserve::Server> servers = p.parsing();
    std::vector<webserve::Socket*> sockets;
    int new_socket;
    try
    {
        for (size_t i = 0; i < servers.size(); i++)
            sockets.push_back(creat_socket(servers[i]._port));
        std::cout << servers.size() << std::endl;
        std::cout << "\n+++++++ Waiting for new connection ++++++++" << std::endl << std::endl;
        if ((new_socket = accept((sockets[0])->getSocketFd(), (struct sockaddr *)((sockets[0])->getAddress()), (socklen_t*)((sockets[0])->getSizeofAddress())))<0)
        {
            std::cout << new_socket << std::endl;
            std::cout << (sockets[0])->getSocketFd() << std::endl;
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        char buffer[30000] = {0};
        read( new_socket , buffer, 30000);
        printf("%s\n",buffer );
    }
    catch(const std::string& e)
    {
        std::cerr << e << '\n';
    }
    
    return 0;
}

