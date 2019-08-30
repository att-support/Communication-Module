#include "stdafx.h"
#include "CommLogger.h"
#include <Util/Path.h>

CommLogger CommLogger::_myInstance;
Logger* CommLogger::_logger = NULL;

//
// ÉçÉKÅ[
//

CommLogger::CommLogger()
{
}

CommLogger::~CommLogger()
{
	if (_logger != NULL) {
		delete _logger;
	}
}

void CommLogger::Init(const std::string& logFileNameSuffix, const std::string& logDirPath)
{
	if (_logger == NULL) {
		if (!Path::IsExist(logDirPath)) {
			Path::MakeDir(logDirPath);
		}
		_logger = new Logger(logFileNameSuffix, logDirPath);
	}
}

void CommLogger::Destroy()
{
	if (_logger == NULL) {
		delete _logger;
	}
}
