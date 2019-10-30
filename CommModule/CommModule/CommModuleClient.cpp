#include "stdafx.h"
#include "CommModuleClient.h"

//
// �ʐM���W���[���i�N���C�A���g�j
//

//
// �R���X�g���N�^
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
// �f�X�g���N�^
//
CommModuleClient::~CommModuleClient()
{
	_tcpClient->RemoveReceiver();
	delete _tcpClient;
}

//
// ���M����
//
int CommModuleClient::Send(const unsigned char* cmdData, int cmdLen)
{
	return _tcpClient->Send(cmdData, cmdLen);
}
//
// TCP/IP�ʐM��~����
//
int CommModuleClient::TcpStop()
{
	_isConnected = false;
	return _tcpClient->Disconnect();
}

//
// �X���b�h�O����
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
// �X���b�h�㏈��
//
int CommModuleClient::ThreadTermProc()
{
	_isConnected = false;
	if (_tcpClient->Disconnect() != 0) {
		return -1;
	}
	return 0;
}

