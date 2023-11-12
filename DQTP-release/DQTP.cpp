// DQTP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "InputOutput.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CNetwork Network;
	CInputOutput Inputoutput(&Network);
	//int res=0;
	//for (int i=0;i<200;i++)
	//{
	//	double test=(rand()%10000)/10000.0;
	//	if (test<0.0045)
	//	{
	//		res++;
	//		cout<<test<<"	";
	//	}
	//}
	//cout<<res;
	//getchar();
//	Inputoutput.GenerateInput();
	//Inputoutput.ReadInput();
	//Network.InitRoutingInfo();
//	Network.SimTSC(20);
	//Network.SimTSC_TASK();
//	Network.SimREA(20);
//	Network.SimNRDQTP(4000);
//	cout<<Network.generateNum<<"	"<<Network.testNum<<endl;
//	Network.SimNRDQTP_TASK();
//	Network.SimDQTP_TASK();
	Inputoutput.ReadBasicInfo();
	Network.SimulateAEPR();
	Inputoutput.OutputATCT();
	return 0;
}

