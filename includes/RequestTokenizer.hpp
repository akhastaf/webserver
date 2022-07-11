#ifndef REQUESTTOKENIZER_HPP
#define REQUESTTOKENIZER_HPP

#include <map>
#include <vector>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include<ctype.h>
#include "Tokenizer.hpp"

namespace webserve {
    std::string::iterator white_spaces1(std::string::iterator it, std::string::iterator end);
    std::string::iterator crlf_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator colon_keyword1(std::string::iterator it, std::string::iterator end);
    std::string::iterator coma_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator string_literal1(std::string::iterator it, std::string::iterator end);
    class RequestTokenizer
    {
        private:
            std::string _text;
            std::string::iterator _cursor;
            std::vector<std::pair<std::string, std::string::iterator (*)(std::string::iterator, std::string::iterator)> > _specs;
            bool _has_more_tokens();
        public:
            typedef struct token Token;
            RequestTokenizer(std::string text);
            ~RequestTokenizer();
            Token get_next_token();
    };
}
#endif