#include "stdafx.h"
#include "CommCtrlInfo.h"
#include <Util/BinData.h>

//
// í êMêßå‰èÓïÒ
//

CommCtrlInfo::CommCtrlInfo()
{
	_type = BML_IF_COMM_TYPE_UNIT_INFO;
	_port = 0;
	_queSize = 0;
}
CommCtrlInfo::CommCtrlInfo(E_BML_IF_COMM_TYPE type, int port, int queSize)
{
	_type = type;
	_port = port;
	_queSize = queSize;
}
CommCtrlInfo::~CommCtrlInfo()
{

}

void CommCtrlInfo::SetType(E_BML_IF_COMM_TYPE type)
{
	_type = type;
}
E_BML_IF_COMM_TYPE CommCtrlInfo::GetType()
{
	return _type;
}
void CommCtrlInfo::SetPort(int port)
{
	_port = port;
}
int CommCtrlInfo::GetPort()
{
	return _port;
}
void CommCtrlInfo::SetQueSize(int queSize)
{
	_queSize = queSize;
}
int CommCtrlInfo::GetQueSize()
{
	return _queSize;
}

void CommCtrlInfo::SetBinData(unsigned char*& dstData)
{
	if (dstData == NULL) {
		return;
	}
	BinData::Set8bit((unsigned char)_type, dstData);
	BinData::Set16bit((unsigned short)_port, dstData);
	BinData::Set32bit((unsigned long)_queSize, dstData);
}

void CommCtrlInfo::GetBinData(const unsigned char*& srcData)
{
	_type = (E_BML_IF_COMM_TYPE)BinData::Get8bit(srcData);
	_port = (int)BinData::Get16bit(srcData);
	_queSize = (int)BinData::Get32bit(srcData);
}


