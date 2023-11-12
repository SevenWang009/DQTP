#include "StdAfx.h"
#include "Link.h"


CLink::CLink(void)
{
	m_uiFairChannelAllocation=0;
	m_uiSuccessEntangle=0;
	m_uiEntanglement=0;
	m_uiFairChannelAllocation=0;
	m_dWeight=1.0;
}


CLink::~CLink(void)
{
}

CLink::CLink(const CLink& Link)
{
	m_uiSourceId=Link.m_uiSourceId;
	m_uiSinkId=Link.m_uiSinkId;
	m_uiLinkId=Link.m_uiLinkId;
	m_dLinkProb=Link.m_dLinkProb;
	m_dAggProb=Link.m_dAggProb;
	m_uiChannelNum=Link.m_uiChannelNum;
	m_uiUsedChannel=Link.m_uiUsedChannel;
	m_uiRemainingChannel=Link.m_uiRemainingChannel;
	m_uiEntanglement=Link.m_uiEntanglement;
	m_uiSuccessEntangle=Link.m_uiSuccessEntangle;
	m_uiOccupiedEntanglement=Link.m_uiOccupiedEntanglement;

	m_mCarriedDemand=Link.m_mCarriedDemand;
	m_vCarriedDemand=Link.m_vCarriedDemand;
	m_dWeight=Link.m_dWeight;
	m_uiFairChannelAllocation=Link.m_uiFairChannelAllocation;
}

void CLink::operator=(const CLink& Link)
{
	m_uiSourceId=Link.m_uiSourceId;
	m_uiSinkId=Link.m_uiSinkId;
	m_uiLinkId=Link.m_uiLinkId;
	m_dLinkProb=Link.m_dLinkProb;
	m_dAggProb=Link.m_dAggProb;
	m_uiChannelNum=Link.m_uiChannelNum;
	m_uiUsedChannel=Link.m_uiUsedChannel;
	m_uiRemainingChannel=Link.m_uiRemainingChannel;
	m_uiEntanglement=Link.m_uiEntanglement;
	m_uiSuccessEntangle=Link.m_uiSuccessEntangle;
	m_uiOccupiedEntanglement=Link.m_uiOccupiedEntanglement;

	m_mCarriedDemand=Link.m_mCarriedDemand;
	m_vCarriedDemand=Link.m_vCarriedDemand;
	m_dWeight=Link.m_dWeight;
	m_uiFairChannelAllocation=Link.m_uiFairChannelAllocation;
}