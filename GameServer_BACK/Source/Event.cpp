#include "stdafx.h"
#include "Event.h"
#include "logproc.h"
#include "gObjMonster.h"
#include "DSProtocol.h"
#include "GameServer.h"
#include "GameMain.h"
#include "..\common\winutil.h"
#include "BloodCastle.h"
#include "TNotice.h"
#include "PCBangPointSystem.h"
#include "DoubleGoerEvent.h"
#include "protocol.h"
#include "GameEvent.h"


void EventChipEventProtocolCore(BYTE protoNum, LPBYTE aRecv, int aLen)
{
#if (TRACE_PROTOCOL==1)
	LogAddHeadHex("EVENT_SERVER", aRecv, aLen);
#endif

	switch ( protoNum )
	{
		case 0x01:
			EGRecvEventChipInfo((PMSG_ANS_VIEW_EC_MN *)aRecv);
			break;
		case 0x02:
			EGResultRegEventChip((PMSG_ANS_REGISTER_EVENTCHIP *)aRecv);
			break;
		case 0x03:
			EGRecvRegMutoNum((PMSG_ANS_REGISTER_MUTONUM *)aRecv);
			break;
		case 0x04:
			EGRecvChangeRena((PMSG_ANS_RESET_EVENTCHIP *)aRecv);
			break;
		case 0x05:
			EGRecvStoneInfo((PMSG_ANS_VIEW_STONES *)aRecv);
			break;
		case 0x06:
			EGRecvRegStone((PMSG_ANS_REGISTER_STONES *)aRecv);
			break;
		case 0x07:
			EGRecvDeleteStone((PMSG_ANS_DELETE_STONES *)aRecv);
			break;
		case 0x09:
			EGRecvChangeStones((PMSG_ANS_RESET_EVENTCHIP *)aRecv);
			break;
		case 0x08:
			EGRecv2AnvRegSerial((PMSG_ANS_2ANIV_SERIAL *)aRecv);
			break;
		case 0x10:
			EGRecvRegRingGift((PMSG_ANS_REG_RINGGIFT *)aRecv);
			break;
		case 0x15:
			EGAnsRegCCOfflineGift((PMSG_ANS_REG_CC_OFFLINE_GIFT *)aRecv);
			break;
		case 0x16:
			EGAnsRegDLOfflineGift((PMSG_ANS_REG_DL_OFFLINE_GIFT *)aRecv);
			break;
		case 0x17:
			EGAnsRegHTOfflineGift((PMSG_ANS_REG_HT_OFFLINE_GIFT *)aRecv);
			break;
#if (__CUSTOM_QUEST__ == 1)
		case 0x65:
			{
				PBMSG_HEAD2 * lpRecv = (PBMSG_HEAD2*)aRecv;

				switch(lpRecv->subcode)
				{
				case 0x01:
					g_MyQuest.AnsUserInfo(aRecv);
					break;
				}
			}
			break;
#endif
#if (CUSTOM_PETS == 1)
		case 0x66:
			{
				PBMSG_HEAD2 * lpRecv = (PBMSG_HEAD2*)aRecv;

				switch(lpRecv->subcode)
				{

				case 0x00:
					gCustomPets.SummoInfoAnswer((LPBMSG_ANSSUMMONINFO)aRecv);
					break;
				}
			}
			break;
#endif
#if (CUSTOM_OLYMP==1)
		case 0x68:
			{
				PBMSG_HEAD2 * lpRecv = (PBMSG_HEAD2*)aRecv;

				switch(lpRecv->subcode)
				{
				case 0x00:
					g_CustomOlymp.GS_DGAnsOlympInitData(aRecv);
					break;
				case 0x01:
					g_CustomOlymp.GS_DGAnsOlympDateChange(aRecv);
					break;
				case 0x02:
					g_CustomOlymp.GS_DGAnsOlympReg(aRecv);
					break;
				case 0x05:
					g_CustomOlymp.GS_DGAnsOlympReward(aRecv);
					break;
				case 0x06:
					g_CustomOlymp.GS_DGAnsOlympStats(aRecv);
					break;
				}
			}
			break;
#endif
	}
}


void DataSendEventChip(PCHAR pMsg, int size)
{
	if ( IsEventChipServerConnected == FALSE )
	{
		wsEvenChipServerCli.Close();
		wsEvenChipServerCli.CreateSocket(ghWnd);
		if ( GMEventChipServerConnect(EventServerIP, WM_GM_EVENTCHIP_CLIENT_MSG_PROC) == FALSE )
		{
			IsEventChipServerConnected = FALSE;
			LogAddTD("Can not connect EventChip Server");

			return;
		}

		::IsEventChipServerConnected = TRUE;

	}

	if ( ::IsEventChipServerConnected != FALSE )
	{
		::wsEvenChipServerCli.DataSend((char *)pMsg, size);
	}
}

