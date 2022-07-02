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
    
   std::string::iterator number_literal(std::string::iterator it, std::string::iterator end)
   {
       while (it != end && isdigit(*it))
           it++;
       return it;
   }
   std::string::iterator white_spaces(std::string::iterator it, std::string::iterator end)
   {
       while (it != end && isspace(*it))
        it++;
        return it;
   }
   std::string::iterator comment(std::string::iterator it, std::string::iterator end)
   {
        if (*it == '#')
        {
            while (it != end)
            {
                if (*it == '\n')
                {
                    it++;
                    break;
                }
                it++;
            }
        }
        return it;
   }
   std::string::iterator server_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 6)
       {
        std::string::iterator tmp = it + 6;
        std::string str(it, tmp);
        if (str == "server" && !isgraph(*tmp))
            return tmp;
       }
       return it;
   }
   std::string::iterator location_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 8)
       {
        std::string::iterator tmp = it + 8;
        std::string str(it, tmp);
        if (str == "location" && !isgraph(*tmp))
            return tmp;
       }
       return it;
   }
   
   std::string::iterator error_page_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 10)
       {
        std::string::iterator tmp = it + 10;
        std::string str(it, tmp);
        if (str == "error_page" && !isgraph(*tmp))
                return tmp;
       }
       return it;
   }
   std::string::iterator index_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 5)
       {
        std::string::iterator tmp = it + 5;
        std::string str(it, tmp);
        if (str == "index" && !isgraph(*tmp))
                return tmp;
       }
       return it;
   }
   std::string::iterator autoindex_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 9)
       {
        std::string::iterator tmp = it + 9;
        std::string str(it, tmp);
        if (str == "autoindex" && !isgraph(*tmp))
                return tmp;
       }
       return it;
   }
   std::string::iterator root_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 4)
       {
        std::string::iterator tmp = it + 4;
        std::string str(it, tmp);
        if (str == "root" && !isgraph(*tmp))
                return tmp;
       }
       return it;
   }
   std::string::iterator return_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 6)
       {
        std::string::iterator tmp = it + 6;
        std::string str(it, tmp);
        if (str == "return" && !isgraph(*tmp))
                return tmp;
       }
       return it;
   }
   std::string::iterator cgi_pass_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 8)
       {
        std::string::iterator tmp = it + 8;
        std::string str(it, tmp);
        if (str == "cgi_pass" && !isgraph(*tmp))
                return tmp;
       }
       return it;
   }
   std::string::iterator cgi_ext_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 7)
       {
        std::string::iterator tmp = it + 7;
        std::string str(it, tmp);
        if (str == "cgi_ext" && !isgraph(*tmp))
                return tmp;
       }
       return it;
   }
   std::string::iterator upload_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 11)
       {
        std::string::iterator tmp = it + 11;
        std::string str(it, tmp);
        if (str == "upload_pass" && !isgraph(*tmp))
                return tmp;
       }
       return it;
   }
   std::string::iterator allowed_methods_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 13)
       {
        std::string::iterator tmp = it + 13;
        std::string str(it, tmp);
        if (str == "allow_methods" && !isgraph(*tmp))
                return tmp;
       }
       return it;
   }
   std::string::iterator methods_keyword(std::string::iterator it, std::string::iterator end)
   {
       std::string str;
       std::string::iterator tmp;
       if (std::distance(it, end) > 3)
       {
        tmp = it + 3;
        str.clear();
        str.insert(str.begin(), it, tmp);
        if (str == "GET" && (!isgraph(*tmp) || *tmp == ';'))
            return tmp;
       }
       if (std::distance(it, end) > 4)
       {
        tmp = it + 4;
        str.clear();
        str.insert(str.begin(), it, tmp);
        if (str == "POST" && (!isgraph(*tmp) || *tmp == ';'))
            return tmp;
       }
       if (std::distance(it, end) > 6)
       {
        tmp = it + 6;
        str.clear();
        str.insert(str.begin(), it, tmp);
        if (str == "DELETE" && (!isgraph(*tmp) || *tmp == ';'))
            return tmp;
       }
       return it;
   }
   std::string::iterator client_body_max_size_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 20)
       {
        std::string::iterator tmp = it + 20;
        std::string str(it, tmp);
        if (str == "max_client_body_size" && !isgraph(*tmp))
                return tmp;
       }
       return it;
   }
   std::string::iterator listen_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 6)
       {
        std::string::iterator tmp = it + 6;
        std::string str(it, tmp);
        if (str == "listen" && !isgraph(*tmp))
            return tmp;
       }
       return it;
   }
   std::string::iterator server_name_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (std::distance(it, end) > 11)
       {
        std::string::iterator tmp = it + 11;
        std::string str(it, tmp);
        if (str == "server_name" && !isgraph(*tmp))
            return tmp;
       }
       return it;
   }
   std::string::iterator openbracet_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (it != end && *it == '{')
        it++;
       return it;
   }
   std::string::iterator closebracet_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (it != end && *it == '}')
        it++;
       return it;
   }
   std::string::iterator semicolon_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (it != end && *it == ';')
        it++;
       return it;
   }
   std::string::iterator colon_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (it != end && *it == ':')
        it++;
       return it;
   }
   std::string::iterator point_keyword(std::string::iterator it, std::string::iterator end)
   {
       if (it != end && *it == '.')
        it++;
       return it;
   }
   std::string::iterator string_literal(std::string::iterator it, std::string::iterator end)
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
       if (!isdigit(*it))
       {
        while (it != end)
        {
            if (!(isalnum(*it) || (ispunct(*it) && !strchr("{};:", *it))))
                    break;
            it++;
        }
       }
       return it;
   }

    
    struct token
    {
        std::string type;
        std::string value;
        //token(std::string const& t, std::string const& v) : type(t), value(v) {} 
    };
    std::ostream& operator<< (std::ostream& o, struct token t)
    {
        o << "token type : " << t.type << " value : " << t.value << std::endl;
        return o;
    }
    class Tokenizer
    {
    private:
    
        std::string _text;
        std::string::iterator _cursor;
        std::vector<std::pair<std::string, std::string::iterator (*)(std::string::iterator, std::string::iterator)> > _specs;

        bool _has_more_tokens() { return (_cursor != _text.end()); }
    public:
        typedef struct token Token;
        Tokenizer(std::string text) : _text(text), _cursor(_text.begin()) 
        {
            _specs.push_back(std::make_pair("null", white_spaces)); // "null", "/^\s$/"
            _specs.push_back(std::make_pair("null", comment)); // "null", "/^#.*\n$"
            _specs.push_back(std::make_pair("{", openbracet_keyword)); // // "null", "/^{$/"
            _specs.push_back(std::make_pair("}", closebracet_keyword));
            _specs.push_back(std::make_pair(";", semicolon_keyword));
            _specs.push_back(std::make_pair(":", colon_keyword));
            _specs.push_back(std::make_pair(".", point_keyword));
            _specs.push_back(std::make_pair("SERVER", server_keyword));
            _specs.push_back(std::make_pair("LOCATION", location_keyword));
            _specs.push_back(std::make_pair("LISTEN", listen_keyword));
            _specs.push_back(std::make_pair("SERVER_NAME", server_name_keyword));
            _specs.push_back(std::make_pair("ERROR_PAGE", error_page_keyword));
            _specs.push_back(std::make_pair("CLINET_BODY_MAX_SIZE", client_body_max_size_keyword));
            _specs.push_back(std::make_pair("INDEX", index_keyword));
            _specs.push_back(std::make_pair("AUTOINDEX", autoindex_keyword));
            _specs.push_back(std::make_pair("ROOT", root_keyword));
            _specs.push_back(std::make_pair("RETURN", return_keyword));
            _specs.push_back(std::make_pair("UPLOAD", upload_keyword));
            _specs.push_back(std::make_pair("CGI_PASS", cgi_pass_keyword));
            _specs.push_back(std::make_pair("CGI_EXT", cgi_ext_keyword));
            _specs.push_back(std::make_pair("ALLOWED_METHOD", allowed_methods_keyword));
            _specs.push_back(std::make_pair("METHOD", methods_keyword));
            _specs.push_back(std::make_pair("NUMBER", number_literal));
            _specs.push_back(std::make_pair("STRING", string_literal));
        }
        ~Tokenizer() {}
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
                std::cout << "type " << t.type << " value "  << t.value << std::endl;
                if (t.type == "STRING" && (t.value[0] == '\'' || t.value[0] == '"'))
                    t.value = t.value.substr(1, t.value.length() - 2);
                return t;
            }
                throw ("undefinde token " + std::string(_cursor, _cursor + 1));
            return t;
        }

    };
    

}

#endif