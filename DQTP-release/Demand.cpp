#include "StdAfx.h"
#include "Demand.h"


CDemand::CDemand(void)
{
	m_uiTeleportedQubit=0;
	m_uiSuccessSent=0;
	m_uiWindowSize=1;
	m_euWinState=SS;
	m_uiTotalMemory=0;
	m_uiTotalThroughput=0;
	m_uiTotalWinSize=0;
	m_uiWorkingSlot=0;
	m_bClosed=false;
	m_uiFinishTime=0;
}


CDemand::~CDemand(void)
{
}


CDemand::CDemand(const CDemand& Demand)
{
	m_uiSourceId=Demand.m_uiSourceId;
	m_uiSinkId=Demand.m_uiSinkId;
	m_uiDemandId=Demand.m_uiDemandId;
	m_uiQubitNum=Demand.m_uiQubitNum;
	m_uiTeleportedQubit=Demand.m_uiTeleportedQubit;
	m_uiRemainQubit=Demand.m_uiRemainQubit;
	m_uiWindowSize=Demand.m_uiWindowSize;
	m_uiTotalWinSize=Demand.m_uiTotalWinSize;
	m_uiTotalThroughput=Demand.m_uiTotalThroughput;
	m_uiSuccessSent=Demand.m_uiSuccessSent;
	m_uiHomeTask=Demand.m_uiHomeTask;
	m_uiWorkingSlot=Demand.m_uiWorkingSlot;
	m_uiTotalMemory=Demand.m_uiTotalMemory;
	m_euWinState=Demand.m_euWinState;
	m_lPathNode=Demand.m_lPathNode;
	m_lPathLink=Demand.m_lPathLink;
	m_mSuccessLinks=Demand.m_mSuccessLinks;
	m_mBreakPoints=Demand.m_mBreakPoints;
	m_bClosed=Demand.m_bClosed;
	m_uiFinishTime=Demand.m_uiFinishTime;
	m_dAverageWinSize=Demand.m_dAverageWinSize;

	m_mEntanglementPath=Demand.m_mEntanglementPath;
	m_mCoveredLinks=Demand.m_mCoveredLinks;
	m_mUnusedEntanglement=Demand.m_mUnusedEntanglement;


	m_mNodeToIndex=Demand.m_mNodeToIndex;
	m_mAvailableMemory=Demand.m_mAvailableMemory;
}


void CDemand::operator=(const CDemand& Demand)
{
	m_uiSourceId=Demand.m_uiSourceId;
	m_uiSinkId=Demand.m_uiSinkId;
	m_uiDemandId=Demand.m_uiDemandId;
	m_uiQubitNum=Demand.m_uiQubitNum;
	m_uiTeleportedQubit=Demand.m_uiTeleportedQubit;
	m_uiRemainQubit=Demand.m_uiRemainQubit;
	m_uiWindowSize=Demand.m_uiWindowSize;
	m_uiTotalWinSize=Demand.m_uiTotalWinSize;
	m_uiTotalThroughput=Demand.m_uiTotalThroughput;
	m_uiSuccessSent=Demand.m_uiSuccessSent;
	m_uiWorkingSlot=Demand.m_uiWorkingSlot;
	m_uiHomeTask=Demand.m_uiHomeTask;
	m_uiTotalMemory=Demand.m_uiTotalMemory;
	m_euWinState=Demand.m_euWinState;
	m_lPathNode=Demand.m_lPathNode;
	m_lPathLink=Demand.m_lPathLink;
	m_mSuccessLinks=Demand.m_mSuccessLinks;
	m_mBreakPoints=Demand.m_mBreakPoints;
	m_bClosed=Demand.m_bClosed;
	m_uiFinishTime=Demand.m_uiFinishTime;
	m_dAverageWinSize=Demand.m_dAverageWinSize;

	m_mEntanglementPath=Demand.m_mEntanglementPath;
	m_mCoveredLinks=Demand.m_mCoveredLinks;
	m_mUnusedEntanglement=Demand.m_mUnusedEntanglement;


	m_mNodeToIndex=Demand.m_mNodeToIndex;
	m_mAvailableMemory=Demand.m_mAvailableMemory;
}