#pragma warning ( disable : 4101 )
void EGRecvEventChipInfo(PMSG_ANS_VIEW_EC_MN * aRecv)
{
	LPOBJ lpObj = &gObj[aRecv->iINDEX];

	PMSG_EVENTCHIPINFO eventchipeventinfo;
	char msg[255];

	PHeadSetB((LPBYTE)&eventchipeventinfo, 0x94, sizeof(eventchipeventinfo));
	eventchipeventinfo.Type = 0;
	eventchipeventinfo.ChipCount = aRecv->nEVENT_CHIPS;
	lpObj->EventChipCount = aRecv->nEVENT_CHIPS;
	lpObj->MutoNumber = aRecv->iMUTO_NUM;

	DataSend(lpObj->m_Index, (LPBYTE)&eventchipeventinfo, eventchipeventinfo.h.size);

	lpObj->UseEventServer = FALSE;
}
#pragma warning ( default : 4101 )



void EGResultRegEventChip(PMSG_ANS_REGISTER_EVENTCHIP * aRecv)
{
	PMSG_REGEVENTCHIP_RESULT Result;
	LPOBJ lpObj;
	int aIndex;

	PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
	lpObj = &gObj[aRecv->iINDEX];
	aIndex = aRecv->iINDEX;

	if ( aRecv->bSUCCESS == FALSE )
	{
		Result.ChipCount = -1;
		LogAddTD("[EventChip] [%s][%s] RegEventServer Fail (RegEventchip) %d",
			lpObj->AccountID, lpObj->Name, aRecv->Pos);

	}
	else
	{
		Result.ChipCount = aRecv->nEVENT_CHIPS;
		gObjInventoryDeleteItem(aIndex, aRecv->Pos);
		GCInventoryItemDeleteSend(aIndex, aRecv->Pos, 1);

		LogAddTD("[EventChip] [%s][%s] Delete EventChip (%d)",
			lpObj->AccountID, lpObj->Name, aRecv->Pos);
	}

	Result.Type = 0;

	DataSend(aIndex, (LPBYTE)&Result, Result.h.size);

	lpObj->UseEventServer = FALSE;
}




void EGRecvRegMutoNum( PMSG_ANS_REGISTER_MUTONUM* aRecv)
{
	LPOBJ lpObj;
	int aIndex;
	

	lpObj = &gObj[aRecv->iINDEX];
	aIndex = aRecv->iINDEX;

	PMSG_GETMUTONUMBER_RESULT Result;

	PHeadSetB((LPBYTE)&Result, 0x96, sizeof(Result));
	
	if ( gObjFind10EventChip(aIndex) == FALSE )
	{
		Result.MutoNum[0] = -1;
		Result.MutoNum[1] = 0;
		Result.MutoNum[2] = 0;

		DataSend(aIndex, (LPBYTE)&Result, Result.h.size);

		lpObj->UseEventServer = FALSE;

		return;
	}

	gObjDelete10EventChip(aIndex);
	Result.MutoNum[0] = aRecv->iMUTO_NUM / 1000000;
	Result.MutoNum[1] = aRecv->iMUTO_NUM / 1000 - aRecv->iMUTO_NUM / 1000000 * 1000;
	Result.MutoNum[2] = aRecv->iMUTO_NUM % 1000;
	lpObj->MutoNumber = aRecv->iMUTO_NUM;
	
	LogAddTD("[EventChip] [%s][%s] Make MutoNumber %d,%d,%d",
		lpObj->AccountID, lpObj->Name, 
		Result.MutoNum[0], Result.MutoNum[1], Result.MutoNum[2]);

	DataSend(aIndex, (LPBYTE)&Result, Result.h.size);
	lpObj->UseEventServer = FALSE;
}



void EGRecvChangeRena( PMSG_ANS_RESET_EVENTCHIP* aRecv)
{
	PMSG_REGEVENTCHIP_RESULT Result;
	LPOBJ lpObj;
	int aIndex;

	PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
	lpObj = &gObj[aRecv->iINDEX];
	aIndex = aRecv->iINDEX;

	if ( aRecv->bSUCCESS != FALSE )
	{
		lpObj->Money += lpObj->EventChipCount * 3000;
		GCMoneySend(aIndex, lpObj->Money);

		LogAddTD("[EventChip] [%s][%s] ChangeRena AddMoney(%d)",
			lpObj->AccountID, lpObj->Name, lpObj->EventChipCount * 3000);
	}
	else
	{
		LogAddTD("[EventChip] [%s][%s] ChangeRena Fail",
			lpObj->AccountID, lpObj->Name);
	}

	Result.ChipCount = 0;
	lpObj->EventChipCount = 0;

	DataSend(aIndex, (LPBYTE)&Result, Result.h.size);

	lpObj->UseEventServer = FALSE;
}



LPOBJ pEventObj;

