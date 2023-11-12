#pragma once
class CSimEvent
{
public:
	CSimEvent(void);
	~CSimEvent(void);

	EVENTID m_uiEventId;
	EVENTTYPE m_euEventType;
	TIMEUNT m_uiEventType;

	EVENTID m_uiAssociateEventId;


	list<EVENTID> m_lPreviousEventId;
	list<TIMEUNT> m_lPreviousTimeUnit;
};

