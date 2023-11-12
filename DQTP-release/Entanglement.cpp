#include "StdAfx.h"
#include "Entanglement.h"


CEntanglement::CEntanglement(void)
{
}


CEntanglement::~CEntanglement(void)
{
}


CEntanglement::CEntanglement(const CEntanglement& Entanglement)
{
	m_uiSourceId=Entanglement.m_uiSourceId;
	m_uiSinkId=Entanglement.m_uiSinkId;
	m_uiCreateEventId=Entanglement.m_uiCreateEventId;
	m_uiDestroyEventId=Entanglement.m_uiDestroyEventId;
	m_uiHomeDemand=Entanglement.m_uiHomeDemand;
	m_uiHomePathId=Entanglement.m_uiHomePathId;
	m_euEntangleType=Entanglement.m_euEntangleType;

	m_lNodeList=Entanglement.m_lNodeList;
	m_lLinkList=Entanglement.m_lLinkList;
}

void CEntanglement::operator=(const CEntanglement& Entanglement)
{
	m_uiSourceId=Entanglement.m_uiSourceId;
	m_uiSinkId=Entanglement.m_uiSinkId;
	m_uiCreateEventId=Entanglement.m_uiCreateEventId;
	m_uiDestroyEventId=Entanglement.m_uiDestroyEventId;
	m_uiHomeDemand=Entanglement.m_uiHomeDemand;
	m_uiHomePathId=Entanglement.m_uiHomePathId;
	m_euEntangleType=Entanglement.m_euEntangleType;

	m_lNodeList=Entanglement.m_lNodeList;
	m_lLinkList=Entanglement.m_lLinkList;
}