#include "VirtualServers.hpp"
#include "Response.hpp"
#include "Request.hpp"


int main(int ac, char **av)
{
    std::string filename = "test.conf";
    if (ac == 2)
        filename = av[1];
        
    try
    {
        webserve::VirtualServers virtual_servers(filename);
        virtual_servers.connect();
    }
    catch(const std::string& e)
    {
        std::cerr << e << std::endl;
    }
    
    return 0;
}
