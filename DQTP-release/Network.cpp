#include "StdAfx.h"
#include "Network.h"


CNetwork::CNetwork(void)
{
	m_uiCompleteSessions=0;
	testNum=0;
	generateNum=0;
	m_uiSlotId=0;
}


CNetwork::~CNetwork(void)
{
}


bool CNetwork::ShortestPath(NODEID SourceId, NODEID SinkId, list<NODEID>& NodeList, list<LINKID>& LinkList)
{
	UINT NodeNum=m_vAllNode.size();
	vector<WEIGHT> DisVector(NodeNum,INF);
	vector<NODEID> PreNode(NodeNum,SourceId);
	vector<bool> Visited(NodeNum,false);
	DisVector[SourceId]=0;
	Visited[SourceId]=true;
	NODEID CurrentNode=SourceId;
	while(CurrentNode!=SinkId)
	{
		list<NODEID>::iterator adjNodeIter;
		adjNodeIter=m_vAllNode[CurrentNode].m_lAdjNode.begin();
		for (;adjNodeIter!=m_vAllNode[CurrentNode].m_lAdjNode.end();adjNodeIter++)
		{
			LINKID medianLink=m_mNodePairToLink[make_pair(CurrentNode,*adjNodeIter)];
			if (DisVector[CurrentNode]+m_vAllNode[*adjNodeIter].m_dWeight+m_vAllLink[medianLink].m_dWeight < DisVector[*adjNodeIter])
			{
				DisVector[*adjNodeIter]=DisVector[CurrentNode]+m_vAllNode[*adjNodeIter].m_dWeight+m_vAllLink[medianLink].m_dWeight;
				PreNode[*adjNodeIter]=CurrentNode;
			}
		}
		//label next node
		WEIGHT minDis=INF;
		for (NODEID NodeId=0;NodeId<NodeNum;NodeId++)
		{
			if (Visited[NodeId]) continue;
			if (DisVector[NodeId] < minDis)
			{
				minDis=DisVector[NodeId];
				CurrentNode=NodeId;
			}
		}
		if (minDis>=INF) return false;
		Visited[CurrentNode]=true;
	}
	CurrentNode=SinkId;
	while(CurrentNode!=SourceId)
	{
		NODEID Previous=PreNode[CurrentNode];
		LINKID medianLink=m_mNodePairToLink[make_pair(Previous,CurrentNode)];
		NodeList.push_front(CurrentNode);
		LinkList.push_front(medianLink);
		CurrentNode=Previous;
	}
	NodeList.push_front(CurrentNode);
	return true;
}


void CNetwork::InitAllWeightOne()
{
	vector<CNode>::iterator NodeIter;
	NodeIter=m_vAllNode.begin();
	for (;NodeIter!=m_vAllNode.end();NodeIter++)
	{
		NodeIter->m_dWeight=1.0;
	}
	vector<CLink>::iterator LinkIter;
	LinkIter=m_vAllLink.begin();
	for (;LinkIter!=m_vAllLink.end();LinkIter++)
	{
		LinkIter->m_dWeight=1.0;
	}
}

WEIGHT CNetwork::CalculateNodeWeight(NODEID NodeId)
{
	UINT DemandNum=m_vAllNode[NodeId].m_mCarriedDemand.size();
	UINT Memory=m_vAllNode[NodeId].m_uiMemory;
	return 1.0+(DemandNum*1.0)/(Memory*1.0);
}

WEIGHT CNetwork::CalculateLinkWeight(LINKID LinkId)
{
	UINT DemandNum=m_vAllLink[LinkId].m_mCarriedDemand.size();
	UINT ChannelNum=m_vAllLink[LinkId].m_uiChannelNum;
	return 1.0+(DemandNum*1.0)/(ChannelNum*1.0);
}

void CNetwork::UpdateRouting(DEMANDID demandId, list<NODEID>& NodeList, list<LINKID>& LinkList)
{
	m_vAllDemand[demandId].m_lPathNode=NodeList;
	m_vAllDemand[demandId].m_lPathLink=LinkList;
	list<NODEID>::iterator nodeIter;
	nodeIter=NodeList.begin();
	for (;nodeIter!=NodeList.end();nodeIter++)
	{
		m_vAllNode[*nodeIter].m_mCarriedDemand[demandId]=0;
		WEIGHT nodeWeight=CalculateNodeWeight(*nodeIter);
		m_vAllNode[*nodeIter].m_dWeight=nodeWeight;
	}
	list<LINKID>::iterator linkIter;
	linkIter=LinkList.begin();
	for (;linkIter!=LinkList.end();linkIter++)
	{
		m_vAllLink[*linkIter].m_mCarriedDemand[demandId]=0;
		m_vAllLink[*linkIter].m_vCarriedDemand.push_back(demandId);
		WEIGHT linkWeight=CalculateLinkWeight(*linkIter);
		m_vAllLink[*linkIter].m_dWeight=linkWeight;
	}
}


void CNetwork::ConfigureNodeIndex()
{
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		UINT idx=0;
		list<NODEID>::iterator nodeIter;
		nodeIter=demandIter->m_lPathNode.begin();
		for (;nodeIter!=demandIter->m_lPathNode.end();nodeIter++)
		{
			demandIter->m_mNodeToIndex[*nodeIter]=idx;
			idx++;
		}
	}
}

void CNetwork::InitRoutingInfo()
{
	cout<<"Initialize routing information..."<<endl;
	InitAllWeightOne();
	vector<CDemand>::iterator DemandIter;
	DemandIter=m_vAllDemand.begin();
	for (;DemandIter!=m_vAllDemand.end();DemandIter++)
	{
		NODEID sourceId=DemandIter->m_uiSourceId;
		NODEID sinkId=DemandIter->m_uiSinkId;
		list<NODEID> NodeList;
		list<LINKID> LinkList;
		bool success=ShortestPath(sourceId,sinkId,NodeList,LinkList);
		UpdateRouting(DemandIter->m_uiDemandId, NodeList, LinkList);
	}
	ConfigureNodeIndex();
}

void CNetwork::SetFairAllocation()
{
	vector<CNode>::iterator nodeIter;
	nodeIter=m_vAllNode.begin();
	for (;nodeIter!=m_vAllNode.end();nodeIter++)
	{
		if (nodeIter->m_mCarriedDemand.empty())
		{
			continue;
		}
		UINT memory=nodeIter->m_uiMemory;
		UINT session=nodeIter->m_mCarriedDemand.size();
		nodeIter->m_uiFairMemoryAllocation=(memory/session);
	}
	vector<CLink>::iterator linkIter;
	linkIter=m_vAllLink.begin();
	for (;linkIter!=m_vAllLink.end();linkIter++)
	{
		if (linkIter->m_mCarriedDemand.empty())
		{
			continue;
		}
		UINT channel=linkIter->m_uiChannelNum;
		UINT session=linkIter->m_mCarriedDemand.size();
		linkIter->m_uiFairChannelAllocation=(channel/session);
	}
}

UINT CNetwork::GetWindowSize(DEMANDID demandId)
{
	UINT winSize=INFUINT;
	list<NODEID>::iterator nodeIter;
	nodeIter=m_vAllDemand[demandId].m_lPathNode.begin();
	for (;nodeIter!=m_vAllDemand[demandId].m_lPathNode.end();nodeIter++)
	{
		if (m_vAllNode[*nodeIter].m_uiFairMemoryAllocation < winSize)
		{
			winSize = m_vAllNode[*nodeIter].m_uiFairMemoryAllocation;
		}
	}

	list<LINKID>::iterator linkIter;
	linkIter=m_vAllDemand[demandId].m_lPathLink.begin();
	for (;linkIter!=m_vAllDemand[demandId].m_lPathLink.end();linkIter++)
	{
		if (m_vAllLink[*linkIter].m_uiFairChannelAllocation < winSize)
		{
			winSize = m_vAllLink[*linkIter].m_uiFairChannelAllocation;
		}
	}
	m_vAllDemand[demandId].m_uiWindowSize = winSize;
	return winSize;
}

void CNetwork::CentralizedWindowSize()
{
	SetFairAllocation();
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		UINT winsize=GetWindowSize(demandIter->m_uiDemandId);
		demandIter->m_uiWindowSize=winsize;
	}
}


UINT CNetwork::SimEntanglementLinkNum(LINKID LinkId, UINT winSize)
{
	UINT num=0;
	for (UINT i=0;i<winSize;i++)
	{
		double rannum=(rand()%10000)/10000.0;
		if (rannum<m_vAllLink[LinkId].m_dAggProb)
			num++;
	}
	return num;
}

bool CNetwork::SimEntanglePath(DEMANDID demandId)
{
	list<NODEID>::iterator nodeIter, tailIter;;
	nodeIter=m_vAllDemand[demandId].m_lPathNode.begin();
	nodeIter++;
	tailIter=m_vAllDemand[demandId].m_lPathNode.end();
	tailIter--;
	for (;nodeIter!=tailIter;nodeIter++)
	{
		double rannum=(rand()%10000)/10000.0;
		if (rannum>m_vAllNode[*nodeIter].m_dSwapProb)
			return false;
	}
	return true;
}

void CNetwork::GenerateEntangleTime(UINT EntangleNum, vector<double>& entangleTime)
{
	for (UINT i=0;i<EntangleNum;i++)
	{
		double oneTime=(rand()%10000)/10000.0;
		entangleTime.push_back(oneTime);
	}
	sort(entangleTime.begin(),entangleTime.end());
}

double CNetwork::MemoryUsage(DEMANDID demandId, vector<vector<double>>& entangleTime)
{
	double totalMemory=m_vAllDemand[demandId].m_lPathLink.size() * 2 * m_vAllDemand[demandId].m_uiWindowSize * 1.0;
	double wasteTime=0;
	for (UINT link1=0,link2=1; link2<entangleTime.size();link1++,link2++)
	{
		for (UINT idx1=0,idx2=0; idx1<entangleTime[link1].size() && idx2<entangleTime[link2].size(); idx1++, idx2++)
		{
			double swappingTime=max(entangleTime[link1][idx1], entangleTime[link2][idx2]);
			wasteTime += (1-swappingTime);
		}
	}
	//teleportation save
	for (UINT idx=0;;idx++)
	{
		double teleTime=0;
		for (UINT link=0;link<entangleTime.size();link++)
		{
			if (idx>=entangleTime[link].size())
			{
				totalMemory -=wasteTime;
				return totalMemory;
			}
			if (entangleTime[link][idx] > teleTime)
			{
				teleTime=entangleTime[link][idx];
			}
		}
		wasteTime += 2*(1-teleTime);
	}
	totalMemory -=wasteTime;
	return totalMemory;
}


