/*
 * TcpServer.cpp
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
#include "TcpServer.h"


TcpServer::TcpServer(int port)
: Thread()
{
	char nameBuf[256];
	sprintf(nameBuf,"TCP Server(port:%d)",port);
	_thName = std::string(nameBuf);
	m_nameForLog = "[" + _thName + "] ";

	m_sock = INVALID_SOCKET;
	m_hostAddr.sin_family = AF_INET;
	m_hostAddr.sin_port = htons(port);
	m_hostAddr.sin_addr.s_addr = INADDR_ANY;
	m_hostPort = port;
	m_initReceiveSize = 0;
	m_receiveSize = 0;
	m_receiver = NULL;
	m_keepAlive = false;
	m_keepAlive_idleTime = 0;
	m_keepAlive_interval = 0;
	m_keepAlive_retryCount = 0;
	m_logOn = false;
	m_processStop = false;
}

TcpServer::~TcpServer() {
	ServerStop();
}

int TcpServer::ServerStart() {
	if(IsRunning()){
		if(m_logOn){
			TCPLOG(m_nameForLog+"Already started.");
		}
		return 0;
	}

	if (InitServer() != 0) {
		return -1;
	}

	m_processStop = false;
	int ret = StartThread();
	if(ret == 0){
		if(m_logOn){
			TCPLOG(m_nameForLog + "Start server.");
		}
	}
	else{
		if(m_logOn){
			TCPLOG(m_nameForLog + "[ERROR] Start server failed.");
		}
		return -1;
	}
	return 0;
}

int TcpServer::ServerStop() {
	if(m_processStop == true){
		WaitThreadEnd();
		return 0;
	}
	m_processStop = true;
	std::vector<TcpServerSession*> _sessions;
	Lock();
	for (std::map<std::string, TcpServerSession*>::iterator itr = m_sessionList.begin();
		itr != m_sessionList.end(); itr++)
	{
		_sessions.push_back((*itr).second);
	}
	Unlock();
	for (std::vector<TcpServerSession*>::iterator itr = _sessions.begin();
		itr != _sessions.end(); itr++)
	{
		(*itr)->StopSession();
	}
	//TCPLOG(m_nameForLog + "[DEBUG] Lock ServerStop");
	Lock();
	for (std::vector<TcpServerSession*>::iterator itr = m_delSessions.begin(); itr != m_delSessions.end(); itr++) {
		delete *itr;
	}
	m_delSessions.clear();
	for(std::map<std::string,TcpServerSession*>::iterator itr = m_sessionList.begin();
			itr != m_sessionList.end(); itr++)
	{
		((*itr).second)->StopSession();
		delete ((*itr).second);
	}
	m_sessionList.clear();
	Unlock();
	//TCPLOG(m_nameForLog + "[DEBUG] Unlock ServerStop");
	if(m_sock != INVALID_SOCKET){
		CloseSocket();
	}
	if(IsRunning()){
		StopThread(3);
	}
	return 0;
}

int TcpServer::SetReceiver(TcpReceiveInterface* receiver, int rcvSize) {
	if(m_receiver != NULL){
		if(m_logOn){
			TCPLOG(m_nameForLog + "Overwrite receiver.");
		}
		//正常扱いとする
	}
	m_receiver = receiver;
	m_initReceiveSize = rcvSize;
	m_receiveSize = rcvSize;
	return 0;
}

void TcpServer::RemoveReceiver() {
	m_receiver = NULL;
}

int TcpServer::Send(const std::vector<unsigned char>& data, const std::string& ipAddr) {
	int ret = -1;
	//全接続先に送信
	if(ipAddr == ""){
		Lock();
		bool isSent = false;
		for(std::map<std::string,TcpServerSession*>::iterator itr = m_sessionList.begin();
				itr != m_sessionList.end(); itr++)
		{
			if (((*itr).second)->Send(data) == 0) {
				isSent = true;
			}
		}
		Unlock();
		if (isSent) {
			ret = 0;
		}
	}
	//指定接続先に送信
	else{
		Lock();
		if(m_sessionList.find(ipAddr) == m_sessionList.end()){
			TCPLOG(m_nameForLog + "[ERROR] Send failed. (no sessions. ip-addr=%s)",ipAddr.c_str());
		}
		else {
			if (m_sessionList[ipAddr]->Send(data) == 0) {
				ret = 0;
			}
		}
		Unlock();
	}
	return ret;
}

int TcpServer::Send(const unsigned char* data, int size, const std::string& ipAddr) {
	int ret = 0;
	//全接続先に送信
	if(ipAddr == ""){
		Lock();
		if (m_sessionList.empty()) {
			TCPLOG(m_nameForLog + "[ERROR] Send failed. (no sessions.)");
			ret = -1;
		}
		else {
			for (std::map<std::string, TcpServerSession*>::iterator itr = m_sessionList.begin();
				itr != m_sessionList.end(); itr++)
			{
				int sendSize = ((*itr).second)->Send(data, size);
				if (sendSize == SOCKET_ERROR) {
					TCPLOG(m_nameForLog + "[ERROR] Send failed. size=%d errno=%d (%s)", size, errno, strerror(errno));
					ret = -1;
				}
			}
		}
		Unlock();
	}
	//指定接続先に送信
	else{
		Lock();
		if(m_sessionList.find(ipAddr) == m_sessionList.end()){
			Unlock();
			TCPLOG(m_nameForLog + "[ERROR] Send failed. (no sessions. ip-addr=%s)",ipAddr.c_str());
			return -1;
		}
		int sendSize = m_sessionList[ipAddr]->Send(data, size);
		if (sendSize == SOCKET_ERROR) {
			TCPLOG(m_nameForLog + "[ERROR] Send failed. size=%d errno=%d (%s)", size, errno, strerror(errno));
			ret = -1;
		}
		Unlock();
	}
	return ret;
}

int TcpServer::InitServer()
{
	if (m_logOn) {
		TCPLOG(m_nameForLog + "Create socket.");
	}
	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock == INVALID_SOCKET) {
#if defined(WIN32) || defined(WIN64)
		TCPLOG(m_nameForLog + "[ERROR] Create socket failed. (err=%d)", WSAGetLastError());
#else
		TCPLOG("ソケット生成失敗 errno=%d (%s)",
			errno, strerror(errno));
#endif
		return -1;
	}
	//SO_REUSEADDRを設定してTIME_WAIT状態でもすぐに使えるようにする
	int yes = 1;
	setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

	if (m_logOn) {
		TCPLOG(m_nameForLog + "Start Bind.");
	}
	int ret = bind(m_sock, (struct sockaddr *)&m_hostAddr, sizeof(m_hostAddr));
	if (ret == SOCKET_ERROR) {
#if defined(WIN32) || defined(WIN64)
		TCPLOG(m_nameForLog + "[ERROR] Bind failed. (err=%d)", WSAGetLastError());
#else
		TCPLOG("バインド失敗 errno=%d (%s)", errno, strerror(errno));
#endif
		CloseSocket();
		return -1;
	}
	if (m_logOn) {
		TCPLOG(m_nameForLog + "Binded.");
		TCPLOG(m_nameForLog + "Start listen.");
	}
	ret = listen(m_sock, 5);
	if (ret == SOCKET_ERROR) {
#if defined(WIN32) || defined(WIN64)
		TCPLOG(m_nameForLog + "[ERROR] Listen failed. (err=%d)", WSAGetLastError());
#else
		TCPLOG("リッスン失敗 errno=%d (%s)", errno, strerror(errno));
#endif
		CloseSocket();
		return -1;
	}
	if (m_logOn) {
		TCPLOG(m_nameForLog + "Listened.");
	}

	FD_ZERO(&m_readfds);
	FD_SET(m_sock, &m_readfds);
	if (m_logOn) {
		TCPLOG(m_nameForLog + "Waiting connect.");
	}

	return 0;
}

int TcpServer::ThreadInitProc() {
	return 0;
}

int TcpServer::ThreadProc()
{
	if (!m_processStop) {
		Lock();
		if (!m_delSessions.empty()) {
			for (std::vector<TcpServerSession*>::iterator itr = m_delSessions.begin(); itr != m_delSessions.end(); itr++) {
				delete *itr;
			}
			m_delSessions.clear();
		}
		Unlock();
	}
	else {
		return 0;
	}

	if(m_sock == INVALID_SOCKET){
		Sleep(3000);
		ThreadInitProc();
		if(m_sock == INVALID_SOCKET){
			return 0;
		}
	}
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	fd_set fds;
	memcpy(&fds, &m_readfds, sizeof(fd_set));
	int ret = select((SOCKET_TYPE)((int)m_sock+1), &fds, NULL, NULL, &tv);
	if (ret == 0) {
		//タイムアウト
		return 0;
	}
	if(ret == -1){
#if defined(WIN32) || defined(WIN64)
		TCPLOG(m_nameForLog + "[ERROR] Select failed. (err=%d)", WSAGetLastError());
#else
		TCPLOG("セレクト失敗 errno=%d (%s)", errno, strerror(errno));
#endif
		Sleep(3000); //3秒スリープでログの大量出力抑止
		return 0; //接続待ちスレッド処理は継続
	}
	if (FD_ISSET(m_sock, &fds)) {
		struct sockaddr_in clientAddr;
		int len = sizeof(clientAddr);
		if(m_logOn){
			TCPLOG(m_nameForLog + "Start accept.");
		}
		SOCKET_TYPE clientSock = accept(m_sock, (struct sockaddr *)&clientAddr, &len);
		if(clientSock == INVALID_SOCKET){
#if defined(WIN32) || defined(WIN64)
			TCPLOG(m_nameForLog + "[ERROR] Accept failed. (%d)", WSAGetLastError());
#else
			TCPLOG("アクセプト失敗 errno=%d (%s)", errno, strerror(errno));
#endif
			return 0; //接続待ちスレッド処理は継続
		}
		std::string clientIpAddr = inet_ntoa(clientAddr.sin_addr);
		if(m_logOn){
			TCPLOG(m_nameForLog + "a client is Connected. (ip-addr=%s)", clientIpAddr.c_str());
		}

		TcpServerSession* session = NULL;
		Lock();
		if(m_sessionList.find(clientIpAddr) != m_sessionList.end()){
			TCPLOG(m_nameForLog + "[WARNING] Duplex connection. -> Close previous connection. (ip-addr=%s)", clientIpAddr.c_str());
			session = m_sessionList[clientIpAddr];
			m_sessionList.erase(clientIpAddr);
		}
		//Unlock();
		if(session != NULL){
			session->StopSession();
			delete session;
		}
		Unlock();

		session = new TcpServerSession(m_hostPort, clientIpAddr, clientSock, m_receiveSize, this);
		if(!m_logOn){
			session->LogOff();
		}
		int ret_;
#if defined(WIN32) || defined(WIN64)
		ret_ = session->StartSession();
#else
		if(m_keepAlive){
			ret_ = session->StartSessionWithKeepAlive(m_keepAlive_idleTime,m_keepAlive_interval,m_keepAlive_retryCount);
		}
		else{
			ret_ = session->StartSession();
		}
#endif

		if(ret_ != 0){
			delete session;
			return 0; //接続待ちスレッド処理は継続
		}
		Lock();
/*
		if(m_sessionList.find(clientIpAddr) != m_sessionList.end()){
			TCPLOG("クライアント接続重複->前回接続破棄");
			TcpServerSession* session_ = m_sessionList[clientIpAddr];
			m_sessionList.erase(clientIpAddr);
			delete session_;
		}
*/
		m_sessionList[clientIpAddr] = session;
		Unlock();
		//接続通知
		if(m_receiver != NULL){
			m_receiver->Connected(clientIpAddr);
		}
	}
	return 0;
}

