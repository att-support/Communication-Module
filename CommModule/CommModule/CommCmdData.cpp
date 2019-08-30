#include "stdafx.h"
#include "CommCmdData.h"


//
// データ送信要求電文
//

CommCmdData::CommCmdData()
{
	_cmdType = CMD_TYPE_DATA;
	_cmdAct = CMD_ACT_REQ;
	_cmdData = NULL;
	_logName = "[CommCmdData] ";
}

CommCmdData::~CommCmdData()
{
	if (_cmdData != NULL) {
		delete _cmdData;
	}
}

CommCommand* CommCmdData::Clone()
{
	CommCmdData* clone = new CommCmdData();
	clone->Copy(this);
	clone->_cmdData = _cmdData->Clone();
	clone->_isComplete = true;
	return clone;
}

int CommCmdData::_GetDataLen()
{
	if (_cmdData == NULL) {
		return 0;
	}
	return _cmdData->GetDataSize();
}

void CommCmdData::_SetDataToCommand(unsigned char*& data)
{
	if ((_cmdData != NULL) && (_cmdData->GetDataSize() > 0)) {
		memcpy(data, _cmdData->GetData(), _cmdData->GetDataSize());
		data += _cmdData->GetDataSize();
	}
}

int CommCmdData::_SetDataFromCommand(const unsigned char*& data)
{
	if (_cmdDataLen > CMD_MAX_DATA_LEN) {
		LOG("[Illegal Command] Data length Error. %d (over %d)", _cmdDataLen, CMD_MAX_DATA_LEN);
		return -1;
	}
	if (_cmdData != NULL) {
		delete _cmdData;
	}

	if (_cmdDataLen > 0) {
		_cmdData = new CommData();
		_cmdData->SetData(data, _cmdDataLen);
		data += _cmdDataLen;
	}
	else {
		_cmdData = NULL;
	}

	return 0;
}

void CommCmdData::SetCommandData(ICommData* data)
{
	if (data == NULL) {
		return;
	}
	if (data->GetDataSize() > CMD_MAX_DATA_LEN) {
		return;
	}
	_cmdData = data;
}

ICommData* CommCmdData::GetCommandData()
{
	return _cmdData;
}