#include "stdafx.h"
#include "CommModuleClient.h"

//
// 通信モジュール（クライアント）
//

//
// コンストラクタ
//
CommModuleClient::CommModuleClient(E_BML_IF_COMM_TYPE bmlIfCommType, int queueSize, const std::string& ipAddr, int port)
	: CommModule(bmlIfCommType, queueSize)
{
	_commPort = port;
	_tcpClient = new TcpClient(ipAddr, port);
	_tcpClient->SetReceiver(this, CMD_HEADER_LEN);
	char nameBuf[256];
	sprintf(nameBuf, "[CommModule(type=%d port:%d)] ", bmlIfCommType, _commPort);
	_logName = nameBuf;
}

//
// デストラクタ
//
CommModuleClient::~CommModuleClient()
{
}

//
// 送信処理
//
int CommModuleClient::Send(const unsigned char* cmdData, int cmdLen)
{
	return _tcpClient->Send(cmdData, cmdLen);
}

//
// スレッド前処理
//
int CommModuleClient::ThreadInitProc()
{
	if (_tcpClient->Connect() != 0) {
		return -1;
	}
	_isConnected = true;
	return 0;
}

//
// スレッド後処理
//
int CommModuleClient::ThreadTermProc()
{
	_isConnected = false;
	if (_tcpClient->Disconnect() != 0) {
		return -1;
	}
	return 0;
}

