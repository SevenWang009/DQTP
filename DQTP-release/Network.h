#pragma once
#include "Node.h"
#include "Link.h"
#include "Demand.h"
#include "Task.h"
class CNetwork
{
public:
	CNetwork(void);
	~CNetwork(void);

public:
	vector<CNode> m_vAllNode;
	vector<CLink> m_vAllLink;
	vector<CDemand> m_vAllDemand;
	vector<CTask> m_vAllTask;

	map<pair<NODEID,NODEID>,LINKID> m_mNodePairToLink;

	vector<UINT> Throughput;
	vector<double> OverallMemoryUsage;

	UINT testNum;
	UINT generateNum;
	UINT totalTry;
	UINT m_uiCompleteSessions;
	double m_dSlotTry;

	UINT m_uiNodeNum;
	UINT m_uiLinkNum;

	UINT m_uiSlotId;
	//common functions
	bool ShortestPath(NODEID SourceId, NODEID SinkId, list<NODEID>& NodeList, list<LINKID>& LinkList);
	void InitAllWeightOne();
	WEIGHT CalculateNodeWeight(NODEID NodeId);
	WEIGHT CalculateLinkWeight(LINKID LinkId);
	void InitRoutingInfo();
	void UpdateRouting(DEMANDID demandId, list<NODEID>& NodeList, list<LINKID>& LinkList);
	void ConfigureNodeIndex();
	double JainIndex();
	void setdifference(list<LINKID>& link1, list<LINKID>& link2, list<LINKID>& res);


	//Centralized time slot
	void SetFairAllocation();
	UINT GetWindowSize(DEMANDID demandId);
	void CentralizedWindowSize();
	void CleanAllWinsize();

	UINT SimEntanglementLinkNum(LINKID LinkId, UINT winSize);
	bool SimEntanglePath(DEMANDID demandId);
	UINT SimTSCOneSession(DEMANDID demandId);
	UINT SimTSC(TIMEUNT UnitNum);

	void TSCCloseSession(DEMANDID demandId);
	UINT TSCEntanglementLinkOverEachLink();
	void SimTSC_TASK();
	void ResetNetworkResources();
	UINT TSCTeleportation(DEMANDID demandId, UINT connNum);

	UINT GetUsedMemoryNum();
	double MemoryUsage(DEMANDID demandId, vector<vector<double>>& entangleTime);
	void GenerateEntangleTime(UINT EntangleNum, vector<double>& entangleTime);

	double GetMemoryUtilization();
	UINT GetAllMemory();

	double GetMemoryUtilizationTASK(UINT slot);



	//Random entanglement allocation
	void CleanAllDesiredEntangle();
	UINT EntanglementLinkOverEachLink();
	bool TryAssignOneEntanglement(LINKID linkId);
	void SimOneUnitEntanglement();
	void RefreshCarriedDemand();
	void EntanglementAllocation();
	double WasteEntangleTime(DEMANDID demandId, vector<vector<double>>& entangleTime);

	UINT SimREAOneSession(DEMANDID demandId);
	double CalculateMemoryUsage(TIMEUNT slot, UINT totalUsedMemory);

	UINT SimREA(TIMEUNT UnitNum);


	//NR-DQTP
	void InitializeWindows();
	bool IncreaseWindow(DEMANDID demandId);
	void UpdateResourceUtilization(DEMANDID demandId);
	void CutWindow(DEMANDID demandId);
	void ReleaseResource(DEMANDID demandId, UINT reduceSize);
	bool CheckFeasibility(DEMANDID demandId, DEMANDID& reduceDemand, NODEID& nodeId, LINKID& linkId);
	DEMANDID FindCutDemandOverNode(NODEID nodeId);
	DEMANDID FindCutDemandOverLink(LINKID linkId);
	void RecordWindowSizes();
	double AverageJainIndex();
	double AverageSessionCompletionTime();


