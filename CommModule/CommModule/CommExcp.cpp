#include "stdafx.h"
#include <stdio.h>
#include "CommExcp.h"


CommExcp::CommExcp(const char* fmt, ...)
{
	va_list  arglist;
	va_start(arglist, fmt);
	vsnprintf(_errMsg, sizeof(_errMsg) - 1, fmt, arglist);
	va_end(arglist);
}

CommExcp::~CommExcp()
{
}
