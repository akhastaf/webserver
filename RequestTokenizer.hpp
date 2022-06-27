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
    
    std::string::iterator white_spaces1(std::string::iterator it, std::string::iterator end)
   {
       while (it != end && (isspace(*it) && *it != '\r' && *it != '\n'))
        it++;
        return it;
   }
    std::string::iterator crlf_keyword(std::string::iterator it, std::string::iterator end)
    {
        std::string::iterator tmp = it + 1;
        if (*it == '\r' && *tmp == '\n')
            return it + 2;
        return it;
    }
    std::string::iterator colon_keyword1(std::string::iterator it, std::string::iterator end)
   {
       std::string::iterator tmp = it + 1;
       if (it != end && *it == ':')
        it++;
       return it;
   }
    std::string::iterator coma_keyword(std::string::iterator it, std::string::iterator end)
    {
        if (it != end && *it == ',')
            it++;
        return it;
    }

    std::string::iterator string_literal1(std::string::iterator it, std::string::iterator end)
    {
        std::string::iterator tmp = it;
            if (*tmp == '"')
            {
                tmp++;
                while (tmp != end && *tmp != '"')
                    tmp++;
                if (*tmp == '"')
                    return tmp;
            }
            else if (*tmp == '\'')
            {
                tmp++;
                while (tmp != end && *tmp != '\'')
                    tmp++;
                if (*tmp == '\'')
                    return ++tmp;
            }
            while (it != end)
            {
                tmp = it + 1;
                if (!(isalnum(*it) || (ispunct(*it) && !strchr(":,", *it))))
                        break;
                it++;
            }
        return it;
    }
   
    class RequestTokenizer
    {
    private:
    
        std::string _text;
        std::string::iterator _cursor;
        std::vector<std::pair<std::string, std::string::iterator (*)(std::string::iterator, std::string::iterator)> > _specs;

        bool _has_more_tokens() { return (_cursor != _text.end()); }
    public:
        typedef struct token Token;
        RequestTokenizer(std::string text) : _text(text), _cursor(_text.begin()) 
        {
            _specs.push_back(std::make_pair("null", white_spaces1)); // "null", "/^\s$/"
            _specs.push_back(std::make_pair("CRLF", crlf_keyword));
            // _specs.push_back(std::make_pair("null", comment)); // "null", "/^#.*\n$"
            // _specs.push_back(std::make_pair("{", openbracet_keyword)); // // "null", "/^{$/"
            // _specs.push_back(std::make_pair("}", closebracet_keyword));
            // _specs.push_back(std::make_pair(";", semicolon_keyword));
            _specs.push_back(std::make_pair(":", colon_keyword1));
            _specs.push_back(std::make_pair(",", coma_keyword));
            // _specs.push_back(std::make_pair("SERVER", server_keyword));
            // _specs.push_back(std::make_pair("LOCATION", location_keyword));
            // _specs.push_back(std::make_pair("LISTEN", listen_keyword));
            // _specs.push_back(std::make_pair("SERVER_NAME", server_name_keyword));
            // _specs.push_back(std::make_pair("ERROR_PAGE", error_page_keyword));
            // _specs.push_back(std::make_pair("CLINET_BODY_MAX_SIZE", client_body_max_size_keyword));
            // _specs.push_back(std::make_pair("INDEX", index_keyword));
            // _specs.push_back(std::make_pair("AUTOINDEX", autoindex_keyword));
            // _specs.push_back(std::make_pair("ROOT", root_keyword));
            // _specs.push_back(std::make_pair("RETURN", return_keyword));
            // _specs.push_back(std::make_pair("UPLOAD", upload_keyword));
            // _specs.push_back(std::make_pair("CGI_PASS", cgi_pass_keyword));
            // _specs.push_back(std::make_pair("CGI_EXT", cgi_ext_keyword));
            // _specs.push_back(std::make_pair("ALLOWED_METHOD", allowed_methods_keyword));
            // _specs.push_back(std::make_pair("METHOD", methods_keyword));
            // _specs.push_back(std::make_pair("NUMBER", number_literal));
            _specs.push_back(std::make_pair("STRING", string_literal1));
        }
        ~RequestTokenizer() {}
        Token get_next_token()
        {
            Token t;
            std::string::iterator match;
            if (!_has_more_tokens())
            {
                t.type = "NULL";
                return t;
            }
            for (size_t i = 0; i < _specs.size(); i++)
            {
                match = (_specs[i].second)(_cursor, _text.end());
                if (match == _cursor)
                    continue;
                if (_specs[i].first == "null")
                {
                    _cursor = match;
                    return get_next_token();
                }
                t.type = _specs[i].first;
                t.value = std::string(_cursor, match);
                _cursor = match;
                return t;
            }
                throw ("undefinde token " + std::string(_cursor, _cursor + 1));
            return t;
        }

    };
    

}

#endif