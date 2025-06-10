#include "../includes/ClientConnexion.hpp"

ClientConnexion::ClientConnexion(int fd, Server *server, State state) : _fd(fd), _server(server), _state(state) {
	std::cout << "New Client fd = '"<< fd << "' has been created" << std::endl;
}

State	ClientConnexion::getState() {
	return _state;
}

void	ClientConnexion::setState(State state) {
	_state = state;
}

bool	ClientConnexion::isDoneReading() {

}

void	ClientConnexion::appendToBuffer(char *buffer, int len)
{
	bufferIn.append(buffer, len);
	if (!isDoneReading())
		_state = READING;
	else
		_state = DONE_READING;
}