UINT CNetwork::SimTSCOneSession(DEMANDID demandId)
{
	vector<vector<double>> AllGeneratedTime;
	UINT qubitNum=0;
	UINT minEntangle=INFUINT;
	UINT winSize=m_vAllDemand[demandId].m_uiWindowSize;
	list<LINKID>::iterator linkIter;
	linkIter=m_vAllDemand[demandId].m_lPathLink.begin();
	for (;linkIter!=m_vAllDemand[demandId].m_lPathLink.end();linkIter++)
	{
		UINT Entangle=SimEntanglementLinkNum(*linkIter,winSize);
		vector<double> entangleTime;
		GenerateEntangleTime(Entangle,entangleTime);
		AllGeneratedTime.push_back(entangleTime);
		if (Entangle<minEntangle)
			minEntangle=Entangle;
	}
	for (UINT i=0;i<minEntangle;i++)
	{
		if (SimEntanglePath(demandId))
			qubitNum++;
	}
	double usedMemory=MemoryUsage(demandId,AllGeneratedTime);
	m_vAllDemand[demandId].m_vTotalMemoryUsage.push_back(usedMemory);
	return qubitNum;
}

UINT CNetwork::SimTSC(TIMEUNT UnitNum)
{
	CleanAllWinsize();
	TSCEntanglementLinkOverEachLink();
	UINT TotalThroughput=0;
	for (UINT slot=0; slot<UnitNum; slot++)
	{
		cout<<"Start TSC simulation of slot "<<slot<<endl;
		UINT goodput=0;
		UINT demandNum=m_vAllDemand.size();
		for (DEMANDID demandId=0;demandId<demandNum;demandId++)
		{
			UINT demandput=SimTSCOneSession(demandId);
			goodput += demandput;
		}
		Throughput.push_back(goodput);
		TotalThroughput+=goodput;
	}
	double memoryUtilization=GetMemoryUtilization();
	double fairness=JainIndex();
	ofstream fout;
	fout.open("TSC_result.txt");
	fout<<"Total throughput is "<<TotalThroughput<<endl;
	fout<<"Memory utilization is "<<memoryUtilization<<endl;
	fout<<"Fairness metrics (Jain's index) is "<<fairness<<endl;
	return TotalThroughput;
}

UINT CNetwork::TSCTeleportation(DEMANDID demandId, UINT connNum)
{
	UINT res=0;
	NODEID sourceId=m_vAllDemand[demandId].m_uiSourceId;
	for (UINT cnt=0; cnt<connNum;cnt++)
	{
		double prob=(rand()%10000*1.0)/10000.0;
		if (prob<m_vAllNode[sourceId].m_dTeleProb)
		{
			res++;
		}
	}
	return res;
}


void CNetwork::TSCCloseSession(DEMANDID demandId)
{
	m_vAllDemand[demandId].m_bClosed=true;
	UINT winSize=m_vAllDemand[demandId].m_uiWindowSize;

	list<LINKID>::iterator linkIter;
	linkIter=m_vAllDemand[demandId].m_lPathLink.begin();
	for (;linkIter!=m_vAllDemand[demandId].m_lPathLink.end();linkIter++)
	{
		NODEID sourceId=m_vAllLink[*linkIter].m_uiSourceId;
		NODEID sinkId=m_vAllLink[*linkIter].m_uiSinkId;

		m_vAllNode[sourceId].m_uiRemainMemory+=winSize;
		m_vAllNode[sourceId].m_uiUsedMemory-=winSize;
		m_vAllNode[sourceId].m_mCarriedDemand[demandId]-=winSize;

		m_vAllNode[sinkId].m_uiRemainMemory+=winSize;
		m_vAllNode[sinkId].m_uiUsedMemory-=winSize;
		m_vAllNode[sinkId].m_mCarriedDemand[demandId]-=winSize;

		m_vAllLink[*linkIter].m_uiRemainingChannel+=winSize;
		m_vAllLink[*linkIter].m_uiUsedChannel-=winSize;
		m_vAllLink[*linkIter].m_mCarriedDemand[demandId]-=winSize;
	}
	m_vAllDemand[demandId].m_uiWindowSize=0;
}

void CNetwork::SimTSC_TASK()
{
	TSCEntanglementLinkOverEachLink();
	UINT taskNum=m_vAllTask.size();
	UINT TotalThroughput=0;
	bool allFinish=false;
	UINT slot=0;
	while(!allFinish)
	{
		cout<<"Start simulation of TSC-TASK slot "<<slot<<endl;
		bool needUpdateWin=false;
		allFinish=true;
		for (TASKID taskId=0; taskId<taskNum;taskId++)
		{
			//if (slot==87)
			//{
			//	cout<<taskId<<"	";
			//}
			if (m_vAllTask[taskId].m_bFinished)
			{
				continue;
			}
			bool taskFinish=true;
			list<DEMANDID>::iterator demandIter;
			demandIter=m_vAllTask[taskId].m_lDemands.begin();
			for (;demandIter!=m_vAllTask[taskId].m_lDemands.end();demandIter++)
			{
				if (m_vAllDemand[*demandIter].m_bClosed)
				{
					continue;
				}
				UINT demandput=SimTSCOneSession(*demandIter);
				m_vAllDemand[*demandIter].m_uiTotalWinSize += demandput;
				m_vAllDemand[*demandIter].m_uiWorkingSlot++;
				UINT teleport=TSCTeleportation(*demandIter,demandput);
				if (teleport>=m_vAllDemand[*demandIter].m_uiRemainQubit)
				{
					m_vAllDemand[*demandIter].m_uiRemainQubit=0;
					m_vAllDemand[*demandIter].m_uiTeleportedQubit=m_vAllDemand[*demandIter].m_uiQubitNum;
					TSCCloseSession(*demandIter);
					m_vAllDemand[*demandIter].m_bClosed=true;
					needUpdateWin=true;
					continue;
				}else if (teleport < demandput)
				{//teleportation error occurs
					RestartTaskTSC(taskId);
					needUpdateWin=true;
					taskFinish=false;
					allFinish=false;
					break;
				}else
				{
					m_vAllDemand[*demandIter].m_uiRemainQubit -= teleport;
					m_vAllDemand[*demandIter].m_uiTeleportedQubit += teleport;
					taskFinish=false;
					allFinish=false;
				}
			}
			//if (demandIter!=m_vAllTask[taskId].m_lDemands.end())
			//{// task has been restarted
			//	continue;
			//}
			if (taskFinish)
			{
				m_vAllTask[taskId].m_bFinished=true;
				m_vAllTask[taskId].m_uiFinishTime=slot;
			}
		}
		if (needUpdateWin)
		{
			TSCEntanglementLinkOverEachLink();
		}
		RecordWorkingWindow();
		slot++;
	}
	double memoryUtilization=GetMemoryUtilizationTASK(slot);
	double fairness=AverageJainIndex();
	ofstream fout;
	fout.open("TSC_TASK_result.txt");
	fout<<"Average task completion time is "<<AverageTaskCompletionTime()<<endl;
	fout<<"Memory utilization is "<<memoryUtilization<<endl;
	fout<<"Fairness metrics (Jain's index) is "<<fairness<<endl;
}


double CNetwork::GetMemoryUtilizationTASK(UINT slot)
{
	double totalused=0;
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		vector<double>::iterator usedIter;
		usedIter=demandIter->m_vTotalMemoryUsage.begin();
		for (;usedIter!=demandIter->m_vTotalMemoryUsage.end();usedIter++)
		{
			totalused += (*usedIter);
		}
	}
	UINT totalMemory=GetAllMemory();
	return totalused/(totalMemory*slot);
}


UINT CNetwork::GetUsedMemoryNum()
{
	UINT used=0;
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		used += (demandIter->m_uiWindowSize)*(demandIter->m_lPathLink.size() * 2);
	}
	return used;
}

UINT CNetwork::GetAllMemory()
{
	UINT total=0;
	vector<CNode>::iterator nodeIter;
	nodeIter=m_vAllNode.begin();
	for (;nodeIter!=m_vAllNode.end();nodeIter++)
	{
		total += nodeIter->m_uiMemory;
	}
	return total;
}


void CNetwork::ResetNetworkResources()
{
	vector<CNode>::iterator nodeIter;
	nodeIter=m_vAllNode.begin();
	for (;nodeIter!=m_vAllNode.end();nodeIter++)
	{
		nodeIter->m_mCarriedDemand.clear();
		nodeIter->m_uiUsedMemory=0;
		nodeIter->m_uiRemainMemory=nodeIter->m_uiMemory;
	}
	vector<CLink>::iterator linkIter;
	linkIter=m_vAllLink.begin();
	for (;linkIter!=m_vAllLink.end();linkIter++)
	{
		linkIter->m_uiUsedChannel=0;
		linkIter->m_uiRemainingChannel=linkIter->m_uiChannelNum;
	}
}

UINT CNetwork::TSCEntanglementLinkOverEachLink()
{
	CleanAllWinsize();
	ResetNetworkResources();
	UINT UsedMemory=0;
	bool flag=true;
	while (flag)
	{
		flag=false;
		vector<CDemand>::iterator demandIter;
		demandIter=m_vAllDemand.begin();
		for (;demandIter!=m_vAllDemand.end();demandIter++)
		{
			if (demandIter->m_bClosed)
			{
				continue;
			}
			bool morePath=true;
			list<LINKID>::iterator linkIter;
			linkIter=demandIter->m_lPathLink.begin();
			for (;linkIter!=demandIter->m_lPathLink.end();linkIter++)
			{
				if (TryAssignOneEntanglement(*linkIter))
				{
					UINT sourceId=m_vAllLink[*linkIter].m_uiSourceId;
					UINT sinkId=m_vAllLink[*linkIter].m_uiSinkId;
					m_vAllNode[sourceId].m_mCarriedDemand[demandIter->m_uiDemandId]++;
					m_vAllNode[sinkId].m_mCarriedDemand[demandIter->m_uiDemandId]++;
					m_vAllLink[*linkIter].m_mCarriedDemand[demandIter->m_uiDemandId]++;
					UsedMemory+=2;
					flag=true;
					continue;
				}
				//return back all assigned resources
				list<LINKID>::iterator tempIter=demandIter->m_lPathLink.begin();
				for (;tempIter!=linkIter;tempIter++)
				{
					UINT sourceId=m_vAllLink[*tempIter].m_uiSourceId;
					UINT sinkId=m_vAllLink[*tempIter].m_uiSinkId;
					m_vAllNode[sourceId].m_mCarriedDemand[demandIter->m_uiDemandId]--;
					m_vAllNode[sinkId].m_mCarriedDemand[demandIter->m_uiDemandId]--;
					m_vAllLink[*linkIter].m_mCarriedDemand[demandIter->m_uiDemandId]--;
					m_vAllNode[sourceId].m_uiUsedMemory--;
					m_vAllNode[sourceId].m_uiRemainMemory++;
					m_vAllNode[sinkId].m_uiUsedMemory--;
					m_vAllNode[sinkId].m_uiRemainMemory++;
					m_vAllLink[*linkIter].m_uiUsedChannel--;
					m_vAllLink[*linkIter].m_uiRemainingChannel++;
					UsedMemory-=2;
				}
				morePath=false;
			}
			if (morePath)
			{
				(demandIter->m_uiWindowSize)++;
			}
		}
	}
	return UsedMemory;
}

