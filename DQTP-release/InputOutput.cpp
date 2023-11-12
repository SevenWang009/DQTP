#include "StdAfx.h"
#include "InputOutput.h"


CInputOutput::CInputOutput(void)
{
}


CInputOutput::~CInputOutput(void)
{
}


CInputOutput::CInputOutput(CNetwork* Network)
{
	m_pNetwork=Network;
}



void CInputOutput::GenerateNodeInfo(UINT nodeNum, double prob, double teleProb, UINT minMemory, UINT maxMemory)
{
	ofstream fout;
	fout.open("node.txt");
	for (NODEID nodeId=0; nodeId<nodeNum; nodeId++)
	{
		UINT memory=rand()%(maxMemory-minMemory+1) + minMemory;
		fout<<nodeId<<"	"<<prob<<"	"<<memory<<"	"<<teleProb<<endl;
	}
	fout.close();
}


void CInputOutput::GenerateNodePairs(UINT nodenum, UINT linknum, list<pair<NODEID,NODEID>>& pairs)
{
	set<pair<NODEID,NODEID>> pairsets;
	UINT remainNode=nodenum;
	UINT remainLink=linknum;
	for (UINT sourceId=0; sourceId<nodenum; sourceId++)
	{
		UINT averageDegree=(UINT)(remainLink/remainNode);
		UINT selectRange= (UINT) (2.5 * averageDegree);
		UINT idx=0;
		while (idx<averageDegree)
		{
			UINT sinkId=(sourceId + (rand()%selectRange) + 1)%nodenum;
			UINT presize=pairsets.size();
			pairsets.insert(make_pair(sourceId,sinkId));
			pairsets.insert(make_pair(sinkId,sourceId));
			if (pairsets.size()>presize)
			{
				pairs.push_back(make_pair(sourceId,sinkId));
				idx++;
			}
		}
		remainLink -= averageDegree;
		remainNode--;
	}
}

void CInputOutput::GenerateLinkInfo(UINT linkNum, UINT nodeNum, double prob, UINT slotTry, UINT minChannle, UINT maxChannel)
{
	ofstream fout;
	fout.open("link.txt");
	list<pair<NODEID,NODEID>> pairs;
	GenerateNodePairs(nodeNum,linkNum,pairs);
	LINKID linkId=0;
	list<pair<NODEID,NODEID>>::iterator pairIter;
	pairIter=pairs.begin();
	for (;pairIter!=pairs.end();pairIter++)
	{
		UINT channelNum=minChannle + (rand()%(maxChannel-minChannle+1));
		fout<<linkId<<"	"<<pairIter->first<<"	"<<pairIter->second<<"	"<<prob<<"	"<<prob*slotTry<<"	"<<channelNum<<endl;
		linkId++;
	}
	fout.close();
}

void CInputOutput::GenerateSDPairs(UINT nodeNum, UINT demandNum, UINT taskNum, map<TASKID, list<pair<NODEID,NODEID>>>& SDpairs)
{
	set<pair<NODEID,NODEID>> existingSDpair;
	UINT remainDemand=demandNum;
	UINT remainTask=taskNum;
	for (TASKID taskId=0; taskId<taskNum; taskId++)
	{
		existingSDpair.clear();
		UINT deNum=(UINT)(remainDemand/remainTask);
		UINT idx=0;
		while (idx<deNum)
		{
			NODEID sourceId, sinkId;
			UINT preSize=existingSDpair.size();
			while (preSize==existingSDpair.size())
			{
				sourceId=rand()%nodeNum;
				sinkId=rand()%nodeNum;
				if (sourceId==sinkId)
				{
					continue;
				}
				existingSDpair.insert(make_pair(sourceId,sinkId));
				existingSDpair.insert(make_pair(sinkId,sourceId));
			}
			SDpairs[taskId].push_back(make_pair(sourceId,sinkId));
			idx++;
		}
		remainDemand -= deNum;
		remainTask--;
	}
}

