#pragma once
#include "CommCtrlInfo.h"

class CommModuleInterface
{
public:
	CommModuleInterface() {};
	virtual ~CommModuleInterface() {};

	virtual void NotifyCtrlInfo(CommCtrlInfoList& ctrlInfoList) = 0;
	virtual void NotifyStat(bool isBusy) = 0;
	virtual void NotifyTerm() = 0;
	virtual void NotifyConnected(E_BML_IF_COMM_TYPE commType, int port) = 0;
	virtual void NotifyDisconnected(E_BML_IF_COMM_TYPE commType, int port) = 0;
};