double CNetwork::GetMemoryUtilization()
{
	double totalused=0;
	UINT timeslot=0;
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		vector<double>::iterator usedIter;
		usedIter=demandIter->m_vTotalMemoryUsage.begin();
		for (;usedIter!=demandIter->m_vTotalMemoryUsage.end();usedIter++)
		{
			totalused += (*usedIter);
		}
		timeslot=max(timeslot,demandIter->m_vTotalMemoryUsage.size());
	}
	UINT totalMemory=GetAllMemory();
	return totalused/(totalMemory*timeslot);
}


double CNetwork::JainIndex()
{
	UINT winsum=0;
	UINT winsquaresum=0;
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		winsum += demandIter->m_uiWindowSize;
		winsquaresum += ((demandIter->m_uiWindowSize)*(demandIter->m_uiWindowSize));
	}
	UINT demandNum=m_vAllDemand.size();
	return (winsum*winsum*1.0)/(demandNum * winsquaresum * 1.0);
}


double CNetwork::AverageJainIndex()
{
	double winsum=0;
	double winsquaresum=0;
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		demandIter->m_dAverageWinSize = (demandIter->m_uiTotalWinSize * 1.0)/(demandIter->m_uiWorkingSlot *1.0);
		winsum += demandIter->m_dAverageWinSize;
		winsquaresum += ((demandIter->m_dAverageWinSize)*(demandIter->m_dAverageWinSize));
	}
	UINT demandNum=m_vAllDemand.size();
	return (winsum*winsum*1.0)/(demandNum * winsquaresum * 1.0);
}

//////////////////////REA//////////////////////////////////////

void CNetwork::CleanAllDesiredEntangle()
{
	vector<CLink>::iterator linkIter;
	linkIter=m_vAllLink.begin();
	for (;linkIter!=m_vAllLink.end();linkIter++)
	{
		linkIter->m_uiEntanglement = 0;
		linkIter->m_uiOccupiedEntanglement = 0;
	}
}


bool CNetwork::TryAssignOneEntanglement(LINKID linkId)
{
	NODEID SourceId=m_vAllLink[linkId].m_uiSourceId;
	NODEID SinkId=m_vAllLink[linkId].m_uiSinkId;
	if (m_vAllNode[SourceId].m_uiUsedMemory >= m_vAllNode[SourceId].m_uiMemory)
		return false;
	if (m_vAllNode[SinkId].m_uiUsedMemory >= m_vAllNode[SinkId].m_uiMemory)
		return false;
	if (m_vAllLink[linkId].m_uiUsedChannel >= m_vAllLink[linkId].m_uiChannelNum)
		return false;
	//assgin
	m_vAllNode[SourceId].m_uiUsedMemory++;
	m_vAllNode[SourceId].m_uiRemainMemory--;
	m_vAllNode[SinkId].m_uiUsedMemory++;
	m_vAllNode[SinkId].m_uiRemainMemory--;
	m_vAllLink[linkId].m_uiUsedChannel++;
	m_vAllLink[linkId].m_uiRemainingChannel--;
	m_vAllLink[linkId].m_uiEntanglement++;
	return true;
}

void CNetwork::CleanAllWinsize()
{
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		demandIter->m_uiWindowSize=0;
	}
}

UINT CNetwork::EntanglementLinkOverEachLink()
{
	CleanAllWinsize();
	UINT UsedMemory=0;
	bool flag=true;
	while (flag)
	{
		flag=false;
		vector<CDemand>::iterator demandIter;
		demandIter=m_vAllDemand.begin();
		for (;demandIter!=m_vAllDemand.end();demandIter++)
		{
			if (demandIter->m_bClosed)
			{
				continue;
			}
			bool morePath=true;
			list<LINKID>::iterator linkIter;
			linkIter=demandIter->m_lPathLink.begin();
			for (;linkIter!=demandIter->m_lPathLink.end();linkIter++)
			{
				if (TryAssignOneEntanglement(*linkIter))
				{
					UINT sourceId=m_vAllLink[*linkIter].m_uiSourceId;
					UINT sinkId=m_vAllLink[*linkIter].m_uiSinkId;
					m_vAllNode[sourceId].m_mCarriedDemand[demandIter->m_uiDemandId]++;
					m_vAllNode[sinkId].m_mCarriedDemand[demandIter->m_uiDemandId]++;
					m_vAllLink[*linkIter].m_mCarriedDemand[demandIter->m_uiDemandId]++;
					UsedMemory+=2;
					flag=true;
					continue;
				}
				morePath=false;
			}
			if (morePath)
			{
				(demandIter->m_uiWindowSize)++;
			}
		}
	}
	return UsedMemory;
}


void CNetwork::RefreshCarriedDemand()
{
	vector<CLink>::iterator linkIter;
	linkIter=m_vAllLink.begin();
	for (;linkIter!=m_vAllLink.end();linkIter++)
	{
		map<DEMANDID,UINT>::iterator demandIter;
		demandIter=linkIter->m_mCarriedDemand.begin();
		for (;demandIter!=linkIter->m_mCarriedDemand.end();demandIter++)
		{
			demandIter->second=0;
		}
	}
}

void CNetwork::SimOneUnitEntanglement()
{
	UINT linkNum=m_vAllLink.size();
	for (LINKID linkId=0;linkId<linkNum;linkId++)
	{
		if (m_vAllLink[linkId].m_mCarriedDemand.empty())
		{
			continue;
		}
		UINT successNum=SimEntanglementLinkNum(linkId,m_vAllLink[linkId].m_uiEntanglement);
		m_vAllLink[linkId].m_uiSuccessEntangle=successNum;
		UINT demandNum=m_vAllLink[linkId].m_vCarriedDemand.size();
		for (UINT i=0;i<successNum;i++)
		{
			UINT idx=rand()%demandNum;
			DEMANDID demandId=m_vAllLink[linkId].m_vCarriedDemand[idx];
			m_vAllLink[linkId].m_mCarriedDemand[demandId]++;
		}
	}
}

UINT CNetwork::SimREAOneSession(DEMANDID demandId)
{
	vector<vector<double>> AllGeneratedTime;
	UINT qubitNum=0;
	UINT minEntangle=INFUINT;
	list<LINKID>::iterator linkIter;
	linkIter=m_vAllDemand[demandId].m_lPathLink.begin();
	for (;linkIter!=m_vAllDemand[demandId].m_lPathLink.end();linkIter++)
	{
		UINT Entangle=m_vAllLink[*linkIter].m_mCarriedDemand[demandId];
		vector<double> entangleTime;
		GenerateEntangleTime(Entangle,entangleTime);
		AllGeneratedTime.push_back(entangleTime);
		if (Entangle<minEntangle)
			minEntangle=Entangle;
	}
	m_vAllDemand[demandId].m_uiWindowSize=minEntangle;
	for (UINT i=0;i<minEntangle;i++)
	{
		if (SimEntanglePath(demandId))
			qubitNum++;
	}
	double wasteTime=WasteEntangleTime(demandId,AllGeneratedTime);
	m_vAllDemand[demandId].m_vTotalMemoryWaste.push_back(wasteTime);
	return qubitNum;
}

double CNetwork::WasteEntangleTime(DEMANDID demandId, vector<vector<double>>& entangleTime)
{
	double wasteTime=0;
	for (UINT link1=0,link2=1; link2<entangleTime.size();link1++,link2++)
	{
		for (UINT idx1=0,idx2=0; idx1<entangleTime[link1].size() && idx2<entangleTime[link2].size(); idx1++, idx2++)
		{
			double swappingTime=max(entangleTime[link1][idx1], entangleTime[link2][idx2]);
			wasteTime += (1-swappingTime);
		}
	}
	for (UINT idx=0;;idx++)
	{
		double teleTime=0;
		UINT link=0;
		for (;link<entangleTime.size();link++)
		{
			if (idx>=entangleTime[link].size())
			{
				break;
			}
			if (entangleTime[link][idx] > teleTime)
			{
				teleTime=entangleTime[link][idx];
			}
		}
		if (link<entangleTime.size())
		{
			break;
		}
		wasteTime += 2*(1-teleTime);
	}
	return wasteTime;
}

UINT CNetwork::SimREA(TIMEUNT UnitNum)
{
	cout<<"Start random entanglement allocation method..."<<endl;
	vector<double> recordFairness;
	UINT totalMemory=EntanglementLinkOverEachLink();
	UINT totalThroughput=0;
	double usedMemory=0;
	for (UINT slot=0; slot<UnitNum; slot++)
	{
		RefreshCarriedDemand();
		SimOneUnitEntanglement();
		UINT goodput=0;
		UINT demandNum=m_vAllDemand.size();
		for (DEMANDID demandId=0; demandId<demandNum;demandId++)
		{
			UINT demandPut=SimREAOneSession(demandId);
			goodput += demandPut;
		}
		Throughput.push_back(goodput);
		totalThroughput += goodput;
		usedMemory += CalculateMemoryUsage(slot,totalMemory);
		double fairness=JainIndex();
		recordFairness.push_back(fairness);
	}
	double totalFair=0;
	vector<double>::iterator fairIter;
	fairIter=recordFairness.begin();
	for (;fairIter!=recordFairness.end();fairIter++)
	{
		totalFair += (*fairIter);
	}
	double fairness = totalFair/UnitNum;
	UINT netMemory=GetAllMemory();
	double utilization=usedMemory/(netMemory*UnitNum);
	ofstream fout;
	fout.open("REA_result.txt");
	fout<<"Throughput is "<<totalThroughput<<endl;
	fout<<"Memory utilization is "<<utilization<<endl;
	fout<<"Average fairness is "<<fairness<<endl;
	return totalThroughput;
}


double CNetwork::CalculateMemoryUsage(TIMEUNT slot, UINT totalMemory)
{
	double TotalWaste=0;
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		TotalWaste += demandIter->m_vTotalMemoryWaste[slot];
	}
	OverallMemoryUsage.push_back(totalMemory-TotalWaste);
	return totalMemory-TotalWaste;
}


bool CNetwork::NegativeRemainingMemory()
{
	vector<CNode>::iterator nodeIter;
	nodeIter=m_vAllNode.begin();
	for (;nodeIter!=m_vAllNode.end();nodeIter++)
	{
		if (nodeIter->m_uiRemainMemory > nodeIter->m_uiMemory)
		{
			cout<<nodeIter->m_uiNodeId<<"	"<<endl;
			return false;
		}
	}
	return true;
}


//////////////////NR-DQTP////////////////////////////////////////

void CNetwork::InitializeWindows()
{
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		demandIter->m_uiWindowSize=1;
	}
}


DEMANDID CNetwork::FindCutDemandOverNode(NODEID nodeId)
{
	DEMANDID reduceDemand=IMPOSSIBLEID;
	UINT winSize=0;
	map<DEMANDID,UINT>::iterator demandIter;
	demandIter=m_vAllNode[nodeId].m_mCarriedDemand.begin();
	for (;demandIter!=m_vAllNode[nodeId].m_mCarriedDemand.end();demandIter++)
	{
		if (demandIter->second>winSize)
		{
			winSize=demandIter->second;
			reduceDemand=demandIter->first;
		}
	}
	return reduceDemand;
}

