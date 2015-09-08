#include "stdafx.h"
#include "CheatGuard.h"
#include "GameMain.h"
#include "ResetSystem.h"
#include "..\pugixml\pugixml.hpp"

using namespace pugi;
CheatGuard g_CheatGuard;

CheatGuard::CheatGuard()
{
	this->Log = new CLogToFile(LOG_NAME_CHEATGUARD, LOG_PATH_CHEATGUARD, TRUE);
}

CheatGuard::~CheatGuard()
{
	delete this->Log;
}

void CheatGuard::Init()
{
	ZeroMemory(&this->ModuleSpeedHack, sizeof(this->ModuleSpeedHack));
}

void CheatGuard::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_CHEATGUARD));
}

void CheatGuard::Read(LPSTR File)
{
	VM_START
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);

	if (Result.status != status_ok)
	{
		MsgBox("[CheatGuard] File %s not found!", File);
		return;
	}
	
	this->Log->Output("Loading %s...", File);

	xml_node CheatGuard = Document.child("cheatguard");
	xml_node Property;

	for( xml_node Node = CheatGuard.child("module"); Node; Node = Node.next_sibling() )
	{
		if (!strcmp(Node.attribute("name").as_string(), "SpeedHack"))
		{
			this->ModuleSpeedHack.Enabled = true;
			this->Log->Output("Module %s is activated",
				Node.attribute("name").as_string());
			
			strcpy(this->ModuleSpeedHack.Notice, Node.child("notice").text().as_string());
			this->ModuleSpeedHack.NoticeCount = Node.child("notice").attribute("count").as_int(0);
			
			for( Property = Node.child("property"); Property; Property = Property.next_sibling() )
			{
				if (!strcmp(Property.attribute("name").as_string(), "Disconnect"))
				{
					this->ModuleSpeedHack.Disconnect = Property.text().as_int(0);
				}

				if (!strcmp(Property.attribute("name").as_string(), "InformPlayersAround"))
				{
					this->ModuleSpeedHack.InformPlayersAround = Property.text().as_int(0);
				}

				if (!strcmp(Property.attribute("name").as_string(), "AttackSpeedMaxDelta"))
				{
					this->ModuleSpeedHack.AttackSpeedMaxDelta = Property.text().as_int(0);
				}

				if (!strcmp(Property.attribute("name").as_string(), "MagicSpeedMaxDelta"))
				{
					this->ModuleSpeedHack.MagicSpeedMaxDelta = Property.text().as_int(0);
				}
			}			
		}
	}
	VM_END
}

void CheatGuard::ProcCheckLive(LPOBJ lpUser)
{
	VM_START
	lpUser->m_iCHECK_COUNT++;

	if (lpUser->m_iCHECK_COUNT >= 60) {
		this->Log->Output("[%s] [%s] Low response interval", lpUser->AccountID, lpUser->Name);
#if (ENABLE_LIVEPROC_CHECK == 0)
		lpUser->m_iCHECK_COUNT = 0;
#else
		CloseClient(lpUser->PerSocketContext, 0);
#endif
	}
	VM_END
}

bool CheatGuard::ProcSpeedHack(LPOBJ lpUser, WORD AttackSpeed, WORD MagicSpeed, bool IsSpeedEditor)
{
	VM_START

	if (lpUser->Connected != PLAYER_PLAYING)
	{
		return true;
	}

	int Delta = 0;

	if (IsSpeedEditor)
	{
		lpUser->m_AttackSpeedHackDetectedCount++;
		this->Log->Output("[%s] [%s] Speed editor has been founded",
			lpUser->AccountID, lpUser->Name);
		this->GC_Notice(lpUser, "zCheatGuard", this->ModuleSpeedHack.Notice);
		if (this->ModuleSpeedHack.InformPlayersAround)
		{
			this->GC_InformPlayersAround(lpUser);
		}
	}
	if (AttackSpeed > lpUser->m_AttackSpeed)
	{
		Delta = AttackSpeed - lpUser->m_AttackSpeed;

		if (Delta > this->ModuleSpeedHack.AttackSpeedMaxDelta)
		{
			lpUser->m_AttackSpeedHackDetectedCount++;
			this->Log->Output("[%s] [%s] Attack speed is edited (Client: %d, Server: %d)",
				lpUser->AccountID, lpUser->Name, AttackSpeed, lpUser->m_AttackSpeed);
			this->GC_Notice(lpUser, "zCheatGuard", this->ModuleSpeedHack.Notice);
			if (this->ModuleSpeedHack.InformPlayersAround)
			{
				this->GC_InformPlayersAround(lpUser);
			}
		}
	}
	else if (MagicSpeed > lpUser->m_MagicSpeed)
	{
		Delta = MagicSpeed - lpUser->m_MagicSpeed;

		if (Delta > this->ModuleSpeedHack.MagicSpeedMaxDelta)
		{
			lpUser->m_AttackSpeedHackDetectedCount++;
			this->Log->Output("[%s] [%s] Magic speed is edited (Client: %d, Server: %d)",
				lpUser->AccountID, lpUser->Name, MagicSpeed, lpUser->m_MagicSpeed);
			this->GC_Notice(lpUser, "zCheatGuard", this->ModuleSpeedHack.Notice);
			if (this->ModuleSpeedHack.InformPlayersAround)
			{
				this->GC_InformPlayersAround(lpUser);
			}
		}
	}

	if (lpUser->m_AttackSpeedHackDetectedCount > ModuleSpeedHack.NoticeCount)
	{
		if (this->ModuleSpeedHack.Disconnect)
		{
			return false;
		}
	}

	return true;

	VM_END
}

