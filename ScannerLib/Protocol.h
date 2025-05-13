#ifndef LEUZE_PROTOCOL_H
#define LEUZE_PROTOCOL_H


#include "Connection.h"


namespace Leuze
{
	/** \class Protocol
	\brief Pure virtual base class for protocols.

	Protocol is the interface class for all protocols to
	be used commonly in combination with a connection.
	*/
	class Protocol
	{
	public:
		/**
		Sets the connection to be used for communication.
		@param *pCon The connection to be used.
		*/
		virtual void setConnection(Connection *pCon) = 0;

	};
}


#endif //LEUZE_PROTOCOL_H