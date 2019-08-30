#include "stdafx.h"
#include "CommCmdCtrl.h"
#include <Util/BinData.h>
#include <Util/Conv.h>
#include <Util/Logger.h>

//
// ’ÊM§Œäî•ñ“d•¶
//

CommCmdCtrl::CommCmdCtrl()
{
	_cmdType = CMD_TYPE_INFO;
	_cmdAct = CMD_ACT_REQ;
	_logName = "[CommCmdCtrl] ";
}

CommCmdCtrl::~CommCmdCtrl()
{
}

CommCommand* CommCmdCtrl::Clone()
{
	CommCmdCtrl* clone = new CommCmdCtrl();
	clone->Copy(this);
	clone->_list = _list;
	clone->_isComplete = true;
	return clone;
}

void CommCmdCtrl::AddInfo(CommCtrlInfo& ctrlInfo)
{
	_list.push_back(ctrlInfo);
}

CommCtrlInfoList& CommCmdCtrl::GetCtrlInfoList()
{
	return _list;
}

int CommCmdCtrl::_GetDataLen()
{
	if (_list.empty()) {
		return 0;
	}
	return (int)(_list.size() * CommCtrlInfo::GetBinDataSize()) + 1;
}

void CommCmdCtrl::_SetDataToCommand(unsigned char*& data)
{
	BinData::Set8bit((unsigned char)_list.size(), data);
	for (std::vector<CommCtrlInfo>::iterator itr = _list.begin();
		itr != _list.end(); itr++)
	{
		(*itr).SetBinData(data);
	}
}

int CommCmdCtrl::_SetDataFromCommand(const unsigned char*& data)
{
	_list.clear();
	int len = (int)BinData::Get8bit(data);
	for (int i = 0; i < len; i++) {
		CommCtrlInfo info;
		info.GetBinData(data);
		_list.push_back(info);
	}
	return 0;
}
