#include "stdafx.h"
#include "CommCommand.h"

//
// �d���i���ʁE���j
//

//
// �R���X�g���N�^
//
CommCommand::CommCommand()
{
	_cmdDataLen = 0;
	_isComplete = false;
	_logName = "[CommCommand] ";
}

//
// �f�X�g���N�^
//
CommCommand::~CommCommand()
{
}

//
// �N���[������
//
CommCommand* CommCommand::Clone()
{
	CommCommand* clone = new CommCommand();
	clone->Copy(this);
	clone->_isComplete = true;
	return clone;
}

//
// �d���R�s�[
//
void CommCommand::Copy(CommCommand* cmd)
{
	_cmdType = cmd->_cmdType;
	_cmdAct = cmd->_cmdAct;
	_cmdDataLen = cmd->_cmdDataLen;
}

//
// �d���쐬
//
void CommCommand::MakeCommand(unsigned char** data, int& dataLen)
{
	dataLen = 0;
	if (data == NULL) {
		LOG(_logName + "[ERROR] MakeCommand() error. Bad argument. (data is NULL.");
		return;
	}
	_cmdDataLen = _GetDataLen();
	int cmdLen = _cmdDataLen + CMD_MIN_LEN;
	*data = new unsigned char[cmdLen];
	unsigned char* p = *data;
	//STX
	BinData::Set8bit(_stx, p);
	//Command Type
	BinData::SetString(_cmdType, p);
	//Command Action
	BinData::SetString(_cmdAct, p);
	//Timestamp
	_timestamp.Now();
	std::string strTs = _timestamp.GetNowYYYYMMDDHHMMSS();
	BinData::SetString(strTs, p);
	//DataSize
	std::string dataLenStr = Conv::IntToDecStr(_cmdDataLen, CMD_DATA_LEN_LEN);
	BinData::SetString(dataLenStr, p);
	//Data
	if (_cmdDataLen > 0) {
		_SetDataToCommand(p);
	}
	//ETX
	BinData::Set8bit(_etx, p);

	dataLen = cmdLen;
}

//
// �d���ݒ�
//
int CommCommand::SetCommand(const unsigned char* data, int dataLen)
{
	if (data == NULL) {
		LOG(_logName + "[ERROR] SetCommand() error. Bad argument. (data is NULL)");
		return -1;
	}
	if (dataLen <= 0) {
		LOG(_logName + "[ERROR] SetCommand() error. Bad argument. (dataLen=%d)", dataLen);
		return -1;
	}
	if (dataLen < CMD_MIN_LEN) {
		LOG(_logName + "[ERROR] SetCommand() error. Illegal command. Command length error. %d byte (less than %d byte)", dataLen, CMD_MIN_LEN);
		return -1;
	}
	const unsigned char* p = data;
	unsigned char ucWork;
	//STX
	ucWork = BinData::Get8bit(p);
	if (ucWork != _stx) {
		LOG(_logName + "[ERROR] SetCommand() error. Illegal command. STX Error. (0x%02X)", ucWork);
		return -1;
	}
	//Type
	_cmdType = BinData::GetString(p, CMD_TYPE_LEN);
	//Action
	_cmdAct = BinData::GetString(p, CMD_ACT_LEN);
	//Timestamp
	_timestamp.Set(BinData::GetString(p, CMD_TS_LEN));
	//Data length
	std::string strWork;
	strWork = BinData::GetString(p, CMD_DATA_LEN_LEN);
	_cmdDataLen = Conv::DecStrToInt(strWork);
	//Data
	if (_SetDataFromCommand(p) != 0) {
		return -1;
	}
	//ETX
	ucWork = BinData::Get8bit(p);
	if (ucWork != _etx) {
		LOG(_logName + "[ERROR] SetCommand() error. Illegal command. ETX Error. (0x%02X)", ucWork);
		return -1;
	}
	_isComplete = true;

	return 0;
}

