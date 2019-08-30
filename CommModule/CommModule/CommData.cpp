#include "stdafx.h"
#include "CommData.h"


//
// �ʐM�f�[�^
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

//����M�f�[�^�̊i�[
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
//����M�f�[�^(�̈�擪�|�C���^)�̎擾
//
unsigned char* CommData::GetData()
{
	return _data;
}
//����M�f�[�^�T�C�Y�̎擾
//
int CommData::GetDataSize()
{
	return _dataSize;
}
//�C���X�^���X�̕���
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
