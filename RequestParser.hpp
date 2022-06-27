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

        Token _eat(std::string const &token_type)
        {
            Token t = _lookahead;
            try
            {
                if (t.type == "NULL")
                    std::cerr <<  ("Unexpected end of input, expected : " + token_type) << std::endl;
                if (t.type != token_type)
                     std::cerr <<  ("Unexpected token : " + t.type + ", expected : " + token_type) << std::endl;
                _lookahead = _tokenizer.get_next_token();
            }
            catch (std::string& e)
            {
                std::cerr << e << std::endl;
            }
            return t;
        }
        std::pair<std::string, std::vector<std::string> > headerField()
        {
            std::string fieldName = _eat("STRING").value;
            std::vector<std::string> fieldValues;
            if (_lookahead.type == "STRING")
            {
                fieldValues.push_back(_eat("STRING").value);
                _eat("STRING");
                _eat("CRLF");
                return std::make_pair(fieldName, fieldValues);
            }
            _eat(":");
            while (_lookahead.type != "CRLF")
            {
                fieldValues.push_back(fieldValue());
                if (_lookahead.type == ",")
                    _eat(",");
            }
            _eat("CRLF");
            return std::make_pair(fieldName, fieldValues);
        }

        std::string fieldValue()
        {
            std::string tmp;

            tmp = _eat("STRING").value;
            if (_lookahead.type == ":")
            {
                tmp += _eat(":").value;
                tmp += _eat("STRING").value;
            }
            return tmp;
        }

    public:
        RequestParser(std::string const &text) : _tokenizer(text)
        {
            try
            {
                _lookahead = _tokenizer.get_next_token();
            }
            catch (std::string& e)
            {
                std::cerr << e << std::endl;
                exit(1);
            }
        }
        ~RequestParser() {}

        std::map<std::string, std::vector<std::string> > parsing()
        {
            std::map<std::string, std::vector<std::string> > headerFields;

            // while (_lookahead.type != "NULL")
            // {
            //     std::cout << "Type " << _lookahead.type << " Value " << _lookahead.value << std::endl;
            //     _eat(_lookahead.type);
            // }
            do {
                headerFields.insert(headerField());
            } while (_lookahead.type != "CRLF");
           
            return headerFields;
        }

    };
}

#endif