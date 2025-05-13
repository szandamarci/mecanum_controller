//#include "SerialCom.h"


//using namespace Leuze;


////-------------------------------------------------------------------
//SerialCom::SerialCom(void)
//: m_hComPort(INVALID_HANDLE_VALUE)
//, m_zComPort(NULL)
//, m_eBaudrate(SERIAL_BAUD_115200)
//{
//}
////-------------------------------------------------------------------
//SerialCom::~SerialCom(void)
//{
//	if(INVALID_HANDLE_VALUE != m_hComPort)
//	{
//		close();
//	}
//	if(NULL != m_zComPort)
//	{
//		free(m_zComPort);
//	}
//}
////-------------------------------------------------------------------
//void SerialCom::setComPort(char *zComPort)
//{
//	if(NULL == zComPort)
//	{
//		return;
//	}

//	if(NULL != m_zComPort)
//	{
//		free(m_zComPort);
//	}
//	m_zComPort = (char *) malloc(strlen(zComPort) + 1);
//	strcpy_s(m_zComPort, strlen(zComPort) + 1, zComPort);
//}
////-------------------------------------------------------------------
//void SerialCom::setBaudrate(EBaudrate eBaudrate)
//{
//	m_eBaudrate = eBaudrate;
//}
////-------------------------------------------------------------------
//void SerialCom::connect(void)
//{
//	if(INVALID_HANDLE_VALUE != m_hComPort)
//	{
//		close();
//	}

//	m_hComPort = CreateFile(m_zComPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

//	if(INVALID_HANDLE_VALUE != m_hComPort)
//	{
//		memset(&m_sDCB, 0, sizeof(DCB));
//		m_sDCB.DCBlength = sizeof(DCB);
//		m_sDCB.fBinary = TRUE;
//		m_sDCB.BaudRate = m_eBaudrate;
//		m_sDCB.ByteSize = 8;
//		m_sDCB.Parity = NOPARITY;
//		m_sDCB.StopBits = ONESTOPBIT;

//		if(!SetCommState(m_hComPort, &m_sDCB))
//		{
//			close();
//			return;
//		}

//		//memset(&m_sCommTimeouts, 0, sizeof(COMMTIMEOUTS));
//		m_sCommTimeouts.ReadIntervalTimeout = MAXDWORD;
//		m_sCommTimeouts.ReadTotalTimeoutMultiplier = 0;
//		m_sCommTimeouts.ReadTotalTimeoutConstant = 0;
//		m_sCommTimeouts.WriteTotalTimeoutMultiplier = 0;
//		m_sCommTimeouts.WriteTotalTimeoutConstant = 0;

//		if(!SetCommTimeouts(m_hComPort, &m_sCommTimeouts))
//		{
//			close();
//			return;
//		}

//		DWORD dwEventMask = EV_RXCHAR;
//		if(!SetCommMask(m_hComPort, dwEventMask))
//		{
//			close();
//			return;
//		}

//		if(!PurgeComm(m_hComPort, PURGE_TXCLEAR | PURGE_RXCLEAR))
//		{
//			close();
//			return;
//		}
//	}
//}
////-------------------------------------------------------------------
//bool SerialCom::isConnected(void)
//{
//	return (INVALID_HANDLE_VALUE != m_hComPort) ? true : false;
//}
////-------------------------------------------------------------------
//void SerialCom::close(void)
//{
//	if(INVALID_HANDLE_VALUE != m_hComPort)
//	{
//		CloseHandle(m_hComPort);
//		m_hComPort = INVALID_HANDLE_VALUE;
//	}
//}
////-------------------------------------------------------------------
//int SerialCom::send(const char *pcBuffer, int iLen)
//{
//	int iRetVal = -1;

//	OVERLAPPED sOverlapped;
//	memset(&sOverlapped, 0, sizeof(OVERLAPPED));
//	sOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

//	do
//	{
//		if(INVALID_HANDLE_VALUE == m_hComPort)
//		{
//			break;
//		}

//		DWORD dwBytesSend = 0;
//		if(TRUE == WriteFile(m_hComPort, pcBuffer, iLen, &dwBytesSend, &sOverlapped))
//		{
//			iRetVal = dwBytesSend;
//			break;
//		}

//		DWORD dwErr = GetLastError();
//		if(ERROR_IO_PENDING == dwErr)
//		{
//			DWORD dwRC = WaitForSingleObject(sOverlapped.hEvent, 1000);
//			if(WAIT_OBJECT_0 == dwRC)
//			{
//				if(FALSE == GetOverlappedResult(m_hComPort, &sOverlapped, &dwBytesSend, FALSE))
//				{
//					break;
//				}
//				iRetVal = dwBytesSend;
//			}
//			else if(WAIT_TIMEOUT == dwRC)
//			{
//				iRetVal = 0;
//			}
//		}
//	}while(false);

//	CloseHandle(sOverlapped.hEvent);

//	return iRetVal;
//}
////-------------------------------------------------------------------
//int SerialCom::recv(char *pcBuffer, int iLen)
//{
//	int iRetVal = -1;

//	OVERLAPPED sOverlapped;
//	memset(&sOverlapped, 0, sizeof(OVERLAPPED));
//	sOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

//	do
//	{
//		if(INVALID_HANDLE_VALUE == m_hComPort)
//		{
//			break;
//		}

//		DWORD dwBytesReceived = 0;
//		if(TRUE == ReadFile(m_hComPort, pcBuffer, iLen, &dwBytesReceived, &sOverlapped))
//		{
//			iRetVal = dwBytesReceived;
//			break;
//		}

//		DWORD dwErr = GetLastError();
//		if(ERROR_IO_PENDING == dwErr)
//		{
//			DWORD dwRC = WaitForSingleObject(sOverlapped.hEvent, 1000);
//			if(WAIT_OBJECT_0 == dwRC)
//			{
//				if(FALSE == GetOverlappedResult(m_hComPort, &sOverlapped, &dwBytesReceived, FALSE))
//				{
//					break;
//				}
//				iRetVal = dwBytesReceived;
//			}
//			else if(WAIT_TIMEOUT == dwRC)
//			{
//				iRetVal = 0;
//			}
//		}
//	}while(false);

//	CloseHandle(sOverlapped.hEvent);

//	return iRetVal;
//}
////-------------------------------------------------------------------