int TcpServer::ThreadTermProc() {
	if(m_logOn){
		TCPLOG(m_nameForLog + "Stop waiting connection.");
	}
	CloseSocket();
	std::vector<TcpServerSession*> delList;
	Lock();
	for(std::map<std::string,TcpServerSession*>::iterator itr = m_sessionList.begin();
			itr != m_sessionList.end(); itr++)
	{
		if(m_logOn){
			TCPLOG(m_nameForLog + "Stop session. (client=%s)", ((*itr).first).c_str());
		}
		delList.push_back((*itr).second); //移し変え
	}
	m_sessionList.clear();
	Unlock();
	for(std::vector<TcpServerSession*>::iterator itr = delList.begin();
			itr != delList.end(); itr++){
		(*itr)->StopSession();
		delete (*itr);
	}
	return 0;
}

int TcpServer::ServerSessionReceivedData(const unsigned char* data, int dataSize, TcpServerSession* session)
{
	//受信通知
	int ret;
	//Lock(); 通知先で送信しようとするとデッドロックするため排他廃止
	if(m_receiver != NULL){
		ret = m_receiver->ReceivedData(data, dataSize, session->GetClientIpAddr());
	}
	else{
		TCPLOG(m_nameForLog + "[WARNING] Receiver is nothing.");
		ret = m_receiveSize;
	}
	//Unlock();
	return ret;
}

