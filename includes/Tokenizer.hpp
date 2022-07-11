#ifndef TOLENIZER_HPP
#define TOLENIZER_HPP

#include <map>
#include <vector>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include<ctype.h>

namespace webserve {
    
    std::string::iterator number_literal(std::string::iterator it, std::string::iterator end);
    std::string::iterator white_spaces(std::string::iterator it, std::string::iterator end);
    std::string::iterator comment(std::string::iterator it, std::string::iterator end);
    std::string::iterator server_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator location_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator error_page_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator index_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator autoindex_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator root_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator return_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator cgi_pass_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator upload_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator allowed_methods_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator methods_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator client_body_max_size_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator listen_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator server_name_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator openbracet_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator closebracet_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator semicolon_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator colon_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator point_keyword(std::string::iterator it, std::string::iterator end);
    std::string::iterator string_literal(std::string::iterator it, std::string::iterator end);

    struct token
    {
        std::string type;
        std::string value;
    };
    class Tokenizer
    {
        private:
            std::string _text;
            std::string::iterator _cursor;
            std::vector<std::pair<std::string, std::string::iterator (*)(std::string::iterator, std::string::iterator)> > _specs;
            bool _has_more_tokens();
        public:
            typedef struct token Token;
            Tokenizer(std::string text);
            ~Tokenizer();
            Token get_next_token();
    };  
}

#endif