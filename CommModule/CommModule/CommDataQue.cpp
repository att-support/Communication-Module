#include "stdafx.h"
#include "CommDataQue.h"

//
// 通信データキュー
//

CommDataQue::CommDataQue(int size)
{
	_queSize = size;
}


CommDataQue::~CommDataQue()
{
	Lock();
	for (std::vector<ICommData*>::iterator itr = _que.begin();
		itr != _que.end(); itr++)
	{
		delete (*itr);
	}
	_que.clear();
	Unlock();
}

int CommDataQue::Add(ICommData* data)
{
	if (data == NULL) {
		return QUE_RET_ERROR;
	}
	Lock();
	if (_que.size() >= _queSize) {
		Unlock();
		return QUE_RET_OVERFLOW;
	}
	_que.push_back(data);
	Unlock();
	return QUE_RET_SUCCSESS;
}

ICommData* CommDataQue::GetTop()
{
	Lock();
	if (_que.empty()) {
		Unlock();
		return NULL;
	}
	ICommData* data = _que[0];
	_que.erase(_que.begin());
	Unlock();
	return data;
}

int CommDataQue::Count()
{
	Lock();
	int count_ = (int)_que.size();
	Unlock();
	return count_;
}

bool CommDataQue::IsFull()
{
	Lock();
	bool isFull_ = (_que.size() >= _queSize);
	Unlock();
	return isFull_;
}

bool CommDataQue::IsEmpty()
{
	Lock();
	bool isEmpty_ = _que.empty();
	Unlock();
	return isEmpty_;
}