int TcpServer::Disconnected(const std::string& clientIpAddr, bool destroy)
{
	if(m_processStop) return 0;

	//切断通知
	if(m_receiver != NULL){
		m_receiver->Disconnected(clientIpAddr, m_hostPort);
	}

	TcpServerSession* session = NULL;
	//TCPLOG(m_nameForLog + "[DEBUG] Lock Disconnected");
	Lock();
	if(m_sessionList.find(clientIpAddr) != m_sessionList.end()){
		session = m_sessionList[clientIpAddr];
		m_sessionList.erase(clientIpAddr);
	}
	//Unlock();
	if((session != NULL) && destroy){
		//delete session;
		m_delSessions.push_back(session);
	}
	Unlock();
	//TCPLOG(m_nameForLog + "[DEBUG] Unlock Disconnected");
	//削除済みはOK
	return 0;
}
void TcpServer::CloseSocket()
{
	if(m_sock == INVALID_SOCKET){
		return;
	}

#if defined(WIN32) || defined(WIN64)
	int ret = closesocket(m_sock);
#else
	int ret = close(m_sock);
#endif
	if(ret == SOCKET_ERROR){
#if defined(WIN32) || defined(WIN64)
		TCPLOG(m_nameForLog + "[ERROR] Close socket failed. (err=%d)", WSAGetLastError());
#else
		TCPLOG("ソケットクローズ失敗 errno=%d (%s)", errno, strerror(errno));
#endif
	}
	else{
		TCPLOG("Close scket.");
	}
	m_sock = INVALID_SOCKET;
}
void TcpServer::KeepAliveOn(int idleTime, int interval, int retryCount)
{
	//次回アクセプトから有効
	m_keepAlive = true;
	m_keepAlive_idleTime = idleTime;
	m_keepAlive_interval = interval;
	m_keepAlive_retryCount = retryCount;
}
void TcpServer::KeepAliveOff()
{
	m_keepAlive = false;
}

int TcpServer::GetSessionCount()
{
	return (int)m_sessionList.size();
}

int TcpServer::StopSession(){
	//std::vector<TcpServerSession*> delList;
	//TCPLOG(m_nameForLog + "[DEBUG] Lock StopSession");
	Lock();
	for(std::map<std::string,TcpServerSession*>::iterator itr = m_sessionList.begin();
			itr != m_sessionList.end(); itr++)
	{
		if(m_logOn){
			TCPLOG(m_nameForLog + "Stop session. (client=%s)", ((*itr).first).c_str());
		}
		//delList.push_back((*itr).second); //移し変え
		m_delSessions.push_back((*itr).second); //移し変え
	}
	m_sessionList.clear();
	Unlock();
	//TCPLOG(m_nameForLog + "[DEBUG] Unlock StopSession");

	int ret = 0;
	//for(std::vector<TcpServerSession*>::iterator itr = delList.begin();
	//		itr != delList.end(); itr++){
	//	if((*itr)->StopSession() != 0){
	//		ret = -1;
	//	}
	//	delete (*itr);
	//}
	return ret;
}

