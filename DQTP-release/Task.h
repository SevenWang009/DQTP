#pragma once
class CTask
{
public:
	CTask(void);
	~CTask(void);

	CTask(const CTask& task);
	void operator=(const CTask& task);

	list<DEMANDID> m_lDemands;

	TASKID m_uiTaskId;

	bool m_bFinished;
	TIMEUNT m_uiStartTime;
	TIMEUNT m_uiFinishTime;

	UINT m_uiCompleteSlot;
};

