#include "stdafx.h"
#include "main.h"

pFunctionVOID LoadGUIText = (pFunctionVOID)0x0077D055;

typedef void(*pDrawTexture)(int,float,float,float,float,float,float,float,float,int,int,float);
pDrawTexture DrawTexture = (pDrawTexture)0x00629129;

#if (__CUSTOM_OLYMP__==1)

TOlympNPCWindow gw_OlympNPC;

TOlympNPCWindow::TOlympNPCWindow()
{
	btShow = FALSE;
	btRewardBtn = FALSE;
	btJoinBtn = FALSE;
	btCloseBtn = FALSE;
	//btBoxShow = FALSE;
}

void TOlympNPCWindow::LoadTextures()
{
	LoadGUIText("Custom\\Interface\\Olymp\\Menu.tga",0xFF10,0x2601,2900,1,0);
	LoadGUIText("Custom\\Interface\\Olymp\\btnJoin.tga",0xFF11,0x2601,2900,1,0);
	LoadGUIText("Custom\\Interface\\Olymp\\btnReward.tga",0xFF12,0x2601,2900,1,0);
	LoadGUIText("Custom\\Interface\\Olymp\\btnClose.tga",0xFF13,0x2601,2900,1,0);
}

void TOlympNPCWindow::DeleteTextures()
{

}

TOlympNPCWindow::~TOlympNPCWindow()
{
	DeleteTextures();
}

void TOlympNPCWindow::DrawCloseBtn()
{
	if( *(DWORD*)(0x8B4ACFC) > 409 && *(DWORD*)(0x8B4ACFC) < 431 &&
		*(DWORD*)(0x8B4AD00) > 112 && *(DWORD*)(0x8B4AD00) < 134 )	{
		if( *(BYTE*)(0x8B4AC46) == 0 && this->btCloseBtn == 1 )	{
			SendClose();
			btCloseBtn = 0;
		}

		if( *(BYTE*)(0x8B4AC46) )	{
			DrawTexture(0xFF13,409.0f,112.0f,22.0f,22.0f,0,0,0.87f,0.435f,1,1,1.0f);
			btCloseBtn = 1;
		}
		else{
			DrawTexture(0xFF13,409.0f,112.0f,22.0f,22.0f,0,0.436f,0.87f,0.435f,1,1,1.0f);
		}
	}
	else	{
		btCloseBtn = 0;
		DrawTexture(0xFF13,409.0f,112.0f,22.0f,22.0f,0,0.436f,0.87f,0.435f,1,1,1.0f);
	}
}

void TOlympNPCWindow::DrawJoinBtn()
{
	if( !btOpen )	{
		DrawTexture(0xFF11,268.0f,140.0f,106.0f,26.0f,0.002f,0.387f,0.5121f,0.12675f,1,1,1.0f);
		return;
	}

	
	if( *(DWORD*)(0x8B4ACFC) > 268 && *(DWORD*)(0x8B4ACFC) < 374 &&
		*(DWORD*)(0x8B4AD00) > 140 && *(DWORD*)(0x8B4AD00) < 166 )	{

		if( *(BYTE*)(0x8B4AC46) == 0 && btJoinBtn == 1 )	{
			SendRequest();
			btJoinBtn = 0;
		}
		if( *(BYTE*)(0x8B4AC46) )	{
			DrawTexture(0xFF11,268.0f,140.0f,106.0f,26.0f,0.002f,0.258f,0.5121f,0.12675f,1,1,1.0f);
			btJoinBtn = 1;
		}
		else{
			DrawTexture(0xFF11,268.0f,140.0f,106.0f,26.0f,0.002f,0.129f,0.5121f,0.12675f,1,1,1.0f);
		}
	}
	else	{
		btJoinBtn = 0;
		DrawTexture(0xFF11,268.0f,140.0f,106.0f,26.0f,0.002f,0.001f,0.5121f,0.12675f,1,1,1.0f);
	}
}

void TOlympNPCWindow::DrawRewardBtn()
{
	if( *(DWORD*)(0x8B4ACFC) > 268 && *(DWORD*)(0x8B4ACFC) < 374 &&
		*(DWORD*)(0x8B4AD00) > 180 && *(DWORD*)(0x8B4AD00) < 206 )
	{
		if( *(BYTE*)(0x8B4AC46) == 0 && btRewardBtn == 1 )	{
			SendRewardRequest();
			btRewardBtn = 0;
		}

		if( *(BYTE*)(0x8B4AC46) )	{
			DrawTexture(0xFF12,268.0f,180.0f,106.0f,26.0f,0.002f,0.258f,0.5121f,0.12675f,1,1,1.0f);
			btRewardBtn = 1;
		}
		else	{
			DrawTexture(0xFF12,268.0f,180.0f,106.0f,26.0f,0.002f,0.129f,0.5121f,0.12675f,1,1,1.0f);
		}
	}
	else	{
		btRewardBtn = 0;
		DrawTexture(0xFF12,268.0f,180.0f,106.0f,26.0f,0.002f,0.001f,0.5121f,0.12675f,1,1,1.0f);
	}
}