DEMANDID CNetwork::FindCutDemandOverLink(LINKID linkId)
{
	DEMANDID reduceDemand=IMPOSSIBLEID;
	UINT winSize=0;
	map<DEMANDID,UINT>::iterator demandIter;
	demandIter=m_vAllLink[linkId].m_mCarriedDemand.begin();
	for (;demandIter!=m_vAllLink[linkId].m_mCarriedDemand.end();demandIter++)
	{
		if (demandIter->second>winSize)
		{
			winSize=demandIter->second;
			reduceDemand=demandIter->first;
		}
	}
	return reduceDemand;
}

void CNetwork::ReleaseOneEntanglement(DEMANDID DemandId, list<CEntanglement>::iterator entangle)
{
	NODEID sourceId=entangle->m_uiSourceId;
	NODEID sinkId=entangle->m_uiSinkId;
	m_vAllDemand[DemandId].m_mAvailableMemory[sourceId]++;
	m_vAllDemand[DemandId].m_mAvailableMemory[sinkId]++;
}

void CNetwork::ReleaseOnePath(DEMANDID demandId, UINT pathId)
{
	list<CEntanglement>::iterator entangleIter;
	entangleIter=m_vAllDemand[demandId].m_mEntanglementPath[pathId].begin();
	for (;entangleIter!=m_vAllDemand[demandId].m_mEntanglementPath[pathId].end(); entangleIter++)
	{
		ReleaseOneEntanglement(demandId,entangleIter);
	}
}

void CNetwork::UpdateConnection(DEMANDID demandId, UINT orignalWin, UINT newWin)
{
	for (UINT connId=orignalWin-1; connId>=newWin; connId--)
	{
		ReleaseOnePath(demandId,connId);
		map<UINT,list<CEntanglement>>::iterator connIter;
		connIter=m_vAllDemand[demandId].m_mEntanglementPath.find(connId);
		if (connIter!=m_vAllDemand[demandId].m_mEntanglementPath.end())
		{
			m_vAllDemand[demandId].m_mEntanglementPath.erase(connIter);
		}
		map<UINT,list<LINKID>>::iterator connLinkIter;
		connLinkIter=m_vAllDemand[demandId].m_mCoveredLinks.find(connId);
		if (connLinkIter!=m_vAllDemand[demandId].m_mCoveredLinks.end())
		{
			m_vAllDemand[demandId].m_mCoveredLinks.erase(connLinkIter);
		}
		
	}
}

bool CNetwork::CheckFeasibility(DEMANDID demandId, DEMANDID& reduceDemand, NODEID& nodeId, LINKID& linkId)
{
	list<NODEID>::iterator nodeIter;
	nodeIter=m_vAllDemand[demandId].m_lPathNode.begin();
	for (;nodeIter!=m_vAllDemand[demandId].m_lPathNode.end();nodeIter++)
	{
		if (*nodeIter==m_vAllDemand[demandId].m_uiSourceId || *nodeIter==m_vAllDemand[demandId].m_uiSinkId)
		{
			if (m_vAllNode[*nodeIter].m_uiRemainMemory==0)
			{
				nodeId=*nodeIter;
				linkId=IMPOSSIBLEID;
				reduceDemand=FindCutDemandOverNode(nodeId);
				return false;
			}
		}else
		{
			if (m_vAllNode[*nodeIter].m_uiRemainMemory<=1)
			{
				nodeId=*nodeIter;
				linkId=IMPOSSIBLEID;
				reduceDemand=FindCutDemandOverNode(nodeId);
				return false;
			}
		}
	}
	list<LINKID>::iterator linkIter;
	linkIter=m_vAllDemand[demandId].m_lPathLink.begin();
	for (;linkIter!=m_vAllDemand[demandId].m_lPathLink.end();linkIter++)
	{
		if (m_vAllLink[*linkIter].m_uiRemainingChannel==0)
		{
			nodeId=IMPOSSIBLEID;
			linkId=*linkIter;
			reduceDemand=FindCutDemandOverLink(linkId);
			return false;
		}
	}
	return true;
}


void CNetwork::UpdateResourceUtilization(DEMANDID demandId)
{
	list<LINKID>::iterator linkIter;
	linkIter=m_vAllDemand[demandId].m_lPathLink.begin();
	for (;linkIter!=m_vAllDemand[demandId].m_lPathLink.end();linkIter++)
	{
		NODEID sourceId=m_vAllLink[*linkIter].m_uiSourceId;
		NODEID sinkId=m_vAllLink[*linkIter].m_uiSinkId;
		m_vAllNode[sourceId].m_uiRemainMemory--;
		m_vAllNode[sourceId].m_uiUsedMemory++;
		m_vAllNode[sourceId].m_mCarriedDemand[demandId]++;
		m_vAllDemand[demandId].m_mAvailableMemory[sourceId]++;

		m_vAllNode[sinkId].m_uiRemainMemory--;
		m_vAllNode[sinkId].m_uiUsedMemory++;
		m_vAllNode[sinkId].m_mCarriedDemand[demandId]++;
		m_vAllDemand[demandId].m_mAvailableMemory[sinkId]++;


		m_vAllLink[*linkIter].m_uiRemainingChannel--;
		m_vAllLink[*linkIter].m_uiUsedChannel++;
		m_vAllLink[*linkIter].m_mCarriedDemand[demandId]++;
	}
}


void CNetwork::ReleaseResource(DEMANDID demandId, UINT reduceSize)
{
	list<LINKID>::iterator linkIter;
	linkIter=m_vAllDemand[demandId].m_lPathLink.begin();
	for (;linkIter!=m_vAllDemand[demandId].m_lPathLink.end();linkIter++)
	{
		NODEID sourceId=m_vAllLink[*linkIter].m_uiSourceId;
		NODEID sinkId=m_vAllLink[*linkIter].m_uiSinkId;
		m_vAllNode[sourceId].m_uiRemainMemory+=reduceSize;
		m_vAllNode[sourceId].m_uiUsedMemory-=reduceSize;
		m_vAllNode[sourceId].m_mCarriedDemand[demandId]-=reduceSize;

		m_vAllNode[sinkId].m_uiRemainMemory+=reduceSize;
		m_vAllNode[sinkId].m_uiUsedMemory-=reduceSize;
		m_vAllNode[sinkId].m_mCarriedDemand[demandId]-=reduceSize;

		m_vAllLink[*linkIter].m_uiRemainingChannel+=reduceSize;
		m_vAllLink[*linkIter].m_uiUsedChannel-=reduceSize;
		m_vAllLink[*linkIter].m_mCarriedDemand[demandId]-=reduceSize;

		//m_vAllDemand[demandId].m_mAvailableMemory[sourceId] -= reduceSize;
		//m_vAllDemand[demandId].m_mAvailableMemory[sinkId] -= reduceSize;

		//if (m_vAllDemand[demandId].m_mAvailableMemory[sourceId] < 0 && m_vAllDemand[demandId].m_mAvailableMemory[sinkId] < 0)
		//{// we should release UnusedEntanglement
		//	if (m_vAllDemand[demandId].m_mAvailableMemory[sourceId] + m_vAllDemand[demandId].m_mUnusedEntanglement[*linkIter] < 0 ||
		//		m_vAllDemand[demandId].m_mAvailableMemory[sinkId] + m_vAllDemand[demandId].m_mUnusedEntanglement[*linkIter] < 0)
		//	{
		//		cout<<"check where the memory resources are..."<<endl;
		//		getchar();
		//		exit(1);
		//	}
		//	m_vAllDemand[demandId].m_mUnusedEntanglement[*linkIter] += m_vAllDemand[demandId].m_mAvailableMemory[sourceId];
		//	m_vAllDemand[demandId].m_mAvailableMemory[sourceId]=0;
		//	m_vAllDemand[demandId].m_mAvailableMemory[sinkId]=0;
		//}
	}
}

void CNetwork::RecordWindowSizes()
{
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		demandIter->m_uiTotalWinSize += demandIter->m_uiWindowSize;
	}
}

bool CNetwork::IncreaseWindow(DEMANDID demandId)
{
	m_vAllDemand[demandId].m_uiSuccessSent=0;
	DEMANDID reduceDemand=IMPOSSIBLEID;
	NODEID congestedNode=IMPOSSIBLEID; 
	LINKID congestedLink=IMPOSSIBLEID;
	if (CheckFeasibility(demandId,reduceDemand,congestedNode,congestedLink))
	{
		m_vAllDemand[demandId].m_uiWindowSize++;
		UpdateResourceUtilization(demandId);
		return true;
	}else{
		UINT orignal=m_vAllDemand[reduceDemand].m_uiWindowSize;
		if (reduceDemand==demandId)
		{
			UINT newWin=(orignal+1)/2;
			m_vAllDemand[reduceDemand].m_uiWindowSize=newWin;
			UINT reducesize=orignal-newWin;
			ReleaseResource(reduceDemand,reducesize);
			UpdateConnection(reduceDemand,orignal,newWin);
			m_vAllDemand[reduceDemand].m_euWinState=CA;
		}else{
			m_vAllDemand[demandId].m_uiWindowSize++;
			UpdateResourceUtilization(demandId);
			UINT newWin=orignal/2;
			m_vAllDemand[reduceDemand].m_uiWindowSize=newWin;
			UINT reducesize=orignal-newWin;
			ReleaseResource(reduceDemand,reducesize);
			UpdateConnection(reduceDemand,orignal,newWin);
			m_vAllDemand[reduceDemand].m_euWinState=CA;
		}
		return false;
	}
	return false;
}


bool CNetwork::ConnectTwoEntanglement(DEMANDID DemandId, UINT connID, list<CEntanglement>::iterator entangle1, list<CEntanglement>::iterator entangle2)
{
	if (entangle1->m_uiSinkId != entangle2->m_uiSourceId)
	{
		cout<<"check what happened, the sink of entangle 1 is not source of entangle 2!"<<endl;
		getchar();
		exit(1);
	}
	double rndnum=(rand()%10000)/10000.0;
	if (rndnum>m_vAllNode[entangle1->m_uiSinkId].m_dSwapProb)
	{
		return false;
	}
	NODEID medianNode=entangle2->m_uiSourceId;
	entangle1->m_uiSinkId=entangle2->m_uiSinkId;
	entangle1->m_lLinkList.splice(entangle1->m_lLinkList.end(), entangle2->m_lLinkList);
	entangle2->m_lNodeList.erase(entangle2->m_lNodeList.begin());
	entangle1->m_lNodeList.splice(entangle1->m_lNodeList.end(),entangle2->m_lNodeList);
	entangle1->m_euEntangleType=MULTIHOP;
	m_vAllDemand[DemandId].m_mEntanglementPath[connID].erase(entangle2);
	m_vAllDemand[DemandId].m_mAvailableMemory[medianNode] += 2;
	return true;
}


