// QueueTimer.cpp: implementation of the CQueueTimer class.
// 1.01.00
// strange std::stack compilation - need to check later
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "queuetimer.h"

// Constructor / Destructor

CQueueTimer::CQueueTimer()
{
	this->m_callback = 0;
	this->QueueTimerInit();
}

void CQueueTimer::RegCallBack(WAITORTIMERCALLBACK callback)
{
	this->m_callback = callback;
}

CQueueTimer::~CQueueTimer()
{
	if( this->m_timerqueue != INVALID_HANDLE_VALUE )
	{
		this->DeleteQueueTimer();
	}

	while( this->m_stack.empty() == 0 )
		this->m_stack.pop();
}

void CQueueTimer::QueueTimerInit()
{
	this->m_timerqueue = CreateTimerQueue();
}

void CQueueTimer::PushEventID(int eventID)
{
	this->m_stack.push(eventID);
}

void CQueueTimer::PopEventID()
{
	this->m_stack.pop();
}

int* CQueueTimer::GetEvenetID()
{
	return &this->m_stack.top();
}

int CQueueTimer::CreateTimer(int eventID,unsigned long period,unsigned long flags)
{
	HANDLE hTimer = INVALID_HANDLE_VALUE;
	int* tmpEventID;
	int result;

	this->PushEventID(eventID);
	tmpEventID = this->GetEvenetID();

	result = CreateTimerQueueTimer(&hTimer,this->m_timerqueue,(WAITORTIMERCALLBACK)this->m_callback,tmpEventID,1000,period,flags);

	if( !result )
		this->PopEventID();

return result;
}

void CQueueTimer::DeleteQueueTimer()
{
	DeleteTimerQueue(this->m_timerqueue);
	this->m_timerqueue = INVALID_HANDLE_VALUE;
}

void CQueueTimer::SyncLock()
{
	this->m_sync.Lock();
}

void CQueueTimer::SyncUnlock()
{
	this->m_sync.Unlock();
}