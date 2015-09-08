#include "stdafx.h"
#include "main.h"
#include "ExPoint.h"
#include "Interface.h"
#include "ItemPrice.h"
#include "User.h"
#include "ResetSystem.h"
#include "Other.h"

pFunctionDWORD MainProtocolCore = (pFunctionDWORD)0x0A273961;
pFunctionDWORD MainGetItemValue = (pFunctionDWORD)0x0058DD68;

BOOL ProtocolCore(BYTE HeadCode,LPBYTE pData,int iSize,int bCheck)
{
	switch(HeadCode)
	{
	case 2:
		if( customPost((PMSG_GLOBALPOST_WHISPER*)pData) )
			return true;
		break;
#if (__FIX_HP__==1)
	case 0x26:
		{
			switch( ((PMSG_REFILL*)(pData))->IPos )
			{
			case 0xFF:
				gObjUserSetHP(((PMSG_REFILL*)(pData))->HP);
				gObjUserSetSD(((PMSG_REFILL*)(pData))->SD);
				break;
			default:
				gObjUserSetMAXHP(((PMSG_REFILL*)(pData))->HP);
				gObjUserSetMAXSD(((PMSG_REFILL*)(pData))->SD);
				break;
			}
		}
		break;
	case 0x27:
		{
			switch( ((PMSG_MANASEND*)(pData))->IPos )
			{
			case 0xFF:
				gObjUserSetMP(((PMSG_MANASEND*)(pData))->MP);
				break;
			default:
				gObjUserSetMAXMP(((PMSG_MANASEND*)(pData))->MP);
				break;
			}
		}
		break;
#endif
	case 0x2D:
		if( !RecvBuffInfo((PMSG_SENDEFFECT*)pData) )
			return false;
		break;
	case 0xFD:
		{
			int SubCode;
			// -----
			if( pData[0] == 0xC1 )
			{
				SubCode = pData[3];
			}
			else if(pData[0] == 0xC2 )
			{
				SubCode = pData[4];
			}
			// ----
			switch(SubCode)
			{
			case 0x00:
				{
					g_ItemPrice.SetPrice(pData);
				}
				break;
				// --
			case 0x01:
				{
					g_User.GetBattleInfo((USER_GET_BATTLEINFO*)pData);
				}
				break;
			}
		}
		break;
	case 0xFE:
		{
			PBMSG_HEAD2* pMsg = (PBMSG_HEAD2*)pData;

			switch(pMsg->subcode)
			{
			case 0xFE:
				SetStats(pData,(LPBYTE)(*(DWORD*)(PLAYER_CONST1)));
				break;
#if (__CUSTOM_OLYMP__==1)
			case 0x00:
				gw_OlympNPC.NpcTalk(pData);
				break;
			case 0x01:
				gw_OlympNPC.MessageHandle(pData);
				break;
#endif
#ifdef __NOVUS__
			case 0x03:
				g_ExPoint.Update((EXPOINT_UPDATE_ANS*)pData);
				break;
#endif
			case 0x04:
				{
					g_ResetSystem.GC_RequestDialog((PacketResetDialog*)pData);
				}
				break;
			case 0x05:
				{
					g_ResetSystem.GC_RequestMessageBox((PacketResetMessageBox*)pData);
				}
				break;
			case 0x06:
				{
					g_ResetSystem.GC_AnswerStart((PacketAnswerStart*)pData);
				}
				break;
			}

		}
		return true;
	case 0xF3:
		{
			int SubCode;
			if( pData[0] == 0xC1 )
				SubCode = pData[3];
			else if(pData[0] == 0xC2 )
				SubCode = pData[4];

			switch( SubCode )
			{
#if (__FIX_HP__==1)
			case 3:
				{
					gObjUserSetHP(((PMSG_CHARMAPJOINRESULT*)(pData))->HP);
					gObjUserSetMAXHP(((PMSG_CHARMAPJOINRESULT*)(pData))->MAXHP);
					gObjUserSetMP(((PMSG_CHARMAPJOINRESULT*)(pData))->MP);
					gObjUserSetMAXMP(((PMSG_CHARMAPJOINRESULT*)(pData))->MAXMP);
					gObjUserSetSD(((PMSG_CHARMAPJOINRESULT*)(pData))->SD);
					gObjUserSetMAXSD(((PMSG_CHARMAPJOINRESULT*)(pData))->MAXSD);
				}
				break;
			case 4:
				{
					gObjUserSetHP(((PMSG_CHARREGEN*)(pData))->iLife);
					gObjUserSetMP(((PMSG_CHARREGEN*)(pData))->iMana);
					gObjUserSetSD(((PMSG_CHARREGEN*)(pData))->iShield);
				}
				break;
			case 5:
				gObjUserSetMAXHP(((PMSG_LEVELUP*)(pData))->MAXHP);
				gObjUserSetMAXMP(((PMSG_LEVELUP*)(pData))->MAXMP);
				gObjUserSetMAXSD(((PMSG_LEVELUP*)(pData))->MAXSD);
				break;
#endif
			case 6:
				if( PointAdd(pData,(LPBYTE)(*(DWORD*)(PLAYER_CONST1))) )
					return true;
				break;
#if (__FIX_HP__==1)
			case 0x50:
				gObjUserSetMAXHP(((PMSG_MASTERINFORESULT*)(pData))->MAXHP);
				gObjUserSetMAXMP(((PMSG_MASTERINFORESULT*)(pData))->MAXHP);
				gObjUserSetMAXSD(((PMSG_MASTERINFORESULT*)(pData))->MAXSD);
				break;
			case 0x51:
				gObjUserSetMAXHP(((PMSG_MASTER_LEVELUP*)(pData))->MAXHP);
				gObjUserSetMAXMP(((PMSG_MASTER_LEVELUP*)(pData))->MAXMP);
				gObjUserSetMAXSD(((PMSG_MASTER_LEVELUP*)(pData))->MAXSD);
				break;
#endif
			}
		}
		break;
	}

	return MainProtocolCore(HeadCode,pData,iSize,bCheck);
}

