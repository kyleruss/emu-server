#ifndef MUTIMER_H
#define MUTIMER_H

#include "queuetimer.h"

class CMuTimer
{
public:
	CMuTimer();
	virtual ~CMuTimer();

	void SetMuTimer();
	BOOL GetTimerEnd();
	void SetTimerEnd();

	CQueueTimer m_QueueTimer1;
	CQueueTimer m_QueueTimer2;
private:
	BOOL m_bTimerEnd;
};

extern CMuTimer g_MuTimer;

#endif