void CInputOutput::GenrateDemandInfo(UINT nodeNum, UINT demandNum, UINT taskNum, UINT minDemand, UINT maxDemand)
{
	ofstream fout;
	fout.open("demand.txt");
	map<TASKID, list<pair<NODEID,NODEID>>> SDpairs;
	GenerateSDPairs(nodeNum,demandNum,taskNum,SDpairs);
	DEMANDID demandId=0;
	for (TASKID taskId=0; taskId<taskNum; taskId++)
	{
		list<pair<NODEID,NODEID>>::iterator pairIter;
		pairIter=SDpairs[taskId].begin();
		for (;pairIter!=SDpairs[taskId].end();pairIter++)
		{
			UINT qubitNum=minDemand + (rand()%(maxDemand-minDemand+1));
			fout<<demandId<<"	"<<pairIter->first<<"	"<<pairIter->second<<"	"<<qubitNum<<"	"<<taskId<<endl;
			demandId++;
		}
	}
	fout.close();
}

void CInputOutput::GenerateInput()
{
	cout<<"Generate Input..."<<endl;
	ifstream fin;
	fin.open("basic.txt");
	string buf;
	getline(fin,buf);
	UINT nodeNum, linkNum, demandNum, taskNum, minMemory, maxMemory, minChannel, maxChannel, minDemand, maxDemand, trySlot;
	double nodeProb, linkProb, teleProb;
	fin>>nodeNum>>linkNum>>demandNum>>taskNum>>minMemory>>maxMemory>>minChannel>>maxChannel>>minDemand>>maxDemand>>trySlot;
	fin>>nodeProb>>linkProb>>teleProb;
	GenerateNodeInfo(nodeNum,nodeProb,teleProb, minMemory,maxMemory);
	GenerateLinkInfo(linkNum,nodeNum,linkProb,trySlot,minChannel,maxChannel);
	GenrateDemandInfo(nodeNum,demandNum,taskNum,minDemand,maxDemand);
}




void CInputOutput::ReadNodeInfo()
{
	cout<<"Read node information..."<<endl;
	ifstream fin;
	fin.open("node.txt");
	UINT nodeId, memory;
	double prob, telpProb;
	while (fin>>nodeId>>prob>>memory>>telpProb)
	{
		CNode newNode;
		newNode.m_uiNodeId=nodeId;
		newNode.m_uiMemory=memory;
		newNode.m_uiRemainMemory=memory;
		newNode.m_dTeleProb=telpProb;
		newNode.m_uiUsedMemory=0;
		newNode.m_dSwapProb=prob;
		newNode.m_dTeleProb=telpProb;
		m_pNetwork->m_vAllNode.push_back(newNode);
	}
	fin.close();
}

void CInputOutput::ReadLinkInfo()
{
	cout<<"Read link information..."<<endl;
	ifstream fin;
	fin.open("link.txt");
	UINT linkId, sourceId, sinkId, channel;
	double sinProb, aggProb;
	while(fin>>linkId>>sourceId>>sinkId>>sinProb>>aggProb>>channel)
	{
		CLink newLink;
		newLink.m_uiLinkId=linkId;
		newLink.m_uiSourceId=sourceId;
		newLink.m_uiSinkId=sinkId;
		newLink.m_dLinkProb=sinProb;
		newLink.m_dAggProb=aggProb;
		newLink.m_uiChannelNum=channel;
		newLink.m_uiRemainingChannel=channel;
		newLink.m_uiUsedChannel=0;
		m_pNetwork->m_vAllLink.push_back(newLink);
		m_pNetwork->m_mNodePairToLink[make_pair(sourceId,sinkId)]=linkId;
		m_pNetwork->m_mNodePairToLink[make_pair(sinkId,sourceId)]=linkId;
		m_pNetwork->m_vAllNode[sourceId].m_lAdjNode.push_back(sinkId);
		m_pNetwork->m_vAllNode[sinkId].m_lAdjNode.push_back(sourceId);
		m_pNetwork->m_vAllNode[sourceId].m_lAdjLink.push_back(linkId);
		m_pNetwork->m_vAllNode[sinkId].m_lAdjLink.push_back(linkId);
	}
	m_pNetwork->m_dSlotTry=aggProb/sinProb;
	fin.close();
}

