/*
 * TcpReceiveInterface.h
 *
 *  Created on: 2015/04/01
 *      Author: Attractor T.Unno
 */

#ifndef PROCESSCOM_TCPRECEIVEINTERFACE_H_
#define PROCESSCOM_TCPRECEIVEINTERFACE_H_

#include <vector>

/*
 *
 */
class TcpReceiveInterface {
public:
	virtual~TcpReceiveInterface(){}
	virtual int ReceivedData(const unsigned char* data, int dataSize, const std::string& ipAddr) = 0;
	virtual void Connected(const std::string& ipAddr){};
	virtual void ConnectedToServer(const std::string& ipAddr, int port){};
	virtual void Disconnected(const std::string& ipAddr, int port){};
};


#endif /* PROCESSCOM_TCPRECEIVEINTERFACE_H_ */
