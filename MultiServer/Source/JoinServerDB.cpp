#include "stdafx.h"
#include "MD5.h"
#include "JoinServerDB.h"
#include "Window.h"
#include "Server.h"
#include "JoinServer.h"

CQuery g_JoinServerDB;
CQuery g_JoinServerConnectDB;

#define __AUTOREG__  1

int IsUser(char *szId, char *pass, char * JoominNumber, char & Block, int & DBNumber)
{
	char szQuery[512];

	sprintf(szQuery,"SELECT memb_guid, memb___id, sno__numb, bloc_code from MEMB_INFO WHERE memb___id='%s'",
		szId);

	if(g_JoinServerDB.Exec(szQuery) == FALSE)
	{
		g_Window.ServerLogAdd(ST_JOINSERVER,"error-L3 : ID:'%s' PASS:'%s'",szId, pass);
		g_JoinServerDB.Clear();
		return 2;
	}
	
	if(g_JoinServerDB.Fetch() == SQL_NO_DATA)
	{
		g_JoinServerDB.Clear();

		if( g_JSInfo.bAutoreg == TRUE && strlen(szId) >= g_JSInfo.AutoRegMinLength && strlen(pass) >= g_JSInfo.AutoRegMinLength )
		{
			if( g_JSInfo.bMD5 == TRUE )
			{
				sprintf(szQuery,"INSERT INTO MEMB_INFO (memb___id,memb__pwd,bloc_code,ctl1_code) VALUES ('%s',dbo.fn_md5('%s','%s'),0,0)",
					szId,pass,szId);
			}
			else
			{
				sprintf(szQuery,"INSERT INTO MEMB_INFO (memb___id,memb__pwd,bloc_code,ctl1_code) VALUES ('%s','%s',0,0)",
					szId,pass);
			}

			if( g_JoinServerDB.Exec(szQuery) == FALSE )
			{
				g_Window.ServerLogAdd(ST_JOINSERVER,"error-L3 : ID:'%s' PASS:'%s'",szId, pass);
				g_JoinServerDB.Clear();
				return 2;
			}

			g_JoinServerDB.Clear();
	
			sprintf(szQuery,"SELECT memb_guid, memb___id, sno__numb, bloc_code from MEMB_INFO WHERE memb___id='%s'",szId);
	
			if(g_JoinServerDB.Exec(szQuery) == FALSE)
			{
				g_Window.ServerLogAdd(ST_JOINSERVER,"error-L3 : ID:'%s' PASS:'%s'",szId, pass);
				g_JoinServerDB.Clear();
				return 2;
			}

			if(g_JoinServerDB.Fetch() == SQL_NO_DATA)
			{
				g_JoinServerDB.Clear();
				return 2;
			}
		}
		else
		{
			return 2;
		}
	}

	DBNumber = g_JoinServerDB.GetInt("memb_guid");

	char szBlock[2]="";

	g_JoinServerDB.GetStr("sno__numb", JoominNumber);
	g_JoinServerDB.GetStr("bloc_code", szBlock);

	Block = szBlock[0];	

	if(g_JSInfo.bMD5 == TRUE)
	{
		BYTE btPass[16]={0};
		g_JoinServerDB.Clear();

		sprintf(szQuery,"SELECT memb__pwd FROM MEMB_INFO WHERE memb___id = '%s'",szId);
		if(g_JoinServerDB.ReadBlob(szQuery,btPass) <= 0)
		{
			g_JoinServerDB.Clear();
			return 0;
		}

		DWORD dwAccKey = MakeAccountKey(szId);
		MD5 pMD5Hash;

		if( pMD5Hash.MD5_CheckValue(pass, (char*)btPass, dwAccKey) == FALSE )
		{
			g_JoinServerDB.Clear();
			return 0;
		}
	}
	else
	{
		char szPass[MAX_IDSTRING+1] = {0};
		g_JoinServerDB.Clear();

		sprintf(szQuery,"SELECT memb__pwd FROM MEMB_INFO WHERE memb___id = '%s'",szId);

		if(g_JoinServerDB.Exec(szQuery) == FALSE || g_JoinServerDB.Fetch() == SQL_NO_DATA)
		{
			g_JoinServerDB.Clear();
			return 0;
		}

		g_JoinServerDB.GetStr("memb__pwd",szPass);
		g_JoinServerDB.Clear();

		if(strlen(szPass) < 1)
		{
			return 0;
		}

		if(strcmp(szPass,pass) != NULL)
		{
			return 0;
		}
	}

	g_JoinServerDB.Clear();
	return TRUE;
}