void CInputOutput::ReadDemandInfo()
{
	cout<<"Read demand information..."<<endl;
	ifstream fin;
	fin.open("demand.txt");
	UINT demandId, sourceId, sinkId, qubitNum, taskId;
	while(fin>>demandId>>sourceId>>sinkId>>qubitNum>>taskId)
	{
		CDemand newDemand;
		newDemand.m_uiDemandId=demandId;
		newDemand.m_uiSourceId=sourceId;
		newDemand.m_uiSinkId=sinkId;
		newDemand.m_uiQubitNum=qubitNum;
		newDemand.m_uiRemainQubit=qubitNum;
		newDemand.m_uiHomeTask=taskId;
		m_pNetwork->m_vAllDemand.push_back(newDemand);
		m_mtaskDemands[taskId].push_back(demandId);
	}
	//set task info
	map<TASKID,list<DEMANDID>>::iterator taskIter;
	taskIter=m_mtaskDemands.begin();
	for (;taskIter!=m_mtaskDemands.end();taskIter++)
	{
		CTask newTask;
		newTask.m_uiTaskId=taskIter->first;
		newTask.m_lDemands=taskIter->second;
		newTask.m_bFinished=false;
		m_pNetwork->m_vAllTask.push_back(newTask);
	}
}

void CInputOutput::ReadInput()
{
	ReadNodeInfo();
	ReadLinkInfo();
	ReadDemandInfo();
}








//code for AEPR

void CInputOutput::GenerateQuantumNodes(UINT memory)
{
	CNode newNode;
	newNode.m_uiMemory=memory;
	for (UINT nodeId=0;nodeId<m_pNetwork->m_uiNodeNum;nodeId++)
	{
		newNode.m_uiNodeId=nodeId;
		m_pNetwork->m_vAllNode.push_back(newNode);
	}
}


void CInputOutput::GenerateNetworkTopology(UINT memory, double prob, double aggreeProb)
{
	CNode newNode;
	newNode.m_uiMemory=memory;
	for (UINT nodeId=0;nodeId<m_pNetwork->m_uiNodeNum;nodeId++)
	{
		newNode.m_uiNodeId=nodeId;
		m_pNetwork->m_vAllNode.push_back(newNode);
	}

	list<pair<NODEID,NODEID>> pairs;
	UINT nodeNum=m_pNetwork->m_uiNodeNum;
	GenerateNodePairs(nodeNum, 2*nodeNum, pairs);
	LINKID linkId=0;
	list<pair<NODEID,NODEID>>::iterator pairsIter;
	pairsIter=pairs.begin();
	for (;pairsIter!=pairs.end();pairsIter++)
	{
		NODEID sourceId=pairsIter->first;
		NODEID sinkId=pairsIter->second;
		CLink newLink;
		newLink.m_uiLinkId=linkId;
		newLink.m_uiSourceId=sourceId;
		newLink.m_uiSinkId=sinkId;
		newLink.m_uiChannelNum=5;
		newLink.m_uiRemainingChannel=5;
		newLink.m_uiUsedChannel=0;
		newLink.m_dLinkProb=prob;
		newLink.m_dAggProb=aggreeProb;
		m_pNetwork->m_vAllLink.push_back(newLink);
		m_pNetwork->m_mNodePairToLink[make_pair(sourceId,sinkId)]=linkId;
		m_pNetwork->m_mNodePairToLink[make_pair(sinkId,sourceId)]=linkId;
		m_pNetwork->m_vAllNode[sourceId].m_lAdjNode.push_back(sinkId);
		m_pNetwork->m_vAllNode[sinkId].m_lAdjNode.push_back(sourceId);
		m_pNetwork->m_vAllNode[sourceId].m_lAdjLink.push_back(linkId);
		m_pNetwork->m_vAllNode[sinkId].m_lAdjLink.push_back(linkId);
	}
}

