/*
 * TcpServer.h
 *
 *  Created on: 2015/04/01
 *      Author: Attractor T.Unno
 */

#ifndef PROCESSCOM_TCPSERVER_H_
#define PROCESSCOM_TCPSERVER_H_

#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#if defined(WIN32) || defined(WIN64)
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <Util/Thread.h>
#include <Util/Mutex.h>
#include "TcpServerSession.h"
#include "TcpServerSessionInterface.h"
#include "TcpReceiveInterface.h"
#include "TcpCommon.h"

/*
 *
 */
class TcpServer: public Thread, TcpServerSessionInterface {
public:
	TcpServer(int port);
	virtual ~TcpServer();

	int ServerStart();
	int ServerStop();
	int SetReceiver(TcpReceiveInterface* receiver, int rcvSize);
	void RemoveReceiver();
	int Send(const std::vector<unsigned char>& data, const std::string& ipAddr="");
	int Send(const unsigned char* data, int size, const std::string& ipAddr="");
	void KeepAliveOn(int idleTime, int interval, int retryCount);
	void KeepAliveOff();
	int GetSessionCount();
	int StopSession();

	int getHostPort() const {
		return m_hostPort;
	}

	void LogOff()	{
		m_logOn = false;
	}

protected:
	SOCKET_TYPE m_sock;
	struct sockaddr_in m_hostAddr;
	fd_set m_readfds;
	int m_hostPort;
	TcpReceiveInterface* m_receiver;
	int m_initReceiveSize;
	int m_receiveSize;
	std::vector<char> m_receivedData;
	std::string m_nameForLog;
	std::map<std::string,TcpServerSession*> m_sessionList;
	bool m_keepAlive;
	int m_keepAlive_idleTime;
	int m_keepAlive_interval;
	int m_keepAlive_retryCount;
	bool m_logOn;
	bool m_processStop;
	Mutex m_lockObj;
	std::vector<TcpServerSession*> m_delSessions;

	virtual int ThreadProc();
	virtual int ThreadInitProc();
	virtual int ThreadTermProc();

	virtual int ServerSessionReceivedData(const unsigned char* data, int dataSize, TcpServerSession* session);
	//virtual int Disconnected(TcpServerSession* session);
	virtual int Disconnected(const std::string& clientIp, bool destroy);

	int InitServer();
	void Lock() { m_lockObj.Lock(); }
	void Unlock() { m_lockObj.Unlock(); }

private:
	void CloseSocket();
};

#endif /* PROCESSCOM_TCPSERVER_H_ */
