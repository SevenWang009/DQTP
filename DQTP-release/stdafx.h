// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <vector>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <algorithm>
#include <time.h>
using namespace std;

typedef UINT NODEID;
typedef UINT LINKID;
typedef UINT DEMANDID;
typedef UINT TASKID;
typedef UINT SLOTID;
typedef UINT EVENTID;
typedef UINT TIMEUNT;
typedef UINT TIMESLOT;
typedef UINT EVENTID;
typedef UINT PATHID;
typedef double WEIGHT;

const UINT IMPOSSIBLESTAGE=-1;
const UINT IMPOSSIBLEID=-1;
const double THRESHOLD=0.01;
const double SMALLNUM=0.0001;
const double INFSMALL=0.000001;
const EVENTID IMPOSSIBLEEVENT=-1;
const UINT INFUINT=100000000;
const double INF=10000000000.0;
const double LARGEVALUE=100000.0;

enum EVENTTYPE {CREATEENTANGLE, DESTORYENTANGLE};
enum ENTANGLETYPE {SINGLEHOP, MULTIHOP};
enum WINSTATE {SS, CA};