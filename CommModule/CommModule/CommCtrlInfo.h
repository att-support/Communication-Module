#pragma once
#include "CommCommand.h"
#include "CommDef.h"
#include <vector>

//
// í êMêßå‰èÓïÒ
//

class CommCtrlInfo;
typedef std::vector<CommCtrlInfo> CommCtrlInfoList;

class CommCtrlInfo
{
public:
	CommCtrlInfo();
	CommCtrlInfo(E_BML_IF_COMM_TYPE type, int port, int queSize);
	virtual ~CommCtrlInfo();

	void SetType(E_BML_IF_COMM_TYPE type);
	E_BML_IF_COMM_TYPE GetType();
	void SetPort(int port);
	int GetPort();
	void SetQueSize(int queSize);
	int GetQueSize();

	void SetBinData(unsigned char*& dstData);
	void GetBinData(const unsigned char*& srcData);
	static int GetBinDataSize() { return 7; }	// 1 + 2 + 4

private:
	E_BML_IF_COMM_TYPE _type;
	int _port;
	int _queSize;
};
