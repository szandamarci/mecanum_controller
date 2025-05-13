#ifndef LEUZE_CONNECTION_H
#define LEUZE_CONNECTION_H


namespace Leuze
{
	/** \class Connection
	\brief Pure virtual base class for connections.

	Connection is the interface class for all connections so it can
	be used commonly in combination with a protocol.
	*/
	class Connection
	{
	public:
		/**
		Connects according to the particular configured parameters.
		*/
		virtual void connect(void) = 0;
		/**
		Indicates whether connected or not.
		*/
		virtual bool isConnected(void) = 0;
		/**
		Closes the connectiong.
		*/
		virtual void close(void) = 0;

		/**
		Sends data via the connection.
		@param pcBuffer Byte buffer of data to be send.
		@param iLen Size of the byte buffer to be send.
		\return Bytes sended.
		*/
		virtual int send(const char *pcBuffer, int iLen) = 0;
		/**
		Receives data via the connection.
		@param pcBuffer Byte buffer to store received data.
		@param iLen Size of the byte buffer for receiving.
		\return Bytes received.
		*/
		virtual int recv(char *pcBuffer, int iLen) = 0;

	};
}


#endif //LEUZE_CONNECTION_H
