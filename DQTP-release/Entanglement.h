#pragma once
class CEntanglement
{
public:
	CEntanglement(void);
	~CEntanglement(void);
	CEntanglement(const CEntanglement& Entanglement);
	void operator=(const CEntanglement& Entanglement);


	NODEID m_uiSourceId;
	NODEID m_uiSinkId;
	list<NODEID> m_lNodeList;
	list<LINKID> m_lLinkList;

	EVENTID m_uiCreateEventId;
	EVENTID m_uiDestroyEventId;

	DEMANDID m_uiHomeDemand;
	PATHID m_uiHomePathId;

	ENTANGLETYPE m_euEntangleType;
};