void EGRecvStoneInfo( PMSG_ANS_VIEW_STONES* aRecv)
{
	LPOBJ lpObj = &gObj[aRecv->iINDEX];

	PMSG_EVENTCHIPINFO Result;
	
	PHeadSetB((LPBYTE)&Result, 0x94, sizeof(Result));

	if ( aRecv->bSUCCESS )
		lpObj->iStoneCount = aRecv->iStoneCount;
	else
		lpObj->iStoneCount = 0;

	lpObj->MutoNumber = 0;
	Result.Type = 3;
	Result.ChipCount = aRecv->iStoneCount;

	DataSend(lpObj->m_Index, (LPBYTE)&Result, Result.h.size);

	char msg[128];
	wsprintf(msg, lMsg.Get(MSGGET(4, 153)), Result.ChipCount);
	ChatTargetSend(pEventObj, msg, lpObj->m_Index);
}



void EGRecvRegStone( PMSG_ANS_REGISTER_STONES* aRecv)
{
	PMSG_REGEVENTCHIP_RESULT Result;
	LPOBJ lpObj;
	int aIndex;
	

	PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
	lpObj = &gObj[aRecv->iINDEX];
	aIndex = aRecv->iINDEX;

	if ( aRecv->bSUCCESS != FALSE )
	{
		Result.ChipCount = aRecv->iStoneCount;
		gObjInventoryDeleteItem(aIndex, aRecv->iPosition);
		GCInventoryItemDeleteSend(aIndex, aRecv->iPosition, 1);

		LogAddTD("[Stone] [%s][%s] Delete Stones",
			lpObj->AccountID, lpObj->Name);
	}
	else
	{
		Result.ChipCount = -1;
		
		LogAddTD("[Stone] [%s][%s] RegEventServer Fail (Stones : %d)",
			lpObj->AccountID, lpObj->Name, aRecv->iStoneCount);
	}

	DataSend(aIndex, (LPBYTE)&Result, Result.h.size);

	lpObj->UseEventServer = FALSE;
}



void EGRecvDeleteStone( PMSG_ANS_DELETE_STONES* aRecv)
{
	return;
}



void EGRecvChangeStones( PMSG_ANS_RESET_EVENTCHIP* aRecv)
{
	PMSG_REGEVENTCHIP_RESULT Result;
	LPOBJ lpObj;
	int aIndex;
	

	PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
	lpObj = &gObj[aRecv->iINDEX];
	aIndex = aRecv->iINDEX;

	if ( aRecv->bSUCCESS != FALSE )
	{
		lpObj->Money += lpObj->iStoneCount * 3000;
		GCMoneySend(aIndex, lpObj->Money);

		LogAddTD("[Stones] [%s][%s] ChangeRena AddMoney(%d)",
			lpObj->AccountID, lpObj->Name, lpObj->iStoneCount*3000);
	}
	else
	{
		LogAddTD("[Stones] [%s][%s] ChangeRena Fail",
			lpObj->AccountID, lpObj->Name);
	}

	Result.ChipCount = 0;
	lpObj->iStoneCount = 0;

	DataSend(aIndex, (LPBYTE)&Result, Result.h.size);

	lpObj->UseEventServer = FALSE;
}


struct PMSG_ANS_2ANV_LOTTO_EVENT
{
	PBMSG_HEAD h;	// C1:9D
	BYTE btIsRegistered;	// 3
	char szGIFT_NAME[64];	// 4
};



#define GIFT_2ANV_MAP	235
#define MAX_GIFT_2ANV	50
#define GIFT_2ANV_RANGE(x)  ( (((x))<0)?FALSE:(((x))>MAX_GIFT_2ANV-1)?FALSE:TRUE )


BOOL g_bRingEventItemTextLoad = FALSE;
char g_sz2ANV_GIFT_NAME[MAX_GIFT_2ANV][64];