void MembStat_Connect(char * AccountId, char *ServerName, char *Ip)
{
	char szQuery[256];

	g_JoinServerConnectDB.Clear();
	sprintf(szQuery,"exec WZ_CONNECT_MEMB '%s','%s','%s'", AccountId, ServerName, Ip);

	if(g_JoinServerConnectDB.Exec(szQuery) == FALSE)
	{
		g_Window.ServerLogAdd(ST_JOINSERVER,"error-L1 : %s", szQuery);
	}
}

void MembStat_Disconnect(char * AccountId)
{
	char szQuery[256];

	g_JoinServerConnectDB.Clear();
	sprintf(szQuery,"exec WZ_DISCONNECT_MEMB '%s'", AccountId);

	if( g_JoinServerConnectDB.Exec(szQuery) == FALSE )
	{
		g_Window.ServerLogAdd(ST_JOINSERVER,"error-L1 : %s", szQuery);
	}
}

void AccountBlock(char* szAccountId,int blockcode)
{
	char szQuery[256];

	sprintf(szQuery,"UPDATE MEMB_INFO SET bloc_code = %c WHERE memb___id = '%s'",szAccountId,blockcode);

	if( g_JoinServerDB.Exec(szQuery) == FALSE )
	{
		g_Window.ServerLogAdd(ST_JOINSERVER,"error-L1 : %s",szQuery);
	}
}

int CreateHeart()
{
	char szQuery[256];
	sprintf(szQuery,"SELECT heart_count from LoveHeartCount where Number=0");

	int HeartCount = g_JoinServerDB.ExecGetInt(szQuery);

	if( HeartCount < 0 ) return -1;

	HeartCount += 1;
	if( HeartCount > 217000 ) return -1;
	
	sprintf(szQuery,"UPDATE LoveHeartCount SET heart_count= %d where Number=0", HeartCount);
	if( g_JoinServerDB.Exec(szQuery) == FALSE )
	{
		return -1;
	}
	return HeartCount;
}

