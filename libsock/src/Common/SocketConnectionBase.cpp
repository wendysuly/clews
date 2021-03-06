#include "SocketConnectionBase.hpp"

//Only include unistd.h and netinet/in.h on non-windows platforms
#if defined(__WIN32__) || defined(_WIN32)
#include <WinSock2.h>
#include <io.h> //close, read...
#else
#include <unistd.h>
#include <netinet/in.h>
#endif

#include <stdexcept>
#include <iostream>

using namespace std;

void SocketConnectionBase::sendHeader(size_t messageSize)
{
	cout << "sendHeader(" << messageSize << ")" << endl;
	mWrapMessages = false;
	write("!", 1);
	writeInt32(messageSize);
	write("$", 1);
	mWrapMessages = true;
}

size_t SocketConnectionBase::readHeader()
{
	mWrapMessages = false;
	char startMarker = readn(1, true).at(0);
	int32_t messageSize = readInt32();
	char endMarker = readn(1, true).at(0);
	mWrapMessages = true;

	cout << "readHeader() SM:" << startMarker << " EM:" << endMarker << " Size:" << messageSize << endl;

	if(startMarker != '!' || endMarker != '$')
	{
		throw runtime_error("Improperly wrapped message.");
	}
	return messageSize;
}

SocketConnectionBase::SocketConnectionBase(size_t bufferSize, bool wrapMessages) : mIsOpen(false), mBufferSize(bufferSize), mWrapMessages(wrapMessages) {}

vector<char> SocketConnectionBase::readn(uint32_t bufferSize, bool readUntilFull)
{
	vector<char> data(bufferSize);

    int bytesRead = ::recv(mConnectionFD,data.data(),bufferSize,0);

	if (bytesRead < 0)
	{
		throw runtime_error("Failed to read from socket.");
	}
	if(readUntilFull)
	{
		while(bytesRead < bufferSize) //Read the remaining data;
		{
            bytesRead+= ::recv(mConnectionFD, data.data(), bufferSize-bytesRead, 0);
		}
	}

	return data;
}
vector<char> SocketConnectionBase::read()
{
	if(mWrapMessages)
	{
		return readn(readHeader(), true);
	}
	else
	{
		return readn(mBufferSize);
	}
}

void SocketConnectionBase::write(const char* msg, size_t msgSize)
{
	if(!mIsOpen)
	{
		throw runtime_error("Tried to write to a disconnected socket");
	}
	if(mWrapMessages)
	{
		sendHeader(msgSize);
	}
	cout << "write(" << msg << "," << msgSize << ")" << endl;
    ::send(mConnectionFD, msg, msgSize, 0);
}
void SocketConnectionBase::write(const std::vector<char>& msg)
{
	if(!mIsOpen)
	{
		throw runtime_error("Tried to write to a disconnected socket");
	}
	if(mWrapMessages)
	{
		sendHeader(msg.size());
	}
	cout << "write(vector<char> Size: " << msg.size() << ")" << endl;
    ::send(mConnectionFD, msg.data(), msg.size(), 0);
}
void SocketConnectionBase::write(const std::string& msg)
{
	if(!mIsOpen)
	{
		throw runtime_error("Tried to write to a disconnected socket");
	}
	if(mWrapMessages)
	{
		sendHeader(msg.length());
	}
	cout << "write(" << msg << ") Length: " << msg.length() << ")" << endl;

    ::send(mConnectionFD, (const char*) msg.c_str(), msg.length(), 0);
}

void SocketConnectionBase::operator<<(const vector<char>& msg)
{
    write(msg);
}
void SocketConnectionBase::operator<<(const string& msg)
{
    write(msg);
}

void SocketConnectionBase::writeInt32(int32_t msg)
{
	cout << "writeInt32(" << msg << ")" << endl;
	msg = htonl(msg);
	write((const char*) &msg, 4);
}

int32_t SocketConnectionBase::readInt32()
{
	if(mWrapMessages)
	{
		size_t messageSize = readHeader();
		if(messageSize != 4)
		{
			throw runtime_error("Message is too large to be an int32");
		}
	}
	vector<char> intVector = readn(4, true);
	int32_t* intPtr = (int32_t*) &intVector.at(0);

	return ntohl(*intPtr);
}

void SocketConnectionBase::setMessageWrapping(bool wrapMessages)
{
	mWrapMessages = wrapMessages;
}

bool SocketConnectionBase::messagesWrapped()
{
	return mWrapMessages;
}

bool SocketConnectionBase::isOpen()
{
	return mIsOpen;
}

void SocketConnectionBase::close()
{
	if(mIsOpen)	::close(mConnectionFD);
}

int SocketConnectionBase::FD()
{
	return mConnectionFD;
}
int SocketConnectionBase::bindToFD(int fd)
{
	return dup2(mConnectionFD, fd);
}