void EGRecv2AnvRegSerial( PMSG_ANS_2ANIV_SERIAL* aRecv)
{
	PMSG_ANS_2ANV_LOTTO_EVENT Result;

	PHeadSetB((LPBYTE)&Result, 0x9D, sizeof(Result));

	if ( !OBJMAX_RANGE(aRecv->iINDEX))
	{
		LogAddTD("[Mu_2Anv_Event] Error : Index is out of bound [%d]", aRecv->iINDEX);
		return;
	}

	if ( gObj[aRecv->iINDEX].Connected <= PLAYER_LOGGED )
	{
		LogAddTD("[Mu_2Anv_Event] Error : Index is out of bound [%d]", aRecv->iINDEX);
		return;
	}

	Result.szGIFT_NAME[0] = 0;

	if ( aRecv->btIsRegistered == FALSE )
	{
		Result.btIsRegistered = FALSE;
		
		if ( !GIFT_2ANV_RANGE(aRecv->iGiftNumber-1) )
		{
			LogAddTD("[Mu_2Anv_Event] Error : Gift Index is out of bound [%d]", aRecv->iGiftNumber);
			Result.btIsRegistered = 2;
		}

		if ( gObj[aRecv->iINDEX].Connected > PLAYER_LOGGED )
		{
			PMSG_SERVERCMD ServerCmd;

			PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
			ServerCmd.CmdType = 0;
			ServerCmd.X = gObj[aRecv->iINDEX].X;
			ServerCmd.Y = gObj[aRecv->iINDEX].Y;

			MsgSendV2(&gObj[aRecv->iINDEX], (LPBYTE)&ServerCmd, sizeof(ServerCmd));
			DataSend(aRecv->iINDEX, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
		}

		if ( g_bRingEventItemTextLoad )
		{
			strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), g_sz2ANV_GIFT_NAME[aRecv->iGiftNumber-1]);
		}

		switch ( aRecv->iGiftNumber )
		{
			case 1:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 13),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 13)].Name);
				}
				break;
			case 2:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 14),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 14)].Name);
				}
				break;
			case 3:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(12, 15),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(12, 15)].Name);
				}
				break;
			case 4:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 11),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 5:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 11),
					3, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), "Heart of Love");
				}
				break;
			case 6:
				gObj[aRecv->iINDEX].Money += 500000;
				GCMoneySend(aRecv->iINDEX, gObj[aRecv->iINDEX].Money);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), "500,000 Zen");
				}
				break;
			case 7:
				gObj[aRecv->iINDEX].Money += 50000;
				GCMoneySend(aRecv->iINDEX, gObj[aRecv->iINDEX].Money);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), "50,000 Zen");
				}
				break;
			case 8:	case 9:	case 10:
				gObj[aRecv->iINDEX].Money += 30000;
				GCMoneySend(aRecv->iINDEX, gObj[aRecv->iINDEX].Money);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), "30,000 Zen");
				}
				break;
			case 11:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(12, 19),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(12, 19)].Name);
				}
				break;
			case 12:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(12, 18),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(12, 18)].Name);
				}
				break;
			case 13:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(12, 17),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(12, 17)].Name);
				}
				break;
			case 14:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(12, 16),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(12, 16)].Name);
				}
				break;
			case 15:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(12, 14),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(12, 14)].Name);
				}
				break;
			case 16:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(12, 13),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(12, 13)].Name);
				}
				break;
			case 17:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(12, 12),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(12, 12)].Name);
				}
				break;
			case 18:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(13, 2),
					0, 255, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(13, 2)].Name);
				}
				break;
			case 19:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(13, 3),
					0, 255, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(13, 3)].Name);
				}
				break;
			case 20:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 16),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 16)].Name);
				}
				break;
			case 21:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(13, 0),
					0, 255, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(13, 0)].Name);
				}
				break;
			case 22:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(13, 1),
					0, 255, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(13, 1)].Name);
				}
				break;
			case 23:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 11),
					1, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 24:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 11),
					9, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 25:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 11),
					10, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 26:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 19),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 19)].Name);
				}
				break;
			case 27:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 19),
					1, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 19)].Name);
				}
				break;
			case 28:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 19),
					2, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 19)].Name);
				}
				break;
			case 29:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 19),
					3, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 19)].Name);
				}
				break;
			case 30:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 19),
					4, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 19)].Name);
				}
				break;
			case 31:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 11),
					2, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 32:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 20),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 20)].Name);
				}
				break;
			case 33:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 22),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 22)].Name);
				}
				break;
			case 34:	case 35:	case 36:	case 37:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(13, 15),
					aRecv->iGiftNumber - 34, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(13, 15)].Name);
				}
				break;
			case 38:	case 39:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 11),
					aRecv->iGiftNumber - 27, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 40:	case 41:	case 42:	case 43:	case 44:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 20),
					aRecv->iGiftNumber - 39, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 20)].Name);
				}
				break;
			case 45:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 11),
					8, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 46:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 41),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 41)].Name);
				}
				break;
			case 47:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 42),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 42)].Name);
				}
				break;
			case 48:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 44),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 44)].Name);
				}
				break;
			case 49:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 43),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 43)].Name);
				}
				break;
			case 50:
				ItemSerialCreateSend(gObj[aRecv->iINDEX].m_Index, GIFT_2ANV_MAP, 
					gObj[aRecv->iINDEX].X, gObj[aRecv->iINDEX].Y,
					ItemGetNumberMake(14, 31),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy_s(Result.szGIFT_NAME, sizeof(Result.szGIFT_NAME), ItemAttribute[ITEMGET(14, 31)].Name);
				}
				break;
			default:
				LogAddTD("[Mu_2Anv_Event] Error : iGiftNumber is Out of Boud [%d]", aRecv->iGiftNumber);
				break;
		}
	}
	else if ( aRecv->btIsRegistered == 1 ||
			  aRecv->btIsRegistered == 2 ||
			  aRecv->btIsRegistered == 3 ||
			  aRecv->btIsRegistered == 4 ||
			  aRecv->btIsRegistered == 5 )
	{
		Result.btIsRegistered = aRecv->btIsRegistered;
	}
	else
	{
		Result.btIsRegistered = 4;
		LogAddTD("[Mu_2Anv_Event] Error : Result Value is Wrong [%d]", aRecv->btIsRegistered);
	}

	LogAddTD("[Mu_2Anv_Event] Register Serial Result : %d [%s][%s]",
		aRecv->btIsRegistered, gObj[aRecv->iINDEX].AccountID, gObj[aRecv->iINDEX].Name);

	DataSend(aRecv->iINDEX, (LPBYTE)&Result, Result.h.size);

	gObj[aRecv->iINDEX].UseEventServer = FALSE;
}