int LoveHeartLuckNumberCount=84;
int LoveHeartLuckNumber[84+350+400] = 
{
	 	7,  77,  777,  7777,  77777,
	 1,  11,  111,  1111,  11111, 111111,
	10, 100, 1000, 10000, 100000,
	20, 200, 2000, 20000, 200000,
	30, 300, 3000, 30000,
	40, 400, 4000, 40000,
	50, 500, 5000, 50000,
	60, 600, 6000, 60000,
	70, 700, 7000, 70000,
	80, 800, 8000, 80000,
	90, 900, 9000, 90000,
	 2,  22,  222,  2222, 22222,
	 3,  33,  333,  3333, 33333,
	 4,  44,  444,  4444, 44444,
	 5,  55,  555,  5555, 55555,
	 6,  66,  666,  6666, 66666,
	 8,  88,  888,  8888, 88888,
	 9,  99,  999,  9999, 99999,
	 // 축복의 보석 시작
 	114391,
	24037,
	42955,
	63613,
	135259,
	93043,
	45085,
	173539,
	174199,
	59335,
	64723,
	52303,
	72451,
	179209,
	91051,
	112225,
	107671,
	89467,
	99415,
	132817,
	153439,
	75373,
	7489,
	105043,
	120217,
	67303,
	154033,
	97513,
	184747,
	58555,
	70033,
	133819,
	2935,
	33571,
	94873,
	185461,
	131935,
	149965,
	68371,
	89875,
	137917,
	50047,
	44569,
	87877,
	154081,
	11305,
	10537,
	20557,
	168847,
	59011,
	17305,
	42547,
	73177,
	143635,
	65287,
	105427,
	55477,
	62185,
	168403,
	84991,
	47587,
	63379,
	135853,
	165511,
	169597,
	29371,
	85477,
	12397,
	90571,
	140113,
	48133,
	81865,
	39391,
	126487,
	70195,
	60205,
	6511,
	18367,
	156613,
	140395,
	67423,
	83569,
	85387,
	128755,
	138169,
	111361,
	8833,
	123313,
	127519,
	61801,
	102919,
	115813,
	172483,
	71923,
	180127,
	96361,
	152281,
	70609,
	82975,
	175387,
	133537,
	138799,
	4195,
	139201,
	155347,
	97477,
	174493,
	116407,
	23959,
	49915,
	46825,
	2161,
	81661,
	163969,
	153769,
	103873,
	80635,
	44641,
	82207,
	65449,
	75475,
	111505,
	67261,
	106687,
	71983,
	22807,
	42961,
	61957,
	183157,
	99751,
	112567,
	58165,
	192709,
	6295,
	11677,
	85663,
	80521,
	48307,
	150715,
	139411,
	16975,
	133651,
	40453,
	150031,
	4435,
	184285,
	29527,
	5965,
	32125,
	187747,
	191569,
	65887,
	136129,
	139921,
	128101,
	103363,
	51241,
	32809,
	12979,
	32017,
	9025,
	139099,
	156445,
	156511,
	9511,
	174271,
	4387,
	168985,
	103915,
	144145,
	142345,
	141163,
	195457,
	82219,
	158857,
	175861,
	126295,
	108475,
	66577,
	2113,
	130369,
	176695,
	101767,
	92809,
	60889,
	155545,
	112501,
	84001,
	28309,
	3679,
	40885,
	6973,
	70969,
	94249,
	153793,
	112201,
	117955,
	72913,
	168043,
	77791,
	29161,
	40213,
	53233,
	66037,
	185635,
	83077,
	95443,
	55801,
	176965,
	182593,
	191827,
	125995,
	22291,
	26161,
	145111,
	98821,
	50143,
	1105,
	155785,
	124003,
	15331,
	132679,
	17971,
	128059,
	147259,
	793,
	89263,
	75127,
	97027,
	153817,
	121249,
	157561,
	61411,
	137443,
	184033,
	13087,
	108607,
	148873,
	151891,
	95779,
	120235,
	55639,
	17203,
	169003,
	110113,
	36805,
	13621,
	188557,
	124783,
	161701,
	72073,
	129253,
	153049,
	126889,
	43849,
	166849,
	140425,
	184117,
	34117,
	33145,
	49423,
	108427,
	22717,
	16201,
	107743,
	102763,
	112789,
	143077,
	29233,
	63691,
	141859,
	107197,
	32401,
	22327,
	20887,
	64423,
	81817,
	66655,
	32467,
	181573,
	73771,
	5623,
	191893,
	21979,
	33109,
	179527,
	101617,
	186247,
	32407,
	154123,
	42607,
	45817,
	36307,
	195343,
	15553,
	54175,
	168607,
	99313,
	31153,
	114025,
	78043,
	132055,
	125935,
	134173,
	44863,
	103321,
	95053,
	117613,
	33907,
	106471,
	23095,
	189643,
	63895,
	31273,
	107965,
	56539,
	17701,
	176629,
	102025,
	185209,
	10699,
	71701,
	120721,
	90019,
	49897,
	14635,
	179791,
	40657,
	46759,
	174025,
	162949,
	102313,
	165193,
	60919,
	54103,
	151681,
	11821,
	97891,
	170029,
	88363,
	189019,
	31987,
	47809,
	149557,
	195679,
	46795,
	48367,
	154603,
	128473,
	107005,

	// 영화티켓
	133456,
	28043,
	50114,
	74215,
	157802,
	108550,
	52599,
	202462,
	203232,
	69224,
	75510,
	61020,
	84526,
	209077,
	106226,
	130929,
	125616,
	104378,
	115984,
	154953,
	179012,
	87935,
	8737,
	122550,
	140253,
	78520,
	179705,
	113765,
	215538,
	68314,
	81705,
	156122,
	3424,
	39166,
	110685,
	216371,
	153924,
	174959,
	79766,
	104854,
	160903,
	58388,
	51997,
	102523,
	179761,
	13189,
	12293,
	23983,
	196988,
	68846,
	20189,
	49638,
	85373,
	167574,
	76168,
	122998,
	72549,
	196470,
	99156,
	55518,
	73942,
	158495,
	193096,
	197863,
	34266,
	99723,
	14463,
	105666,
	163465,
	56155,
	95509,
	45956,
	147568,
	81894,
	70239,
	7596,
	21428,
	182715,
	163794,
	78660,
	97497,
	99618,
	150214,
	161197,
	129921,
	10305,
	143865,
	148772,
	72101,
	120072,
	135115,
	201230,
	83910,
	210148,
	112421,
	177661,
	82377,
	96804,
	204618,
	155793,
	161932,
	4894,
	162401,
	181238,
	113723,
	203575,
	135808,
	27952,
	58234,
	54629,
	2521,
	95271,
	191297,
	179397,
	121185,
	94074,
	52081,
	95908,
	76357,
	88054,
	130089,
	78471,
	124468,
	83980,
	26608,
	50121,
	72283,
	213683,
	116376,
	131328,
	67859,
	7344,
	13623,
	99940,
	93941,
	56358,
	175834,
	162646,
	19804,
	155926,
	47195,
	175036,
	5174,
	214999,
	34448,
	6959,
	37479,
	76868,
	158817,
	163241,
	149451,
	120590,
	59781,
	38277,
	15142,
	37353,
	10529,
	162282,
	182519,
	182596,
	11096,
	203316,
	5118,
	197149,
	121234,
	168169,
	166069,
	164690,
	95922,
	185333,
	205171,
	147344,
	126554,
	77673,
	2465,
	152097,
	206144,
	118728,
	108277,
	71037,
	181469,
	131251,
	98001,
	33027,
	4292,
	47699,
	8135,
	82797,
	109957,
	179425,
	130901,
	137614,
	85065,
	196050,
	90756,
	34021,
	46915,
	62105,
	77043,
	216574,
	96923,
	111350,
	65101,
	206459,
	213025,
	146994,
	26006,
	30521,
	169296,
	115291,
	58500,
	1289,
	181749,
	144670,
	17886,
	154792,
	20966,
	149402,
	171802,
	925,
	104140,
	87648,
	113198,
	179453,
	141457,
	183821,
	71646,
	160350,
	214705,
	15268,
	126708,
	173685,
	177206,
	111742,
	140274,
	64912,
	20070,
	197170,
	128465,
	42939,
	15891,
	145580,
	188651,
	84085,
	150795,
	178557,
	148037,
	51157,
	194657,
	163829,
	214803,
	39803,
	38669,
	57660,
	126498,
	26503,
	18901,
	125700,
	119890,
	131587,
	166923,
	34105,
	74306,
	165502,
	125063,
	37801,
	26048,
	24368,
	75160,
	95453,
	77764,
	37878,
	211835,
	86066,
	6560,
	25642,
	38627,
	209448,
	118553,
	37808,
	179810,
	49708,
	53453,
	42358,
	18145,
	63204,
	196708,
	115865,
	36345,
	133029,
	91050,
	154064,
	146924,
	156535,
	52340,
	120541,
	110895,
	137215,
	39558,
	124216,
	26944,
	74544,
	36485,
	125959,
	65962,
	20651,
	206067,
	119029,
	216077,
	12482,
	83651,
	140841,
	105022,
	58213,
	17074,
	209756,
	47433,
	54552,
	203029,
	190107,
	119365,
	192725,
	71072,
	63120,
	176961,
	13791,
	114206,
	198367,
	103090,
	37318,
	55777,
	174483,
	54594,
	56428,
	180370,
	149885,
	124839,
	5958,
	166825,
	196645,
	56323,
	184892,
	61580,
	86955,
	136228,
	54657,
	62021,
	19006,
	73172,
	204198,
	214866,
	50310,
	114080,
	139819,
	170976,
	144271,
	116824,
	24872,
	135325,
	185844,
	34553,
	146609,
	138657,
	7862,
	94452,
	194209,
	145909,
	22051,
	91421,
	101228,
	89461,
	155275,
	154484,
	1303,
	139651,
	199130,
	6854,
	16801,
	205052,
	107710,
	105288,
	80942,
	126414,
	95873,
	144390,
	107437,
	69371,
	214488,
	131146,
	169940,
	102453,
	192200,
	44647,
	198283,
	32215,
	100983,
	161883,
	61790,
	99338,
	16794
};


