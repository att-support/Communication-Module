/*
 * TcpCommon.cpp
 *
 *  Created on: 2018/09/13
 *      Author: Attractor T.Unno
 */
#include "TcpCommon.h"

bool TcpCommon::_isStarted = false;

TcpCommon::TcpCommon()
{
}
TcpCommon::~TcpCommon()
{
}

int TcpCommon::Startup()
{
#if defined(WIN32) || defined(WIN64)
	if(!_isStarted){
		WORD ver=MAKEWORD(2,2);
		WSADATA wsa;
		return WSAStartup(ver,&wsa);
	}
#endif
	return 0;
}

void TcpCommon::Cleanup()
{
#if defined(WIN32) || defined(WIN64)
	if(_isStarted){
		WSACleanup();
	}
#endif
}