static const char g_szRingEventOfflineGift[4][32] = { "100µ∑ π¬π›¡ˆ",
													  "10µ∑ π¬π›¡ˆ",
													  "5µ∑ π¬π›¡ˆ",
													  "2µ∑ π¬π›¡ˆ"};
															



void EGRecvRegRingGift( PMSG_ANS_REG_RINGGIFT* aRecv)
{
	gObj[aRecv->iINDEX].UseEventServer = FALSE;

	if ( gObjIsConnected(aRecv->iINDEX) == FALSE )
		return;

	if ( strcmp(aRecv->szUID, gObj[aRecv->iINDEX].AccountID))
		return;

	if ( aRecv->btIsRegistered == 1 )
	{
		if ( CHECK_LIMIT(aRecv->btGiftKind-1, 4) )
		{
			char szTemp[256];
			wsprintf(szTemp, "%s¥‘ ≤≤º≠ %sø° ¥Á√∑µ«ºÃΩ¿¥œ¥Ÿ.",
				gObj[aRecv->iINDEX].Name, g_szRingEventOfflineGift[aRecv->btGiftKind-1]);

			AllSendServerMsg(szTemp);

			LogAddTD("[Ring Event] [%s][%s] Register Succeeded Result:%d, Gift:%d",
				gObj[aRecv->iINDEX].AccountID, gObj[aRecv->iINDEX].Name,
				aRecv->btIsRegistered, aRecv->btGiftKind);
		}
		else
		{
			LogAddTD("[Ring Event] [%s][%s] Register Failed Result:%d, Gift:%d (out of bound, 1~4)",
				gObj[aRecv->iINDEX].AccountID, gObj[aRecv->iINDEX].Name,
				aRecv->btIsRegistered, aRecv->btGiftKind);
		}

		return;
	}

	LogAddTD("[Ring Event] [%s][%s] Register Failed Result : %d",
		gObj[aRecv->iINDEX].AccountID, gObj[aRecv->iINDEX].Name,
		aRecv->btIsRegistered);

	if ( gObjIsConnected(aRecv->iINDEX) == TRUE )
	{
		MapC[gObj[aRecv->iINDEX].MapNumber].MoneyItemDrop(100000, (BYTE)gObj[aRecv->iINDEX].X, (BYTE)gObj[aRecv->iINDEX].Y);
	}
}


void EGReqDevilSquareEnter(PMSG_REQ_MOVEDEVILSQUARE* lpMsg,int iIndex)
{
	if ( !gObjIsConnected(iIndex) )
		return;

	PMSG_REQ_BLOODCASTLE_ENTER pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x06;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.AccountID, gObj[iIndex].AccountID, 10);
	memcpy(pMsg.GameID, gObj[iIndex].Name, 10);
	pMsg.ServerCode = gGameServerCode / 20;
	pMsg.iObjIndex = iIndex;
	pMsg.iBloodCastle = lpMsg->SquareNumber;
	pMsg.iBloodCastleItemPos = lpMsg->InvitationItemPos;

	if ( !IsDevilSquareEventConnected )
	{
		wsRServerCli.Close();
		wsRServerCli.CreateSocket(ghWnd);

		if ( !GMRankingServerConnect(RankingServerIP, WM_GM_RANKING_CLIENT_MSG_PROC) )
		{
			IsDevilSquareEventConnected = 0;
//			LogAddTD("Can not connect Ranking Server");
			return;
		}

		IsDevilSquareEventConnected = TRUE;
	}

	if ( IsDevilSquareEventConnected )
	{
		wsRServerCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
	}
}

void EGAnsDevilSquareEnter(PMSG_ANS_BLOODCASTLE_ENTER* lpMsg)
{
	if ( !lpMsg)
		return;

	if ( !gObjIsConnected(lpMsg->iObjIndex))
		return;

	char szAccountID[11] = {0};
	char szName[11] = {0};
	memcpy(szAccountID, lpMsg->AccountID, 10);
	memcpy(szName, lpMsg->GameID, 10);

	if ( strcmp(gObj[lpMsg->iObjIndex].AccountID, szAccountID) ||
		 strcmp(gObj[lpMsg->iObjIndex].Name, szName) )
		 return;

	if( lpMsg->iResult == 1 )
	{
		PMSG_REQ_MOVEDEVILSQUARE pMsg;
		pMsg.SquareNumber = lpMsg->iBloodCastle;
		pMsg.InvitationItemPos = lpMsg->iBloodCastleItemPos;

		GCReqmoveDevilSquare(&pMsg,lpMsg->iObjIndex);
	}
	else
	{
		PMSG_RESULT_MOVEBLOODCASTLE pResult;
		PHeadSetB((LPBYTE)&pResult,0x9A,sizeof(pResult));
		pResult.Result = 6;

		DataSend(lpMsg->iObjIndex,(LPBYTE)&pResult,pResult.h.size);
	}
}