BOOL RecvBuffInfo(PMSG_SENDEFFECT* lpMsg)
{
	if( lpMsg == NULL )
		return false;

	if( lpMsg->iEffect == BUFF_BLESS ||
		lpMsg->iEffect == BUFF_BLESS2 )
	{
		(lpMsg->byEffectOption == 1) ? TMainClass::g_iUSER_ADD_STATS = 0 : TMainClass::g_iUSER_ADD_STATS = lpMsg->iEffectValue;
	}
return true;
}

BOOL PointAdd(LPBYTE aRecv,LPBYTE pPlayer)
{
	PMSG_LVPOINTADDRESULT* lpMsg = (PMSG_LVPOINTADDRESULT*)aRecv;

	if( lpMsg->ResultType < 0x15 )
	{
#if( __FIX_HP__ == 1)
		if( lpMsg->ResultType == 2 )
			gObjUserSetMAXHP(lpMsg->MAXHPANDMANA);
		else if( lpMsg->ResultType == 3 )
			gObjUserSetMAXMP(lpMsg->MAXHPANDMANA);

		gObjUserSetMAXSD(lpMsg->MAXSD);
#endif
		return false;
	}

#if (__FIX_HP__ == 1)
	gObjUserSetMAXSD(lpMsg->MAXSD);
#endif

	int type = lpMsg->ResultType -= 0x15;

	switch( type )
	{
	case 0:
		*(WORD*)(pPlayer+0x18) = lpMsg->NewValue;
		break;
	case 1:
		*(WORD*)(pPlayer+0x1A) = lpMsg->NewValue;
		break;
	case 2:
		*(WORD*)(pPlayer+0x1C) = lpMsg->NewValue;
		*(WORD*)(pPlayer+0x26) = lpMsg->MaxLifeAndMana;
#if (__FIX_HP__ == 1)
		gObjUserSetMAXHP(lpMsg->MAXHPANDMANA);
#endif
		break;
	case 3:
		*(WORD*)(pPlayer+0x1E) = lpMsg->NewValue;
		*(WORD*)(pPlayer+0x28) = lpMsg->MaxLifeAndMana;
#if (__FIX_HP__ == 1)
		gObjUserSetMAXMP(lpMsg->MAXHPANDMANA);
#endif
		break;
	case 4:
		*(WORD*)(pPlayer+0x20) = lpMsg->NewValue;
		break;
	}

	*(WORD*)(pPlayer+0x2C) = lpMsg->wMaxShield;
	*(WORD*)(pPlayer+0x42) = lpMsg->MaxBP;
	*(WORD*)(pPlayer+0x74) = lpMsg->LvlUpPt;

	MainInitUser((LPBYTE)(*(DWORD*)(PLAYER_CONST1)));
	return true;
}

void SetStats(LPBYTE aRecv,LPBYTE pPlayer)
{
	PMSG_SETSTATS* lpMsg = (PMSG_SETSTATS*)aRecv;

	*(WORD*)(pPlayer+0x18) = lpMsg->Str;
	*(WORD*)(pPlayer+0x1A) = lpMsg->Dex;
	*(WORD*)(pPlayer+0x1C) = lpMsg->Vit;
	*(WORD*)(pPlayer+0x1E) = lpMsg->Ene;
	*(WORD*)(pPlayer+0x20) = lpMsg->Lea;

	*(WORD*)(pPlayer+0x42) = lpMsg->MaxBP;

	//*(WORD*)(pPlayer+0x26) = lpMsg->MaxLife;
	//*(WORD*)(pPlayer+0x28) = lpMsg->MaxMana;

	//*(WORD*)(pPlayer+0x2C) = lpMsg->wMaxShield;
	

#if (__FIX_HP__ == 1)
	gObjUserSetMAXHP(lpMsg->MaxLife);
	gObjUserSetMAXMP(lpMsg->MaxMana);
	gObjUserSetMAXSD(lpMsg->wMaxShield);
#endif

	*(WORD*)(pPlayer+0x74) = lpMsg->LvlUpPt;


	MainInitUser((LPBYTE)(*(DWORD*)(PLAYER_CONST1)));
}

