#pragma once

// includes
#include "user.h"

// defines
#define MUHELPER_STAGE_COUNT	5

// bot impletation
class MUHelper {
public:
	MUHelper() { }
	~MUHelper() { }

	void Load();
	void Init();
	void Read();
	
	void Start();
	void Run();

private:
}; 

// global instance
extern MUHelper g_MUHelper;