void EGDevilSquareEnter(int iIndex)
{
	if ( !gObjIsConnected(iIndex) )
		return;

	PMSG_REQ_BLOODCASTLE_ENTERCOUNT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x07;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.AccountID, gObj[iIndex].AccountID, 10);
	memcpy(pMsg.GameID, gObj[iIndex].Name, 10);
	pMsg.ServerCode = gGameServerCode / 20;
	pMsg.iObjIndex = iIndex;

	if ( !IsDevilSquareEventConnected )
	{
		wsRServerCli.Close();
		wsRServerCli.CreateSocket(ghWnd);

		if ( !GMRankingServerConnect(RankingServerIP, WM_GM_RANKING_CLIENT_MSG_PROC) )
		{
			IsDevilSquareEventConnected = 0;
//			LogAddTD("Can not connect Ranking Server");
			return;
		}

		IsDevilSquareEventConnected = TRUE;
	}

	if ( IsDevilSquareEventConnected )
	{
		wsRServerCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
	}
}

void EGReqBloodCastleEnter(PMSG_REQ_MOVEBLOODCASTLE* lpMsg,int iIndex)
{
	if ( !gObjIsConnected(iIndex) )
		return;

	PMSG_REQ_BLOODCASTLE_ENTER pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x08;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.AccountID, gObj[iIndex].AccountID, 10);
	memcpy(pMsg.GameID, gObj[iIndex].Name, 10);
	pMsg.ServerCode = gGameServerCode / 20;
	pMsg.iObjIndex = iIndex;
	pMsg.iBloodCastle = lpMsg->iBridgeNumber;
	pMsg.iBloodCastleItemPos = lpMsg->iItemPos;

	if ( !IsDevilSquareEventConnected )
	{
		wsRServerCli.Close();
		wsRServerCli.CreateSocket(ghWnd);

		if ( !GMRankingServerConnect(RankingServerIP, WM_GM_RANKING_CLIENT_MSG_PROC) )
		{
			IsDevilSquareEventConnected = 0;
//			LogAddTD("Can not connect Ranking Server");
			return;
		}

		IsDevilSquareEventConnected = TRUE;
	}

	if ( IsDevilSquareEventConnected )
	{
		wsRServerCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
	}
}

void EGAnsBloodCastleEnter(PMSG_ANS_BLOODCASTLE_ENTER* lpMsg)
{
	if ( !lpMsg)
		return;

	if ( !gObjIsConnected(lpMsg->iObjIndex))
		return;

	char szAccountID[11] = {0};
	char szName[11] = {0};
	memcpy(szAccountID, lpMsg->AccountID, 10);
	memcpy(szName, lpMsg->GameID, 10);

	if ( strcmp(gObj[lpMsg->iObjIndex].AccountID, szAccountID) ||
		 strcmp(gObj[lpMsg->iObjIndex].Name, szName) )
		 return;

	if( lpMsg->iResult == 1 )
	{
		PMSG_REQ_MOVEBLOODCASTLE pMsg;
		pMsg.iBridgeNumber = lpMsg->iBloodCastle;
		pMsg.iItemPos = lpMsg->iBloodCastleItemPos;

		CGRequestEnterBloodCastle(&pMsg,lpMsg->iObjIndex);
	}
	else
	{
		PMSG_RESULT_MOVEBLOODCASTLE pResult;
		PHeadSetB((LPBYTE)&pResult,0x9A,sizeof(pResult));
		pResult.Result = 6;

		DataSend(lpMsg->iObjIndex,(LPBYTE)&pResult,pResult.h.size);
	}
}

void EGBloodCastleEnter(int iIndex)
{
	if ( !gObjIsConnected(iIndex) )
		return;

	PMSG_REQ_BLOODCASTLE_ENTERCOUNT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x09;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.AccountID, gObj[iIndex].AccountID, 10);
	memcpy(pMsg.GameID, gObj[iIndex].Name, 10);
	pMsg.ServerCode = gGameServerCode / 20;
	pMsg.iObjIndex = iIndex;

	if ( !IsDevilSquareEventConnected )
	{
		wsRServerCli.Close();
		wsRServerCli.CreateSocket(ghWnd);

		if ( !GMRankingServerConnect(RankingServerIP, WM_GM_RANKING_CLIENT_MSG_PROC) )
		{
			IsDevilSquareEventConnected = 0;
//			LogAddTD("Can not connect Ranking Server");
			return;
		}

		IsDevilSquareEventConnected = TRUE;
	}

	if ( IsDevilSquareEventConnected )
	{
		wsRServerCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
	}
}

