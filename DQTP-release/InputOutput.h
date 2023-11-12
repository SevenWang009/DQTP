#pragma once
#include "Network.h"

class CInputOutput
{
public:
	CInputOutput(void);
	~CInputOutput(void);
	CInputOutput(CNetwork* Network);
	CNetwork* m_pNetwork;

	map<TASKID,list<DEMANDID>> m_mtaskDemands;

	void GenerateInput();
	void GenerateNodePairs(UINT nodenum, UINT linknum, list<pair<NODEID,NODEID>>& pairs);
	void GenerateNodeInfo(UINT nodeNum, double prob, double teleProb, UINT minMemory, UINT maxMemory);
	void GenerateLinkInfo(UINT linkNum, UINT nodeNum, double prob, UINT slotTry, UINT minChannle, UINT maxChannel);
	void GenrateDemandInfo(UINT nodeNum, UINT demandNum, UINT taskNum, UINT minDemand, UINT maxDemand);
	void GenerateSDPairs(UINT nodeNum, UINT demandNum, UINT taskNum, map<TASKID, list<pair<NODEID,NODEID>>>& SDpairs);

	void ReadTopology();
	void ReadNodeInfo();
	void ReadLinkInfo();
	void ReadDemandInfo();
	void ReadInput();
	void OutputCompletionTime();
	void OutputMemoryUtilization();
	void OutputFairness();





	// code for AEPR
	void ReadBasicInfo();
	void GenerateNetworkTopology(UINT memory, double prob, double aggreeProb);
	void GenerateQuantumNodes(UINT memory);
	void GenerateQuantumLinks();
	void GenerateTrafficMatrix(UINT demandNum, UINT taskNum);
	void ScaleTrafficMatrix(TASKID taskId);
	void ScaleDownDemands(NODEID nodeId, list<DEMANDID>& allDemands, map<DEMANDID,UINT>& newDemand);

	double OutputATCT();
};

