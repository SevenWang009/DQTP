#pragma once
class CLink
{
public:
	CLink(void);
	~CLink(void);
	CLink(const CLink& Link);

	void operator=(const CLink& Link);

	NODEID m_uiSourceId;
	NODEID m_uiSinkId;

	LINKID m_uiLinkId;

	double m_dLinkProb;
	double m_dAggProb;

	UINT m_uiChannelNum;
	UINT m_uiUsedChannel;
	UINT m_uiRemainingChannel;

	UINT m_uiEntanglement; //desired
	UINT m_uiOccupiedEntanglement;//existing


	UINT m_uiSuccessEntangle;

	map<DEMANDID,UINT> m_mCarriedDemand;
	vector<DEMANDID> m_vCarriedDemand;


	WEIGHT m_dWeight;
	UINT m_uiFairChannelAllocation;



	map<DEMANDID,UINT> m_mEntangleForEachDemand;

};

