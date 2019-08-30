#pragma once

#include "CommData.h"
#include <vector>

//-------- Return Codes ---------------------------------------------

#define QUE_RET_SUCCSESS		 0	//正常。成功。
#define QUE_RET_ERROR			-1	//エラー。失敗。
#define QUE_RET_OVERFLOW		-2	//オーバーフロー
#define QUE_RET_NODATA			-3	//データなし


//-------- Class ----------------------------------------------------

//
// 通信データキュー
//
#include <Util/Mutex.h>

class CommDataQue
{
public:
	CommDataQue(int size);
	virtual ~CommDataQue();

	int Add(ICommData* data);
	ICommData* GetTop();
	int Count();
	bool IsFull();
	bool IsEmpty();

private:
	int _queSize;
	std::vector<ICommData*> _que;
	Mutex _locker;
	void Lock() {
		_locker.Lock();
	}
	void Unlock() {
		_locker.Unlock();
	}
};