list<CEntanglement>::iterator CNetwork::DestroyTwoEntanglement(DEMANDID DemandId, UINT connId, list<CEntanglement>::iterator entangle1, list<CEntanglement>::iterator entangle2)
{
	list<LINKID>::iterator coveredLinkIter;
	coveredLinkIter=m_vAllDemand[DemandId].m_mCoveredLinks[connId].begin();
	for (;coveredLinkIter!=m_vAllDemand[DemandId].m_mCoveredLinks[connId].end();coveredLinkIter++)
	{
		if (*coveredLinkIter == entangle1->m_lLinkList.front())
		{
			break;
		}
	}
	if (coveredLinkIter==m_vAllDemand[DemandId].m_mCoveredLinks[connId].end())
	{
		cout<<"why cannot find the entanglement"<<endl;
		getchar();
		exit(1);
	}
	NODEID source=entangle1->m_uiSourceId;
	NODEID sink=entangle1->m_uiSinkId;
	//m_vAllNode[source].m_mAssignedMemory[DemandId]++;
	//m_vAllNode[sink].m_mAssignedMemory[DemandId]++;
	m_vAllDemand[DemandId].m_mAvailableMemory[source]++;
	m_vAllDemand[DemandId].m_mAvailableMemory[sink]++;
	source=entangle2->m_uiSourceId;
	sink=entangle2->m_uiSinkId;
	//m_vAllNode[source].m_mAssignedMemory[DemandId]++;
	//m_vAllNode[sink].m_mAssignedMemory[DemandId]++;
	m_vAllDemand[DemandId].m_mAvailableMemory[source]++;
	m_vAllDemand[DemandId].m_mAvailableMemory[sink]++;
	list<LINKID>::iterator entangleLinkIter;
	entangleLinkIter=entangle1->m_lLinkList.begin();
	for (;entangleLinkIter!=entangle1->m_lLinkList.end();entangleLinkIter++)
	{
		if (*coveredLinkIter != *entangleLinkIter)
		{
			cout<<"why two links are not the same??"<<endl;
			getchar();
			exit(1);
		}
		coveredLinkIter=m_vAllDemand[DemandId].m_mCoveredLinks[connId].erase(coveredLinkIter);
	}
	entangleLinkIter=entangle2->m_lLinkList.begin();
	for (;entangleLinkIter!=entangle2->m_lLinkList.end();entangleLinkIter++)
	{
		if (*coveredLinkIter != *entangleLinkIter)
		{
			cout<<"why two links are not the same??"<<endl;
			getchar();
			exit(1);
		}
		coveredLinkIter=m_vAllDemand[DemandId].m_mCoveredLinks[connId].erase(coveredLinkIter);
	}
	m_vAllDemand[DemandId].m_mEntanglementPath[connId].erase(entangle1);
	entangle2=m_vAllDemand[DemandId].m_mEntanglementPath[connId].erase(entangle2);
	return entangle2;
}

void CNetwork::setdifference(list<LINKID>& link1, list<LINKID>& link2, list<LINKID>& res)
{
	list<LINKID>::iterator iter1, iter2;
	iter1=link1.begin();
	iter2=link2.begin();
	while (iter1!=link1.end() && iter2!=link2.end())
	{
		if (*iter1 == *iter2)
		{
			iter1++;
			iter2++;
		}else{
			res.push_back(*iter1);
			iter1++;
		}
	}
	while (iter1!=link1.end())
	{
		res.push_back(*iter1);
		iter1++;
	}
}

void CNetwork::UsedByEntanglements(DEMANDID demandId, map<NODEID,UINT>& usedTime)
{
	map<UINT,list<CEntanglement>>::iterator pathIter;
	pathIter=m_vAllDemand[demandId].m_mEntanglementPath.begin();
	for (;pathIter!=m_vAllDemand[demandId].m_mEntanglementPath.end();pathIter++)
	{
		list<CEntanglement>::iterator entangleIter;
		entangleIter=pathIter->second.begin();
		for (;entangleIter!=pathIter->second.end();entangleIter++)
		{
			usedTime[entangleIter->m_uiSourceId]++;
			usedTime[entangleIter->m_uiSinkId]++;
		}
	}
}

void CNetwork::SingleHopEntanglements(DEMANDID demandId, map<LINKID,UINT>& usedTime)
{
	map<UINT,list<CEntanglement>>::iterator pathIter;
	pathIter=m_vAllDemand[demandId].m_mEntanglementPath.begin();
	for (;pathIter!=m_vAllDemand[demandId].m_mEntanglementPath.end();pathIter++)
	{
		list<CEntanglement>::iterator entangleIter;
		entangleIter=pathIter->second.begin();
		for (;entangleIter!=pathIter->second.end();entangleIter++)
		{
			if (entangleIter->m_lLinkList.size()==1)
			{
				usedTime[entangleIter->m_lLinkList.front()]++;
			}
		}
	}
}

UINT CNetwork::TryEntangleNum(DEMANDID demandId, map<LINKID,UINT>& entangleNum)
{
	UINT memory=0;
	map<NODEID,UINT> availableMemory=m_vAllDemand[demandId].m_mAvailableMemory;
	UINT winSize=m_vAllDemand[demandId].m_uiWindowSize;
	//first try to create entanglements over the uncovered link
	map<UINT,list<LINKID>> coveredLinks=m_vAllDemand[demandId].m_mCoveredLinks;
	list<LINKID> pathLinks=m_vAllDemand[demandId].m_lPathLink;
	pathLinks.sort();
	map<UINT,list<LINKID>>::iterator pathIter;
	pathIter=coveredLinks.begin();
	for (;pathIter!=coveredLinks.end();pathIter++)
	{
		list<LINKID> uncoveredLinks;
		pathIter->second.sort();
		setdifference(pathLinks,pathIter->second,uncoveredLinks);
		list<LINKID>::iterator uncoverIter;
		uncoverIter=uncoveredLinks.begin();
		for (;uncoverIter!=uncoveredLinks.end();uncoverIter++)
		{
			NODEID sourceId=m_vAllLink[*uncoverIter].m_uiSourceId;
			NODEID sinkId=m_vAllLink[*uncoverIter].m_uiSinkId;
			entangleNum[*uncoverIter]++;
			availableMemory[sourceId]--;
			availableMemory[sinkId]--;
			memory += 2;
		}
	}
	map<LINKID,UINT> usedTime;
	SingleHopEntanglements(demandId,usedTime);
	//using remaining resources to creat entanglements
	list<LINKID>::iterator linkIter;
	linkIter=m_vAllDemand[demandId].m_lPathLink.begin();
	for (;linkIter!=m_vAllDemand[demandId].m_lPathLink.end();linkIter++)
	{
		NODEID sourceId=m_vAllLink[*linkIter].m_uiSourceId;
		NODEID sinkId=m_vAllLink[*linkIter].m_uiSinkId;
		UINT moreEntangle=0;
		if (winSize>entangleNum[*linkIter]+m_vAllDemand[demandId].m_mUnusedEntanglement[*linkIter]+usedTime[*linkIter])
		{
			moreEntangle=min(min(availableMemory[sourceId],availableMemory[sinkId]),winSize-entangleNum[*linkIter]-m_vAllDemand[demandId].m_mUnusedEntanglement[*linkIter]-usedTime[*linkIter]);
		}
		entangleNum[*linkIter]+=moreEntangle;
		availableMemory[sourceId]-=moreEntangle;
		availableMemory[sinkId]-=moreEntangle;
		memory+=(2*moreEntangle);
	}
	return memory;
}


void CNetwork::GenerateEntangle(map<LINKID,UINT>& tryNum, map<LINKID,UINT>& successEntangle)
{
	map<LINKID,UINT>::iterator linkIter;
	linkIter=tryNum.begin();
	for (;linkIter!=tryNum.end();linkIter++)
	{
		for (UINT cnt=0;cnt<linkIter->second;cnt++)
		{
			generateNum++;
			double rndNum=(rand()%10000)/10000.0;
			if (rndNum<m_vAllLink[linkIter->first].m_dLinkProb)
			{
				testNum++;
				successEntangle[linkIter->first]++;
			}
		}
	}
}


bool CNetwork::InsertNewSegment(DEMANDID demandId, UINT connId, LINKID linkId)
{
	NODEID linkSouce=m_vAllLink[linkId].m_uiSourceId;
	NODEID linkSink=m_vAllLink[linkId].m_uiSinkId;
	m_vAllDemand[demandId].m_mAvailableMemory[linkSouce]--;
	m_vAllDemand[demandId].m_mAvailableMemory[linkSink]--;
	// insert covered link
	list<LINKID>::iterator linkIter;
	linkIter=m_vAllDemand[demandId].m_mCoveredLinks[connId].begin();
	for (;linkIter!=m_vAllDemand[demandId].m_mCoveredLinks[connId].end();linkIter++)
	{
		NODEID iterSouce=m_vAllLink[*linkIter].m_uiSourceId;
		NODEID iterSink=m_vAllLink[*linkIter].m_uiSinkId;
		if (m_vAllDemand[demandId].m_mNodeToIndex[iterSouce] <= m_vAllDemand[demandId].m_mNodeToIndex[linkSouce] && m_vAllDemand[demandId].m_mNodeToIndex[iterSink]<=m_vAllDemand[demandId].m_mNodeToIndex[linkSouce])
		{
			continue;
		}
		m_vAllDemand[demandId].m_mCoveredLinks[connId].insert(linkIter,linkId);
		break;
	}
	if (linkIter==m_vAllDemand[demandId].m_mCoveredLinks[connId].end())
	{
		m_vAllDemand[demandId].m_mCoveredLinks[connId].insert(linkIter,linkId);
	}
	//if (m_vAllDemand[demandId].m_mCoveredLinks[connId].empty())
	//{
	//	m_vAllDemand[demandId].m_mCoveredLinks[connId].push_back(linkId);
	//}
	//add segment
	NODEID segSource, segSink;
	if (m_vAllDemand[demandId].m_mNodeToIndex[linkSouce] < m_vAllDemand[demandId].m_mNodeToIndex[linkSink])
	{
		segSource=linkSouce;
		segSink=linkSink;
	}else{
		segSource=linkSink;
		segSink=linkSouce;
	}
	CEntanglement newSeg;
	newSeg.m_uiSourceId=segSource;
	newSeg.m_uiSinkId=segSink;
	newSeg.m_lLinkList.push_back(linkId);
	newSeg.m_lNodeList.push_back(segSource);
	newSeg.m_lNodeList.push_back(segSink);
	newSeg.m_euEntangleType=SINGLEHOP;
	newSeg.m_uiHomeDemand=demandId;
	newSeg.m_uiHomePathId=connId;
	list<CEntanglement>::iterator segIter;
	segIter=m_vAllDemand[demandId].m_mEntanglementPath[connId].begin();
	for (;segIter!=m_vAllDemand[demandId].m_mEntanglementPath[connId].end();segIter++)
	{
		NODEID preSource=segIter->m_uiSourceId;
		NODEID preSink=segIter->m_uiSinkId;
		if (m_vAllDemand[demandId].m_mNodeToIndex[preSource]<=m_vAllDemand[demandId].m_mNodeToIndex[segSource] && m_vAllDemand[demandId].m_mNodeToIndex[preSink]<=m_vAllDemand[demandId].m_mNodeToIndex[segSource])
		{
			continue;
		}
		m_vAllDemand[demandId].m_mEntanglementPath[connId].insert(segIter,newSeg);
		return true;
	}
	if (segIter==m_vAllDemand[demandId].m_mEntanglementPath[connId].end())
	{
		m_vAllDemand[demandId].m_mEntanglementPath[connId].insert(segIter,newSeg);
		return true;
	}
	return true;
}

