/*
 * TcpLogger.h
 *
 *  Created on: 2018/09/05
 *      Author: Attractor T.Unno
 */

#ifndef TCPLOGGER_H_
#define TCPLOGGER_H_

//Please change the following log output processing definition according to the environment.
#include <CommLogger.h>
#define TCPLOG(fmt,...) LOG(fmt, ## __VA_ARGS__)

#endif /* TCPLOGGER_H_ */
