#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>
#include <vector>
#include <string>
#include <sys/time.h>
#include <sstream>

namespace webserve
{
    long int get_current_time();
    template<typename T>
    std::string toString(T s)
    {
        std::stringstream ss;
        std::string str;

        ss << s;
        ss >> str;
        return str;
    }
}

#endif