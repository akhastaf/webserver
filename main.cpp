#include "includes/VirtualServers.hpp"
#include "includes/Response.hpp"
#include "includes/Request.hpp"
#include "includes/MimeTypes.hpp"

int main(int ac, char **av)
{
    std::string filename = "default.conf";
    if (ac == 2)
        filename = av[1];
        
    try
    {
        webserve::MimeTypes::set();
        webserve::VirtualServers virtual_servers(filename);
        signal(SIGPIPE, SIG_IGN);        
        virtual_servers.connect();
    }
    catch(const std::string& e)
    {
        std::cerr << e << std::endl;
    }
    
    return 0;
}