void EGReqBloodCastleEnterCount(int iIndex)
{
	if ( !gObjIsConnected(iIndex) )
		return;

	PMSG_REQ_BLOODCASTLE_ENTERCOUNT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x0B;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.AccountID, gObj[iIndex].AccountID, 10);
	memcpy(pMsg.GameID, gObj[iIndex].Name, 10);
	pMsg.ServerCode = gGameServerCode / 20;
	pMsg.iObjIndex = iIndex;

	if ( !IsDevilSquareEventConnected )
	{
		wsRServerCli.Close();
		wsRServerCli.CreateSocket(ghWnd);

		if ( !GMRankingServerConnect(RankingServerIP, WM_GM_RANKING_CLIENT_MSG_PROC) )
		{
			IsDevilSquareEventConnected = 0;
//			LogAddTD("Can not connect Ranking Server");
			return;
		}

		IsDevilSquareEventConnected = TRUE;
	}

	if ( IsDevilSquareEventConnected )
	{
		wsRServerCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
	}
}


struct PMSG_ANS_CL_ENTERCOUNT
{
	PBMSG_HEAD h;	// C1:9F
	BYTE btEventType;	// 3
	BYTE btLeftEnterCount;	// 4
};


void EGAnsBloodCastleEnterCount( PMSG_ANS_BLOODCASTLE_ENTERCOUNT* lpMsg)
{
	if ( !lpMsg)
		return;

	if ( !gObjIsConnected(lpMsg->iObjIndex))
		return;

	char szAccountID[11] = {0};
	char szName[11] = {0};
	memcpy(szAccountID, lpMsg->AccountID, 10);
	memcpy(szName, lpMsg->GameID, 10);

	if ( strcmp(gObj[lpMsg->iObjIndex].AccountID, szAccountID) ||
		 strcmp(gObj[lpMsg->iObjIndex].Name, szName) )
		 return;

	PMSG_ANS_CL_ENTERCOUNT pMsgSend;

	pMsgSend.h.c = 0xC1;
	pMsgSend.h.headcode = 0x9F;
	pMsgSend.h.size = sizeof(pMsgSend);
	pMsgSend.btEventType = 2;
	pMsgSend.btLeftEnterCount = lpMsg->iLeftCount;

	DataSend(lpMsg->iObjIndex, (LPBYTE)&pMsgSend, sizeof(pMsgSend));
}




struct PMSG_REQ_REG_CC_OFFLINE_GIFT
{
	PBMSG_HEAD h;	// C1:15
	int iINDEX;	// 4
	char szUID[11];	// 8
	WORD wServerCode;	// 14
	char szNAME[11];	// 16
};


void EGReqRegCCOfflineGift(int iIndex)
{
	if ( !gObjIsConnected(iIndex) )
		return;

	PMSG_REQ_REG_CC_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x15;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.szUID, gObj[iIndex].AccountID, 11);
	memcpy(pMsg.szNAME, gObj[iIndex].Name, 11);
	pMsg.wServerCode = gGameServerCode / 20;
	pMsg.iINDEX = iIndex;
	pMsg.szUID[10] = 0;
	pMsg.szNAME[10] = 0;

	DataSendEventChip((PCHAR)&pMsg, sizeof(pMsg));
}




void EGAnsRegCCOfflineGift( PMSG_ANS_REG_CC_OFFLINE_GIFT* lpMsg)
{
	if ( !lpMsg)
		return;

	if ( !lpMsg->iResultCode )
		return;

	char szAccountID[11] = {0};
	char szName[11] = {0};
	char szGIFT_NAME[50] = {0};
	memset(szAccountID, 0, sizeof(szAccountID));
	memset(szName, 0, sizeof(szName));
	memset(szGIFT_NAME, 0, sizeof(szGIFT_NAME));
	memcpy(szAccountID, lpMsg->szUID, sizeof(szAccountID));
	memcpy(szName, lpMsg->szNAME, sizeof(szName));
	memcpy(szGIFT_NAME, lpMsg->szGIFT_NAME, sizeof(szGIFT_NAME));
	szAccountID[10] = 0;
	szName[10] = 0;
	szGIFT_NAME[49] = 0;
	char szText[256] = {0};

	wsprintf(szText, lMsg.Get(MSGGET(6,74)), szName, szGIFT_NAME);
	AllSendServerMsg(szText);

	LogAddTD("[Chaos Castle] [%s][%s] Success to Register OffLine Gift (GIFT:%s)",
		szAccountID, szName, szGIFT_NAME);
}





struct PMSG_REQ_REG_DL_OFFLINE_GIFT
{
	PBMSG_HEAD h;	// C1:16
	int iINDEX;	// 4
	char szUID[11];	// 8
	WORD wServerCode;	// 14
	char szNAME[11];	// 16
};


