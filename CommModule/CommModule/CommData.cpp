#include "stdafx.h"
#include "CommData.h"


//
// 通信データ
//

CommData::CommData()
	: _data(NULL), _dataSize(0)
{
}

CommData::~CommData()
{
	if (_data != NULL) {
		delete[] _data;
	}
}

//送受信データの格納
//
void CommData::SetData(const unsigned char* pData, int dataSize)
{
	if (pData == NULL) return;

	if (dataSize <= 0) return;

	if (_data != NULL) {
		delete[] _data;
	}
	_dataSize = dataSize;
	_data = new unsigned char[_dataSize];
	memcpy_s(_data, _dataSize, pData, dataSize);
}
//送受信データ(領域先頭ポインタ)の取得
//
unsigned char* CommData::GetData()
{
	return _data;
}
//送受信データサイズの取得
//
int CommData::GetDataSize()
{
	return _dataSize;
}
//インスタンスの複製
//
ICommData* CommData::Clone()
{
	CommData* clone = new CommData();
	if ((this->_data != NULL) && (this->_dataSize > 0)) {
		clone->_dataSize = this->_dataSize;
		clone->_data = new unsigned char[clone->_dataSize];
		memcpy_s(clone->_data, clone->_dataSize, this->_data, this->_dataSize);
	}
	return clone;
}
