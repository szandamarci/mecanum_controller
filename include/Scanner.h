/*
 * Scanner.h
 *
 *  Created on: Nov 14, 2014
 *      Author: miki
 */

#ifndef SRC_SCANNER_H_
#define SRC_SCANNER_H_

#include "TcpSocket.h"
#include "ROD4BinaryProtocol.h"
#include "SinCosTable.h"

using namespace Leuze;
using namespace Leuze::ROD4;


#define SCANNER_IP			"192.168.60.3"
#define SCANNER_PORT		9008

#define MAX_SEGMENT_NUM		529


class Scanner {
	TcpSocket* pConnection;
	ROD4BinaryProtocol* s_pBinaryProtocol;

	bool measurementEnabled;
public:
	Scanner();
	~Scanner();

	bool connect(void);
	void disconnect(void);
	bool enableMeasurement(bool enable);
	bool receiveDatagram(Datagram* pDatagram);
};

#endif /* SRC_SCANNER_H_ */