BOOL customPost(PMSG_GLOBALPOST_WHISPER* lpMsg)
{
	if( lpMsg == NULL )
		return false;

	if( lpMsg->chatmsg[0] != '!' )
		return false;

	int tmpClass = pMessageBoxThis();

	if( tmpClass == 0 )
		return false;

	char* pText = &lpMsg->chatmsg[1];

	char* tmpWrongSymbolCheck = strstr(pText, "<"); //
	if (tmpWrongSymbolCheck != NULL) {
		LogAdd("wrong symbol");
		return false;
	}

	char szName[11];
	szName[10] = '\0';
	memcpy(szName,lpMsg->id,10);

	char szMsg[512];

#if (COMMONZ_DOWNGRADE == 1)
	char g_postFormat[] = "<FONT color='#FFE22B8A'>[Global] %s: %s</FONT><BR>";
	sprintf(szMsg, g_postFormat, szName, pText);
#else 
	char g_postFormat[] = "<FONT color='#FF%02X%02X%02X'>[Global] %s: %s</FONT><BR>";
	sprintf(szMsg, g_postFormat, 
		g_Other.ChatGlobalColor[0],
		g_Other.ChatGlobalColor[1], 
		g_Other.ChatGlobalColor[2],
		szName, pText);
#endif

	DWORD dwNum = MultiByteToWideChar( CP_ACP, 0, szMsg, -1, NULL, 0 );

	if( !dwNum )
		return true;

	wchar_t *pwText;
	pwText = new wchar_t[ dwNum ];
	MultiByteToWideChar( CP_ACP, 0, szMsg, -1, pwText, dwNum );

	dwNum = WideCharToMultiByte( CP_UTF8, 0, pwText, -1, NULL, 0, NULL, NULL );
	char *psText;
	psText = new char[ dwNum ];
	WideCharToMultiByte( CP_UTF8, 0, pwText, -1, psText, dwNum, NULL, NULL );

	g_Interface.ScaleFormAction(*(DWORD*)(tmpClass+0x120), "SetChatLogText", "%s %d", psText, 2);
	return true;
}

#define GET_ITEMOPT_LEVEL(x)		((x >> 3) & 0xF)

DWORD GetItemValue(LPBYTE itemInfo, int type)
{
	DWORD Money				= 0;
	BYTE Divisor			= 3;
	// ----
	if( g_ItemPrice.GetPrice(*(DWORD*)(itemInfo + 4), GET_ITEMOPT_LEVEL(*(DWORD*)(itemInfo + 8)), Money, Divisor) )
	{
		if( Money != 0 )
		{
			if( type == 1 )
			{
				Money /= Divisor;
			}
			// ----
			return Money;
		}
	}
	return MainGetItemValue(itemInfo,type);
	/*if( type != 0 )
		return MainGetItemValue(itemInfo,type);

	BYTE itemPos = *(BYTE*)(itemInfo+0x52)*8 + *(BYTE*)(itemInfo+0x51);

	if( g_ShopPrice[itemPos] == (DWORD)-1 )
		return MainGetItemValue(itemInfo,type);
	else
		return g_ShopPrice[itemPos];*/
}

Protocol g_Protocol;

void Protocol::Load()
{
	this->m_ActiveSocket = INVALID_SOCKET;

	//SetOp((LPVOID)(WSOCK_SEND-1), this->DataSend_, NONE);
}

void Protocol::DataRecv(DWORD Case, LPBYTE Data, int Len, int aIndex)
{

}

int Protocol::DataSend_(SOCKET Socket, const char* Data, int Len, int Flags)
{
	LPBYTE lpBuff = (LPBYTE)Data;

	if( !(lpBuff[0] == 0xC1 && lpBuff[2] == 0xF4) )
	{
		if( *(DWORD*)(WINSOCK_CLASS_ADDR) == WINSOCK_CLASS_DEFAULT )
		{
			VMBEGIN
			if( Socket != INVALID_SOCKET 
				&& (g_Protocol.m_ActiveSocket == INVALID_SOCKET 
				|| g_Protocol.m_ActiveSocket != Socket) )
			{
				g_Protocol.m_ActiveSocket = Socket;
			}
			VMEND

			if( lpBuff[0] == 0xC1 || lpBuff[0] == 0xC3 )	
			{
				lpBuff[1] ^= 0x10;
				lpBuff[2] ^= 0x40;
			}
			else	
			{
				lpBuff[1] ^= 0x11;
				lpBuff[2] ^= 0x41;
				lpBuff[3] ^= 0x31;
			}
			lpBuff[0] ^= 0x20;
		}
	}

	if( g_Protocol.m_ActiveSocket != INVALID_SOCKET )
	{
		return send(g_Protocol.m_ActiveSocket, Data, Len, Flags);
	}

	return send(Socket, Data, Len, Flags);
}

bool Protocol::DataSend(LPBYTE Data, int Size)
{
	if( this->m_ActiveSocket == INVALID_SOCKET )
	{
		return false;
	}
	this->DataSend_(this->m_ActiveSocket, (PCHAR)Data, Size, 0);
	return true;
}