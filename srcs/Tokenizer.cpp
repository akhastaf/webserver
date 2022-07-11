#include "../includes/Tokenizer.hpp"

std::string::iterator webserve::number_literal(std::string::iterator it, std::string::iterator end)
{
    while (it != end && isdigit(*it))
        it++;
    return it;
}
std::string::iterator webserve::white_spaces(std::string::iterator it, std::string::iterator end)
{
    while (it != end && isspace(*it))
        it++;
    return it;
}
std::string::iterator webserve::comment(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::server_keyword(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::location_keyword(std::string::iterator it, std::string::iterator end)
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

std::string::iterator webserve::error_page_keyword(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::index_keyword(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::autoindex_keyword(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::root_keyword(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::return_keyword(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::cgi_pass_keyword(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::upload_keyword(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::allowed_methods_keyword(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::methods_keyword(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::client_body_max_size_keyword(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::listen_keyword(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::server_name_keyword(std::string::iterator it, std::string::iterator end)
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
std::string::iterator webserve::openbracet_keyword(std::string::iterator it, std::string::iterator end)
{
    if (it != end && *it == '{')
        it++;
    return it;
}
std::string::iterator webserve::closebracet_keyword(std::string::iterator it, std::string::iterator end)
{
    if (it != end && *it == '}')
        it++;
    return it;
}
std::string::iterator webserve::semicolon_keyword(std::string::iterator it, std::string::iterator end)
{
    if (it != end && *it == ';')
        it++;
    return it;
}
std::string::iterator webserve::colon_keyword(std::string::iterator it, std::string::iterator end)
{
    if (it != end && *it == ':')
        it++;
    return it;
}
std::string::iterator webserve::point_keyword(std::string::iterator it, std::string::iterator end)
{
    if (it != end && *it == '.')
        it++;
    return it;
}
std::string::iterator webserve::string_literal(std::string::iterator it, std::string::iterator end)
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

bool webserve::Tokenizer::_has_more_tokens() { return (_cursor != _text.end()); }

webserve::Tokenizer::Tokenizer(std::string text) : _text(text), _cursor(_text.begin()) 
{
    _specs.push_back(std::make_pair("null", white_spaces));
    _specs.push_back(std::make_pair("null", comment));
    _specs.push_back(std::make_pair("{", openbracet_keyword));
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
    _specs.push_back(std::make_pair("ALLOWED_METHOD", allowed_methods_keyword));
    _specs.push_back(std::make_pair("METHOD", methods_keyword));
    _specs.push_back(std::make_pair("NUMBER", number_literal));
    _specs.push_back(std::make_pair("STRING", string_literal));
}
webserve::Tokenizer::~Tokenizer() {}
webserve::Tokenizer::Token webserve::Tokenizer::get_next_token()
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
        if (t.type == "STRING" && (t.value[0] == '\'' || t.value[0] == '"'))
            t.value = t.value.substr(1, t.value.length() - 2);
        return t;
    }
        throw ("undefinde token " + std::string(_cursor, _cursor + 1));
    return t;
}