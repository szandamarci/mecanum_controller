#ifndef LEUZE_TCPSOCKET_H
#define LEUZE_TCPSOCKET_H

#include "Connection.h"
#include "stdio.h"
#include <errno.h>
#ifdef WIN32
#include <windows.h>
#endif
//#ifdef unix
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1
#define closesocket(s) ::close(s);
typedef int SOCKET;
typedef sockaddr_in SOCKADDR_IN;
//#endif


namespace Leuze
{
	/** \class TcpSocket
	\brief Implementation of a connection oriented ethernet connection (TCP/IP).

	TcpSocket provides an interface to communicate
	with a ROD4/ROD4plus scanner via a ethernet connection (TCP/IP).
	*/
	class TcpSocket : public Connection
	{
	private:
		static bool s_bWsaInitialized;
		static int  s_nInstanceCount;

		SOCKET       m_socket;
		SOCKADDR_IN  m_sockAddrHost;

	public:
		/**
		C'tor - initializes communication.
		*/
		TcpSocket(void);
		/**
		D'tor - releases resources.
		*/
		virtual ~TcpSocket(void);
		/**
		Sets the host IP address (of scanner).
		*/
                void setHostIp(const char* zIp);
		/**
		Sets the host port (of scanner).
		*/
		void setHostPort(int iPort);
		/**
		Connects via a socket.
		*/
		virtual void connect(void);
		/**
		Indicates whether connected or not.
		\return true if connected (otherwise false).
		*/
		virtual bool isConnected(void);
		/**
		Closes the socket.
		*/
		virtual void close(void);
		/**
		Sends bytes via the com port.
		@param pcBuffer Buffer (bytes) to be send.
		@param iLen Size of the send buffer.
		\return Amount of bytes send.
		*/
		virtual int send(const char *pcBuffer, int iLen);
		/**
		Receives bytes via the com port.
		@param pcBuffer Buffer (bytes) to store the received data.
		@param iLen Size of the receive buffer.
		\return Amount of bytes received.
		*/
		virtual int recv(char *pcBuffer, int iLen);

	};
}


#endif //LEUZE_TCPSOCKET_H
