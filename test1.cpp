#include "VirtualServers.hpp"

int main(int ac, char **av)
{
    std::string filename = "test.conf";
    if (ac == 2)
        filename = av[1];
        
    try
    {
        webserve::VirtualServers virtual_servers(filename);
        // webserve::VirtualServers virtual_servers(" server \n \
        // {\n \
        //     server_name test.com test.net;\n\
        //     error_page 404 /public/404.html;\n \
        //     error_page 500 502 503 504 /public/50x.html;\n \
        //     max_client_body_size 100M;\n \
        //     listen 127.0.0.1:8080; \n\
        //     upload_pass /upload\n; \
        //     autoindex on;\n \
        //     index index.html;\n \
        //     root /www/html;\n \
        //     allow_methods GET POST DELETE;\n \
        //     location /test {\n \
        //         return 301 'https://$host$request_uri';\n \
        //     }\n \
        //     location /test2  {\n \
        //         root /www/html/test2;\n \
        //         cgi_pass php_cgi;\n \
        //         cgi_ext php;\n \
        //     }\n \
        // } \n \
        //   server { \n \
        //   listen localhost:80; \n \
        //   server_name localhost;\n \
        //     allow_methods GET;\n \
        //   }");

        virtual_servers.connect();
    }
    catch(const std::string& e)
    {
        std::cerr << e << std::endl;
    }
    
    return 0;
}
