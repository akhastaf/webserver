# include "../includes/Request.hpp"


webserve::Request::Request() : _rawRequest(""), _isHeaderComplete(false), _bodysize(0), _filename(""), _isComplete(false), _len_address(sizeof(_address)), _isTimeOUt(false) {}
webserve::Request::Request(Request const& req) 
{
    *this = req;
}
webserve::Request::~Request() 
{
    // unlink(_path.c_str());
}
webserve::Request& webserve::Request::operator= (Request const& req)
{
    _rawRequest = req._rawRequest;
    _isHeaderComplete = req._isHeaderComplete;
    _bodysize = req._bodysize;
    _filename =  req._filename;
    _request = req._request;
    _host = req._host;
    _port = req._port;
    _isComplete = req._isComplete;
    _path = req._path;
    _isChunked = req._isChunked;
    _isKeepAlive = req._isKeepAlive;
    _body = req._body;
    _isTimeOUt = req._isTimeOUt;
    return *this;
}

void    webserve::Request::clear()
{
    _request.erase(_request.begin(), _request.end());
    _rawRequest.clear();
    _isHeaderComplete = false;
    _isComplete = false;
    _port = 0;
    _host.clear();
    _bodysize = 0;
    _filename.clear();
    _path.clear();
    _body.clear();
    _isTimeOUt = false;
    unlink(_path.c_str());
}

std::map<std::string, std::vector<std::string> >& webserve::Request::getRequest()
{
    return _request;
}

void webserve::Request::append(std::string str, size_t len) 
{
    size_t r;
    std::string dst;
    if (!len || _isComplete)
        return;
    if (_isHeaderComplete && _request.count("Content-Length") && len)
    {
        body(str);
        return;
    }
    _rawRequest.append(str);
    if ((r = _rawRequest.find("\r\n\r\n")) != std::string::npos)
    {
        r += 4;
        dst = _rawRequest.substr(r);
        _isHeaderComplete = true;
        _rawRequest.erase(r);
        parse();
        print();
        if (!_request.count("Content-Length") || (has("Content-Length") && !std::stol(_request["Content-Length"][0])))
        {
            _isComplete = true;
            _isHeaderComplete = true;
            return;
        }
        if (has("Transfer-Encoding") && get("Transfer-Encoding").front() == "chunked")
            _isChunked = true;
        else
            _isChunked = false;
        body(dst);
    }
}

size_t  webserve::Request::_getHex(std::string n)
{
    size_t num;
    std::stringstream stream;
    stream << n;
    stream >> std::hex >> num;
    stream.clear();
    return num;
}

bool    webserve::Request::_getChunkedBody(std::string str)
{
    size_t pos, tmp, len;

    if ((pos = _body.find("0\r\n\r\n")) == std::string::npos)
        _body.append(str);
    else
    {
        pos = 0;
        while(true){
            tmp = _body.find("\r\n", pos);
            len = _getHex(_body.substr(pos, tmp - pos));
            _body.erase(pos, (tmp + 2) - pos);
            pos += len;
            _body.erase(pos, 2);
            
            if (len == 0)
                break;
        }
        _isComplete = true;
    }
    return false;
}

void webserve::Request::body(std::string str)
{
    time_t t;
    std::ofstream file;
    if (_filename.empty())
    {
        time(&t);
        _filename = toString(t);
        _path = "/tmp/_body" + _filename;
    }
    if (_isChunked && !_getChunkedBody(str))
        return ;
    if (_isChunked && _isComplete)
        str = _body;
    file.open(_path, std::ofstream::app);
    file << str;
    file.close();
    _bodysize += str.length();
    if (_request.count("Content-Length"))
    {
        if (_bodysize == (size_t)std::stol(_request["Content-Length"][0]))
        {
            _isComplete = true;
            return;
        }
    }
}

void    webserve::Request::parse()
{
    RequestParser parser(_rawRequest);
    _request = parser.parsing();
    _port = 80;
    size_t pos; 
    _host = get("Host").front();
    if ((pos = _host.find(":")) != std::string::npos)
    {
        _port = std::stoi(_host.substr(pos + 1));
        _host = _host.substr(0, pos);
    }
    if (has("Connection") && get("Connection").front() == "keep-alive")
        _isKeepAlive = true;
    else
        _isKeepAlive = false;
}
bool    webserve::Request::has(std::string const& field) const
{
    if (_request.count(field))
        return true;
    return false;
}
std::vector<std::string>&    webserve::Request::get(std::string const& field)
{
    return _request[field];
}
void webserve::Request::print()
{
    std::cout << _rawRequest << std::endl;
}

std::string webserve::Request::getFilename() const
{
    return _filename;
}
std::string& webserve::Request::getPath()
{
    return _path;
}
std::string webserve::Request::getHost() const
{
    return _host;
}
int webserve::Request::getPort() const
{
    return _port;
}
bool webserve::Request::isRequestComplete() const { return _isComplete; }
void webserve::Request::setRequestComplete(bool state) { _isComplete = state; }
size_t webserve::Request::getBodysize() const { return _bodysize; }
bool webserve::Request::isKeepAlive() 
{
    return _isKeepAlive;
}

std::string webserve::Request::getTransferEncoding()
{
    if (has("Transfer-Encoding"))
        return get("Transfer-Encoding").front();
    return std::string();
}
std::string webserve::Request::getContentType()
{
    std::string type;
    if (has("Content-Type"))
    {
        for (size_t i = 0; i < get("Content-Type").size(); i++)
        {
            type += get("Content-Type")[i];
            if (i + 1 < get("Content-Type").size() && get("Content-Type")[i].back() != ';')
                type += ", ";
            else if (i + 1 < get("Content-Type").size())
                type += " ";
        }
        return type;
    }
    return std::string();
}
std::string webserve::Request::getUri()
{
    if (has("Uri"))
        return get("Uri").front();
    return std::string();
}

std::string webserve::Request::getMethod()
{
    if (has("Method"))
        return get("Method").front();
    return std::string();
}

std::string webserve::Request::getContentLength()
{
    if (has("Content-Length"))
        return get("Content-Length").front();
    return std::string();
}
std::string webserve::Request::getConnection()
{
    if (has("Connection"))
        return get("Connection").front();
    return std::string();
}
std::string webserve::Request::getQuery()
{
    if (has("Query"))
        return get("Query").front();
    return std::string();
}
std::string webserve::Request::getCookies()
{
    if (has("Cookie"))
        return get("Cookie").front();
    return std::string();
}

std::string webserve::Request::getUserAgent()
{
    std::string userAgent;
    if (has("User-Agent"))
    {
        for (size_t i = 0; i < get("User-Agent").size(); i++)
        {
            userAgent += get("User-Agent")[i];
            if (i + 1 < get("User-Agent").size())
                userAgent += ", ";
        }
        return userAgent;
    }
    return std::string();
}

struct sockaddr* webserve::Request::getAddress()
{
    return &_address;
}
int* webserve::Request::getLengthAddress()
{
    return &_len_address;
}

void    webserve::Request::setTimeOUT(bool state)
{
    _isTimeOUt = state;
}
bool    webserve::Request::getTimeOUT() const
{
    return _isTimeOUt;
}