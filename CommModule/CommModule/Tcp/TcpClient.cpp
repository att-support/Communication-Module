/*
 * TcpClient.cpp
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
#include "TcpClient.h"


TcpClient::TcpClient(const std::string& ipAddr, int port)
: Thread()
{
	char nameBuf[256];
	sprintf(nameBuf,"TCP Client(host=%s:%d)",ipAddr.c_str(),port);
	_thName = std::string(nameBuf);
	m_nameForLog = "[" + _thName + "] ";

	m_sock = INVALID_SOCKET;
	m_hostAddr.sin_family = AF_INET;
	m_hostAddr.sin_port = htons(port);
	m_hostAddr.sin_addr.s_addr = inet_addr(ipAddr.c_str());
	m_hostIpAddr = ipAddr;
	m_hostPort = port;
	m_initReceiveSize = 0;
	m_receiveSize = 0;
	m_receiver = NULL;
	m_connected = false;
	m_keepAlive = false;
	m_keepAlive_idleTime = 0;
	m_keepAlive_interval = 0;
	m_keepAlive_retryCount = 0;
	m_logOn = false;
	m_processStop = false;
}

TcpClient::~TcpClient() {
	//if(m_sock != INVALID_SOCKET){
		CloseSocket();
	//}
	//受信スレッドは基底クラスThreadBaseのデストラクタで停止される
}

int TcpClient::Connect() {
	int retVal = 0;
	int ret  = _Connect();
	if(ret != 0) {
		retVal = -1;
	}
	m_processStop = false;
	ret = StartThread();
	if(ret != 0) {
		TCPLOG(m_nameForLog + "[ERROR] Receive thread start failed.");
		return -1;
	}
	return retVal;
}

int TcpClient::ConnectOne() {
	if(_Connect() != 0) {
		return -1;
	}
	m_processStop = false;
	if(StartThread() != 0) {
		TCPLOG(m_nameForLog + "[ERROR] Receive thread start failed.");
		return -1;
	}
	return 0;
}

int TcpClient::Disconnect() {
	m_processStop = true;
	//StopThread(5);
	StopThread(300);	//接続先がいない場合のコネクトタイムアウト時間を考慮して長めに設定
	//if(m_sock != INVALID_SOCKET){
		CloseSocket();
	//}
	return 0;
}

int TcpClient::Send(const unsigned char* data, int size)
{
	if(data == NULL){
		TCPLOG(m_nameForLog + "[ERROR] Send failed. (Data is NULL)");
		return -1;
	}
	if(size <= 0){
		TCPLOG(m_nameForLog + "[ERROR] Send failed. (size=%d)",size);
		return -1;
	}
	if(!m_connected){
		TCPLOG(m_nameForLog + "[ERROR] Send failed. (not connected)");
		return -1;
	}
	Lock();
	int sendSize = send(m_sock,(const char*)data,size,0);
	if(sendSize == SOCKET_ERROR){
		TCPLOG(m_nameForLog + "[ERROR] Send failed. size=%d errno=%d (%s)",size,errno,strerror(errno));
		Unlock();
		return -1;
	}
	Unlock();
	if(m_logOn){
		TCPLOG(m_nameForLog + "Send. size=%d/%d",sendSize,size);
	}
	return 0;
}

int TcpClient::Send(const std::vector<unsigned char>& data) {
	if(data.size() == 0){
		TCPLOG(m_nameForLog + "[ERROR] Send failed. (size=0)");
		return -1;
	}
	if(!m_connected){
		TCPLOG(m_nameForLog + "[ERROR] Send failed. (not connected)");
		return -1;
	}
	Lock();
	int sendSize = send(m_sock,(const char*)&data[0],(int)data.size(),0);
	if(sendSize == SOCKET_ERROR){
		TCPLOG(m_nameForLog + "[ERROR] Send failed. size=%d errno=%d (%s)",data.size(),errno,strerror(errno));
		Unlock();
		return -1;
	}
	Unlock();
	if(m_logOn){
		TCPLOG(m_nameForLog + "Send. size=%d/%d",sendSize,data.size());
	}
	return 0;
}

int TcpClient::SetReceiver(TcpReceiveInterface* receiver, int rcvSize) {
	if(receiver == NULL){
		TCPLOG(m_nameForLog + "[ERROR] Add receiver failed. (NULL)");
		return -1;
	}
	m_receiver = receiver;
	m_initReceiveSize = rcvSize;
	m_receiveSize = rcvSize;
	return 0;
}

void TcpClient::RemoveReceiver() {
	m_receiver = NULL;
}

int TcpClient::ThreadInitProc() {
	_Connect();
	if(m_logOn){
		TCPLOG(m_nameForLog + "Receive thread start.");
	}
	return 0;
}

int TcpClient::ThreadProc()
{
	if(m_processStop) return 0;

	int ret;
	//未接続の場合は接続
	if(!m_connected){
		ret = _Connect();
		if(ret != 0){
			//Sleep(5000); //コネクトリトライ間隔
			for(int i=0;i<50;i++){
				Sleep(100); //コネクトリトライ間隔
				if(m_processStop){
					break;
				}
			}
			return 0; //受信処理は継続
		}
	}

	if (m_receiveSize == 0) {
		return 0;
	}

	//受信確認
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	fd_set fds;
	memcpy(&fds, &m_readfds, sizeof(fd_set));
	ret = select((SOCKET_TYPE)((int)m_sock+1), &fds, NULL, NULL, &tv);
	if (ret == 0) {
		//タイムアウト
		//TCPLOG(m_nameForLog + "[DEBUG] Timeout.");
		return 0;
	}
	if(ret == -1){
		TCPLOG(m_nameForLog + "[ERROR] Select failed. errno=%d (%s)",errno,strerror(errno));
		CloseSocket();
		//Sleep(5000); //コネクトリトライ間隔
		for(int i=0;i<50;i++){
			Sleep(100); //コネクトリトライ間隔
			if(m_processStop){
				break;
			}
		}
		return 0; //受信処理は継続
	}
	//受信あり
	if (FD_ISSET(m_sock, &fds)) {
		//受信
		char* buf = new char[m_receiveSize];
		int rcvSize = recv(m_sock, buf, m_receiveSize, 0);
		if(rcvSize == 0){
			TCPLOG(m_nameForLog + "Disconnected.");
			CloseSocket();
			delete[] buf;
			return 0;
		}
		else if(rcvSize < 0){
			TCPLOG(m_nameForLog + "[ERROR] Receive failed. size=%d errno=%d (%s)",m_receiveSize,errno,strerror(errno));
			CloseSocket();
			delete[] buf;
			return 0;
		}
		else{
		}
		//受信データ蓄積
		std::vector<char> data(buf, buf+ rcvSize);
		delete[] buf;
		m_receivedData.insert(m_receivedData.end(), data.begin(), data.end());
		//指定サイズとなったか
		//if (m_receivedData.size() >= (size_t)m_receiveSize) {
		if(rcvSize >= m_receiveSize){
			//通知先がある場合は通知
			if(m_receiver != NULL) {
				if(m_logOn){
					TCPLOG(m_nameForLog + "Received. size=%d",m_receivedData.size());
				}
				m_receiveSize = m_receiver->ReceivedData(&m_receivedData[0], (int)m_receivedData.size(), m_hostIpAddr);
			}
			else {
				TCPLOG(m_nameForLog + "[WARNING] Receiver is nothing.");
				m_receiveSize = m_initReceiveSize;
			}
			m_receivedData.clear();
		}
		//指定サイズ未満の場合は次回受信サイズを算出して受信待ち
		else{
			m_receiveSize -= rcvSize;
		}
	}
	return 0;
}

int TcpClient::ThreadTermProc() {
	if(m_logOn){
		TCPLOG(m_nameForLog + "Receive thread stop.");
	}
	CloseSocket();
	return 0;
}

int TcpClient::_Connect()
{
	if(m_connected){
		return 0;
	}
	int ret;
	if(m_sock == INVALID_SOCKET){
		if(m_logOn){
			TCPLOG(m_nameForLog + "Create socket.");
		}
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET){
#if defined(WIN32) || defined(WIN64)
			TCPLOG(m_nameForLog + "[ERROR] Create socket failed. (err=%d)", WSAGetLastError());
#else
			TCPLOG(m_nameForLog + "ソケット生成失敗 errno=%d (%s)",
					errno, strerror(errno));
#endif
			return -1;
		}
	}
#if defined(WIN32) || defined(WIN64)
	//TODO
#else
	//KeepAlive ON（切断検知用）
	if(m_keepAlive){
		if(m_logOn){
			TCPLOG(m_nameForLog + "キープアライブ ON(初回送信時間=%d秒, 送信間隔=%d秒, リトライ回数=%d)",
					m_keepAlive_idleTime,m_keepAlive_interval,m_keepAlive_retryCount);
		}
		int option = 1;
		ret = setsockopt( m_sock, SOL_SOCKET, SO_KEEPALIVE, (void*)&option, sizeof(option) );
		if(ret == -1){
			TCPLOG(m_nameForLog + "キープアライブ設定失敗 errno=%d (%s)", errno, strerror(errno));
			return -1;
		}
		//キープアライブを最初に投げ始めるまでの時間（秒）
		option = m_keepAlive_idleTime;
		ret = setsockopt( m_sock, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&option, sizeof(option) );
		if(ret == -1){
			TCPLOG(m_nameForLog + "キープアライブ設定失敗(初回送信時間=%d秒) errno=%d (%s)", option, errno, strerror(errno));
			return -1;
		}
		//キープアライブパケットを投げる間隔（秒）
		option = m_keepAlive_interval;
		ret = setsockopt( m_sock, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&option, sizeof(option) );
		if(ret == -1){
			TCPLOG(m_nameForLog + "キープアライブ設定失敗(送信間隔=%d秒) errno=%d (%s)", option, errno, strerror(errno));
			return -1;
		}
		//キープアライブパケットを投げリトライする回数
		option =  m_keepAlive_retryCount;
		ret = setsockopt( m_sock, IPPROTO_TCP, TCP_KEEPCNT, (void*)&option, sizeof(option) );
		if(ret == -1){
			TCPLOG(m_nameForLog + "キープアライブ設定失敗(リトライ回数=%d) errno=%d (%s)", option, errno, strerror(errno));
			return -1;
		}
	}
	//KeepAlive OFF
	else{
		if(m_logOn){
			TCPLOG(m_nameForLog + "キープアライブ OFF");
		}
		int option = 0;
		ret = setsockopt( m_sock, SOL_SOCKET, SO_KEEPALIVE, (void*)&option, sizeof(option) );
		if(ret == -1){
			TCPLOG(m_nameForLog + "キープアライブ設定失敗 errno=%d (%s)", errno, strerror(errno));
			return -1;
		}
	}
#endif

	if(m_logOn){
		TCPLOG(m_nameForLog + "Connect start.");
	}
	ret = connect(m_sock, (struct sockaddr *)&m_hostAddr, sizeof(m_hostAddr));
	if(ret == SOCKET_ERROR){
#if defined(WIN32) || defined(WIN64)
		TCPLOG(m_nameForLog + "[ERROR] Connect failed. (err=%d)", WSAGetLastError());
#else
		TCPLOG(m_nameForLog + "コネクト失敗 errno=%d (%s)", errno, strerror(errno));
#endif
		return -1;
	}
	m_connected = true;
	if(m_logOn){
		TCPLOG(m_nameForLog + "Connected.");
	}
	FD_ZERO(&m_readfds);
	FD_SET(m_sock, &m_readfds);
	m_receiveSize = m_initReceiveSize;
	m_receivedData.clear();
	//接続通知
	if(m_receiver != NULL){
		m_receiver->ConnectedToServer(m_hostIpAddr,m_hostPort);
	}
	return 0;
}

void TcpClient::CloseSocket()
{
	m_connected = false;

	if(m_sock == INVALID_SOCKET) return;

#if defined(WIN32) || defined(WIN64)
	int ret = closesocket(m_sock);
#else
	int ret = close(m_sock);
#endif
	if(ret == SOCKET_ERROR){
#if defined(WIN32) || defined(WIN64)
		TCPLOG(m_nameForLog + "[ERROR] Close socket failed. (err=%d)", WSAGetLastError());
#else
		TCPLOG(m_nameForLog + "ソケットクローズ失敗 errno=%d (%s)",
				errno, strerror(errno));
#endif
	}
	else{
		TCPLOG(m_nameForLog + "Close socket.");
	}
	m_sock = INVALID_SOCKET;
	//切断通知
	if(m_receiver != NULL){
		m_receiver->Disconnected(m_hostIpAddr, m_hostPort);
	}
}
void TcpClient::KeepAliveOn(int idleTime, int interval, int retryCount)
{
	//次回コネクトから有効
	m_keepAlive = true;
	m_keepAlive_idleTime = idleTime;
	m_keepAlive_interval = interval;
	m_keepAlive_retryCount = retryCount;
}
void TcpClient::KeepAliveOff()
{
	m_keepAlive = false;
}

const std::string& TcpClient::GetHostIpAddr()
{
	return m_hostIpAddr;
}