void CInputOutput::ScaleDownDemands(NODEID nodeId, list<DEMANDID>& allDemands, map<DEMANDID,UINT>& newDemand)
{
	UINT totalDemand=0;
	multimap<UINT, DEMANDID, greater<UINT>> sortedDemand;
	list<DEMANDID>::iterator demandIter;
	demandIter=allDemands.begin();
	for (;demandIter!=allDemands.end();demandIter++)
	{
		UINT qubit=m_pNetwork->m_vAllDemand[*demandIter].m_uiQubitNum;
		sortedDemand.insert(make_pair(qubit,*demandIter));
		totalDemand+=qubit;
	}
	if (allDemands.size()>=m_pNetwork->m_vAllNode[nodeId].m_uiMemory)
	{//reserve the largest demands
		multimap<UINT, DEMANDID, greater<UINT>>::iterator sortedDemandIter;
		sortedDemandIter=sortedDemand.begin();
		for (UINT cnt=0;cnt<m_pNetwork->m_vAllNode[nodeId].m_uiMemory;cnt++)
		{
			newDemand[sortedDemandIter->second]=1;
			sortedDemandIter++;
		}
		for (;sortedDemandIter!=sortedDemand.end();sortedDemandIter++)
		{
			newDemand[sortedDemandIter->second]=0;
		}
	}
	//int remain=totalDemand-m_pNetwork->m_vAllNode[nodeId].m_uiMemory;
	//double scale=(m_pNetwork->m_vAllNode[nodeId].m_uiMemory*1.0)/(totalDemand*1.0);
	int remain=totalDemand-10;//m_pNetwork->m_vAllNode[nodeId].m_uiMemory;
	double scale=10.0/(totalDemand*1.0);
	multimap<UINT, DEMANDID, greater<UINT>>::iterator sortedDemandIter;
	sortedDemandIter=sortedDemand.begin();
	while (remain>0)
	{
		UINT newSize=(UINT)(sortedDemandIter->first * scale);
		newDemand[sortedDemandIter->second] = newSize;
		remain -= (sortedDemandIter->first - newSize);
		sortedDemandIter++;
	}
	for (;sortedDemandIter!=sortedDemand.end();sortedDemandIter++)
	{
		newDemand[sortedDemandIter->second]=sortedDemandIter->first;
	}
}

void CInputOutput::ScaleTrafficMatrix(TASKID taskId)
{
	map<NODEID,UINT> workload;
	map<NODEID,list<DEMANDID>> nodedemand;
	list<DEMANDID>::iterator demandIter;
	demandIter=m_pNetwork->m_vAllTask[taskId].m_lDemands.begin();
	for (;demandIter!=m_pNetwork->m_vAllTask[taskId].m_lDemands.end();demandIter++)
	{
		NODEID sourceId=m_pNetwork->m_vAllDemand[*demandIter].m_uiSourceId;
		NODEID sinkId=m_pNetwork->m_vAllDemand[*demandIter].m_uiSinkId;
		UINT qubitnum=m_pNetwork->m_vAllDemand[*demandIter].m_uiQubitNum;
		workload[sourceId]+=qubitnum;
		workload[sinkId]+=qubitnum;
		nodedemand[sourceId].push_back(*demandIter);
		nodedemand[sinkId].push_back(*demandIter);
	}
	map<NODEID,UINT>::iterator nodeIter;
	nodeIter=workload.begin();
	for (;nodeIter!=workload.end();nodeIter++)
	{
		if (nodeIter->second > m_pNetwork->m_vAllNode[nodeIter->first].m_uiMemory)
		{
			map<DEMANDID,UINT> newDemand;
			ScaleDownDemands(nodeIter->first,nodedemand[nodeIter->first],newDemand);
			map<DEMANDID,UINT>::iterator newDemandIter;
			newDemandIter=newDemand.begin();
			for (;newDemandIter!=newDemand.end();newDemandIter++)
			{
				DEMANDID preDemand=m_pNetwork->m_vAllDemand[newDemandIter->first].m_uiQubitNum;
				DEMANDID updatedDemand=newDemandIter->second;
				DEMANDID reduceDemand=preDemand-updatedDemand;
				NODEID sourceId=m_pNetwork->m_vAllDemand[newDemandIter->first].m_uiSourceId;
				NODEID sinkId=m_pNetwork->m_vAllDemand[newDemandIter->first].m_uiSinkId;
				workload[sourceId] -= reduceDemand;
				workload[sinkId] -= reduceDemand;
				m_pNetwork->m_vAllDemand[newDemandIter->first].m_uiQubitNum=newDemandIter->second;
				m_pNetwork->m_vAllDemand[newDemandIter->first].m_uiTeleportedQubit=0;
				m_pNetwork->m_vAllDemand[newDemandIter->first].m_uiRemainQubit=newDemandIter->second;
				if (updatedDemand==0)
				{
					m_pNetwork->m_vAllDemand[newDemandIter->first].m_bClosed=true;
					m_pNetwork->m_vAllDemand[newDemandIter->first].m_uiFinishTime=0;
				}
			}
		}
	}
}

