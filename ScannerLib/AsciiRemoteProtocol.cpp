#include "AsciiRemoteProtocol.h"
//#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#ifndef NULL
#define NULL 0
#endif


using namespace Leuze;
using namespace Leuze::ROD4;


//-------------------------------------------------------------------
const int Leuze::ROD4::SERIAL_BUFFER_SIZE = 7424;
//-------------------------------------------------------------------
AsciiRemoteProtocol::AsciiRemoteProtocol(void)
: m_pConnection(NULL)
, m_nBytesInDataBuffer(0)
{
	m_pcDataBuffer = (char *) malloc(SERIAL_BUFFER_SIZE);
}
//-------------------------------------------------------------------
AsciiRemoteProtocol::~AsciiRemoteProtocol(void)
{
	free(m_pcDataBuffer);
	m_pcDataBuffer = NULL;
}
//-------------------------------------------------------------------
void AsciiRemoteProtocol::setConnection(Connection *pCon)
{
	m_pConnection = pCon;
}
//-------------------------------------------------------------------
bool AsciiRemoteProtocol::recvVersion(char **ppcBuffer, int *piLen)
{
	if( (NULL == ppcBuffer) ||
		(NULL == piLen) )
	{
		return false;
	}

        char pcBuffer[] = "\002V\003"; //<STX>V<ETX>
	int iBufLen = strlen(pcBuffer);

	if(iBufLen != m_pConnection->send(pcBuffer, strlen(pcBuffer)))
	{
		return false;
	}

	if(internalReceive() < 0)
	{
		return false;
	}

	FrameType eFrameType = checkDatagramBuffer(ppcBuffer, piLen);
	while(INCOMPLETE_FRAME == eFrameType)
	{
		if(internalReceive() < 0)
		{
			break;
		}
		eFrameType = checkDatagramBuffer(ppcBuffer, piLen);
	}

	return (VALID_ASCII_FRAME == eFrameType) ? true : false;
}
//-------------------------------------------------------------------
bool AsciiRemoteProtocol::clearMeasurementSegment(int iIndex)
{
	if( (iIndex < 1) || (iIndex > 12) )
	{
		return false;
	}

	const int acBufferSize = 7 + 1;
	char acBuffer[acBufferSize];
#ifdef WIN32
	sprintf_s (acBuffer, acBufferSize, "\002DS %d\003", iIndex); //e.g. <STX>DS 12<ETX>
#else
        sprintf(acBuffer, "\002DS %d\003", iIndex); //e.g. <STX>DS 12<ETX>
#endif
	int iBufLen = strlen(acBuffer);

	if(iBufLen != m_pConnection->send(acBuffer, strlen(acBuffer)))
	{
		return false;
	}

	return true;
}
//-------------------------------------------------------------------
bool AsciiRemoteProtocol::setMeasurementSegment(int iIndex, int iStart, int iStop, int iSegmGap, int iScanGap)
{
	if( (iIndex < 1) || (iIndex > 12) ||
		(iStart < 0) || (iStart > 528) ||
		(iStop <= iStart) || (iStop > 528) ||
		(iSegmGap < 0) || (iSegmGap > 7) ||
		(iScanGap < 0) || (iScanGap > 11) )
	{
		return false;
	}

	const int acBufferSize = 20 + 1;
	char acBuffer[acBufferSize];
#ifdef WIN32
	sprintf_s(acBuffer, acBufferSize, "\002CS %d %d %d %d %d\003", iIndex, iStart, iStop, iSegmGap, iScanGap); //e.g. <STX>CS 12 239 289 2 10<ETX>
#else
        sprintf(acBuffer, "\002CS %d %d %d %d %d\003", iIndex, iStart, iStop, iSegmGap, iScanGap); //e.g. <STX>CS 12 239 289 2 10<ETX>
#endif
	int iBufLen = strlen(acBuffer);

	if(iBufLen != m_pConnection->send(acBuffer, strlen(acBuffer)))
	{
		return false;
	}

	return true;
}
//-------------------------------------------------------------------
bool AsciiRemoteProtocol::enableMeasurement(bool bEnable)
{
        const char *pcBuffer = bEnable ? "\002M+\003" : "\002M-\003";
	int iBufLen = strlen(pcBuffer);

	if(iBufLen != m_pConnection->send(pcBuffer, strlen(pcBuffer)))
	{
		return false;
	}

	return true;
}
//-------------------------------------------------------------------
bool AsciiRemoteProtocol::recvRaw(char **ppcBuffer, int *piLen)
{
	if( (NULL == ppcBuffer) ||
		(NULL == piLen) )
	{
		return false;
	}

	int iRC = internalReceive();
	if(iRC < 0)
	{
		return false;
	}

	//receive and check data
	FrameType eFrameType = checkDatagramBuffer(ppcBuffer, piLen); //hint: memory is allocated potentially!
	while(INCOMPLETE_FRAME == eFrameType)
	{
		if(internalReceive() < 0)
		{
			break;
		}
		eFrameType = checkDatagramBuffer(ppcBuffer, piLen); //hint: memory is allocated potentially!
	}

	return (VALID_ASCII_FRAME == eFrameType) ? true : false;
}
//-------------------------------------------------------------------
int AsciiRemoteProtocol::internalReceive(void)
{
	int nBytesToReceive = SERIAL_BUFFER_SIZE - m_nBytesInDataBuffer;

	if(NULL == m_pConnection)
	{
		return -1;
	}

	//use connection to receive bytes into internal buffer
	int nBytesReceived = m_pConnection->recv((char *) &m_pcDataBuffer[m_nBytesInDataBuffer], nBytesToReceive);

	if(nBytesReceived > 0)
	{
		m_nBytesInDataBuffer += nBytesReceived;
	}

	return nBytesReceived;
}
//-------------------------------------------------------------------
AsciiRemoteProtocol::FrameType AsciiRemoteProtocol::checkDatagramBuffer(char **ppcBuffer, int *piLen)
{
	FrameType eFrameType = INVALID_FRAME;
	char *pcData = &m_pcDataBuffer[0];

	*ppcBuffer = NULL;
	*piLen = 0;

	do
	{
		//while not start of frame remove leading bytes
		for(;;)
		{
			if(m_nBytesInDataBuffer < 2) //at least STX and ETX
			{
				break;
			}
			if(0x02 == pcData[0]) //<STX> --> Start of frame
			{
				eFrameType = INCOMPLETE_FRAME; //just to indicate that start is now detected
				break;
			}

			--m_nBytesInDataBuffer;
			memmove(&pcData[0], &pcData[1], m_nBytesInDataBuffer);
		}

		//if no start detected abort
		if(INCOMPLETE_FRAME != eFrameType)
		{
			break; //loop
		}

		//look for end of frame
		int iIndex;
		for(iIndex = 1; iIndex < m_nBytesInDataBuffer; ++iIndex)
		{
			if(0x03 == pcData[iIndex]) //<ETX> --> End of frame
			{
				break;
			}
		}

		//if end of frame not found abort
		if(iIndex >= m_nBytesInDataBuffer)
		{
			eFrameType = INCOMPLETE_FRAME;
			break;
		}

		eFrameType = VALID_ASCII_FRAME;

		//return frame data
		*piLen = iIndex - 1;
		*ppcBuffer = (char *) malloc(iIndex); //returned allocated memory should only be freed by a call to 'freeBuffer(...)'!
		memcpy(*ppcBuffer, &m_pcDataBuffer[1], *piLen);
		(*ppcBuffer)[*piLen] = '\0';

		//remove found frame from internal buffer
		int nBytesToRemove = iIndex + 1;
		int nBytesToShift = m_nBytesInDataBuffer - nBytesToRemove;
		memmove(&m_pcDataBuffer[0], &m_pcDataBuffer[nBytesToRemove], nBytesToShift);
		m_nBytesInDataBuffer -= nBytesToRemove;
	}while(false); //do only one time

	return eFrameType;
}
//-------------------------------------------------------------------
void AsciiRemoteProtocol::freeBuffer(char *pcBuffer)
{
	if(NULL != pcBuffer)
	{
		free(pcBuffer);
	}
}
//-------------------------------------------------------------------
