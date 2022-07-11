#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <map>
#include <vector>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include "RequestTokenizer.hpp"
#include "Server.hpp"

namespace webserve
{
    class RequestParser
    {
        private:
            typedef struct token Token;
            RequestTokenizer _tokenizer;
            Token _lookahead;
            Token _eat(std::string const &token_type);
            std::pair<std::string, std::vector<std::string> > headerField();
            std::string fieldValue();
            void _firstLine(std::map<std::string, std::vector<std::string> >& headerFields);

        public:
            RequestParser(std::string const &text);
            ~RequestParser();
            std::map<std::string, std::vector<std::string> > parsing();

    };
}

#endif