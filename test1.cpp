#include "VirtualServers.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "MimeTypes.hpp"




int main(int ac, char **av)
{
    std::string filename = "test.conf";
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