void CInputOutput::GenerateTrafficMatrix(UINT demandNum, UINT taskNum)
{
	map<TASKID, list<pair<NODEID,NODEID>>> SDpairs;
	GenerateSDPairs(m_pNetwork->m_uiNodeNum, demandNum*taskNum, taskNum, SDpairs);
	DEMANDID demandId=0;
	for (TASKID taskId=0; taskId<taskNum; taskId++)
	{
		CTask newTask;
		newTask.m_uiTaskId=taskId;
		list<pair<NODEID,NODEID>>::iterator pairIter;
		pairIter=SDpairs[taskId].begin();
		for (;pairIter!=SDpairs[taskId].end();pairIter++)
		{
			//UINT qubitNum=1 + (rand()%(m_pNetwork->m_vAllNode[0].m_uiMemory));
			UINT qubitNum=1 + (rand()%10);
			newTask.m_lDemands.push_back(demandId);
			CDemand newDemand;
			newDemand.m_uiDemandId=demandId;
			newDemand.m_uiSourceId=pairIter->first;
			newDemand.m_uiSinkId=pairIter->second;
			newDemand.m_uiQubitNum=qubitNum;
			newDemand.m_uiRemainQubit=qubitNum;
			newDemand.m_uiTeleportedQubit=0;
			m_pNetwork->m_vAllDemand.push_back(newDemand);
			demandId++;
		}
		m_pNetwork->m_vAllTask.push_back(newTask);
		ScaleTrafficMatrix(taskId);
	}
}


double CInputOutput::OutputATCT()
{
	UINT totalTime=0;
	UINT taskNum=m_pNetwork->m_vAllTask.size();
	vector<CTask>::iterator taskIter;
	taskIter=m_pNetwork->m_vAllTask.begin();
	for (;taskIter!=m_pNetwork->m_vAllTask.end();taskIter++)
	{
		totalTime += taskIter->m_uiCompleteSlot;
	}
	double atct=totalTime*1.0/(taskNum*1.0);
	ofstream fout;
	fout.open("actc.txt");
	fout<<atct<<endl;
	fout.close();
	return atct;
}


void CInputOutput::ReadBasicInfo()
{
	ifstream fin;
	fin.open("basic.txt");
	UINT nodeNum, tryNum, memory;
	double prob;
	fin>>nodeNum>>prob>>tryNum>>memory;
	m_pNetwork->m_uiNodeNum=nodeNum;
	GenerateNetworkTopology(memory,prob,tryNum*prob);
	UINT demandNum, taskNum;
	fin>>demandNum>>taskNum;
	GenerateTrafficMatrix(demandNum,taskNum);
}