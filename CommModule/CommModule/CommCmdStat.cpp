#include "stdafx.h"
#include "CommCmdStat.h"


//
// ó‘Ô’Ê’m“d•¶
//

CommCmdStat::CommCmdStat()
{
	_cmdType = CMD_TYPE_STAT;
	_cmdAct = CMD_ACT_REQ;
	_isBusy = false;
	_logName = "[CommCmdStat] ";
}

CommCmdStat::~CommCmdStat()
{
}

CommCommand* CommCmdStat::Clone()
{
	CommCmdStat* clone = new CommCmdStat();
	clone->Copy(this);
	clone->_isBusy = _isBusy;
	clone->_isComplete = true;
	return clone;
}

bool CommCmdStat::IsBusy()
{
	return _isBusy;
}

void CommCmdStat::SetBusy(bool isBusy)
{
	_isBusy = isBusy;
}

int CommCmdStat::_GetDataLen()
{
	return 1;
}

void CommCmdStat::_SetDataToCommand(unsigned char*& data)
{
	if (_isBusy) {
		BinData::Set8bit(1, data);
	}
	else {
		BinData::Set8bit(0, data);
	}
}

int CommCmdStat::_SetDataFromCommand(const unsigned char*& data)
{
	unsigned char stat = BinData::Get8bit(data);
	if (stat == 0) {
		_isBusy = false;
	}
	else {
		_isBusy = true;
	}
	return 0;
}