bool CNetwork::AssignOneEntangleToConnection(DEMANDID demandId, LINKID linkId)
{
//	map<UINT,list<LINKID>>::iterator connIter;
	for (DEMANDID connId=0; connId<m_vAllDemand[demandId].m_uiWindowSize; connId++)
	{
		list<LINKID>::iterator linkIter;
		linkIter=find(m_vAllDemand[demandId].m_mCoveredLinks[connId].begin(),m_vAllDemand[demandId].m_mCoveredLinks[connId].end(),linkId);
		if (linkIter==m_vAllDemand[demandId].m_mCoveredLinks[connId].end())
		{
			InsertNewSegment(demandId,connId, linkId);
			return true;
		}
	}
	m_vAllDemand[demandId].m_mUnusedEntanglement[linkId]++;
	NODEID sourceId=m_vAllLink[linkId].m_uiSourceId;
	NODEID sinkId=m_vAllLink[linkId].m_uiSinkId;
	m_vAllDemand[demandId].m_mAvailableMemory[sourceId]--;
	m_vAllDemand[demandId].m_mAvailableMemory[sinkId]--;
	return false;
}

void CNetwork::AssignEntangleToConnection(DEMANDID demandId, map<LINKID,UINT>& successEntangle)
{
	map<LINKID,UINT>::iterator entangleIter;
	entangleIter=successEntangle.begin();
	for (;entangleIter!=successEntangle.end();entangleIter++)
	{
		for (UINT cnt=0;cnt<entangleIter->second;cnt++)
		{
			AssignOneEntangleToConnection(demandId,entangleIter->first);
		}
	}
}

void CNetwork::InitEntanglementAssignment()
{
	vector<CDemand>::iterator demandIter;
	demandIter!=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		list<LINKID>::iterator linkIter;
		linkIter=demandIter->m_lPathLink.begin();
		for (;linkIter!=demandIter->m_lPathLink.end();linkIter++)
		{
			NODEID sourceId=m_vAllLink[*linkIter].m_uiSourceId;
			NODEID sinkId=m_vAllLink[*linkIter].m_uiSinkId;
			m_vAllNode[sourceId].m_mCarriedDemand[demandIter->m_uiDemandId]+=(demandIter->m_uiWindowSize);
			m_vAllNode[sinkId].m_mCarriedDemand[demandIter->m_uiDemandId]+=(demandIter->m_uiWindowSize);
			m_vAllLink[*linkIter].m_uiEntanglement += (demandIter->m_uiWindowSize);
		}
	}
}


void CNetwork::AssignUnusedEntangleToConnection(DEMANDID demandId)
{
	map<LINKID,UINT> assignedNum;
	map<LINKID,UINT>::iterator entangleIter;
	entangleIter=m_vAllDemand[demandId].m_mUnusedEntanglement.begin();
	for (;entangleIter!=m_vAllDemand[demandId].m_mUnusedEntanglement.end();entangleIter++)
	{
		for (UINT cnt=0;cnt<entangleIter->second;cnt++)
		{
			UINT winsize=m_vAllDemand[demandId].m_uiWindowSize;
			//map<UINT,list<LINKID>>::iterator connIter;
			//connIter=m_vAllDemand[demandId].m_mCoveredLinks.begin();
			//for (;connIter!=m_vAllDemand[demandId].m_mCoveredLinks.end();connIter++)
			for(UINT connId=0; connId<winsize;connId++)
			{
				list<LINKID>::iterator linkIter;
				linkIter=find(m_vAllDemand[demandId].m_mCoveredLinks[connId].begin(),m_vAllDemand[demandId].m_mCoveredLinks[connId].end(),entangleIter->first);
				if (linkIter==m_vAllDemand[demandId].m_mCoveredLinks[connId].end())
				{
					InsertNewSegment(demandId,connId, entangleIter->first);
					//the memory usage has been counted when updating the m_mUnusedEntanglement. here we should add back it
					NODEID sourceId=m_vAllLink[entangleIter->first].m_uiSourceId;
					NODEID sinkId=m_vAllLink[entangleIter->first].m_uiSinkId;
					m_vAllDemand[demandId].m_mAvailableMemory[sourceId]++;
					m_vAllDemand[demandId].m_mAvailableMemory[sinkId]++;
					assignedNum[entangleIter->first]++;
					break;
				}
			}
		}
	}
	entangleIter=assignedNum.begin();
	for (;entangleIter!=assignedNum.end();entangleIter++)
	{
		m_vAllDemand[demandId].m_mUnusedEntanglement[entangleIter->first] -= assignedNum[entangleIter->first];
	}
}

bool CNetwork::AdjustOneConnection(DEMANDID demandId, UINT connId)
{
	if (m_vAllDemand[demandId].m_mEntanglementPath[connId].empty())
	{
		return false;
	}
	if (m_vAllDemand[demandId].m_mEntanglementPath[connId].size() == 1)
	{
		if (m_vAllDemand[demandId].m_mCoveredLinks[connId].size()==m_vAllDemand[demandId].m_lPathLink.size())
		{
			ReleaseOneSuccessConnection(demandId,connId);
			return true;
		}
	}
	list<CEntanglement>::iterator entangleIter1, entangleIter2;
	entangleIter1=m_vAllDemand[demandId].m_mEntanglementPath[connId].begin();
	entangleIter2=entangleIter1;
	entangleIter2++;
	while(entangleIter2!=m_vAllDemand[demandId].m_mEntanglementPath[connId].end())
	{
		if (entangleIter1->m_uiSinkId == entangleIter2->m_uiSourceId)
		{
			if (ConnectTwoEntanglement(demandId,connId,entangleIter1,entangleIter2))
			{
				entangleIter2=entangleIter1;
				entangleIter2++;
			}else{
				entangleIter1=DestroyTwoEntanglement(demandId,connId,entangleIter1,entangleIter2);
				if (entangleIter1==m_vAllDemand[demandId].m_mEntanglementPath[connId].end())
				{
					break;
				}
				entangleIter2=entangleIter1;
				entangleIter2++;
			}
		}else{
			entangleIter1++;
			entangleIter2++;
		}
	}
	if (m_vAllDemand[demandId].m_mEntanglementPath[connId].size() == 1)
	{
		if (m_vAllDemand[demandId].m_mCoveredLinks[connId].size()==m_vAllDemand[demandId].m_lPathLink.size())
		{
			ReleaseOneSuccessConnection(demandId,connId);
			return true;
		}
	}
	return false;
}


bool CNetwork::AdjustOneDemandConnection(DEMANDID demandId)
{
	UINT increaseNum=0;
	for (UINT connId=0;connId<m_vAllDemand[demandId].m_uiWindowSize;connId++)
	{
		if (AdjustOneConnection(demandId,connId))
		{
			cout<<"Session "<<demandId<<" successfully teleports one qubit."<<endl;
			m_vAllDemand[demandId].m_uiTeleportedQubit++;
			m_vAllDemand[demandId].m_uiRemainQubit--;
			if (m_vAllDemand[demandId].m_uiRemainQubit==0)
			{
				return true;
			}
			if (m_vAllDemand[demandId].m_euWinState==SS)
			{
				increaseNum++;
			}else{
				m_vAllDemand[demandId].m_uiSuccessSent++;
				if (m_vAllDemand[demandId].m_uiSuccessSent == m_vAllDemand[demandId].m_uiWindowSize)
				{
					increaseNum++;
					m_vAllDemand[demandId].m_uiSuccessSent=0;
				}
			}
		}
	}
	//if (m_vAllDemand[demandId].m_uiRemainQubit<=0)
	//{
	//	return true;
	//}
	for (UINT i=0; i< increaseNum; i++)
	{
		IncreaseWindow(demandId);
	}
	return false;
}

void CNetwork::AdjustConnections()
{
	for (DEMANDID demandId=0; demandId<m_vAllDemand.size();demandId++)
	{
		AdjustOneDemandConnection(demandId);
	}
}

void CNetwork::ReleaseOneSuccessConnection(DEMANDID demandId, UINT connId)
{
	m_vAllDemand[demandId].m_uiTotalThroughput ++;
	m_vAllDemand[demandId].m_mEntanglementPath[connId].clear();
	m_vAllDemand[demandId].m_mCoveredLinks[connId].clear();
	NODEID sourceId=m_vAllDemand[demandId].m_uiSourceId;
	NODEID sinkId=m_vAllDemand[demandId].m_uiSinkId;
	m_vAllDemand[demandId].m_mAvailableMemory[sourceId]++;
	m_vAllDemand[demandId].m_mAvailableMemory[sinkId]++;
}

UINT CNetwork::MemroyHostEntanglement(DEMANDID demandId)
{
	UINT memory=0;
	map<LINKID,UINT>::iterator unusedIter;
	unusedIter=m_vAllDemand[demandId].m_mUnusedEntanglement.begin();
	for (;unusedIter!=m_vAllDemand[demandId].m_mUnusedEntanglement.end();unusedIter++)
	{
		memory += 2*(unusedIter->second);
	}
	map<UINT,list<CEntanglement>>::iterator entangleIter;
	entangleIter=m_vAllDemand[demandId].m_mEntanglementPath.begin();
	for (;entangleIter!=m_vAllDemand[demandId].m_mEntanglementPath.end();entangleIter++)
	{
		memory += 2*(entangleIter->second.size());
	}
	return memory;
}

bool CNetwork::SimNRDTPOneSession(DEMANDID demandId, TIMEUNT slot)
{
	//if (demandId==1 && slot==92)
	//{
	//	cout<<"kkkkk"<<endl;
	//}
	AssignUnusedEntangleToConnection(demandId);
	map<LINKID,UINT> tryNum, successEntangle;
	UINT memory = MemroyHostEntanglement(demandId);
	memory += TryEntangleNum(demandId,tryNum);
	m_vAllDemand[demandId].m_uiTotalMemory += memory;
	m_vAllDemand[demandId].m_uiWorkingSlot++;
	GenerateEntangle(tryNum,successEntangle);
	//if (demandId==1 && !successEntangle.empty())
	//{
	//	cout<<"kkkkk"<<endl;
	//}
	AssignEntangleToConnection(demandId,successEntangle);
	if (AdjustOneDemandConnection(demandId))
	{
		CloseSession(demandId);
		m_vAllDemand[demandId].m_uiFinishTime=slot;
		m_uiCompleteSessions++;
		return true;
	}
	return false;
}

