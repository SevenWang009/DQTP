#pragma once
#include "Entanglement.h"
class CDemand
{
public:
	CDemand(void);
	~CDemand(void);
	CDemand(const CDemand& Demand);
	void operator=(const CDemand& Demand);

	NODEID m_uiSourceId;
	NODEID m_uiSinkId;
	DEMANDID m_uiDemandId;
	UINT m_uiQubitNum;
	UINT m_uiRemainQubit;
	UINT m_uiTeleportedQubit;
	UINT m_uiWindowSize;
	UINT m_uiTotalThroughput;
	UINT m_uiFinishTime;

	UINT m_uiTotalMemory;

	UINT m_uiHomeTask;

	bool m_bClosed;

	//control window
	WINSTATE m_euWinState;
	UINT m_uiSuccessSent;//success qubit in current window
	UINT m_uiTotalWinSize;
	UINT m_uiWorkingSlot;
	double m_dAverageWinSize;

	list<NODEID> m_lPathNode;
	list<LINKID> m_lPathLink;

	multimap<UINT,list<LINKID>, greater<UINT>> m_mSuccessLinks;
	multimap<UINT,list<NODEID>> m_mBreakPoints;

	map<UINT,list<CEntanglement>> m_mEntanglementPath;
	map<UINT,list<LINKID>> m_mCoveredLinks;

	map<LINKID,UINT> m_mUnusedEntanglement;

	vector<double> m_vTotalMemoryUsage;//for TSC
	vector<double> m_vTotalMemoryWaste;//for REA


	//for DQTP
	map<NODEID,UINT> m_mNodeToIndex;
	map<NODEID,UINT> m_mAvailableMemory; // memory already assigned to this demand

};

