#pragma once

#include "stdafx.h"

#define THREAD_LOCK_START(Section)	(EnterCriticalSection(Section))
#define THREAD_LOCK_END(Section)	(LeaveCriticalSection(Section))
#define THREAD_LOCK_CREATE(Section)	(InitializeCriticalSection(Section));
#define THREAD_LOCK_DELETE(Section) (DeleteCriticalSection(Section))