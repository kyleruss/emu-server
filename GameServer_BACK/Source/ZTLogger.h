#pragma once

#include "stdafx.h"
#include "ZTExpress.h"

// log manager class impletation
class ZTLogger {
public:
	ZTLogger(const char* Path) {
		m_FilePath.assign(Path);
		Initialize();
	};
	
	~ZTLogger() {
		THREAD_LOCK_DELETE(&m_CriticalSection);
	};
	
	void Initialize() {
		CreateDirectory(m_FilePath.c_str(), 0);
		THREAD_LOCK_CREATE(&m_CriticalSection);
	}

	void addLog(const char* Text, ...) {
		char DateTimePath[MAX_PATH];
		m_TextBuffer.clear();
		va_list ArgList;
		SYSTEMTIME TimeStamp;

		THREAD_LOCK_START(&m_CriticalSection);

		va_start(ArgList, Text);
		int Size = _vscprintf(Text, ArgList);
		m_TextBuffer.resize(Size++);
		vsnprintf_s((char*)m_TextBuffer.data(), Size, _TRUNCATE, Text, ArgList);
		va_end(ArgList);

		GetLocalTime(&TimeStamp);
		wsprintf(DateTimePath, "%s\\%04d%02d%02d.log",
			m_FilePath.c_str(), TimeStamp.wYear, TimeStamp.wMonth, TimeStamp.wDay);

		if ((m_FileStream = fopen(DateTimePath, "a+")) == 0) {
			THREAD_LOCK_END(&m_CriticalSection);
			return;
		}
		
		fprintf(m_FileStream, "[%02d:%02d:%02d] %s\n", 
			TimeStamp.wHour, TimeStamp.wMinute, TimeStamp.wSecond, m_TextBuffer.c_str());
		fclose(m_FileStream);

		THREAD_LOCK_END(&m_CriticalSection);
	}

private:
	FILE* m_FileStream;
	std::string m_FilePath;
	std::string m_TextBuffer;
	CRITICAL_SECTION m_CriticalSection;
};

extern ZTLogger LuaLog;