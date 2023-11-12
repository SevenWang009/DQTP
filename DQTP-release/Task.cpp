#include "StdAfx.h"
#include "Task.h"


CTask::CTask(void)
{
	m_bFinished=false;
	m_uiCompleteSlot=0;
}


CTask::~CTask(void)
{
}


CTask::CTask(const CTask& task)
{
	m_uiTaskId=task.m_uiTaskId;
	m_uiStartTime=task.m_uiStartTime;
	m_uiFinishTime=task.m_uiFinishTime;
	m_lDemands=task.m_lDemands;
	m_bFinished=task.m_bFinished;


	m_uiCompleteSlot=task.m_uiCompleteSlot;
}


void CTask::operator=(const CTask& task)
{
	m_uiTaskId=task.m_uiTaskId;
	m_uiStartTime=task.m_uiStartTime;
	m_uiFinishTime=task.m_uiFinishTime;
	m_lDemands=task.m_lDemands;
	m_bFinished=task.m_bFinished;

	m_uiCompleteSlot=task.m_uiCompleteSlot;
}