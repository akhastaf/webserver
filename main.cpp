#include "Server.hpp"
#include "Config.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cerr << "bad arguments passed" << std::endl;
        exit(1);
    }
    std::string fileConfig = av[1];
    webserve::Config config(fileConfig);
    return 0;
}