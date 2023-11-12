#pragma once
class CNode
{
public:
	CNode(void);
	~CNode(void);
	CNode(const CNode& Node);

	void operator=(const CNode& Node);


	NODEID m_uiNodeId;
	double m_dSwapProb;
	double m_dTeleProb;

	UINT m_uiMemory;
	UINT m_uiUsedMemory;
	UINT m_uiRemainMemory;

	WEIGHT m_dWeight;

	list<NODEID> m_lAdjNode;
	list<LINKID> m_lAdjLink;

	map<DEMANDID,UINT> m_mCarriedDemand;//record the number of memory assigned to each demand

	UINT m_uiFairMemoryAllocation;

};

