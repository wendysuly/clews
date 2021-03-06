#ifndef SOCKET_STREAM_H
#define SOCKET_STREAM_H

#if defined(__sun) && defined(__SVR4)
typedef long id_t;
typedef id_t ctid_t;
typedef id_t zoneid_t;
#endif

//Only include unistd.h and sys/socket.h on non-windows platforms.
#if defined(__WIN32__) || defined(_WIN32)
#include <WinSock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#endif

#include <fcntl.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <vector>
#include <sys/types.h>

#include "SocketConnection.hpp"
#include "SocketClient.hpp"

class Socket
{
private:
	int mSocketFD;
	uint32_t mPortNumber;
	size_t mBufferSize;
	bool mIsOpen;
	std::vector<SocketConnection*> mConnections;
public:
	Socket(uint32_t portNumber, size_t bufferSize=512);
	~Socket();

	SocketConnection* open(std::string hostname);

	void create();
	void destroy();

	void listen();
	SocketConnection* accept();
};

#endif
