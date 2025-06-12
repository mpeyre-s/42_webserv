#include "../includes/ClientConnexion.hpp"

ClientConnexion::ClientConnexion(int fd, Server *server, State state) : _fd(fd), _server(server), _state(state), _bodySize(0) {
	_request = NULL;

}

ClientConnexion::~ClientConnexion() {
	delete _server;
	std::cout << "Client with fd " << _fd << "has been removed" << std::endl;
}

State	ClientConnexion::getState() {
	return _state;
}

Server	*ClientConnexion::getServer() {
	return _server;
}

std::string	&ClientConnexion::getBufferIn() {
	return bufferIn;
}

void	ClientConnexion::setState(State state) {
	_state = state;
}

void	ClientConnexion::setRequest(Request *request) {
	_request = request;
}

void ClientConnexion::setBufferOut(std::string buff) {
	bufferOut = buff;
}

bool	ClientConnexion::checkChunked(size_t body_start)
{
	std::string buffer = bufferIn.substr(body_start);
	std::size_t pos = buffer.find("0\r\n");
	if (pos != std::string::npos)
	{
		std::string buffer2 = buffer.substr(pos);
		std::size_t pos = buffer2.find("\r\n\r\n");
		if (pos != std::string::npos)
			return true ;
	}
	return false ;
}

bool	ClientConnexion::checkContentLength(size_t pos, size_t body_start)
{
	if (!_bodySize) {
		std::istringstream str(bufferIn.substr(pos));
		str >> _bodySize;
	}

	if (bufferIn.size() - body_start < (unsigned int)_bodySize)
		return false ;
	return true ;
}


bool	ClientConnexion::isDoneReading()
{
	std::size_t body_start = bufferIn.find("\r\n\r\n");
		if (body_start == std::string::npos)
			return false ;
	body_start += 4;
	if (bufferIn.size() >= 4 && bufferIn.substr(0, 4) == "GET ")
		return true ;
	if (bufferIn.size() >= 7 && bufferIn.substr(0, 7) == "DELETE ")
		return true ;

	std::size_t pos = bufferIn.find("Content-Length:");
	if (pos == std::string::npos)
		return (checkChunked(body_start));
	else
		return (checkContentLength(pos + 15, body_start));

	return false ;
}

void	ClientConnexion::appendToBuffer(char *buffer, int len)
{
	bufferIn.append(buffer, len);
	if (!isDoneReading())
		_state = READING;
	else
		_state = DONE_READING;
}
