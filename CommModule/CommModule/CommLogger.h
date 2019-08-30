#pragma once

#include <Util/Logger.h>

#define LOG(fmt,...) \
	if(CommLogger::GetInstance()!=NULL) \
		CommLogger::GetInstance()->Logging(fmt, ## __VA_ARGS__)

//
// ÉçÉKÅ[
//

class CommLogger
{
public:
	static void Init(const std::string& logFileNameSuffix, const std::string& logDirPath);
	static void Destroy();
	static Logger* GetInstance() {
		return _logger;
	}

private:
	CommLogger();
	virtual ~CommLogger();
	static CommLogger _myInstance;
	static Logger* _logger;
};

