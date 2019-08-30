/*
 * TcpClient.h
 *
 *  Created on: 2015/04/01
 *      Author: Attractor T.Unno
 */

#ifndef PROCESSCOM_TCPCLIENT_H_
#define PROCESSCOM_TCPCLIENT_H_

#include <string>
#include <vector>
#include <sys/types.h>

#if defined(WIN32) || defined(WIN64)
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#endif
#include <Util/Thread.h>
#include <Util/Mutex.h>
#include "TcpReceiveInterface.h"
#include "TcpCommon.h"

/*
 *
 */
class TcpClient: public Thread {
public:
	TcpClient(const std::string& ipAddr, int port);
	virtual ~TcpClient();
	int Connect();
	int ConnectOne();
	int Disconnect();
	int Send(const std::vector<unsigned char>& data);
	int Send(const unsigned char* data, int size);
	int SetReceiver(TcpReceiveInterface* receiver, int rcvSize);
	void KeepAliveOn(int idleTime, int interval, int retryCount);
	void KeepAliveOff();

	const std::string& GetHostIpAddr();

	const std::string& getHostIpAddr() const {
		return m_hostIpAddr;
	}

	int getHostPort() const {
		return m_hostPort;
	}

	void LogOff(){
		m_logOn = false;
 	}

protected:
	SOCKET_TYPE m_sock;
	struct sockaddr_in m_hostAddr;
	std::string m_hostIpAddr;
	fd_set m_readfds;
	int m_hostPort;
	TcpReceiveInterface* m_receiver;
	int m_initReceiveSize;
	int m_receiveSize;
	std::vector<unsigned char> m_receivedData;
	std::string m_nameForLog;
	bool m_connected;
	bool m_keepAlive;
	int m_keepAlive_idleTime;
	int m_keepAlive_interval;
	int m_keepAlive_retryCount;
	bool m_logOn;
	bool m_processStop;
	Mutex m_lockObj;

	virtual int ThreadProc();
	virtual int ThreadInitProc();
	virtual int ThreadTermProc();

	void Lock() { m_lockObj.Lock(); }
	void Unlock() { m_lockObj.Unlock(); }

private:
	int _Connect();
	void CloseSocket();

};

#endif /* PROCESSCOM_TCPCLIENT_H_ */
