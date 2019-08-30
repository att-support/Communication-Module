#pragma once

class CommExcp
{
public:
	CommExcp(const char* fmt, ...);
	virtual ~CommExcp();
	operator const char* () const { return _errMsg; }

protected:

	// Error message
	char _errMsg[1024];
};

