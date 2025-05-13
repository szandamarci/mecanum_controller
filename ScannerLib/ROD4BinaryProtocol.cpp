#include "ROD4BinaryProtocol.h"
#include "ROD4.h"
#include "SinCosTable.h"

//#include <malloc.h>
#include <stdlib.h>

#include <string.h>
#include <stdio.h>
#include <iostream>

#ifndef NULL
#define NULL 0
#endif


#define SCANNUMBER(x) ((x[0]<<24)+(x[2]<<16)+(x[4]<<8)+x[6])
#define BYTES2SHORT(x) ((x[0]<<8)+x[1])
#define DISTANCE(x) ((x[0] << 8) + (0xFE & x[1]))


using namespace Leuze;
using namespace Leuze::ROD4;


//-------------------------------------------------------------------
const int Leuze::ROD4::MAX_DATAGRAM_SIZE = 1620;
//-------------------------------------------------------------------
ROD4BinaryProtocol::ROD4BinaryProtocol(void)
: m_pConnection(NULL)
, m_nBytesInDatagramBuffer(0)
, m_ucChecksum('\0')
{
	m_pucDatagramBuffer = (unsigned char *) malloc(MAX_DATAGRAM_SIZE);
}
//-------------------------------------------------------------------
ROD4BinaryProtocol::~ROD4BinaryProtocol(void)
{
	free(m_pucDatagramBuffer);
	m_pucDatagramBuffer = NULL;
}
//-------------------------------------------------------------------
void ROD4BinaryProtocol::setConnection(Connection *pCon)
{
	m_pConnection = pCon;
}
//-------------------------------------------------------------------
bool ROD4BinaryProtocol::enableMeasurement(bool bEnable)
{
    //send command 'STX' 'SB+'/'SB-' 'ETX'
    const char * pcBuffer = bEnable ? "\002SB+\003" : "\002SB-\003";
	int iBufLen = strlen(pcBuffer);

	if(iBufLen != m_pConnection->send(pcBuffer, strlen(pcBuffer)))
	{
		return false;
	}

	return true;
}
//-------------------------------------------------------------------
bool ROD4BinaryProtocol::receive(Datagram *pRetVal)
{
    FrameType eFrameType = checkDatagramBuffer(*pRetVal);

    //keep receiving data and checking it until a complete and valid frame has been received
    int nTimesNothingReceived = 0;					//avoids looping forever
    while( (INCOMPLETE_FRAME == eFrameType) && (nTimesNothingReceived < 1000) )
    {
        internalReceive();

        if(0 == m_nBytesInDatagramBuffer)
        {
            //no single byte has been received
            ++nTimesNothingReceived;
        }
        else
        {
            //reset timeout counter
            nTimesNothingReceived = 0;
        }

        eFrameType = checkDatagramBuffer(*pRetVal);
    }

    if( (VALID_ALIVE_FRAME != eFrameType) &&
            (VALID_DATA_FRAME != eFrameType) &&
            (VALID_WARNING_FRAME != eFrameType) &&
            (VALID_ERROR_FRAME != eFrameType) )
    {
        return false;
    }

    return true;
}
//-------------------------------------------------------------------
void ROD4BinaryProtocol::internalReceive(void)
{
	int nBytesToReceive = MAX_DATAGRAM_SIZE - m_nBytesInDatagramBuffer;

	if( (NULL == m_pConnection) ||
		(0 == nBytesToReceive) )
	{
		//no connection or nothing to receive
		return;
	}

	//use connection to receive bytes into internal buffer
	int nBytesReceived = m_pConnection->recv((char *) &m_pucDatagramBuffer[m_nBytesInDatagramBuffer], nBytesToReceive);

	if(nBytesReceived > 0)
	{
		m_nBytesInDatagramBuffer += nBytesReceived;
	}
}
//-------------------------------------------------------------------
unsigned char ROD4BinaryProtocol::calculateChecksum(unsigned char ucByte)
{
	m_ucChecksum ^= ucByte;
	return ucByte;
}
//-------------------------------------------------------------------
int ROD4BinaryProtocol::removeFillBytes(int iStartIndex, int iLen)
{
	int nBytesRemoved = 0;
    unsigned char *pucPtr = &m_pucDatagramBuffer[iStartIndex];

    // search sequence and remove fill byte
    for(int lv = 2; lv < iLen; ++lv)
	{
        if(0xFF == pucPtr[lv])
		{
            if( (0x00 == pucPtr[lv - 2]) &&
				(0x00 == pucPtr[lv - 1]) )
			{
                // sequence '0x00 0x00 0xFF' found, now remove 0xFF by shifting the rest by one byte
				--m_nBytesInDatagramBuffer;
				memmove(&pucPtr[lv], &pucPtr[lv + 1], m_nBytesInDatagramBuffer - (iStartIndex + lv));
				++nBytesRemoved;
            }
        }
    }

    return nBytesRemoved;
}
//-------------------------------------------------------------------
FrameType ROD4BinaryProtocol::checkDatagramBuffer(Datagram &datagram)
{
	FrameType eFrameType = INVALID_FRAME;
	unsigned char *pucData = &m_pucDatagramBuffer[0];
	int lv = 0;

	do
	{
		if(0 == m_nBytesInDatagramBuffer)
		{
			//buffer is empty
			eFrameType = INCOMPLETE_FRAME;
			break;
		}

		//search for start bytes (0x00 0x00)
		for(;;)
		{
			if(m_nBytesInDatagramBuffer <= 3)
			{
				break; //only for(;;)
			}
			if( (0x00 == pucData[0]) &&
				(0x00 == pucData[1]) &&
				(0x00 != pucData[2]) && //ignore stop sequence
				(0xFF != pucData[2]))	//ignore fill bytes
			{
				eFrameType = INCOMPLETE_FRAME;	//the frame is possibly incomplete
				break;
			}

			//remove bytes before start sequence
			--m_nBytesInDatagramBuffer;
			memmove(&pucData[0], &pucData[1], m_nBytesInDatagramBuffer);
		}

		int nBytesLeft = m_nBytesInDatagramBuffer;

		if(INCOMPLETE_FRAME != eFrameType)
		{
			//did not find start
			m_nBytesInDatagramBuffer = 0; //throw all bytes away
			break; //loop
		}

		//search for stop bytes (0x00 0x00 0x00)
		int iEndOfMessageIndex = 0;
		for(int i = 0; i < m_nBytesInDatagramBuffer-3; i++)
		{
			if(m_pucDatagramBuffer[i] == 0x00 && 
				m_pucDatagramBuffer[i+1] == 0x00 &&
				m_pucDatagramBuffer[i+2] == 0x00)
			{
				//found end token
				iEndOfMessageIndex = i-1;
				break;
			}
		}
		//check for complete frame
		if(iEndOfMessageIndex == 0)
		{
			// no more data in buffer, but not end of datagram (message is incomplete)
			eFrameType = INCOMPLETE_FRAME;
			break;
		}

		//ignore start bytes
		pucData += 2;
		nBytesLeft -= 2;


		/////////////////////////////////////////////////////////////////
		// Within the header there should not be any fill bytes (0xFF) //
		//  - command:      must be one of: 0x23, 0x53 or 0x54         //
		//  - option 1:     must be at least 0x01 (= 'only Option 1')  //
		//  - option 2:     bit 7 always 1                             //
		//  - option 3:     bit 7 always 1                             //
		//  - password:     bit 7 always 1 (applies to all 8 bytes)    //
		//  - scannumber:   contains fill bytes (0xFE)                 //
		//  - resolution:   should be at least 1                       //
		//  - start angle:  must be between 1 and 529                  //
		//  - stop angle:   must be between 1 and 529                  //
		//                                                             //
		//  ==> so I don't care for them!                              //
		/////////////////////////////////////////////////////////////////

		//command (1 byte)
		if( (VALID_ALIVE_FRAME != *pucData) &&
			(VALID_DATA_FRAME != *pucData) &&
			(VALID_ERROR_FRAME != *pucData) &&
			(VALID_WARNING_FRAME != *pucData) )
		{
			//invalid message identifier
			m_nBytesInDatagramBuffer = 0; //throw all bytes away
			eFrameType = INVALID_FRAME;
			break;
		}

		//initialize checksum variable with message identifier
		//the 'checksum' is a XOR operation on the message's user data
		m_ucChecksum = *pucData;

		//set frame structure member status to message identifier
		datagram.frame.msg_identifier = *pucData++;
		nBytesLeft -= 1;


		//options (1 - 3 bytes)
		if(0x00 == *pucData)
		{
			m_nBytesInDatagramBuffer = 0; //throw all bytes away
			eFrameType = INVALID_FRAME;
			break;
		}
		datagram.frame.option1.byte = calculateChecksum(*pucData++);
		nBytesLeft -= 1;

		if(datagram.frame.option1.bit.numberOfOptions > 1)
		{
			//option byte 2 follows
			if(0x00 == *pucData)
			{
				m_nBytesInDatagramBuffer = 0; //throw all bytes away
				eFrameType = INVALID_FRAME;
				break;
			}
			datagram.frame.option2.byte = calculateChecksum(*pucData++);
			nBytesLeft -= 1;
			if(datagram.frame.option1.bit.numberOfOptions > 2)
			{
				//option byte 3 follows
				if(0x00 == *pucData)
				{
					m_nBytesInDatagramBuffer = 0; //throw all bytes away
					eFrameType = INVALID_FRAME;
					break;
				}
				datagram.frame.option3.byte = calculateChecksum(*pucData++);
				nBytesLeft -= 1;
			}
		}


		switch(datagram.frame.msg_identifier)
		{
		case VALID_ALIVE_FRAME: // alive message
			{
				//correction of checksum
				if(m_ucChecksum == 0x00)
				{
					m_ucChecksum = 0xff;
				}

				//assert checksum
				if(m_ucChecksum != *pucData)
				{
					m_nBytesInDatagramBuffer = 0; //throw all bytes away
					break;
				}

				//discard checksum and end bytes
				pucData += 4; //points now behind datagram


				//remove datagram from receive buffer
				int nBytesToRemove = pucData - m_pucDatagramBuffer;
				int nBytesToShift = m_nBytesInDatagramBuffer - nBytesToRemove;
				memmove(&m_pucDatagramBuffer[0], pucData, nBytesToShift);
				m_nBytesInDatagramBuffer -= nBytesToRemove;

				eFrameType = VALID_ALIVE_FRAME;
				break;
			}
		case VALID_DATA_FRAME: // measuring
			{
				// scannumber
				// assume 0xFEs between scan number octets
				if( (0xFE != pucData[1]) ||
					(0xFE != pucData[3]) ||
					(0xFE != pucData[5]) ||
					(0xFE != pucData[7]) )
				{
					m_nBytesInDatagramBuffer = 0; //throw all bytes away
					eFrameType = INVALID_FRAME;
					break;
				}
				calculateChecksum(pucData[0]);
				calculateChecksum(pucData[2]);
				calculateChecksum(pucData[4]);
				calculateChecksum(pucData[6]);
				datagram.header.scanNumber = SCANNUMBER(pucData);
				pucData += 8; //4 bytes scannumber, 4 bytes fillbytes
				nBytesLeft -= 8;


				// output resolution
				if(0x00 == *pucData)
				{
					m_nBytesInDatagramBuffer = 0; //throw all bytes away
					eFrameType = INVALID_FRAME;
					break;
				}
				datagram.header.resolution = calculateChecksum(*pucData++);
				nBytesLeft -= 1;


				// start angle 
				datagram.header.start = BYTES2SHORT(pucData);
				if( (datagram.header.start < MIN_SEGMENT) ||
					(MAX_SEGMENT < datagram.header.start) )
				{
					m_nBytesInDatagramBuffer = 0; //throw all bytes away
					eFrameType = INVALID_FRAME;
					break;
				}
				calculateChecksum(*pucData++);
				calculateChecksum(*pucData++);
				nBytesLeft -= 2;


				//stop angle
				datagram.header.stop = BYTES2SHORT(pucData);
				if( (datagram.header.stop < datagram.header.start) ||
					(MAX_SEGMENT < datagram.header.stop) )
				{
					m_nBytesInDatagramBuffer = 0; //throw all bytes away
					eFrameType = INVALID_FRAME;
					break;
				}
				calculateChecksum(*pucData++);
				calculateChecksum(*pucData++);
				nBytesLeft -= 2;

				//start of measure values
				unsigned char *pucMeasureData = pucData;

				//calculate checksum of the rest (the measure data)
				for(lv = 1; lv < iEndOfMessageIndex; ++lv) // without stop bytes
				{
					calculateChecksum(*pucData++);
					if( (0x00 == pucData[1]) &&
						(0x00 == pucData[2]) &&
						(0x00 == pucData[3]) )
					{
						//pucData points to the checksum (end of a frame)
						break;
					}
				}

				//correction of checksum
				if(m_ucChecksum == 0x00)
				{
					m_ucChecksum = 0xff;
				}

				//assert checksum
				if(m_ucChecksum != *pucData)
				{
					m_nBytesInDatagramBuffer = 0; //throw all bytes away
					eFrameType = INVALID_FRAME;
					break;
				}

				datagram.frame.checksum = m_ucChecksum;

				//remove fill bytes (possible in measure values)
				int iStartIndex = pucMeasureData - m_pucDatagramBuffer;
				int iLen = iEndOfMessageIndex - iStartIndex;
				int nRemovedBytes = removeFillBytes(iStartIndex, iLen);
				m_nBytesInDatagramBuffer -= nRemovedBytes;

				//correction by number of removed bytes
				pucData -= nRemovedBytes;

				int nMeasureBytes = pucData - pucMeasureData;
				if(nMeasureBytes % 2 != 0)
				{
					//distances are transmitted in words, so the number of bytes must be even always
					m_nBytesInDatagramBuffer = 0; //throw all bytes away
					eFrameType = INVALID_FRAME;
					break;
				}
				int nMeasureValues = nMeasureBytes / 2;						//1..529
				datagram.distanceCount = nMeasureValues;
				datagram.distances = (unsigned short *) malloc(nMeasureValues * sizeof(unsigned short));
				for(lv = 0; lv < nMeasureValues; ++lv)
				{
					datagram.distances[lv] = DISTANCE(pucMeasureData);
					pucMeasureData += 2;
				}

				//discard checksum and end bytes
				pucMeasureData += 4; //points now behind datagram


				//remove processed datagram from receive buffer
				int nBytesToRemove = (pucMeasureData - m_pucDatagramBuffer) - nRemovedBytes;
				int nBytesToShift = m_nBytesInDatagramBuffer - nBytesToRemove;
				memmove(&m_pucDatagramBuffer[0], pucMeasureData, nBytesToShift);
				m_nBytesInDatagramBuffer -= nBytesToRemove;

				eFrameType = VALID_DATA_FRAME;
				break;
			}
		case VALID_ERROR_FRAME:
			{
				datagram.distanceCount = 1; //one error
				datagram.distances = (unsigned short *) malloc(2 * sizeof(unsigned short)); //error number and location
				datagram.distances[0] = BYTES2SHORT(pucData); //error number
				calculateChecksum(*pucData++);
				calculateChecksum(*pucData++);
				//error parameter is ignored
				calculateChecksum(*pucData++);
				calculateChecksum(*pucData++);
				datagram.distances[1] = BYTES2SHORT(pucData); //error location
				calculateChecksum(*pucData++);
				calculateChecksum(*pucData++);

				//correction of checksum
				if(m_ucChecksum == 0x00)
				{
					m_ucChecksum = 0xff;
				}

				//assert checksum
				if(m_ucChecksum != *pucData)
				{
					m_nBytesInDatagramBuffer = 0; //throw all bytes away
					break;
				}

				//discard checksum and end bytes
				pucData += 4; //points now behind datagram


				//remove processed datagram from receive buffer
				int nBytesToRemove = pucData - m_pucDatagramBuffer;
				int nBytesToShift = m_nBytesInDatagramBuffer - nBytesToRemove;
				memmove(&m_pucDatagramBuffer[0], pucData, nBytesToShift);
				m_nBytesInDatagramBuffer -= nBytesToRemove;

				eFrameType = VALID_ERROR_FRAME;
				break;
			}
		case VALID_WARNING_FRAME:
			{
				datagram.distanceCount = 1; //one warning
				datagram.distances = (unsigned short *) malloc(2 * sizeof(unsigned short)); //warning number and location
				datagram.distances[0] = BYTES2SHORT(pucData); //warning number
				calculateChecksum(*pucData++);
				calculateChecksum(*pucData++);
				//warning parameter is ignored
				calculateChecksum(*pucData++);
				calculateChecksum(*pucData++);
				datagram.distances[1] = BYTES2SHORT(pucData); //warning location
				calculateChecksum(*pucData++);
				calculateChecksum(*pucData++);


				//correction of checksum
				if(m_ucChecksum == 0x00)
				{
					m_ucChecksum = 0xff;
				}

				//assert checksum
				if(m_ucChecksum != *pucData)
				{
					m_nBytesInDatagramBuffer = 0; //throw all bytes away
					break;
				}

				//discard checksum and end bytes
				pucData += 4; //points now behind datagram


				//remove processed datagram from receive buffer
				int nBytesToRemove = pucData - m_pucDatagramBuffer;
				int nBytesToShift = m_nBytesInDatagramBuffer - nBytesToRemove;
				memmove(&m_pucDatagramBuffer[0], pucData, nBytesToShift);
				m_nBytesInDatagramBuffer -= nBytesToRemove;

				eFrameType = VALID_WARNING_FRAME;
				break;
			}
		}

	}while(false);

	return eFrameType;
}
//-------------------------------------------------------------------
