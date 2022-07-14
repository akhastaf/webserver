#include "../includes/RequestTokenizer.hpp"


std::string::iterator webserve::white_spaces1(std::string::iterator it, std::string::iterator end)
{
    while (it != end && (isspace(*it) && *it != '\r' && *it != '\n'))
    it++;
    return it;
}
std::string::iterator webserve::crlf_keyword(std::string::iterator it, std::string::iterator end)
{
    (void)end;
    std::string::iterator tmp = it + 1;
    if (*it == '\r' && *tmp == '\n')
        return it + 2;
    return it;
}
std::string::iterator webserve::colon_keyword1(std::string::iterator it, std::string::iterator end)
{
    if (it != end && *it == ':')
    it++;
    return it;
}
std::string::iterator webserve::coma_keyword(std::string::iterator it, std::string::iterator end)
{
    if (it != end && *it == ',')
        it++;
    return it;
}

std::string::iterator webserve::string_literal1(std::string::iterator it, std::string::iterator end)
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

bool webserve::RequestTokenizer::_has_more_tokens() { return (_cursor != _text.end()); }
webserve::RequestTokenizer::RequestTokenizer(std::string text) : _text(text), _cursor(_text.begin()) 
{
    _specs.push_back(std::make_pair("null", white_spaces1));
    _specs.push_back(std::make_pair("CRLF", crlf_keyword));
    _specs.push_back(std::make_pair(":", colon_keyword1));
    _specs.push_back(std::make_pair(",", coma_keyword));
    _specs.push_back(std::make_pair("STRING", string_literal1));
}
webserve::RequestTokenizer::~RequestTokenizer() {}
webserve::RequestTokenizer::Token webserve::RequestTokenizer::get_next_token()
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