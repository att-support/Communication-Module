/*
 * TcpCommon.h
 *
 *  Created on: 2018/09/13
 *      Author: Attractor T.Unno
 */

#ifndef PROCESSCOM_TCPCOMMON_H_
#define PROCESSCOM_TCPCOMMON_H_

#if defined(WIN32) || defined(WIN64)
#include <winsock2.h>
#else
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#if defined(WIN32) || defined(WIN64)
typedef SOCKET SOCKET_TYPE;
#else
typedef int SOCKET_TYPE;
#endif

/*
 *
 */
class TcpCommon {
public:
	static int Startup();
	static void Cleanup();

private:
	TcpCommon();
	virtual ~TcpCommon();
	static bool _isStarted;
};

#endif /* PROCESSCOM_TCPCOMMON_H_ */
