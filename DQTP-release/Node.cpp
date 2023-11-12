#include "StdAfx.h"
#include "Node.h"


CNode::CNode(void)
{
	m_dWeight=0;
}


CNode::~CNode(void)
{
}


CNode::CNode(const CNode& Node)
{
	m_uiNodeId=Node.m_uiNodeId;
	m_uiMemory=Node.m_uiMemory;
	m_uiUsedMemory=Node.m_uiUsedMemory;
	m_uiRemainMemory=Node.m_uiRemainMemory;
	m_dSwapProb=Node.m_dSwapProb;
	m_dTeleProb=Node.m_dTeleProb;
	m_lAdjLink=Node.m_lAdjLink;
	m_lAdjNode=Node.m_lAdjNode;
	m_mCarriedDemand=Node.m_mCarriedDemand;

	m_dWeight=Node.m_dWeight;
	m_uiFairMemoryAllocation=Node.m_uiFairMemoryAllocation;
}


void CNode::operator=(const CNode& Node)
{
	m_uiNodeId=Node.m_uiNodeId;
	m_uiMemory=Node.m_uiMemory;
	m_uiUsedMemory=Node.m_uiUsedMemory;
	m_uiRemainMemory=Node.m_uiRemainMemory;
	m_dSwapProb=Node.m_dSwapProb;
	m_dTeleProb=Node.m_dTeleProb;
	m_lAdjLink=Node.m_lAdjLink;
	m_lAdjNode=Node.m_lAdjNode;
	m_mCarriedDemand=Node.m_mCarriedDemand;

	m_dWeight=Node.m_dWeight;
	m_uiFairMemoryAllocation=Node.m_uiFairMemoryAllocation;
}