//
// �w�b�_�Œ蕔�ݒ�
//
int CommCommand::SetHeader(const unsigned char* data, int dataLen)
{
	if (data == NULL) {
		LOG(_logName + "[ERROR] SetHeader() error. Bad argument. (data is NULL)");
		return -1;
	}
	if (dataLen <= 0) {
		LOG(_logName + "[ERROR] SetHeader() error. Bad argument. (dataLen=%d)", dataLen);
		return -1;
	}
	if (dataLen < CMD_HEADER_LEN) {
		LOG(_logName + "[ERROR] SetHeader() error. Illegal command. Command length error. %d byte (less than %d byte)", dataLen, CMD_HEADER_LEN);
		return -1;
	}
	const unsigned char* p = data;
	unsigned char ucWork;
	//STX
	ucWork = BinData::Get8bit(p);
	if (ucWork != _stx) {
		LOG(_logName + "[ERROR] SetHeader() error. Illegal command. STX error. (0x%02X)", ucWork);
		return -1;
	}
	//Type
	_cmdType = BinData::GetString(p, CMD_TYPE_LEN);
	//Action
	_cmdAct = BinData::GetString(p, CMD_ACT_LEN);
	//Timestamp
	_timestamp.Set(BinData::GetString(p, CMD_TS_LEN));
	//Data length
	std::string strWork;
	strWork = BinData::GetString(p, CMD_DATA_LEN_LEN);
	_cmdDataLen = Conv::DecStrToInt(strWork);

	return 0;
}

//
// �f�[�^���ݒ�(�w�b�_�Œ蕔�ȊO�̎c��)
//
int CommCommand::SetPartialData(const unsigned char* data, int dataLen)
{
	// Command size error
	if (dataLen != GetRemainSize()) {
		LOG(_logName + "[ERROR] SetPartialData() error. Illegal command. remain command size mismatch. (receive size=%d, remain size=%d)",
			dataLen, GetRemainSize());
		return -1;
	}
	//Data
	if (_SetDataFromCommand(data) != 0) {
		return -1;
	}
	//ETX
	unsigned char ucWork = BinData::Get8bit(data);
	if (ucWork != _etx) {
		LOG(_logName + "[ERROR] SetPartialData() error. Illegal command. ETX error. (0x%02X)", ucWork);
		return -1;
	}
	_isComplete = true;

	return 0;
}

//
// �c���M�T�C�Y�擾
//
int CommCommand::GetRemainSize()
{
	if (_isComplete) {
		return 0;
	}

	return _cmdDataLen + 1; //Data length + ETX
}

//
// �d����ʐݒ�
//
void CommCommand::SetType(const std::string& type)
{
	_cmdType = type;
}

//
// �d����ʎ擾
//
std::string CommCommand::GetType()
{
	return _cmdType;
}

//
// �v��/������ʐݒ�
//
void CommCommand::SetAction(const std::string& act)
{
	_cmdAct = act;
}

//
// �v��/������ʎ擾
//
std::string CommCommand::GetAction()
{
	return _cmdAct;
}

//
// ������ʐݒ�
//
void CommCommand::SetReplyCode(CMD_REP_CODE code)
{
	switch (code) {
	case CMD_REP_ACK:
		_cmdAct = CMD_ACT_ACK;
		break;
	case CMD_REP_NAK:
		_cmdAct = CMD_ACT_NAK;
		break;
	case CMD_REP_BUSY:
		_cmdAct = CMD_ACT_BUSY;
		break;
	default:
		_cmdAct = "";
		break;
	}
}

//
// ������ʎ擾
//
CMD_REP_CODE CommCommand::GetReplyCode()
{
	if (_cmdAct == CMD_ACT_ACK) {
		return CMD_REP_ACK;
	}
	else if (_cmdAct == CMD_ACT_NAK) {
		return CMD_REP_NAK;
	}
	else if (_cmdAct == CMD_ACT_BUSY) {
		return CMD_REP_BUSY;
	}
	else {
		return CMD_REP_UNKNOWN;
	}
}

//
// �^�C���X�^���v�擾
//
SysTime CommCommand::GetTimestamp()
{
	return _timestamp;
}

//
// �w�b�_�Œ蕔�R�s�[
//
void CommCommand::CopyHeader(CommCommand* cmd)
{
	_cmdType = cmd->_cmdType;
	_cmdAct = cmd->_cmdAct;
	_timestamp = cmd->_timestamp;
	_cmdDataLen = cmd->_cmdDataLen;
}

//
// �f�[�^���擾
//
// ���d�����ƂɎ���
int CommCommand::_GetDataLen()
{
	return 0;
}

//
// �f�[�^�ݒ�i�����o���j
//
// ���d�����ƂɎ���
void CommCommand::_SetDataToCommand(unsigned char*& data)
{
	return;
}

//
// �f�[�^�ݒ�i�ǂݏo���j
//
// ���d�����ƂɎ���
int CommCommand::_SetDataFromCommand(const unsigned char*& data)
{
	return 0;
}
