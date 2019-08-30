/*
 * TcpServerSessionInterface.h
 *
 *  Created on: 2015/04/01
 *      Author: Attractor T.Unno
 */

#ifndef PROCESSCOM_TCPSERVERSESSIONINTERFACE_H_
#define PROCESSCOM_TCPSERVERSESSIONINTERFACE_H_

#include "TcpServerSession.h"

/*
 *
 */
class TcpServerSession;
class TcpServerSessionInterface {
public:
	virtual~TcpServerSessionInterface(){}
	virtual int ServerSessionReceivedData(const unsigned char* data, int dataSize, TcpServerSession* session) = 0;
	virtual int Disconnected(const std::string& clientIpAddr, bool destroy) = 0;
};

#endif /* PROCESSCOM_TCPSERVERSESSIONINTERFACE_H_ */
