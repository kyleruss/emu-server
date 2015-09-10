#ifndef QUEUETIMER_H
#define QUEUETIMER_H

#include "..\common\TSync.h"


class CQueueTimer
{
public:
	CQueueTimer();
	void RegCallBack(WAITORTIMERCALLBACK callback);

	virtual ~CQueueTimer();

public:
	void QueueTimerInit();
	void PushEventID(int eventID);
	void PopEventID();
	int* GetEvenetID();
	int CreateTimer(int eventID,unsigned long period,unsigned long flags);
	void DeleteQueueTimer();

	void SyncLock();
	void SyncUnlock();

private:
	WAITORTIMERCALLBACK m_callback;// 4
	HANDLE				m_timerqueue;	// 8
	std::stack<int>		m_stack;	// C
	TSync				m_sync;		// 3C
};

#endif