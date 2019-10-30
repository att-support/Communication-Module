#include "stdafx.h"
#include "CommModuleServer.h"

//
// 通信モジュール（サーバ）
//

//
// コンストラクタ
//
CommModuleServer::CommModuleServer(E_BML_IF_COMM_TYPE bmlIfCommType, int queueSize, int port)
	: CommModule(bmlIfCommType,queueSize)
{
	_commPort = port;
	_tcpServer = new TcpServer(port);
	_tcpServer->SetReceiver(this, CMD_HEADER_LEN);
	char nameBuf[256];
	sprintf(nameBuf, "[CommModule(type=%d port:%d)] ", bmlIfCommType, _commPort);
	_logName = nameBuf;
}

//
// デストラクタ
//
CommModuleServer::~CommModuleServer()
{
	_tcpServer->RemoveReceiver();
	delete _tcpServer;
}

//
// 送信処理
//
int CommModuleServer::Send(const unsigned char* cmdData, int cmdLen)
{
	return _tcpServer->Send(cmdData, cmdLen);
}
//
// TCP/IP通信停止処理
//
int CommModuleServer::TcpStop()
{
	return _tcpServer->ServerStop();
}

//
// スレッド前処理
//
int CommModuleServer::ThreadInitProc()
{
	return _tcpServer->ServerStart();
}

//
// スレッド後処理
//
int CommModuleServer::ThreadTermProc()
{
	return _tcpServer->ServerStop();
}
