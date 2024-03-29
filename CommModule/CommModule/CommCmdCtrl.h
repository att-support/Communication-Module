#pragma once
#include <vector>
#include "CommDef.h"
#include "CommCommand.h"
#include "CommCtrlInfo.h"

//
// 通信制御情報電文
//

class CommCmdCtrl :
	public CommCommand
{
public:
	CommCmdCtrl();
	virtual ~CommCmdCtrl();
	CommCommand* Clone();

	void AddInfo(CommCtrlInfo& ctrlInfo);
	CommCtrlInfoList& GetCtrlInfoList();

private:
	CommCtrlInfoList _list;

	int _GetDataLen();
	void _SetDataToCommand(unsigned char*& data);
	int _SetDataFromCommand(const unsigned char*& data);
};

