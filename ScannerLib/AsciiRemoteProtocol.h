#ifndef LEUZE_ASCIIREMOTEPROTOCOL_H
#define LEUZE_ASCIIREMOTEPROTOCOL_H


#include "Connection.h"


namespace Leuze
{
	namespace ROD4
	{
		/**
		Size of the internal used send/receive buffer (7424).
		*/
		extern const int SERIAL_BUFFER_SIZE;

		/** \class AsciiRemoteProtocol
		\brief Implementation of the ASCII Remote protocol.

		AsciiRemoteProtocol provides an interface above a connection to communicate
		with a ROD4/ROD4plus scanner via the ASCII Remote protocol.
		*/
		class AsciiRemoteProtocol
		{
		private:
			/** \enum FrameType
			*/
			enum FrameType {
				VALID_ASCII_FRAME   =  0, /**< */
				INVALID_FRAME       = -1, /**< */
				INCOMPLETE_FRAME    = -2  /**< */
			};

		private:
			Connection *m_pConnection;
			char       *m_pcDataBuffer;
			int         m_nBytesInDataBuffer;

		public:
			/**
			C'tor - Allocates memory for the internal send/receive buffer.
			*/
			AsciiRemoteProtocol(void);
			/**
			D'tor - Releases allocated memory.
			*/
			virtual ~AsciiRemoteProtocol(void);
			/**
			Sets the connection to be used.
			@param *pCon The connection to be used.
			*/
			virtual void setConnection(Connection *pCon);
			/**
			Receives the version of the scanner (e.g. 'V 01.00').
			@param ppcBuffer [IN/OUT] Address of a pointer to a buffer (the memory for the buffer
			is allocated and must be freed after usage by calling 'freeBuffer(...)'.
			@param piLen [OUT] Pointer to a variable for holding the size of the allocated buffer memory.
			\return Success of the operation (true --> received version and allocated memory).
			*/
			bool recvVersion(char **ppcBuffer, int *piLen);
			/**
			Clears a measurement segment.
			@param iIndex Segment number (1..12).
			\return Success (command send).
			*/
			bool clearMeasurementSegment(int iIndex);
			/**
			Configures a measurement segment.
			@param iIndex Segment number (1..12).
			@param iStart Index of start segment (0..528).
			@param iStop Index of stop segment (iStart..528).
			@param iSegmGap Gap between segments (0..7) (0 --> each segment, 1 --> every second segment).
			@param iScanGap Gap between scans (0..11) (0 --> each scan, 1 --> every second scan).			
			\return Success (command send).
			*/
			bool setMeasurementSegment(int iIndex, int iStart, int iStop, int iSegmGap, int iScanGap);
			/**
			Starts/Stops measuring.
			@param bEnable Flag to enable or disable measuring.
			\return Success of the operation.
			*/
			bool enableMeasurement(bool bEnable);
			/**
			Receives a raw ASCII Remote frame from the scanner.
			@param ppcBuffer [IN/OUT] Address of a pointer to a buffer (the memory for the buffer
			is allocated and must be freed after usage by calling 'freeBuffer(...)'.
			@param piLen [OUT] Pointer to a variable for holding the size of the allocated buffer memory.
			\return Success of the operation (true --> received bytes and allocated memory)
			*/
			bool recvRaw(char **ppcBuffer, int *piLen);
			/**
			Frees the allocated buffer by a previous call of 'recvRaw'.
			*/
			void freeBuffer(char *pcBuffer);

		private:
			/**
			Receives bytes via the connection.
			*/
			int internalReceive(void);
			/**
			Checks the given buffer for ASCII Remote frames.
			@param ppcBuffer [IN] Pointer to a buffer.
			@param piLen Pointer [IN/OUT] to a variable holding the size of the buffer. In case
			of a complete and valid frame the length of the frame is stored.
			\return Indicates the content of the buffer
			(complete frame, incomplete frame or invalid frame).
			*/
			FrameType checkDatagramBuffer(char **ppcBuffer, int *piLen);

		};
	}
}


#endif //LEUZE_ASCIIREMOTEPROTOCOL_H