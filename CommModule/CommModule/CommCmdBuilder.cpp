#include "stdafx.h"
#include "CommCmdBuilder.h"


//
// �d�������N���X
//

//
// �R���X�g���N�^
//
CommCmdBuilder::CommCmdBuilder()
{
}

//
// �f�X�g���N�^
//
CommCmdBuilder::~CommCmdBuilder()
{
}

//
// �d������
//
CommCommand* CommCmdBuilder::CreateCommand(CommCommand* srcCmd)
{
	CommCommand* cmd = NULL;
	if (srcCmd->GetAction() == CMD_ACT_REQ) {
		if (srcCmd->GetType() == CMD_TYPE_INFO) {
			cmd = new CommCmdCtrl();
		}
		else if (srcCmd->GetType() == CMD_TYPE_STAT) {
			cmd = new CommCmdStat();
		}
		else if (srcCmd->GetType() == CMD_TYPE_DATA) {
			cmd = new CommCmdData();
		}
		else if (srcCmd->GetType() == CMD_TYPE_TERM) {
			cmd = new CommCommand();
		}
		else {
			cmd = new CommCommand();
		}
	}
	//Reply
	else {
		cmd = new CommCommand();
	}
	cmd->CopyHeader(srcCmd);
	return cmd;
}

//
// �����d������
//
CommCommand* CommCmdBuilder::CreateReplyCommand(CommCommand* cmd, const std::string& replyCode)
{
	CommCommand* replyCmd = new CommCommand();
	replyCmd->SetType(cmd->GetType());
	replyCmd->SetAction(replyCode);
	return replyCmd;
}

//
// �ʐM������d������
//
CommCmdCtrl* CommCmdBuilder::CreateCtrlCommand()
{
	return new CommCmdCtrl();
}

//
// �f�[�^���M�d������
//
CommCmdData* CommCmdBuilder::CreateDataCommand()
{
	return new CommCmdData();
}

//
// ��Ԓʒm�d������
//
CommCmdStat* CommCmdBuilder::CreateStatCommand()
{
	return new CommCmdStat();
}

//
// �I���ʒm�d������
//
CommCommand* CommCmdBuilder::CreateTermCommand()
{
	CommCommand* cmd = new CommCommand();
	cmd->SetType(CMD_TYPE_TERM);
	cmd->SetAction(CMD_ACT_REQ);
	return cmd;
}

