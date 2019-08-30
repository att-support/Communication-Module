#pragma once
#include "CommCommand.h"
#include "CommCmdCtrl.h"
#include "CommCmdData.h"
#include "CommCmdStat.h"

//
// �d�������N���X
//

class CommCmdBuilder
{
public:
	CommCmdBuilder();
	virtual ~CommCmdBuilder();

	// �d������
	static CommCommand* CreateCommand(CommCommand* cmd);
	// �����d������
	static CommCommand* CreateReplyCommand(CommCommand* cmd, const std::string& replyCode);
	// �ʐM������d������
	static CommCmdCtrl* CreateCtrlCommand();
	// �f�[�^���M�d������
	static CommCmdData* CreateDataCommand();
	// ��Ԓʒm�d������
	static CommCmdStat* CreateStatCommand();
	// �I���ʒm�d������
	static CommCommand* CreateTermCommand();
};

