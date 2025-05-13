#ifndef LEUZE_ROD4BINARYPROTOCOL_H
#define LEUZE_ROD4BINARYPROTOCOL_H


#include "ROD4BinaryProtocolTypes.h"
#include "Connection.h"


namespace Leuze
{
	namespace ROD4
	{
		/**
		Maximum size of a datagram (1620).
		*/ 
		extern const int MAX_DATAGRAM_SIZE;

		/** \enum FrameType
		Defines most important ROD message identifiers and two internally used ones.
		*/
		enum FrameType {
			VALID_ALIVE_FRAME   =  0x14, /**< */
			VALID_DATA_FRAME    =  0x23, /**< */
			VALID_ERROR_FRAME   =  0x53, /**< */
			VALID_WARNING_FRAME =  0x54, /**< */
			INVALID_FRAME       = -1,    /**< */
			INCOMPLETE_FRAME    = -2     /**< */
		};

		/** \class ROD4BinaryProtocol
		\brief Implementation of the ROD4 binary protocol.

		ROD4BinaryProtocol provides an interface above a connection to communicate
		with a ROD4/ROD4plus scanner via the ROD4 binary protocol.
		*/
		class ROD4BinaryProtocol
		{
		private:
			Connection     *m_pConnection;
			/**
			Pointer to decode buffer
			*/
			unsigned char *m_pucDatagramBuffer;
			/**
			Index of last valid byte in decode buffer
			*/
			int            m_nBytesInDatagramBuffer;
			/**
			Locally calculated XOR 'checksum'
			*/
			unsigned char  m_ucChecksum;

		public:
			/**
			C'tor - Allocates memory for the internal send/receive buffer.
			*/
			ROD4BinaryProtocol(void);
			/**
			D'tor - Releases allocated memory.
			*/
			~ROD4BinaryProtocol(void);
			/**
			Sets the connection to be used.
			@param *pCon The connection to be used.
			*/
			virtual void setConnection(Connection *pCon);
			/**
			Receives a single datagram.
			\return NULL if nothing received, not complete yet or invalid.
			*/
                        bool receive(Datagram *);
			/**
			Starts/Stops measuring.
			@param bEnable Flag to enable or disable measuring.
			\return Success of the operation.
			*/
			bool enableMeasurement(bool bEnable);

		private:
			/**
			Receives bytes via the connection.
			*/
			void internalReceive(void);
			/**
			Checksum calculation (bitwise XOR).
			\return unmodified uCharacter
			*/
			unsigned char calculateChecksum(unsigned char ucByte);
			/**
			Removes the fill bytes from the internal buffer.
			@param iStartIndex Index where to start within the internal buffer.
			@param iLen Size of the part of the internal buffer to examin.
			\return Amount of bytes removed.
			*/
			int removeFillBytes(int iStartIndex, int iLen);
			/**
			Checks the internal buffer for ROD4binary frames.
			@param datagram A datagram to fill with appropriate data.
			\return Indicates the content of the buffer.
			*/
			FrameType checkDatagramBuffer(Datagram &datagram);

		};
	}
}


#endif //LEUZE_ROD4BINARYPROTOCOL_H
