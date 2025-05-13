//#ifndef LEUZE_SERIALCOM_H
//#define LEUZE_SERIALCOM_H


//#include "Connection.h"
//#include <windows.h>


//namespace Leuze
//{
//	/** \enum EBaudrate
//	Possible baudrates.
//	*/
//	enum EBaudrate
//	{
//		SERIAL_BAUD_9600    =    9600,	//all ROD4
//		SERIAL_BAUD_19200   =   19200,	//all ROD4
//		SERIAL_BAUD_38400   =   38400,	//all ROD4
//		SERIAL_BAUD_57600   =   57600,	//all ROD4
//		SERIAL_BAUD_109700  =  109700,	//ROD4, ROD4-2x
//		SERIAL_BAUD_115200  =  115200,	//ROD4-3x
//		SERIAL_BAUD_343500  =  343500,	//ROD4, ROD4-2x
//		SERIAL_BAUD_384000  =  384000,	//ROD4-3x
//		SERIAL_BAUD_687500  =  687500,	//ROD4-3x
//		SERIAL_BAUD_768000  =  768000,	//ROD4, ROD4-2x
//		SERIAL_BAUD_1031000 = 1031000	//ROD4-3x with Firmware > v8.3
//	};

//	/** \class SerialCom
//	\brief Implementation of a serial connection.

//	SerialCom provides an interface to communicate
//	with a ROD4/ROD4plus scanner via a serial connection.
//	*/
//	class SerialCom : public Connection
//	{
//	private:
//		HANDLE     m_hComPort;
//		char      *m_zComPort;
//		EBaudrate  m_eBaudrate;

//		DCB          m_sDCB;
//		COMMTIMEOUTS m_sCommTimeouts;

//	public:
//		/**
//		C'tor - Sets default parameters.
//		*/
//		SerialCom(void);
//		/**
//		D'tor - Frees resources.
//		*/
//		virtual ~SerialCom(void);
//		/**
//		Sets the com port (e.g. COM1)
//		*/
//		void setComPort(char *zComPort);
//		/**
//		Sets the baudrate of the local com port.
//		*/
//		void setBaudrate(EBaudrate eBaudrate);
//		/**
//		Opens the com port.
//		*/
//		virtual void connect(void);
//		/**
//		Indicates whether com port is open or not.
//		\return true if com port is open (otherwise false).
//		*/
//		virtual bool isConnected(void);
//		/**
//		Closes the com port.
//		*/
//		virtual void close(void);
//		/**
//		Sends bytes via the com port.
//		@param pcBuffer Buffer (bytes) to be send.
//		@param iLen Size of the send buffer.
//		\return Amount of bytes send.
//		*/
//		virtual int send(const char *pcBuffer, int iLen);
//		/**
//		Receives bytes via the com port.
//		@param pcBuffer Buffer (bytes) to store the received data.
//		@param iLen Size of the receive buffer.
//		\return Amount of bytes received.
//		*/
//		virtual int recv(char *pcBuffer, int iLen);

//	};
//}


//#endif //LEUZE_SERIALCOM_H