void TOlympNPCWindow::Draw()
{
	if( !this->btShow )
		return;

	DrawTexture(0xFF10,207.0f,110.0f,226.0f,126.0f,0,0,0.551f,0.615f,1,1,1.0f);

	DrawJoinBtn();
	DrawRewardBtn();
	DrawCloseBtn();

	//char szTmp[128];
	//sprintf(szTmp,"[%d/%d] Btn: [%d]",*(DWORD*)(0x8B4ACFC),*(DWORD*)(0x8B4AD00),*(BYTE*)(0x8B4AC46));
	//CDCTabbedTextOut(CDC_InterfaceThis(), 256, 256, szTmp, 0, 0, (LPINT)1, 0);
}

void TOlympNPCWindow::NpcTalk(LPBYTE Info)
{
	TOlympNPCWindow::PSMG_OLP_NPCTALK* lpMsg = (TOlympNPCWindow::PSMG_OLP_NPCTALK*)Info;

	btRegistered = lpMsg->btRegistered;
	btOpen = lpMsg->btOpen;
	btShow = TRUE;

	*(DWORD*)(0x8B4AC46) = FALSE;
}

void TOlympNPCWindow::SendClose()
{
	VMBEGIN

	BYTE buff[4];
	TOlympNPCWindow::PMSG_CLOSEWINDOW pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xFE;
	pMsg.h.subcode = 0xFE;

	CStreamPacketEngine_Client ps;
	ps.AddData(&pMsg,sizeof(pMsg));
	ps.InsertPacket(buff);

	if( g_s != INVALID_SOCKET )
		g_MainClass.MySend(g_s,(const char*)&buff,sizeof(TOlympNPCWindow::PMSG_CLOSEWINDOW),0);

	VMEND

	btShow = FALSE;
}

void TOlympNPCWindow::SendRequest()
{
	VMBEGIN
	BYTE buff[4];
	TOlympNPCWindow::PMSG_REQUESTREG pMsg;
	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xFE;
	pMsg.h.subcode = 0x01;

	CStreamPacketEngine_Client ps;
	ps.AddData(&pMsg,sizeof(pMsg));
	ps.InsertPacket(buff);

	if( g_s != INVALID_SOCKET )
		g_MainClass.MySend(g_s,(const char*)&buff,sizeof(TOlympNPCWindow::PMSG_CLOSEWINDOW),0);

	VMEND
}

void TOlympNPCWindow::SendRewardRequest()
{
	VMBEGIN
	BYTE buff[4];
	TOlympNPCWindow::PMSG_REQUESTREG pMsg;
	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xFE;
	pMsg.h.subcode = 0x02;

	CStreamPacketEngine_Client ps;
	ps.AddData(&pMsg,sizeof(pMsg));
	ps.InsertPacket(buff);

	if( g_s != INVALID_SOCKET )
		g_MainClass.MySend(g_s,(const char*)&buff,sizeof(TOlympNPCWindow::PMSG_CLOSEWINDOW),0);

	VMEND
}

void TOlympNPCWindow::MessageHandle(LPBYTE aRecv)
{
	SendClose();
	char szMessage[128];

	PMSG_OLP_MESSAGEBOX* lpMsg = (PMSG_OLP_MESSAGEBOX*)aRecv;

	switch( lpMsg->btMessage )
	{
	case 1:
		sprintf(szMessage,"Match rooms are full!");
		break;
	case 2:
		sprintf(szMessage,"You have already registered!");
		break;
	case 3:
		sprintf(szMessage,"Failed to register, please try again later!");
		break;
	case 4:
		sprintf(szMessage,"Ongoing fights, please wait!");
		break;
	case 5:
		sprintf(szMessage,"You have used all your avaible battle's");
		break;
	case 6:
		sprintf(szMessage,"You have used all your avaible points");
		break;
	case 7:
		sprintf(szMessage,"You can register only once in 5 minute(s)");
		break;
	case 8:
		sprintf(szMessage,"Only reset %d and above can register!",lpMsg->aInfo1);
		break;
	case 10:
		sprintf(szMessage,"You have successfully registered!");
		break;
	case 11:
		sprintf(szMessage,"You have no entry in olympic ranks!");
		break;
	case 12:
		{
			sprintf(szMessage,"You have already been rewarded!");
			sprintf(szMessage,"You have been ranked (#%d) last week!",lpMsg->aInfo1);
		}
		break;
	case 13:
		{
			sprintf(szMessage,"You have been successfully rewarded!");
			sprintf(szMessage,"Your rank (#%d) - recieved (%d) pts!",lpMsg->aInfo1,lpMsg->aInfo2);
		}
		break;
	case 14:
		sprintf(szMessage,"A Hero cannot register to olymp!");
		break;
	default:
		return;
	}

	CDC_MessageBox(CDC_GetMessageBox(CDC_GetMessageThis()),0,0,szMessage,"Olymp",0,0,0);
}

#endif