#include "stdafx.h"
#include "CommModuleServer.h"

//
// �ʐM���W���[���i�T�[�o�j
//

//
// �R���X�g���N�^
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
// �f�X�g���N�^
//
CommModuleServer::~CommModuleServer()
{
	_tcpServer->RemoveReceiver();
	delete _tcpServer;
}

//
// ���M����
//
int CommModuleServer::Send(const unsigned char* cmdData, int cmdLen)
{
	return _tcpServer->Send(cmdData, cmdLen);
}
//
// TCP/IP�ʐM��~����
//
int CommModuleServer::TcpStop()
{
	return _tcpServer->ServerStop();
}

//
// �X���b�h�O����
//
int CommModuleServer::ThreadInitProc()
{
	return _tcpServer->ServerStart();
}

//
// �X���b�h�㏈��
//
int CommModuleServer::ThreadTermProc()
{
	return _tcpServer->ServerStop();
}