	//代码需要保证所有的entanglement都和connection同向
	void InitEntanglementAssignment();
	bool ConnectTwoEntanglement(DEMANDID DemandId, UINT connID, list<CEntanglement>::iterator entangle1, list<CEntanglement>::iterator entangle2);//connect entangle2 into entangle1
	list<CEntanglement>::iterator DestroyTwoEntanglement(DEMANDID DemandId, UINT connID, list<CEntanglement>::iterator entangle1, list<CEntanglement>::iterator entangle2);//destroy both entangles when swapping fails
	void ReleaseOneEntanglement(DEMANDID demandId, list<CEntanglement>::iterator entangle);
	void ReleaseOnePath(DEMANDID demandId, UINT pathId);
	void UpdateConnection(DEMANDID demandId, UINT orignalWin, UINT newWin);
	UINT TryEntangleNum(DEMANDID demandId, map<LINKID,UINT>& entangleNum);//return the number of used memory
	UINT MemroyHostEntanglement(DEMANDID demandId);//calculate the memory used to host existing entanglements
	void GenerateEntangle(map<LINKID,UINT>& tryNum, map<LINKID,UINT>& successEntangle);
	bool InsertNewSegment(DEMANDID demandId, UINT connId, LINKID linkId);
	bool AssignOneEntangleToConnection(DEMANDID demandId, LINKID linkId);//true for successfully used, false for not used
	void AssignUnusedEntangleToConnection(DEMANDID demandId);
	void ConcreteAssignment(DEMANDID demandId, UINT connId, LINKID linkId);
	void AssignEntangleToConnection(DEMANDID demandId, map<LINKID,UINT>& successEntangle);
	bool AdjustOneConnection(DEMANDID demandId, UINT connId);// true for e2e connection, and false for otherwise
	bool AdjustOneDemandConnection(DEMANDID demandId);//true for close a session, while false for continue
	void AdjustConnections();
	void ReleaseOneSuccessConnection(DEMANDID demandId, UINT connId);
	void InitializeNRDQTP();
	UINT TimeUnitMemory();
	void SimNRDQTPOneSlot(TIMEUNT slot);
	bool SimNRDTPOneSession(DEMANDID demandId, TIMEUNT slot);
	UINT GetNRDQTPThroughput();
	double DQTPMemoryUtilization(TIMEUNT UnitNum);
	UINT SimNRDQTP(TIMEUNT UnitNum);

	void CloseSession(DEMANDID demandId);
	void RestartSession(DEMANDID demandId);
	void RestartTask(TASKID taskId);


	bool NegativeRemainingMemory();
	bool CheckMemory();
	void RestartSessionTSC(DEMANDID demandId);
	void RestartTaskTSC(TASKID taskId);

	void UsedByEntanglements(DEMANDID demandId, map<NODEID,UINT>& usedTime);
	void SingleHopEntanglements(DEMANDID demandId, map<LINKID,UINT>& usedTime);

	void RecordWorkingWindow();
	void AveragingWindowSize();
	TIMEUNT SimNRDQTP_TASK();
	double AverageTaskCompletionTime();
	bool AdjustOneDemandConnection_TASK(DEMANDID demandId);
	bool AdjustTaskConnections(TASKID taskId, TIMEUNT slot);
	void ReleaseOneSuccessConnection_TASK(DEMANDID demandId, UINT connId);
	bool SimNRDTPOneTASK(TASKID taskId, TIMEUNT slot);
	
	//DQTP
	void ChangeParameterForDQTP();
	TIMEUNT SimDQTP_TASK();



	//AEPR
	void RecoverResources();
	void SetWeightForAEPR(DEMANDID demandId);
	bool RouteOneDemand(DEMANDID demandId);
	bool AllDemandComplete(TASKID taskId);
	void SimulateOneTask(TASKID taskId);
	void SimulateOneSlot(TASKID taskId);
	void SimulateAEPR();
};

