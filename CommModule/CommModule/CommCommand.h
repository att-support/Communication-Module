#pragma once

#include <string>
#include <Util/SysTime.h>
#include <Util/BinData.h>
#include <Util/Conv.h>
#include "CommLogger.h"

//
// �d�����
//
#define CMD_TYPE_INFO		"INFO"
#define CMD_TYPE_DATA		"DATA"
#define CMD_TYPE_STAT		"STAT"
#define CMD_TYPE_TERM		"TERM"

//
// �v��/�������
//
#define CMD_ACT_REQ			"REQ"
#define CMD_ACT_ACK			"ACK"
#define CMD_ACT_NAK			"NAK"
#define CMD_ACT_BUSY		"BSY"

//
// �萔
//
#define CMD_HEADER_LEN		30	// 1+4+3+14+8
#define CMD_MIN_LEN			(CMD_HEADER_LEN + 1)	// minimum command length
#define CMD_TYPE_LEN		4
#define CMD_ACT_LEN			3
#define CMD_TS_LEN			14
#define CMD_DATA_LEN_LEN	8
#define CMD_MAX_DATA_LEN	99999999

//
// �������
//
typedef enum {
	CMD_REP_UNKNOWN,
	CMD_REP_ACK,
	CMD_REP_NAK,
	CMD_REP_BUSY
}CMD_REP_CODE;

//
// �d���i���ʁE���j
//

class CommCommand
{
public:
	// �R���X�g���N�^
	CommCommand();
	// �f�X�g���N�^
	virtual ~CommCommand();
	// �N���[������
	virtual CommCommand* Clone();

	// �d���쐬
	void MakeCommand(unsigned char** data, int& dataLen);
	// �d���ݒ�
	int SetCommand(const unsigned char* data, int dataLen);
	// �w�b�_�Œ蕔�ݒ�
	int SetHeader(const unsigned char* data, int dataLen);
	// �f�[�^���ݒ�(�w�b�_�Œ蕔�ȊO�̎c��)
	int SetPartialData(const unsigned char* data, int dataLen);
	// �c���M�T�C�Y�擾
	int GetRemainSize();
	// �w�b�_�Œ蕔�R�s�[
	void CopyHeader(CommCommand* cmd);
	// �d����ʐݒ�
	void SetType(const std::string& type);
	// �d����ʎ擾
	std::string GetType();
	// �v��/������ʐݒ�
	std::string GetAction();
	// �v��/������ʎ擾
	void SetAction(const std::string& act);
	// ������ʐݒ�
	void SetReplyCode(CMD_REP_CODE code);
	// ������ʎ擾
	CMD_REP_CODE GetReplyCode();
	// �^�C���X�^���v�擾
	SysTime GetTimestamp();
	// ����I�y���[�^�[
	CommCommand& operator=(const CommCommand& obj) {
		_cmdType = obj._cmdType;
		_cmdAct = obj._cmdAct;
		_timestamp = obj._timestamp;
		_cmdDataLen = obj._cmdDataLen;
		_isComplete = obj._isComplete;
		return *this;
	}

protected:
	const int _stx = 0x02;				// STX
	const int _etx = 0x03;				// ETX
	std::string _cmdType;				// �d�����
	std::string _cmdAct;				// �v��/�������
	SysTime _timestamp;					// �^�C���X�^���v
	int _cmdDataLen;					// �f�[�^��
	bool _isComplete;					// �d��������� (true:����)
	std::string _logName;				// ���O�o�͗p�v���t�B�N�X����

	// �d���R�s�[
	void Copy(CommCommand* cmd);
	// �f�[�^���擾
	virtual int _GetDataLen();
	// �f�[�^�ݒ�i�����o���j
	virtual void _SetDataToCommand(unsigned char*& data);
	// �f�[�^�ݒ�i�ǂݏo���j
	virtual int _SetDataFromCommand(const unsigned char*& data);
};