UINT CNetwork::GetNRDQTPThroughput()
{
	UINT totalthroughput=0;
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		totalthroughput += demandIter->m_uiTotalThroughput;
	}
	return totalthroughput;
}

void CNetwork::SimNRDQTPOneSlot(TIMEUNT slot)
{
	UINT demandNum=m_vAllDemand.size();
	for (DEMANDID demandId=0;demandId<demandNum;demandId++)
	{
		if (m_vAllDemand[demandId].m_bClosed)
		{
			continue;
		}
		SimNRDTPOneSession(demandId, slot);
	}
}


void CNetwork::InitializeNRDQTP()
{
	CleanAllWinsize();
	UINT DemandNum=m_vAllDemand.size();
	for (DEMANDID demandId=0; demandId<DemandNum;demandId++)
	{
//		cout<<demandId<<"	";
		IncreaseWindow(demandId);
	}
}

double CNetwork::DQTPMemoryUtilization(TIMEUNT UnitNum)
{
	UINT usedMemory=0;
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		usedMemory += demandIter->m_uiTotalMemory;
	}
	return (usedMemory*1.0)/(GetAllMemory()*UnitNum*1.0);
}

UINT CNetwork::TimeUnitMemory()
{
	UINT memory=0;
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		memory += (demandIter->m_uiWindowSize)*(demandIter->m_lPathLink.size()*2);
	}
	return memory;
}

double CNetwork::AverageSessionCompletionTime()
{
	UINT totalCT=0;
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		totalCT += demandIter->m_uiFinishTime;
	}
	UINT demandNum=m_vAllDemand.size();
	return totalCT*1.0/(demandNum*1.0);
}

UINT CNetwork::SimNRDQTP(TIMEUNT UnitNum)
{
	cout<<"Start NR-DQTP simulation..."<<endl;
	//first initialize window size to be 1
	InitializeNRDQTP();
	double memory=0;
	UINT idx=0;
	for(;idx<UnitNum;idx++)
	{
		//if (idx>2000)
		//{
		//	for (DEMANDID demandId=0;demandId<m_vAllDemand.size();demandId++)
		//	{
		//		if (!m_vAllDemand[demandId].m_bClosed)
		//		{
		//			cout<<"kkkkkkkkkkkkkkkkkk"<<endl;
		//		}
		//	}
		//}
		if (m_uiCompleteSessions == m_vAllDemand.size())
		{
			break;
		}
		cout<<"Simulation for the time unit "<<idx<<endl;
		SimNRDQTPOneSlot(idx);
		memory += TimeUnitMemory();
		RecordWindowSizes();
	}
	UINT resThroughput=GetNRDQTPThroughput();
	double averageCT=AverageSessionCompletionTime();
	double utilization= DQTPMemoryUtilization(idx);
	double fairness=AverageJainIndex();
	ofstream fout;
	fout.open("NRDQTP_result.txt");
	fout<<"Total throughput is "<<resThroughput<<endl;
	fout<<"Average session completion time is "<<averageCT<<endl;
	fout<<"Memory utilization is "<<utilization<<endl;
	fout<<"Fairness index is "<<fairness<<endl;
	fout.close();
	return resThroughput;
}

void CNetwork::CloseSession(DEMANDID demandId)
{
	m_vAllDemand[demandId].m_bClosed=true;
	UINT winSize=m_vAllDemand[demandId].m_uiWindowSize;

	list<LINKID>::iterator linkIter;
	linkIter=m_vAllDemand[demandId].m_lPathLink.begin();
	for (;linkIter!=m_vAllDemand[demandId].m_lPathLink.end();linkIter++)
	{
		NODEID sourceId=m_vAllLink[*linkIter].m_uiSourceId;
		NODEID sinkId=m_vAllLink[*linkIter].m_uiSinkId;

		m_vAllNode[sourceId].m_uiRemainMemory+=winSize;
		m_vAllNode[sourceId].m_uiUsedMemory-=winSize;
		m_vAllNode[sourceId].m_mCarriedDemand[demandId]-=winSize;

		m_vAllNode[sinkId].m_uiRemainMemory+=winSize;
		m_vAllNode[sinkId].m_uiUsedMemory-=winSize;
		m_vAllNode[sinkId].m_mCarriedDemand[demandId]-=winSize;

		m_vAllLink[*linkIter].m_uiRemainingChannel+=winSize;
		m_vAllLink[*linkIter].m_uiUsedChannel-=winSize;
		m_vAllLink[*linkIter].m_mCarriedDemand[demandId]-=winSize;
	}
	m_vAllDemand[demandId].m_mAvailableMemory.clear();
	m_vAllDemand[demandId].m_mCoveredLinks.clear();
	m_vAllDemand[demandId].m_mEntanglementPath.clear();
	m_vAllDemand[demandId].m_mUnusedEntanglement.clear();
	//for (UINT connId=0; connId<winSize;connId++)
	//{
	//	ReleaseOnePath(demandId,connId);
	//}
	m_vAllDemand[demandId].m_uiWindowSize=0;
}

void CNetwork::RestartSessionTSC(DEMANDID demandId)
{
	m_vAllDemand[demandId].m_bClosed=false;
	m_vAllDemand[demandId].m_euWinState=SS;
	m_vAllDemand[demandId].m_uiRemainQubit=m_vAllDemand[demandId].m_uiQubitNum;
	m_vAllDemand[demandId].m_uiTeleportedQubit=0;
	m_vAllDemand[demandId].m_mEntanglementPath.clear();
	m_vAllDemand[demandId].m_mCoveredLinks.clear();
	m_vAllDemand[demandId].m_mAvailableMemory.clear();
}


void CNetwork::RestartSession(DEMANDID demandId)
{
	m_vAllDemand[demandId].m_bClosed=false;
	m_vAllDemand[demandId].m_euWinState=SS;
	m_vAllDemand[demandId].m_uiRemainQubit=m_vAllDemand[demandId].m_uiQubitNum;
	m_vAllDemand[demandId].m_uiTeleportedQubit=0;
	m_vAllDemand[demandId].m_mEntanglementPath.clear();
	m_vAllDemand[demandId].m_mCoveredLinks.clear();
	m_vAllDemand[demandId].m_mAvailableMemory.clear();
	IncreaseWindow(demandId);
}


void CNetwork::RestartTaskTSC(TASKID taskId)
{
	list<DEMANDID>::iterator demandIter;
	demandIter=m_vAllTask[taskId].m_lDemands.begin();
	for (;demandIter!=m_vAllTask[taskId].m_lDemands.end();demandIter++)
	{
		RestartSessionTSC(*demandIter);
	}
}

void CNetwork::RestartTask(TASKID taskId)
{
	list<DEMANDID>::iterator demandIter;
	demandIter=m_vAllTask[taskId].m_lDemands.begin();
	for (;demandIter!=m_vAllTask[taskId].m_lDemands.end();demandIter++)
	{
		RestartSession(*demandIter);
	}
}



bool CNetwork::AdjustOneDemandConnection_TASK(DEMANDID demandId)
{
	UINT increaseNum=0;
	NODEID sourceId=m_vAllDemand[demandId].m_uiSourceId;
	for (UINT connId=0;connId<m_vAllDemand[demandId].m_uiWindowSize;connId++)
	{
		if (AdjustOneConnection(demandId,connId))
		{
			//handle teleportation failure
			double failProb=m_vAllNode[sourceId].m_dTeleProb;
			double rndNum=(rand()%10000)/10000.0;
			if (rndNum>failProb)
			{//teleportation fails
				return false;
			}
			m_vAllDemand[demandId].m_uiTeleportedQubit++;
			m_vAllDemand[demandId].m_uiRemainQubit--;
			if (m_vAllDemand[demandId].m_uiRemainQubit==0)
			{
				m_vAllDemand[demandId].m_bClosed=true;
				CloseSession(demandId);
				return true;
			}
			if (m_vAllDemand[demandId].m_euWinState==SS)
			{
				increaseNum++;
			}else{
				m_vAllDemand[demandId].m_uiSuccessSent++;
				if (m_vAllDemand[demandId].m_uiSuccessSent == m_vAllDemand[demandId].m_uiWindowSize)
				{
					m_vAllDemand[demandId].m_uiSuccessSent=0;
					increaseNum++;
				}
			}
		}
	}
	for (UINT i=0;i<increaseNum;i++)
	{
		//if (demandId==16)
		//{
		//	cout<<"kkkkkkkkkk"<<endl;
		//}
		IncreaseWindow(demandId);
	}
	return true;
}

bool CNetwork::AdjustTaskConnections(TASKID taskId, TIMEUNT slot)
{
	bool allfinished=true;
	list<DEMANDID>::iterator demandIter;
	demandIter=m_vAllTask[taskId].m_lDemands.begin();
	for (;demandIter!=m_vAllTask[taskId].m_lDemands.end();demandIter++)
	{
		if (m_vAllDemand[*demandIter].m_bClosed)
		{
			continue;
		}
		if (!AdjustOneDemandConnection_TASK(*demandIter))
		{
			cout<<"Due to the failure of session "<<*demandIter<<", we have to restart Task "<<taskId<<endl;
			demandIter=m_vAllTask[taskId].m_lDemands.begin();
			for (;demandIter!=m_vAllTask[taskId].m_lDemands.end();demandIter++)
			{
				ReleaseResource(*demandIter,m_vAllDemand[*demandIter].m_uiWindowSize);
				m_vAllDemand[*demandIter].m_mAvailableMemory.clear();
				m_vAllDemand[*demandIter].m_uiWindowSize=0;
			}
			RestartTask(taskId);
			return false;
		}
		if (!m_vAllDemand[*demandIter].m_bClosed)
		{
			allfinished=false;
		}
	}
	if (allfinished)
	{
		m_vAllTask[taskId].m_uiFinishTime=slot;
		m_vAllTask[taskId].m_bFinished=true;
	}
	return allfinished;
}


bool CNetwork::CheckMemory()
{
	
	vector<CNode>::iterator nodeIter;
	nodeIter=m_vAllNode.begin();
	for (;nodeIter!=m_vAllNode.end();nodeIter++)
	{
		UINT usedMemory=0;
		map<DEMANDID, UINT>::iterator demandIter;
		demandIter=nodeIter->m_mCarriedDemand.begin();
		for (;demandIter!=nodeIter->m_mCarriedDemand.end();demandIter++)
		{
			usedMemory+=demandIter->second;
		}
		if (usedMemory!=nodeIter->m_uiUsedMemory)
		{
			cout<<nodeIter->m_uiNodeId<<endl;
			return false;
		}
	}
	return true;
}