void EGReqRegDLOfflineGift(int iIndex)
{
	if ( !gObjIsConnected(iIndex) )
		return;

	PMSG_REQ_REG_DL_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x16;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.szUID, gObj[iIndex].AccountID, 11);
	memcpy(pMsg.szNAME, gObj[iIndex].Name, 11);
	pMsg.wServerCode = gGameServerCode / 20;
	pMsg.iINDEX = iIndex;
	pMsg.szUID[10] = 0;
	pMsg.szNAME[10] = 0;

	DataSendEventChip((PCHAR)&pMsg, sizeof(pMsg));
}




void EGAnsRegDLOfflineGift( PMSG_ANS_REG_DL_OFFLINE_GIFT* lpMsg)
{
	if ( !lpMsg)
		return;

	if ( !lpMsg->iResultCode )
		return;

	char szAccountID[11] = {0};
	char szName[11] = {0};
	char szGIFT_NAME[50] = {0};
	memset(szAccountID, 0, sizeof(szAccountID));
	memset(szName, 0, sizeof(szName));
	memset(szGIFT_NAME, 0, sizeof(szGIFT_NAME));
	memcpy(szAccountID, lpMsg->szUID, sizeof(szAccountID));
	memcpy(szName, lpMsg->szNAME, sizeof(szName));
	memcpy(szGIFT_NAME, lpMsg->szGIFT_NAME, sizeof(szGIFT_NAME));
	szAccountID[10] = 0;
	szName[10] = 0;
	szGIFT_NAME[49] = 0;
	char szText[256] = {0};

	wsprintf(szText, "[¥Ÿ≈©∑ŒµÂ ±‚≥‰ ¿Ã∫•∆Æ] %s ¥‘≤≤º≠ %s ∞Ê«∞ø° ¥Á√∑µ«ºÃΩ¿¥œ¥Ÿ.", szName, szGIFT_NAME);
	AllSendServerMsg(szText);

	LogAddTD("[DarkLord Heart Event] [%s][%s] Success to Register OffLine Gift (GIFT:%s)",
		szAccountID, szName, szGIFT_NAME);
}



struct PMSG_REQ_REG_HT_OFFLINE_GIFT
{
	PBMSG_HEAD h;	// C1:17
	int iINDEX;	// 4
	char szUID[11];	// 8
	WORD wServerCode;	// 14
	char szNAME[11];	// 16
};

void EGReqRegHTOfflineGift(int iIndex)
{
	if ( !gObjIsConnected(iIndex) )
		return;

	PMSG_REQ_REG_HT_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x17;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.szUID, gObj[iIndex].AccountID, 11);
	memcpy(pMsg.szNAME, gObj[iIndex].Name, 11);
	pMsg.wServerCode = gGameServerCode / 20;
	pMsg.iINDEX = iIndex;
	pMsg.szUID[10] = 0;
	pMsg.szNAME[10] = 0;

	DataSendEventChip((PCHAR)&pMsg, sizeof(pMsg));
}




void EGAnsRegHTOfflineGift( PMSG_ANS_REG_HT_OFFLINE_GIFT* lpMsg)
{
	if ( !lpMsg)
		return;

	if ( !lpMsg->iResultCode )
	{
		if ( gObjIsConnected(lpMsg->iINDEX))
		{
			LPOBJ lpObj = &gObj[lpMsg->iINDEX];

			MapC[lpObj->MapNumber].MoneyItemDrop(1000000, lpObj->X, lpObj->Y);
		}
		return;
	}

	char szAccountID[11] = {0};
	char szName[11] = {0};
	char szGIFT_NAME[50] = {0};
	memset(szAccountID, 0, sizeof(szAccountID));
	memset(szName, 0, sizeof(szName));
	memset(szGIFT_NAME, 0, sizeof(szGIFT_NAME));
	memcpy(szAccountID, lpMsg->szUID, sizeof(szAccountID));
	memcpy(szName, lpMsg->szNAME, sizeof(szName));
	memcpy(szGIFT_NAME, lpMsg->szGIFT_NAME, sizeof(szGIFT_NAME));
	szAccountID[10] = 0;
	szName[10] = 0;
	szGIFT_NAME[49] = 0;
	char szText[256] = {0};

	wsprintf(szText, "[º˚∞‹¡¯ ∫∏π∞ªÛ¿⁄ ¿Ã∫•∆Æ] %s ¥‘≤≤º≠ %s ∞Ê«∞ø° ¥Á√∑µ«ºÃΩ¿¥œ¥Ÿ.", szName, szGIFT_NAME);
	AllSendServerMsg(szText);

	LogAddTD("[Hidden TreasureBox Event] [%s][%s] Success to Register OffLine Gift (GIFT:%s)",
		szAccountID, szName, szGIFT_NAME);
}