bool CheatGuard::CG_LiveProc(LPOBJ lpUser, PacketCheatGuardCGClientInfo* lpData, int HeadCode, int Size)
{
	if (HeadCode != 0x10)
	{
		this->Log->Output("[%s] [%s] Client information headcode is not correct",
			lpUser->AccountID, lpUser->Name);
		return false;
	}

	if (Size != sizeof(PacketCheatGuardCGClientInfo))
	{
		this->Log->Output("[%s] [%s] Client information size is not correct",
			lpUser->AccountID, lpUser->Name);
		return false;
	}

	WORD tmpCheck[2];

	tmpCheck[0] = (lpData->CheckCode==1) ? (lpData->wWord1^0x8AFC) : (lpData->wWord2^0x8AFC);
	tmpCheck[1] = (lpData->CheckCode==1) ? ( (((lpData->wWord2^0xA8CF)/1024)*16) | ((lpData->wWord2^0xA8CF)&0x0F)) : ( (((lpData->wWord1^0xA8CF)/1024)*16) | ((lpData->wWord1^0xA8CF)&0x0F));

	if (tmpCheck[0] != tmpCheck[1] || tmpCheck[0] == 0 || tmpCheck[1] == 0)
	{
		this->Log->Output("[%s] [%s] Client information checkcode is not correct",
			lpUser->AccountID, lpUser->Name);
		return false;
	}

	if (lpUser->Connected > PLAYER_CONNECTED)
	{
		lpUser->ConnectCheckTime = GetTickCount();
		
		WORD AttackSpeed = lpData->wAttackSpeed1 ^ 0xDEFA;
		WORD MagicSpeed = lpData->wAttackSpeed2 ^ 0xAFDA;

		if (!this->ProcSpeedHack(lpUser, AttackSpeed, MagicSpeed, lpData->IsSpeedEditor))
		{
			return false;
		}
	}

	lpUser->m_iCHECK_COUNT = 0;
	return true;
}

void CheatGuard::GC_Notice(LPOBJ lpUser, LPSTR Title, LPSTR Text, ...)
{
	VM_START
	if (strlen(Text) < 1)
	{
		return;
	}

	char szBuffer[512] = "";

	va_list pArguments;
	va_start(pArguments, Text);
	vsprintf(szBuffer, Text, pArguments);
	va_end(pArguments);

	PacketResetMessageBox lpRequest = { 0 };
	lpRequest.Head.set((LPBYTE)&lpRequest, 0xFE, 0x05, sizeof(PacketResetMessageBox));
	memcpy(lpRequest.Title, Title, sizeof(lpRequest.Title)-1);
	memcpy(lpRequest.Text, szBuffer, sizeof(lpRequest.Text)-1);

	DataSend(lpUser->m_Index, (LPBYTE)&lpRequest, lpRequest.Head.size);
	VM_END
}

void CheatGuard::GC_InformPlayersAround(LPOBJ lpUser)
{
	VM_START
	for (int ViewIndex = 0; ViewIndex < MAX_VIEWPORT; ViewIndex++)
	{
		if (lpUser->VpPlayer2[ViewIndex].type == OBJ_USER)
		{
			char Text[256] = { 0 };
			sprintf(Text, "[zCheatGuard] Player %s may use cheats!", lpUser->Name);
			GCServerMsgStringSend(Text, lpUser->VpPlayer2[ViewIndex].number, 1);
		}
	}
	VM_END
}