int LoveHeartEventCheck(int number)
{
	for( int n=0; n<sizeof(LoveHeartLuckNumber)/sizeof(int); n++)
	{
		if( LoveHeartLuckNumber[n] == number )
		{
			return TRUE;
		}
	}
	return FALSE;
}

int HeartEventLuck(int heart_count, char *id, char *server, char *name)
{
	char szQuery[256];
	sprintf(szQuery,"INSERT INTO LoveHeart ( Number, Id, Server, Name ) Values (%d, '%s','%s','%s')", heart_count, id, server, name);
	
	if( g_JoinServerDB.Exec(szQuery) == FALSE )
	{
		return FALSE;
	}
	return TRUE;
}

int HeartEventAll(int heart_count, char *id, char *server, char *name)
{
	char szQuery[256];
	sprintf(szQuery,"INSERT INTO LoveHeartAll ( Number, Id, Server, Name ) Values (%d, '%s','%s','%s')", heart_count, id, server, name);
	
	if( g_JoinServerDB.Exec(szQuery) == FALSE )
	{
		return FALSE;
	}
	return TRUE;
}

int CreateHeartAll()
{
	char szQuery[256];
		
	sprintf(szQuery,"UPDATE LoveHeartCount SET heart_count=heart_count+1  where Number=1");
	if( g_JoinServerDB.Exec(szQuery) == FALSE )
	{
		return -1;
	}
	return TRUE;
}

