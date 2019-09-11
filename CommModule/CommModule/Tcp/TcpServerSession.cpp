/*
 * TcpServerSession.cpp
 *
 *  Created on: 2015/04/01
 *      Author: Attractor T.Unno
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#if defined(WIN32) || defined(WIN64)
#else
#include <unistd.h>
#endif
#include "TcpLogger.h"
#include "TcpServerSession.h"

TcpServerSession::TcpServerSession(int serverPort, const std::string& clientIpAddr, SOCKET_TYPE sock, int rcvSize, TcpServerSessionInterface* receiver)
: Thread()
{
	char nameBuf[256];
	sprintf(nameBuf,"TCP Server Session(port=%d client=%s)",serverPort,clientIpAddr.c_str());
	_thName = std::string(nameBuf);
	m_nameForLog = "[" + _thName + "] ";

	m_sock = sock;
	FD_ZERO(&m_readfds);
	FD_SET(m_sock, &m_readfds);

	m_clientIpAddr = clientIpAddr;
	m_hostPort = serverPort;
	m_initReceiveSize = rcvSize;
	m_receiveSize = rcvSize;
	m_receiver = receiver;
	m_logOn = false;
	m_isAlive = false;
	m_sessionEnd = false;
}

TcpServerSession::~TcpServerSession() {
	//StopSession();
//	if(!m_isAlive){
//		return;
//	}
	if(m_isAlive){
		m_isAlive = false;
		CloseSocket(false);
	}
//	m_isAlive = false;
//	CloseSocket(false);
	//デストラクタでは確実にスレッドの停止を待つ
	//(ただし自身で終了する場合以外)
	if(IsRunning() && !m_sessionEnd){
		int ret = StopThread(3);
		if(ret != 0){
			TCPLOG(m_nameForLog + "[ERROR] Stop receive thread failed.");
		}
	}
}

int TcpServerSession::StartSession()
{
	if(m_receiver == NULL) {
		TCPLOG(m_nameForLog + "[WARNING] Receiver is nothing.");
		return -1;
	}
	int ret = StartThread();
	if(ret != 0) {
		TCPLOG(m_nameForLog + "[ERROR] Start receive thread failed.");
		return -1;
	}
	m_isAlive = true;
	return 0;
}

#if defined(WIN32) || defined(WIN64)
//TODO
#else
int TcpServerSession::StartSessionWithKeepAlive(int idleTime, int interval, int retryCount)
{
	int ret;
	if(m_logOn){
		TCPLOG("キープアライブ ON(初回送信時間=%d秒, 送信間隔=%d秒, リトライ回数=%d)",
				idleTime,interval,retryCount);
	}
	int option = 1;
	ret = setsockopt( m_sock, SOL_SOCKET, SO_KEEPALIVE, (void*)&option, sizeof(option) );
	if(ret == -1){
		TCPLOG("キープアライブ設定失敗 errno=%d (%s)", errno, strerror(errno));
		return -1;
	}
	//キープアライブを最初に投げ始めるまでの時間（秒）
	ret = setsockopt( m_sock, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&idleTime, sizeof(idleTime) );
	if(ret == -1){
		TCPLOG("キープアライブ設定失敗(初回送信時間=%d秒) errno=%d (%s)", option, errno, strerror(errno));
		return -1;
	}
	//キープアライブパケットを投げる間隔（秒）
	ret = setsockopt( m_sock, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&interval, sizeof(interval) );
	if(ret == -1){
		TCPLOG("キープアライブ設定失敗(送信間隔=%d秒) errno=%d (%s)", option, errno, strerror(errno));
		return -1;
	}
	//キープアライブパケットを投げリトライする回数
	ret = setsockopt( m_sock, IPPROTO_TCP, TCP_KEEPCNT, (void*)&retryCount, sizeof(retryCount) );
	if(ret == -1){
		TCPLOG("キープアライブ設定失敗(リトライ回数=%d) errno=%d (%s)", option, errno, strerror(errno));
		return -1;
	}
	ret = StartThread();
	if(ret != 0) {
		TCPLOG("受信スレッド開始失敗");
		return -1;
	}
	m_isAlive = true;
	return 0;
}
#endif

//自スレッドからの呼び出し禁止（必ずスレッド停止待ちでタイムアウトする）
//Disconnected()コールバック先からの呼び出しも同じなので注意
int TcpServerSession::StopSession()
{
	if(!m_isAlive){
		return 0;
	}
	m_isAlive = false;
	CloseSocket(false);
	if(IsRunning()){
		int ret = StopThread(3);
		if(ret != 0){
			TCPLOG(m_nameForLog + "[ERROR] Stop receive thread failed.");
		}
	}
	return 0;
}

int TcpServerSession::Send(const unsigned char* data, int size) {
	if(data == NULL){
		TCPLOG(m_nameForLog + "[ERROR] Send failed. (Data is NULL)");
		return -1;
	}
	if(size <= 0){
		TCPLOG(m_nameForLog + "[ERROR] Send failed. (size=%d)",size);
		return -1;
	}

	Lock();
	int sendSize = send(m_sock,(const char*)data,size,0);
	if(sendSize == SOCKET_ERROR){
		TCPLOG(m_nameForLog + "[ERROR] Send failed. size=%d errno=%d (%s)",size,errno,strerror(errno));
		Unlock();
		CloseSocket(true);
		return -1;
	}
	Unlock();
	if(m_logOn){
		TCPLOG(m_nameForLog + "Send. size=%d/%d",sendSize,size);
	}
	return 0;
}

int TcpServerSession::Send(const std::vector<unsigned char>& data) {
	if(data.size() == 0){
		TCPLOG(m_nameForLog + "[ERROR] Send failed. (size=0)");
		return -1;
	}
	Lock();
	int sendSize = send(m_sock,(const char*)&data[0],(int)data.size(),0);
	if(sendSize == SOCKET_ERROR){
		TCPLOG(m_nameForLog + "[ERROR] Send failed. size=%d errno=%d (%s)",data.size(),errno,strerror(errno));
		Unlock();
		CloseSocket(true);
		return -1;
	}
	Unlock();
	if(m_logOn){
		TCPLOG(m_nameForLog + "Send. size=%d/%d",sendSize,data.size());
	}
	return 0;
}

int TcpServerSession::ThreadProc() {

	if (m_receiveSize == 0) {
		return 0;
	}
	int ret;
	//受信確認
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	fd_set fds;
	memcpy(&fds, &m_readfds, sizeof(fd_set));
	ret = select((SOCKET_TYPE)((int)m_sock+1), &fds, NULL, NULL, &tv);
	if (ret == 0) {
		//タイムアウト
		return 0;
	}
	if(ret == -1){
		m_sessionEnd = true;
		TCPLOG(m_nameForLog + "[ERROR] Select failed. errno=%d (%s)",errno,strerror(errno));
		return -1; //スレッド終了
	}

	//受信あり
	if (FD_ISSET(m_sock, &fds)) {
		//受信
		char* buf = new char[m_receiveSize];
		int rcvSize = recv(m_sock, buf, m_receiveSize, 0);
		if(rcvSize == 0){
			m_sessionEnd = true;
			TCPLOG(m_nameForLog + "Disconnected.");
			delete[] buf;
			return -1; //スレッド終了
		}
		else if(rcvSize < 0){
			m_sessionEnd = true;
			TCPLOG(m_nameForLog + "[ERROR] Receive failed. size=%d errno=%d (%s)",m_receiveSize,errno,strerror(errno));
			delete[] buf;
			return -1; //スレッド終了
		}
		else{
		}
		//受信データ蓄積
		//std::vector<char> data(buf, buf+ m_receiveSize);
		std::vector<char> data(buf, buf + rcvSize);
		m_receivedData.insert(m_receivedData.end(), data.begin(), data.end());
		//指定サイズとなったか
		//if(m_receivedData.size() >= (size_t)m_receiveSize){
		if (rcvSize >= m_receiveSize) {
				//受信通知
			if(m_logOn){
				TCPLOG(m_nameForLog + "Reveived. size=%d",m_receivedData.size());
			}
			m_receiveSize = m_receiver->ServerSessionReceivedData(&m_receivedData[0], (int)m_receivedData.size(), this);
			m_receivedData.clear();

		}
		//指定サイズ未満の場合は次回受信サイズを算出して受信待ち
		else{
			m_receiveSize -= rcvSize;
		}
		delete[] buf;
	}
	return 0;
}

int TcpServerSession::ThreadInitProc() {
	if(m_logOn){
		TCPLOG(m_nameForLog + "Start receive thread.");
	}
	return 0;
}

int TcpServerSession::ThreadTermProc() {
	m_isAlive = false;
	if(m_logOn){
		TCPLOG(m_nameForLog + "Stop receive thread.");
	}
	CloseSocket(true);
	return 0;
}

std::string TcpServerSession::GetClientIpAddr() const
{
	return m_clientIpAddr;
}

void TcpServerSession::CloseSocket(bool notify)
{
	Lock();
	if(m_sock == INVALID_SOCKET){
		Unlock();
		return;
	}
#if defined(WIN32) || defined(WIN64)
	int ret = closesocket(m_sock);
#else
	int ret = close(m_sock);
#endif
	if(ret == -1){
#if defined(WIN32) || defined(WIN64)
		TCPLOG(m_nameForLog + "[ERROR] Close socket failed. (err=%d)", WSAGetLastError());
#else
		TCPLOG("ソケットクローズ失敗 errno=%d (%s)", errno, strerror(errno));
#endif
	}
	else{
		TCPLOG(m_nameForLog + "Close socket.");
	}
	m_sock = INVALID_SOCKET;
	Unlock();
	//切断通知
	if((m_receiver != NULL) && notify){
		m_receiver->Disconnected(m_clientIpAddr, notify);
	}
}