bool CNetwork::SimNRDTPOneTASK(TASKID taskId, TIMEUNT slot)
{
	list<DEMANDID>::iterator demandIter;
	demandIter=m_vAllTask[taskId].m_lDemands.begin();
	for (;demandIter!=m_vAllTask[taskId].m_lDemands.end();demandIter++)
	{
		//if (slot==776)
		//{
		//	cout<<taskId<<"	";
		//}
		//if (*demandIter == 3)
		//{
		//	cout<<"kkkkkkkkkkkk"<<endl;
		//}
		if (m_vAllDemand[*demandIter].m_bClosed)
		{
			continue;
		}
		AssignUnusedEntangleToConnection(*demandIter);
		map<LINKID,UINT> tryNum, successEntangle;
		UINT memory=TryEntangleNum(*demandIter,tryNum);
		m_vAllDemand[*demandIter].m_uiTotalMemory += memory;
		GenerateEntangle(tryNum,successEntangle);
		if (*demandIter==39 && !successEntangle.empty())
		{
			//cout<<m_vAllDemand[*demandIter].m_uiRemainQubit<<"	"<<m_vAllDemand[*demandIter].m_uiWindowSize<<endl;
			//if (m_vAllDemand[*demandIter].m_uiRemainQubit==15 && m_vAllDemand[*demandIter].m_uiWindowSize==9)
			{
				cout<<"slot="<<slot<<endl;
			}
		}
		AssignEntangleToConnection(*demandIter,successEntangle);
	}
	return AdjustTaskConnections(taskId,slot);
}


double CNetwork::AverageTaskCompletionTime()
{
	UINT totalCT=0;
	vector<CTask>::iterator taskIter;
	taskIter=m_vAllTask.begin();
	for (;taskIter!=m_vAllTask.end();taskIter++)
	{
		totalCT += (taskIter->m_uiFinishTime+1);
	}
	UINT taskNum=m_vAllTask.size();
	return totalCT*1.0/(taskNum*1.0);
}

void CNetwork::RecordWorkingWindow()
{
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		if (demandIter->m_bClosed)
			continue;
		if (demandIter->m_uiWindowSize == 0)
			continue;
		demandIter->m_uiTotalWinSize += demandIter->m_uiWindowSize;
		demandIter->m_uiWorkingSlot ++; 
	}
}

void CNetwork::AveragingWindowSize()
{
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		demandIter->m_uiTotalWinSize /= demandIter->m_uiWorkingSlot;
	}
}

TIMEUNT CNetwork::SimNRDQTP_TASK()
{
	cout<<"Start simulating NR-DQTP in the task mode..."<<endl;
	InitializeNRDQTP();
	TIMEUNT slot=0;
	bool allfinished=false;
	while (!allfinished)
	{
		if (slot%200==0)
		{
			cout<<"Simulate the time slot "<<slot<<endl;
		}
		allfinished=true;
		for (TASKID taskId=0; taskId<m_vAllTask.size();taskId++)
		{
			if (m_vAllTask[taskId].m_bFinished)
			{
				continue;
			}
			//if (slot==519)
			//{
			//	cout<<taskId<<"	";
			//}
			if (!SimNRDTPOneTASK(taskId,slot))
			{
				allfinished=false;
			}
			//if (slot==519 && m_vAllDemand[31].m_uiWindowSize != m_vAllNode[2].m_mCarriedDemand[31])
			//{
			//	cout<<"kkkkkkkkkk  "<<taskId<<endl;
			//	getchar();
			//}
			//if (!CheckMemory())
			//{
			//	cout<<taskId<<"	"<<slot<<endl;
			//	getchar();
			//}
			//if (!NegativeRemainingMemory())
			//{
			//	cout<<"Negative remaining memory "<<taskId<<"	"<<slot<<endl;
			//	getchar();
			//}
		}
		slot++;
		RecordWorkingWindow();
	}

	double averageCT=AverageTaskCompletionTime();
	double fairness = AverageJainIndex();
	double utilization = DQTPMemoryUtilization(slot);
	ofstream fout;
	fout.open("DQTP_TASK_result.txt");
	fout<<"Average task completion time is "<<averageCT/m_dSlotTry<<endl;
	fout<<"Fairness index is "<<fairness<<endl;
	fout<<"Memory utilization is "<<utilization<<endl;
	return slot;
}

void CNetwork::ChangeParameterForDQTP()
{
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemand.begin();
	for (;demandIter!=m_vAllDemand.end();demandIter++)
	{
		UINT qubit=demandIter->m_uiQubitNum;
		double teleProb=m_vAllNode[demandIter->m_uiSourceId].m_dTeleProb;
		UINT newqubit=(UINT)(qubit/teleProb + 1);
		if ((rand()%10000*1.0/10000.0)< qubit/teleProb - (UINT)(qubit/teleProb))
		{
			newqubit++;
		}
		demandIter->m_uiQubitNum=newqubit;
		demandIter->m_uiRemainQubit=newqubit;
	}
	vector<CNode>::iterator nodeIter;
	nodeIter=m_vAllNode.begin();
	for (; nodeIter!=m_vAllNode.end();nodeIter++)
	{
		nodeIter->m_dTeleProb=1;
	}
}


TIMEUNT CNetwork::SimDQTP_TASK()
{
	ChangeParameterForDQTP();
	return SimNRDQTP_TASK();
}



void CNetwork::RecoverResources()
{
	vector<CNode>::iterator nodeIter;
	nodeIter=m_vAllNode.begin();
	for (;nodeIter!=m_vAllNode.end();nodeIter++)
	{
		nodeIter->m_uiRemainMemory=nodeIter->m_uiMemory;
		nodeIter->m_uiUsedMemory=0;
	}

	vector<CLink>::iterator linkIter;
	linkIter=m_vAllLink.begin();
	for (;linkIter!=m_vAllLink.end();linkIter++)
	{
		linkIter->m_uiRemainingChannel=linkIter->m_uiChannelNum;
		linkIter->m_uiUsedChannel=0;
		linkIter->m_dWeight=1.0;
	}
}


void CNetwork::SetWeightForAEPR(DEMANDID demandId)
{
	NODEID sourceId=m_vAllDemand[demandId].m_uiSourceId;
	NODEID sinkId=m_vAllDemand[demandId].m_uiSinkId;
	vector<CLink>::iterator linkIter;
	linkIter=m_vAllLink.begin();
	for (;linkIter!=m_vAllLink.end();linkIter++)
	{
		NODEID linkSource=linkIter->m_uiSourceId;
		NODEID linkSink=linkIter->m_uiSinkId;
		if (m_vAllNode[linkSource].m_uiRemainMemory==0 || m_vAllNode[linkSink].m_uiRemainMemory==0)
		{
			linkIter->m_dWeight=INF;
			continue;
		}
		if (linkIter->m_uiRemainingChannel==0)
		{
			linkIter->m_dWeight=INF;
			continue;
		}
		if (m_vAllNode[linkSource].m_uiRemainMemory==1 && (linkSource!=sourceId && linkSource!=sinkId))
		{
			linkIter->m_dWeight=INF;
			continue;
		}
		if (m_vAllNode[linkSink].m_uiRemainMemory==1 && (linkSink!=sourceId && linkSink!=sinkId))
		{
			linkIter->m_dWeight=INF;
			continue;
		}
		linkIter->m_dWeight=1.0;
	}
}

bool CNetwork::RouteOneDemand(DEMANDID demandId)
{
	SetWeightForAEPR(demandId);
	NODEID sourceId=m_vAllDemand[demandId].m_uiSourceId;
	NODEID sinkId=m_vAllDemand[demandId].m_uiSinkId;
	list<LINKID> linkList;
	list<NODEID> nodeList;
	if (ShortestPath(sourceId,sinkId,nodeList,linkList))
	{
//		cout<<"Demand "<<demandId<<" gets an entanglement path"<<endl;
		list<NODEID>::iterator nodeIter;
		nodeIter=nodeList.begin();
		for (;nodeIter!=nodeList.end();nodeIter++)
		{
			if ((*nodeIter)==sourceId || (*nodeIter)==sinkId)
			{
				m_vAllNode[*nodeIter].m_uiRemainMemory--;
				m_vAllNode[*nodeIter].m_uiUsedMemory++;
			}else
			{
				m_vAllNode[*nodeIter].m_uiRemainMemory-=2;
				m_vAllNode[*nodeIter].m_uiUsedMemory+=2;
			}
		}
		double successProb=1;
		list<LINKID>::iterator linkIter;
		linkIter=linkList.begin();
		for (;linkIter!=linkList.end();linkIter++)
		{
			m_vAllLink[*linkIter].m_uiRemainingChannel--;
			m_vAllLink[*linkIter].m_uiUsedChannel++;
			successProb *= m_vAllLink[*linkIter].m_dAggProb;
		}
		double rndNum=(rand()%1000 * 1.0)/1000.0;
		if (rndNum<successProb)
		{
			m_vAllDemand[demandId].m_uiRemainQubit--;
			m_vAllDemand[demandId].m_uiTeleportedQubit++;
			if (m_vAllDemand[demandId].m_uiRemainQubit==0)
			{
				m_vAllDemand[demandId].m_bClosed=true;
				m_vAllDemand[demandId].m_uiFinishTime=m_uiSlotId;
			}
			if (m_vAllDemand[demandId].m_uiTeleportedQubit > m_vAllDemand[demandId].m_uiQubitNum)
			{
				cout<<"check what happened"<<endl;
				getchar();
			}
//			cout<<"Demand "<<demandId<<" fulfills one request"<<endl;
		}
		return true;
	}
	return false;
}

bool CNetwork::AllDemandComplete(TASKID taskId)
{
	if (m_vAllTask[taskId].m_bFinished)
	{
		return true;
	}
	list<DEMANDID>::iterator demandIter;
	demandIter=m_vAllTask[taskId].m_lDemands.begin();
	for (;demandIter!=m_vAllTask[taskId].m_lDemands.end();demandIter++)
	{
		if (!m_vAllDemand[*demandIter].m_bClosed)
		{
			return false;
		}
	}
	m_vAllTask[taskId].m_bFinished=true;
	m_vAllTask[taskId].m_uiCompleteSlot=m_uiSlotId;
	return true;
}


void CNetwork::SimulateOneSlot(TASKID taskId)
{
	RecoverResources();
	bool flag=true;
	while (flag)
	{
		flag=false;
		list<DEMANDID>::iterator demandIter;
		demandIter=m_vAllTask[taskId].m_lDemands.begin();
		for (;demandIter!=m_vAllTask[taskId].m_lDemands.end();demandIter++)
		{
			if (m_vAllDemand[*demandIter].m_bClosed)
			{
				continue;
			}
			if (RouteOneDemand(*demandIter))
			{
				flag=true;
				continue;
			}
		}
	}
}

void CNetwork::SimulateOneTask(TASKID taskId)
{
	m_uiSlotId=0;
	while(!AllDemandComplete(taskId))
	{
		m_uiSlotId++;
		cout<<"Task "<<taskId<<", Slot "<<m_uiSlotId<<endl;
		SimulateOneSlot(taskId);
	}
}

void CNetwork::SimulateAEPR()
{
	vector<CTask>::iterator taskIter;
	taskIter=m_vAllTask.begin();
	for (;taskIter!=m_vAllTask.end();taskIter++)
	{
		SimulateOneTask(taskIter->m_uiTaskId);
	}
}