void RemoveBill(char* szId)
{
	char szQuery[256];

#ifdef ZTEAMDB
	sprintf(szQuery,"DELETE FROM PremiumData WHERE AccountID = '%s'",szId);
#else
	sprintf(szQuery,"DELETE FROM MEMB_PREMIUM WHERE memb___id = '%s'",szId);
#endif

	g_JoinServerConnectDB.Exec(szQuery);

	int iIndex = gObjSearchUser(szId);

	if(iIndex < 0 || iIndex >= MAX_USEROBJECT)
		return;

	SDHP_BILLKILLUSER pMsg;

	pMsg.h.c		= PMHC_BYTE;
	pMsg.h.size		= sizeof( pMsg );
	pMsg.h.headcode = 0x07;

	strcpy(pMsg.Id,szId);
	pMsg.Number = gObj[iIndex].gaIndex;

	DataSend(gObj[iIndex].GameServerIndex,(LPBYTE)&pMsg,sizeof( pMsg ));

	g_Window.ServerLogAdd(ST_JOINSERVER,"[PREMIUM] Bill end usetime [%s][%d]",szId,gObj[iIndex].gaIndex);
}

int GetBill(int number,char* szId,SDHP_BILLSEARCH_RESULT * lpMsg)
{
	char szQuery[512];

	int iIndex = gObjSearchUser(szId);

	if(iIndex < 0 || iIndex >= MAX_USEROBJECT)
		return FALSE;

	int PayCode = 0;
	int LeftTime = -1;
#ifdef ZTEAMDB
	sprintf(szQuery,"SELECT PayCode,DATEDIFF(mi,GETDATE(),ExpireDate) as LeftTime,DATEPART(year,ExpireDate) as EYEAR,DATEPART(month,ExpireDate) as EMONTH,DATEPART(day,ExpireDate) as EDAY,DATEPART(hour,ExpireDate) as EHOUR,DATEPART(minute,ExpireDate) as EMINUTE FROM PremiumData WHERE AccountID = '%s'",szId);
#else
	sprintf(szQuery,"SELECT pay_code,DATEDIFF(mi,GETDATE(),ExpireDate) as LeftTime,DATEPART(year,ExpireDate) as EYEAR,DATEPART(month,ExpireDate) as EMONTH,DATEPART(day,ExpireDate) as EDAY,DATEPART(hour,ExpireDate) as EHOUR,DATEPART(minute,ExpireDate) as EMINUTE FROM MEMB_PREMIUM WHERE memb___id = '%s'",szId);
#endif

	if( g_JoinServerConnectDB.Exec(szQuery) == TRUE && g_JoinServerConnectDB.Fetch() != SQL_NO_DATA)
	{
		LeftTime = g_JoinServerConnectDB.GetInt("LeftTime");
#ifdef ZTEAMDB
		PayCode = g_JoinServerConnectDB.GetInt("PayCode");
#else
		PayCode = g_JoinServerConnectDB.GetInt("pay_code");
#endif
		int end_year = g_JoinServerConnectDB.GetInt("EYEAR");
		int end_month = g_JoinServerConnectDB.GetInt("EMONTH");
		int end_day = g_JoinServerConnectDB.GetInt("EDAY");
		int end_hour = g_JoinServerConnectDB.GetInt("EHOUR");
		int end_minute = g_JoinServerConnectDB.GetInt("EMINUTE");

		g_JoinServerConnectDB.Clear();

		if(LeftTime <= 0)
		{
			PayCode = 0;
			RemoveBill(szId);
		}
		else
		{
			if( gObj[iIndex].lpBill != NULL )
				delete gObj[iIndex].lpBill;

			gObj[iIndex].lpBill = new BillStruct;
			gObj[iIndex].lpBill->number = number;
			gObj[iIndex].lpBill->PayCode = PayCode;
			gObj[iIndex].lpBill->EndTime = LeftTime;

			lpMsg->EndTime = LeftTime;
			sprintf(lpMsg->EndDays,"%04d%02d%02d%02d%02d",end_year,end_month,end_day,end_hour,end_minute);

			g_Window.ServerLogAdd(ST_JOINSERVER,"[PREMIUM] Bill user add [%s] PayCode[%d] Ends:(%04d-%02d-%02d %02d:%02d)",szId,PayCode,end_year,end_month,end_day,end_hour,end_minute);
		}
	}
	else
	{
		g_JoinServerConnectDB.Clear();
		if( g_JSInfo.bPremiumBlock == TRUE )
		{
			PayCode = 0;
			RemoveBill(szId);
		}
	}
	return PayCode;
}
