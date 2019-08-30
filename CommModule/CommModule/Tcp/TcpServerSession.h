/*
 * TcpServerSession.h
 *
 *  Created on: 2015/04/01
 *      Author: Attractor T.Unno
 */

#ifndef PROCESSCOM_TCPSERVERSESSION_H_
#define PROCESSCOM_TCPSERVERSESSION_H_

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
#include "TcpServerSessionInterface.h"
#include "TcpCommon.h"

/*
 *
 */
class TcpServerSessionInterface;
class TcpServerSession: public Thread {
public:
	TcpServerSession(int serverPort, const std::string& clientIpAddr, SOCKET_TYPE sock, int rcvSize, TcpServerSessionInterface* receiver);
	virtual ~TcpServerSession();
	int StartSession();
#if defined(WIN32) || defined(WIN64)
#else
	int StartSessionWithKeepAlive(int idleTime, int interval, int retryCount);
#endif
	int StopSession();
	int Send(const std::vector<unsigned char>& data);
	int Send(const unsigned char* data, int size);
	std::string GetClientIpAddr() const;

	void LogOff(){
		m_logOn = false;
	}

protected:
	SOCKET_TYPE m_sock;
	std::string m_clientIpAddr;
	fd_set m_readfds;
	int m_hostPort;
	TcpServerSessionInterface* m_receiver;
	int m_initReceiveSize;
	int m_receiveSize;
	std::vector<unsigned char> m_receivedData;
	std::string m_nameForLog;
	bool m_logOn;
	bool m_isAlive;
	bool m_sessionEnd;
	Mutex m_lockObj;

	virtual int ThreadProc();
	virtual int ThreadInitProc();
	virtual int ThreadTermProc();

	void Lock() { m_lockObj.Lock(); }
	void Unlock() { m_lockObj.Unlock(); }

private:
	void CloseSocket(bool notify);

};

#endif /* PROCESSCOM_TCPSERVERSESSION_H_ */
