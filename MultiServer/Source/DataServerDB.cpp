#include "stdafx.h"
#include "DataServerDB.h"
#include "zMultiServer.h"

CQuery g_DataServerDB;

int GetItemCount(int svrgroup)
{
	char szQuery[256];
	sprintf(szQuery,"SELECT ItemCount FROM GameServerInfo WHERE Number = %d",svrgroup);

	return g_DataServerDB.ExecGetInt(szQuery);
}

int GetAllChar(char* szId, char * szChar,int& ExVault)
{
	char szQuery[256];
	sprintf(szQuery,"SELECT * FROM AccountCharacter WHERE Id = '%s'",szId);

	if( g_DataServerDB.Exec(szQuery) == FALSE)
	{
		g_DataServerDB.Clear();
		return FALSE;
	}

	if( g_DataServerDB.Fetch() == SQL_NO_DATA )
	{
		g_DataServerDB.Clear();
		sprintf(szQuery,"INSERT INTO AccountCharacter (Id) VALUES ('%s')",szId);
		return FALSE;
	}

	for(int i = 0; i < MAX_CHARS; i++)
	{
		g_DataServerDB.GetStr(i+3,&szChar[(MAX_IDSTRING+1)*i]);
	}

#ifdef ZTEAMDB
	ExVault = g_DataServerDB.GetInt("ExWarehouse");
#else
	ExVault = g_DataServerDB.GetInt("ExVault");
#endif

	g_DataServerDB.Clear();
return TRUE;
}

int GetCharList(char* szId,LPSDHP_CHARLISTCOUNT pCount,LPSDHP_CHARLIST pCL)
{
	char szQuery[256];
	char szChar[MAX_CHARS][MAX_IDSTRING+1] = {0};

	int bExVault = 0;

	if(GetAllChar(szId,&szChar[0][0],bExVault) == FALSE)	return 0;

	pCount->btExtendVault = bExVault;

	int count = 0;

	for(int i = 0; i < MAX_CHARS; i++)
	{
		if(szChar[i][0] == '\0')	continue;
		if(strlen(szChar[i]) < 4)	continue;

		sprintf(szQuery,"SELECT cLevel, Class, CtlCode, DbVersion FROM vCharacterPreview WHERE Name='%s'",szChar[i]);

		if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
		{
			LPSDHP_CHARLIST lpCL = &pCL[count];
			lpCL->Level = g_DataServerDB.GetInt("cLevel");
			lpCL->Class = g_DataServerDB.GetInt("Class");

			if(lpCL->Level >= 250)	pCount->Magumsa = 0x02;
			else if(lpCL->Level >= 220)	pCount->Magumsa = 0x01;

			lpCL->CtlCode = g_DataServerDB.GetInt("CtlCode");
			lpCL->DbVersion = g_DataServerDB.GetInt("DbVersion");
			g_DataServerDB.Clear();

			BYTE btInventory[INVENTORY_BINARY_SIZE];
			wsprintf(szQuery, "SELECT Inventory FROM Character WHERE Name='%s'", szChar[i]);
			g_DataServerDB.ReadBlob(szQuery,&btInventory);

			memset(lpCL->dbInventory, -1, sizeof(pCL->dbInventory));

			if(lpCL->DbVersion == 3)
			{
				for(int i=0;i<12;i++)
				{
					if ( btInventory[0+i*16] == 0xFF && (btInventory[7+i*16] & 0x80 ) == 0x80 && (btInventory[10+i*16] & 0xF0) == 0xF0 )
					{
						lpCL->dbInventory[i*4] = -1;
						lpCL->dbInventory[i*4+1] = -1;
						lpCL->dbInventory[i*4+2] = -1;
						lpCL->dbInventory[i*4+3] = -1;
					}
					else
					{
						lpCL->dbInventory[i*4]  = (btInventory[0+i*16]);	// 0..7 bits of Item
						lpCL->dbInventory[i*4+1]  = (btInventory[1+i*16]);	// Get Level of Item
						lpCL->dbInventory[i*4+2]  = (btInventory[7+i*16]);	// 8 bit     of Item
						lpCL->dbInventory[i*4+3]  = (btInventory[9+i*16]);	// 9..12 bit of Item
					}
				}
			}

			sprintf(szQuery,"SELECT G_Status FROM GuildMember WHERE Name='%s'",szChar[i]);

			int G_Status = g_DataServerDB.ExecGetInt(szQuery);

			if(G_Status == CQuery::CQUERYERROR || G_Status == CQuery::CQUERYEOF)
			{
				lpCL->btGuildStatus = 0xFF;
			}
			else
			{
				lpCL->btGuildStatus = G_Status;
			}

			if( g_CustomHardcoreMode )
			{
				g_DataServerDB.Clear();
				
				sprintf(szQuery, "SELECT LifeCount FROM HardcoreMode WHERE Character = '%s'", szChar[i]);
				short LifeCount = g_DataServerDB.ExecGetInt(szQuery);
			
				if(		LifeCount == CQuery::CQUERYERROR 
					||	LifeCount == CQuery::CQUERYEOF)
				{
					
				}
				else
				{
					if( LifeCount < 1 && lpCL->CtlCode != 32 )
					{
						lpCL->CtlCode = 1;
					}
				}
			}

			lpCL->Index = i;
			lstrcpy(lpCL->Name,szChar[i]);
			count++;
		}

		g_DataServerDB.Clear();
	}
return count;
}

int CreateCharacter(char* szId, char* szName,LPSDHP_CREATECHARRESULT lpResult)
{
	char szQuery[256];
	char szChar[MAX_CHARS][MAX_IDSTRING+1] = {0};

	int iIndex;
	int bExVault = 0;

	if(GetAllChar(szId,&szChar[0][0],bExVault) == FALSE)
	{
		iIndex = 0;
	}
	else
	{
		for(iIndex = 0; iIndex < MAX_CHARS; iIndex++)
		{
			if(szChar[iIndex][0] == 0)	break;
		}
	}

	if(iIndex < 0 || iIndex >= 5)	return 2;

	memcpy(lpResult->Name, szName, MAX_IDSTRING);
	lpResult->Pos = iIndex;

	sprintf(szQuery,"exec WZ_CreateCharacter '%s', '%s', '%d'",szId,szName,lpResult->ClassSkin);
	int nResult = g_DataServerDB.ExecGetInt(szQuery);

	if(nResult == 1)
	{
		sprintf(szQuery,"UPDATE AccountCharacter SET GameID%d='%s' WHERE Id='%s'",iIndex+1,szName,szId);
		g_DataServerDB.Exec(szQuery);
		g_DataServerDB.Clear();

		sprintf(szQuery,"SELECT cLevel FROM vCharacterPreview WHERE Name='%s'",szName);
		lpResult->Level = g_DataServerDB.ExecGetInt(szQuery);
		
		if( g_CustomHardcoreMode )
		{
			g_DataServerDB.Clear();
			sprintf(szQuery, "INSERT INTO HardcoreMode (Character, LifeCount) VALUES('%s', %d)", 
				szName, g_CustomHardcoreDefaultLife);
			g_DataServerDB.Exec(szQuery);
		}
	}

	return nResult;
}

int DeleteCharacter(char* szId,char* szName,char* szGuild,int G_Status)
{
	char szQuery[256];
	char szChar[MAX_CHARS][MAX_IDSTRING+1] = {0};

	sprintf(szQuery,"DELETE FROM Character WHERE Name = '%s' AND AccountID = '%s'",szName,szId);

	if(g_DataServerDB.Exec(szQuery) == FALSE)	{
		g_DataServerDB.Clear();
		return FALSE;
	}

	g_DataServerDB.Clear();

	int bExVault = 0;

	if(GetAllChar(szId,&szChar[0][0],bExVault) == TRUE)
	{
		for(int i = 0; i < MAX_CHARS; i++)
		{
			if(strcmp(szChar[i],szName) != NULL) continue;

			sprintf(szQuery,"UPDATE AccountCharacter SET GameID%d = NULL WHERE Id = '%s'",i+1,szId);
			g_DataServerDB.Exec(szQuery);
			break;
		}
	}

	g_DataServerDB.Clear();

	sprintf(szQuery,"DELETE FROM T_QuestEx_Info WHERE memb_char = '%s'",szName);
	g_DataServerDB.Exec(szQuery);
	g_DataServerDB.Clear();

	sprintf(szQuery,"DELETE FROM T_MasterLevelSystem WHERE CHAR_NAME = '%s'",szName);
	g_DataServerDB.Exec(szQuery);
	g_DataServerDB.Clear();

	int guid = -1;

	sprintf(szQuery,"SELECT GUID FROM T_FriendMain WHERE Name='%s'",szName);

	if( g_DataServerDB.Exec(szQuery) && g_DataServerDB.Fetch() != SQL_NO_DATA )
	{
		guid = g_DataServerDB.GetInt("GUID");
		g_DataServerDB.Clear();

		sprintf(szQuery,"DELETE FROM T_FriendList WHERE GUID = %d",guid);
		g_DataServerDB.Exec(szQuery);
		g_DataServerDB.Clear();

		sprintf(szQuery,"DELETE FROM T_FriendMail WHERE GUID = %d",guid);
		g_DataServerDB.Exec(szQuery);
		g_DataServerDB.Clear();

		sprintf(szQuery,"DELETE FROM T_FriendMain WHERE GUID = %d",guid);
		g_DataServerDB.Exec(szQuery);
	}
	g_DataServerDB.Clear();
	
	if( g_CustomHardcoreMode )
	{
		sprintf(szQuery, "DELETE FROM HardcoreMode WHERE Character = '%s'", 
			szName);
		g_DataServerDB.Exec(szQuery);
	}

	return TRUE;
}

int GetCharacterInfo(char* szAccountID,char* szName,LPSDHP_DBCHAR_INFORESULT lpResult)
{
	char szQuery[512];
	
	if(g_DSInfo.bResetCol == TRUE)
	{
#if (S6EP2==1 || EX700==1 )
		sprintf(szQuery,"SELECT cLevel, Class, LevelUpPoint, Experience, Strength, Dexterity, Vitality, Energy, Money, Life, MaxLife, Mana, MaxMana, MapNumber, MapPosX, MapPosY, MapDir, PkCount, PkLevel, PkTime, CtlCode, DbVersion, Leadership, ChatLimitTime, FruitPoint,ExInventory,%s FROM Character WHERE Name='%s' AND AccountID = '%s'",g_DSInfo.dbResetCol,szName,szAccountID);
#else
		sprintf(szQuery,"SELECT cLevel, Class, LevelUpPoint, Experience, Strength, Dexterity, Vitality, Energy, Money, Life, MaxLife, Mana, MaxMana, MapNumber, MapPosX, MapPosY, MapDir, PkCount, PkLevel, PkTime, CtlCode, DbVersion, Leadership, ChatLimitTime, FruitPoint, %s FROM Character WHERE Name='%s' AND AccountID = '%s'",g_DSInfo.dbResetCol,szName,szAccountID);
#endif
	}
	else
	{
#if (S6EP2==1 || EX700==1 )
		sprintf(szQuery,"SELECT cLevel, Class, LevelUpPoint, Experience, Strength, Dexterity, Vitality, Energy, Money, Life, MaxLife, Mana, MaxMana, MapNumber, MapPosX, MapPosY, MapDir, PkCount, PkLevel, PkTime, CtlCode, DbVersion, Leadership, ChatLimitTime, FruitPoint, ExInventory FROM Character WHERE Name='%s' AND AccountID = '%s'",szName,szAccountID);
#else
		sprintf(szQuery,"SELECT cLevel, Class, LevelUpPoint, Experience, Strength, Dexterity, Vitality, Energy, Money, Life, MaxLife, Mana, MaxMana, MapNumber, MapPosX, MapPosY, MapDir, PkCount, PkLevel, PkTime, CtlCode, DbVersion, Leadership, ChatLimitTime, FruitPoint, FROM Character WHERE Name='%s' AND AccountID = '%s'",szName,szAccountID);
#endif
	}

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		lpResult->Level = (short)g_DataServerDB.GetInt("cLevel"); 
		lpResult->Class = (BYTE)g_DataServerDB.GetInt("Class");
		lpResult->LevelUpPoint = g_DataServerDB.GetInt("LevelUpPoint");
		lpResult->Exp = g_DataServerDB.GetInt("Experience");
#if( FIX_MAXSTAT == 1 )
		lpResult->Str = (WORD)g_DataServerDB.GetInt("Strength"); 
		lpResult->Dex = (WORD)g_DataServerDB.GetInt("Dexterity");
		lpResult->Vit = (WORD)g_DataServerDB.GetInt("Vitality");
		lpResult->Energy = (WORD)g_DataServerDB.GetInt("Energy");
#else
		lpResult->Str = (short)g_DataServerDB.GetInt("Strength"); 
		lpResult->Dex = (short)g_DataServerDB.GetInt("Dexterity");
		lpResult->Vit = (short)g_DataServerDB.GetInt("Vitality");
		lpResult->Energy = (short)g_DataServerDB.GetInt("Energy");
#endif
		lpResult->Money = g_DataServerDB.GetInt("Money");
		lpResult->Life = (WORD)g_DataServerDB.GetFloat("Life");
		lpResult->MaxLife = (WORD)g_DataServerDB.GetFloat("MaxLife");
		lpResult->Mana = (WORD)g_DataServerDB.GetFloat("Mana");
		lpResult->MaxMana = (WORD)g_DataServerDB.GetFloat("MaxMana");
		lpResult->MapNumber= (BYTE)g_DataServerDB.GetInt("MapNumber");
		lpResult->MapX = (BYTE)g_DataServerDB.GetInt("MapPosX");
		lpResult->MapY = (BYTE)g_DataServerDB.GetInt("MapPosY");
		lpResult->Dir = (BYTE)g_DataServerDB.GetInt("MapDir");
		lpResult->PkCount = g_DataServerDB.GetInt("PkCount");
		lpResult->PkLevel = g_DataServerDB.GetInt("PkLevel");
		lpResult->PkTime = g_DataServerDB.GetInt("PkTime");
		lpResult->CtlCode = (BYTE)g_DataServerDB.GetInt("CtlCode");
		lpResult->DbVersion = (BYTE)g_DataServerDB.GetInt("DbVersion");
		lpResult->Leadership = (WORD)g_DataServerDB.GetInt("Leadership");
		lpResult->ChatLitmitTime = (WORD)g_DataServerDB.GetInt("ChatLimitTime");
		lpResult->iFruitPoint  = g_DataServerDB.GetInt("FruitPoint");
#if (S6EP2==1 || EX700==1)
		lpResult->btExInventory = g_DataServerDB.GetInt("ExInventory");
#endif

		if( g_DSInfo.bResetCol == TRUE )
		{
			lpResult->Reset = g_DataServerDB.GetInt(g_DSInfo.dbResetCol);
		}

		g_DataServerDB.Clear();

		BYTE btTemp[sizeof(lpResult->dbInventory)+1]={0};

		sprintf(szQuery,"SELECT Inventory FROM Character WHERE Name = '%s'",szName);
		g_DataServerDB.ReadBlob(szQuery,btTemp);
		memcpy(lpResult->dbInventory,btTemp,sizeof(lpResult->dbInventory));

		sprintf(szQuery,"SELECT MagicList FROM Character WHERE Name = '%s'",szName);
		g_DataServerDB.ReadBlob(szQuery,btTemp);
		memcpy(lpResult->dbMagicList,btTemp,sizeof(lpResult->dbMagicList));

		sprintf(szQuery,"SELECT Quest FROM Character WHERE Name = '%s'",szName);
		g_DataServerDB.ReadBlob(szQuery,btTemp);
		memcpy(lpResult->dbQuest,btTemp,sizeof(lpResult->dbQuest));


		if( g_CustomHardcoreMode )
		{
			g_DataServerDB.Clear();
			sprintf(szQuery, "SELECT LifeCount FROM HardcoreMode WHERE Character = '%s'",
				szName);
			lpResult->HardcoreLife = (short)g_DataServerDB.ExecGetInt(szQuery);
		}
		
		/*g_DataServerDB.Clear();

		sprintf(szQuery, "select %s from %s where %s = '%s'",
			g_DSInfo.CreditColumnCredits, g_DSInfo.CreditTable, 
			g_DSInfo.CreditColumnAccount, szAccountID);

		if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
		{
			lpResult->Credits = g_DataServerDB.GetInt(g_DSInfo.CreditColumnCredits); 
		}*/
		
		return TRUE;
	}

	lpResult->result = FALSE;
	g_DataServerDB.Clear();
	return FALSE;
}

void GetOptionData(char* szName,LPSDHP_SKILLKEYDATA_SEND lpResult)
{
	char szQuery[256];
	sprintf(szQuery,"SELECT * FROM OptionData WHERE Name='%s'",szName);

	if(g_DataServerDB.Exec(szQuery) == TRUE )
	{
		if( g_DataServerDB.Fetch() != SQL_NO_DATA )
		{
			lpResult->GameOption = (BYTE)g_DataServerDB.GetInt("GameOption");
			lpResult->QkeyDefine = (BYTE)g_DataServerDB.GetInt("Qkey");
			lpResult->WkeyDefine = (BYTE)g_DataServerDB.GetInt("Wkey");
			lpResult->EkeyDefine = (BYTE)g_DataServerDB.GetInt("Ekey");
			lpResult->ChatWindow = (BYTE)g_DataServerDB.GetInt("ChatWindow");
			lpResult->RkeyDefine = (BYTE)g_DataServerDB.GetInt("Rkey");
			lpResult->QWERLevel = g_DataServerDB.GetInt("QWERLevel");
			g_DataServerDB.Clear();

			BYTE btTemp[sizeof(lpResult->SkillKeyBuffer)]={0};
			sprintf(szQuery, "SELECT SkillKey from OptionData where Name='%s'", szName);
			g_DataServerDB.ReadBlob(szQuery, btTemp);
			memcpy(lpResult->SkillKeyBuffer, btTemp, sizeof(lpResult->SkillKeyBuffer));
		}
		else
		{
			lpResult->GameOption = 5;
		}
	}

	g_DataServerDB.Clear();
}

void SetCharacterInfo(char* szName,LPSDHP_DBCHAR_INFOSAVE lpMsg)
{
	char szQuery[512];

	if( g_DSInfo.bResetCol == TRUE )
	{
		sprintf(szQuery,"UPDATE Character SET cLevel=%d,Class=%d,LevelUpPoint=%d, Experience=%u, Strength=%d, Dexterity=%d, Vitality=%d, Energy=%d, Money=%d, Life=%f, MaxLife=%f, Mana=%f, MaxMana=%f, MapNumber=%d, MapPosX=%d, MapPosY=%d, MapDir=%d, PkCount=%d, PkLevel=%d, PkTime=%d, DbVersion=3, Leadership=%d, ChatLimitTime=%d, FruitPoint=%d,%s=%d WHERE Name = '%s'",
			lpMsg->Level, lpMsg->Class, lpMsg->LevelUpPoint, lpMsg->Exp, lpMsg->Str, lpMsg->Dex, lpMsg->Vit, lpMsg->Energy, lpMsg->Money, float(lpMsg->Life), float(lpMsg->MaxLife), float(lpMsg->Mana), float(lpMsg->MaxMana), lpMsg->MapNumber, lpMsg->MapX, lpMsg->MapY, lpMsg->Dir, lpMsg->PkCount, lpMsg->PkLevel, lpMsg->PkTime, lpMsg->Leadership, lpMsg->ChatLitmitTime, lpMsg->iFruitPoint,g_DSInfo.dbResetCol,lpMsg->iReset,szName);
	}
	else{
		sprintf(szQuery,"UPDATE Character SET cLevel=%d,Class=%d,LevelUpPoint=%d, Experience=%u, Strength=%d, Dexterity=%d, Vitality=%d, Energy=%d, Money=%d, Life=%f, MaxLife=%f, Mana=%f, MaxMana=%f, MapNumber=%d, MapPosX=%d, MapPosY=%d, MapDir=%d, PkCount=%d, PkLevel=%d, PkTime=%d, DbVersion=3, Leadership=%d, ChatLimitTime=%d, FruitPoint=%d WHERE Name = '%s'",
			lpMsg->Level, lpMsg->Class, lpMsg->LevelUpPoint, lpMsg->Exp, lpMsg->Str, lpMsg->Dex, lpMsg->Vit, lpMsg->Energy, lpMsg->Money, float(lpMsg->Life), float(lpMsg->MaxLife), float(lpMsg->Mana), float(lpMsg->MaxMana), lpMsg->MapNumber, lpMsg->MapX, lpMsg->MapY, lpMsg->Dir, lpMsg->PkCount, lpMsg->PkLevel, lpMsg->PkTime, lpMsg->Leadership, lpMsg->ChatLitmitTime, lpMsg->iFruitPoint,szName);
	}



	g_DataServerDB.Exec(szQuery);
	g_DataServerDB.Clear();

	sprintf(szQuery,"UPDATE Character SET Inventory=? WHERE Name='%s'",szName);
	g_DataServerDB.WriteBlob(szQuery,lpMsg->dbInventory,sizeof(lpMsg->dbInventory));

	sprintf(szQuery,"UPDATE Character SET MagicList=? WHERE Name='%s'",szName);
	g_DataServerDB.WriteBlob(szQuery,lpMsg->dbMagicList,sizeof(lpMsg->dbMagicList));

	sprintf(szQuery,"UPDATE Character SET Quest=? WHERE Name='%s'",szName);
	g_DataServerDB.WriteBlob(szQuery,lpMsg->dbQuest,sizeof(lpMsg->dbQuest));

	if(lpMsg->CharInfoSave == TRUE)
	{
		sprintf(szQuery,"INSERT INTO T_CurCharName (Name) VALUES('%s')",szName);
		g_DataServerDB.Exec(szQuery);
		g_DataServerDB.Clear();
	}

	if( g_CustomHardcoreMode )
	{
		sprintf(szQuery,"UPDATE HardcoreMode SET LifeCount = %d WHERE Character='%s'",
			lpMsg->HardcoreLife, szName);
		g_DataServerDB.Exec(szQuery);
		g_DataServerDB.Clear();
	}	

	/*sprintf(szQuery,"update %s set %s = %d where %s = (select AccountID from Character where Name = '%s')",
		g_DSInfo.CreditTable, g_DSInfo.CreditColumnCredits, lpMsg->Credits,
		g_DSInfo.CreditColumnAccount, szName);
	g_DataServerDB.Exec(szQuery);
	g_DataServerDB.Clear();*/
}

void CreateWarehouseData(char* szId)
{
char szQuery[256];
	sprintf(szQuery,"INSERT INTO warehouse (AccountID, Money, EndUseDate, DbVersion) VALUES ('%s',0, getdate(), 3)",szId);

	g_DataServerDB.Exec(szQuery);
	g_DataServerDB.Clear();
}

int GetWarehouseData(char* szId,LPSDHP_GETWAREHOUSEDB_SAVE lpMsg)
{
char szQuery[256];
	sprintf(szQuery,"SELECT Money, DbVersion, pw FROM warehouse WHERE AccountID='%s'",szId);

	if(g_DataServerDB.Exec(szQuery) == FALSE || g_DataServerDB.Fetch() == SQL_NO_DATA)
	{
		g_DataServerDB.Clear();
		return FALSE;
	}

	lpMsg->Money = g_DataServerDB.GetInt("Money");
	lpMsg->DbVersion = g_DataServerDB.GetInt("DbVersion");
	lpMsg->pw = g_DataServerDB.GetInt("pw");
	g_DataServerDB.Clear();

	sprintf(szQuery,"SELECT Items FROM warehouse WHERE AccountID='%s'",szId);
	if( g_DataServerDB.ReadBlob(szQuery,lpMsg->dbItems) <= 0)
	{
		memset(lpMsg->dbItems,-1,sizeof(lpMsg->dbItems));
	}

	return TRUE;
}

void SetWarehouseData(char* szId,LPSDHP_GETWAREHOUSEDB_SAVE lpMsg)
{
char szQuery[256];
	sprintf(szQuery,"UPDATE warehouse SET Money=%d, pw=%d, DbVersion=3 WHERE AccountID='%s'",lpMsg->Money,lpMsg->pw,szId);

	g_DataServerDB.Exec(szQuery);
	g_DataServerDB.Clear();

	sprintf(szQuery,"UPDATE warehouse SET Items=? WHERE AccountID='%s'",szId);
	g_DataServerDB.WriteBlob(szQuery,lpMsg->dbItems,sizeof(lpMsg->dbItems));
}

void SetInventoryData(char* szName,LPSDHP_DBCHAR_ITEMSAVE lpMsg)
{
char szQuery[256];
	sprintf(szQuery,"UPDATE Character SET DbVersion=3 WHERE Name = '%s'",szName);
	g_DataServerDB.Exec(szQuery);
	g_DataServerDB.Clear();
	
	sprintf(szQuery,"UPDATE Character SET Inventory=? WHERE Name='%s'",szName);
	g_DataServerDB.WriteBlob(szQuery,lpMsg->dbInventory,sizeof(lpMsg->dbInventory));
}

DWORD GetNewSerial()
{
	return g_DataServerDB.ExecGetInt("EXEC WZ_GetItemSerial");
}

DWORD GetNewPet()
{
char szQuery[256];
	DWORD s_serial = g_DataServerDB.ExecGetInt("EXEC WZ_GetItemSerial");

	sprintf(szQuery,"INSERT INTO T_PetItem_Info (ItemSerial, Pet_Level, Pet_Exp) VALUES (%d, %d, %d)",
		s_serial,1,0);

	//g_DataServerDB.Exec(szQuery);
	//g_DataServerDB.Clear();

return s_serial;
}


void GetPetInfo(LPRecv_PetItem_Info lpPet)
{
char szQuery[256];

	sprintf(szQuery,"SELECT Pet_Level, Pet_Exp FROM T_PetItem_Info WHERE ItemSerial=%d",lpPet->nSerial);

	if(g_DataServerDB.Exec(szQuery) == FALSE || g_DataServerDB.Fetch() == SQL_NO_DATA)
	{
		g_DataServerDB.Clear();
		sprintf(szQuery,"INSERT INTO T_PetItem_Info (ItemSerial, Pet_Level, Pet_Exp) VALUES (%d, %d, %d)",
			lpPet->nSerial,1,0);

		g_DataServerDB.Exec(szQuery);
		g_DataServerDB.Clear();

		lpPet->Level = 1;
		lpPet->Exp = 0;
	return;
	}

	lpPet->Level = g_DataServerDB.GetInt("Pet_Level");
	lpPet->Exp = g_DataServerDB.GetInt("Pet_Exp");

	g_DataServerDB.Clear();
}

void SetPetInfo(LPSave_PetItem_Info lpPet)
{
char szQuery[256];
	sprintf(szQuery,"UPDATE T_PetItem_Info SET Pet_Level=%d, Pet_Exp=%d WHERE ItemSerial=%d",
		lpPet->Level,lpPet->Exp,lpPet->nSerial);

	g_DataServerDB.Exec(szQuery);
	g_DataServerDB.Clear();
}

void SetOptionData(char* szName,LPSDHP_SKILLKEYDATA lpData)
{
char szQuery[256];
	sprintf(szQuery,"SELECT Name FROM OptionData WHERE Name='%s'",szName);

	g_DataServerDB.Exec(szQuery);

	if(g_DataServerDB.Fetch() == SQL_NO_DATA)
	{
		g_DataServerDB.Clear();
		sprintf(szQuery,"INSERT INTO OptionData (Name, GameOption, Qkey, Wkey, Ekey, ChatWindow, Rkey, QWERLevel) VALUES ('%s', %d, %d, %d, %d, %d, %d, %d)",
			szName,lpData->GameOption,lpData->QkeyDefine,lpData->WkeyDefine,lpData->EkeyDefine,lpData->ChatWindow,lpData->RkeyDefine,lpData->QWERLevel);

		g_DataServerDB.Exec(szQuery);
	}
	else
	{
		g_DataServerDB.Clear();
		sprintf(szQuery,"UPDATE OptionData SET GameOption=%d, Qkey=%d, Wkey=%d, Ekey=%d, ChatWindow=%d, Rkey=%d, QWERLevel=%d WHERE Name='%s'",
			lpData->GameOption,lpData->QkeyDefine,lpData->WkeyDefine,lpData->EkeyDefine,lpData->ChatWindow,lpData->RkeyDefine,lpData->QWERLevel,szName);
		g_DataServerDB.Exec(szQuery);
	}

	g_DataServerDB.Clear();

	sprintf(szQuery,"UPDATE OptionData SET SkillKey=? where Name='%s'",szName);
	g_DataServerDB.WriteBlob(szQuery,lpData->SkillKeyBuffer,sizeof(lpData->SkillKeyBuffer));
}

void GetCastleOwner(int iMapServerGroup,LPCSP_ANS_OWNERGUILDMASTER pResult)
{
char szQuery[256];
	sprintf(szQuery,"EXEC WZ_CS_GetOwnerGuildMaster %d",iMapServerGroup);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		pResult->iResult = g_DataServerDB.GetInt("QueryResult");
		g_DataServerDB.GetStr("OwnerGuild",pResult->szCastleOwnGuild);
		g_DataServerDB.GetStr("OwnerGuildMaster",pResult->szCastleOwnGuildMaster);
	}
	else
	{
		pResult->iResult = 0;
	}
g_DataServerDB.Clear();
}

int CastleNpcBuy(LPCSP_REQ_NPCBUY lpNpcInfo)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_ReqNpcBuy %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
		lpNpcInfo->wMapSvrNum,lpNpcInfo->iNpcNumber,lpNpcInfo->iNpcIndex,lpNpcInfo->iNpcDfLevel,lpNpcInfo->iNpcRgLevel,lpNpcInfo->iNpcMaxHp,lpNpcInfo->iNpcHp,lpNpcInfo->btNpcX,lpNpcInfo->btNpcY,lpNpcInfo->btNpcDIR);


	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)	{ 
		bRet = g_DataServerDB.GetInt("QueryResult");
	}

	g_DataServerDB.Clear();
return bRet;
}

int CastleNpcRepair(LPCSP_REQ_NPCREPAIR lpNpcInfo,LPCSP_ANS_NPCREPAIR pResult)
{
char szQuery[256];
int bRet = FALSE;
	sprintf(szQuery,"EXEC WZ_CS_ReqNpcRepair %d, %d, %d",
		lpNpcInfo->wMapSvrNum,lpNpcInfo->iNpcNumber,lpNpcInfo->iNpcIndex);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
		pResult->iNpcHpl = g_DataServerDB.GetInt("NPC_HP");
		pResult->iNpcMaxHp = g_DataServerDB.GetInt("NPC_MAXHP");
	}
	g_DataServerDB.Clear();
return bRet;
}

int CastleNpcUpgrade(LPCSP_REQ_NPCUPGRADE lpNpcInfo)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_ReqNpcUpgrade %d, %d, %d, %d, %d",
		lpNpcInfo->wMapSvrNum,lpNpcInfo->iNpcNumber,lpNpcInfo->iNpcIndex,lpNpcInfo->iNpcUpType,lpNpcInfo->iNpcUpValue);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
	}
	g_DataServerDB.Clear();
return bRet;
}

int GetTaxInfo(int iMapServerGroup,LPCSP_ANS_TAXINFO lpResult)
{
char szQuery[256];
int bRet = FALSE;
	sprintf(szQuery,"EXEC WZ_CS_GetCastleTaxInfo %d",iMapServerGroup);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = TRUE;

		lpResult->i64CastleMoney = g_DataServerDB.GetInt64("MONEY");
		lpResult->iTaxRateChaos = g_DataServerDB.GetInt("TAX_RATE_CHAOS");
		lpResult->iTaxRateStore = g_DataServerDB.GetInt("TAX_RATE_STORE");
		lpResult->iTaxHuntZone = g_DataServerDB.GetInt("TAX_HUNT_ZONE");
	}
	g_DataServerDB.Clear();
return bRet;
}

int SetTaxInfo(LPCSP_REQ_TAXRATECHANGE lpInfo,LPCSP_ANS_TAXRATECHANGE lpResult)
{
char szQuery[256];
int bRet = FALSE;
	sprintf(szQuery,"EXEC WZ_CS_ModifyTaxRate %d, %d, %d",
		lpInfo->wMapSvrNum,lpInfo->iTaxKind,lpInfo->iTaxRate);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet =  g_DataServerDB.GetInt("QueryResult");
		lpResult->iTaxKind = g_DataServerDB.GetInt("TaxKind");
		lpResult->iTaxRate = g_DataServerDB.GetInt("TaxRate");
	}
	g_DataServerDB.Clear();
return bRet;
}

int SiegeDateChange(LPCSP_REQ_SDEDCHANGE lpDateInfo)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_ModifyCastleSchedule %d, '%d-%d-%d 00:00:00', '%d-%d-%d 00:00:00'",
		lpDateInfo->wMapSvrNum,lpDateInfo->wStartYear,lpDateInfo->btStartMonth,lpDateInfo->btStartDay,
		lpDateInfo->wEndYear,lpDateInfo->btEndMonth,lpDateInfo->btEndDay);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
	}
	g_DataServerDB.Clear();
return bRet;
}

int GuildMarkRegInfo(int iMapSvrGroup,char* szGuild,LPCSP_ANS_GUILDREGINFO lpResult)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_GetGuildMarkRegInfo %d, '%s'",
		iMapSvrGroup,szGuild);

	if(g_DataServerDB.Exec(szQuery) == TRUE)
	{
		if(g_DataServerDB.Fetch() == SQL_NO_DATA)
		{
			bRet = 2;
			lpResult->iRegMarkCount = 0;
		}
		else
		{
			bRet = 1;
			lpResult->iRegMarkCount = g_DataServerDB.GetInt("REG_MARKS");
			lpResult->btRegRank = g_DataServerDB.GetInt("SEQ_NUM");
			lpResult->bIsGiveUp = g_DataServerDB.GetInt("IS_GIVEUP");
		}
	}
	g_DataServerDB.Clear();
return bRet;
}

int SiegeEndedChange(int iMapSvrGroup, int bSiegeEnded)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_ModifySiegeEnd %d, %d",
		iMapSvrGroup,bSiegeEnded);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
	}
	g_DataServerDB.Clear();
return bRet;
}

int CastleOwnerChange(int iMapSvrGroup, int bOccupied, char* szGuildName)
{
char szQuery[256];
int bRet = FALSE;
	
	sprintf(szQuery,"EXEC WZ_CS_ModifyCastleOwnerInfo %d, %d, '%s'",
		iMapSvrGroup,bOccupied,szGuildName);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
	}
	g_DataServerDB.Clear();
return bRet;
}

int RegAttackGuild(int iMapSvrGroup,char* szGuildName)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_ReqRegAttackGuild %d, '%s'",iMapSvrGroup,szGuildName);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
	}
	g_DataServerDB.Clear();
return bRet;
}

int RestartCastleState(int iMapSvrGroup)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_ResetCastleSiege %d",iMapSvrGroup);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch())
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
	}
	g_DataServerDB.Clear();
return bRet;
}

int RegGuildMark(int iMapSvrGroup,char* szGuildName,int *G_Mark)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_ReqRegGuildMark %d, '%s'",iMapSvrGroup,szGuildName);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
		*G_Mark = g_DataServerDB.GetInt("REG_MARKS");
	}
	g_DataServerDB.Clear();
return bRet;
}

int GuildMarkReset(int iMapSvrGroup,char* szGuildName,int * G_Mark)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_ModifyGuildMarkReset %d, '%s'",iMapSvrGroup,szGuildName);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
		*G_Mark = g_DataServerDB.GetInt("DEL_MARKS");
	}
	g_DataServerDB.Clear();
return bRet;
}

int GuildSetGiveUp(LPCSP_REQ_GUILDSETGIVEUP lpInfo,char* szGuildName,int * G_Mark)
{
char szQuery[256];
int bRet = FALSE;
	
	sprintf(szQuery,"EXEC WZ_CS_ModifyGuildGiveUp %d, '%s', %d",lpInfo->wMapSvrNum,szGuildName,lpInfo->bIsGiveUp);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
		*G_Mark = g_DataServerDB.GetInt("DEL_MARKS");
	}
	g_DataServerDB.Clear();
return bRet;
}

int NpcRemove(int iMapSvrGroup,int NpcNumber,int NpcIndex)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_ReqNpcRemove %d, %d, %d",
		iMapSvrGroup,NpcNumber,NpcIndex);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
	}
	g_DataServerDB.Clear();
return bRet;
}

int CastleTributeMoney(int iMapSvrGroup,int Money,__int64 * RetMoney)
{
char szQuery[256];
int bRet = FALSE;
	sprintf(szQuery,"EXEC WZ_CS_ModifyMoney %d, %d",iMapSvrGroup,Money);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
		*RetMoney = g_DataServerDB.GetInt64("MONEY");
	}
	g_DataServerDB.Clear();
return bRet;
}

int ResetCastleTaxInfo(int iMapSvrGroup)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_ResetCastleTaxInfo %d",iMapSvrGroup);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
	}
	g_DataServerDB.Clear();
return bRet;
}

int ResetSiegeGuildInfo(int iMapSvrGroup)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_ResetSiegeGuildInfo %d",iMapSvrGroup);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
	}
	g_DataServerDB.Clear();
return bRet;
}

int ResetRegSiegeInfo(int iMapSvrGroup)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_ResetRegSiegeInfo %d",iMapSvrGroup);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
	}
	g_DataServerDB.Clear();
return bRet;
}

int GetCastleTotalInfo(int iMapSvrGroup,int CastleCycle,LPCSP_ANS_CSINITDATA lpMsg)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"EXEC WZ_CS_GetCastleTotalInfo %d, %d",iMapSvrGroup,CastleCycle);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = TRUE;

		lpMsg->wStartYear = (WORD)g_DataServerDB.GetInt("SYEAR");
		lpMsg->btStartMonth = (BYTE)g_DataServerDB.GetInt("SMONTH");
		lpMsg->btStartDay = (BYTE)g_DataServerDB.GetInt("SDAY");
		lpMsg->wEndYear = (WORD)g_DataServerDB.GetInt("EYEAR");
		lpMsg->btEndMonth = (BYTE)g_DataServerDB.GetInt("EMONTH");
		lpMsg->btEndDay = (BYTE)g_DataServerDB.GetInt("EDAY");
		lpMsg->btIsSiegeGuildList = (BYTE)g_DataServerDB.GetInt("SIEGE_GUILDLIST_SETTED");
		lpMsg->btIsSiegeEnded = (BYTE)g_DataServerDB.GetInt("SIEGE_ENDED");
		lpMsg->btIsCastleOccupied = (BYTE)g_DataServerDB.GetInt("CASTLE_OCCUPY");
		lpMsg->i64CastleMoney = g_DataServerDB.GetInt64("MONEY");
		lpMsg->iTaxRateChaos = g_DataServerDB.GetInt("TAX_RATE_CHAOS");
		lpMsg->iTaxRateStore = g_DataServerDB.GetInt("TAX_RATE_STORE");
		lpMsg->iTaxHuntZone = g_DataServerDB.GetInt("TAX_HUNT_ZONE");
		lpMsg->iFirstCreate = g_DataServerDB.GetInt("FIRST_CREATE");
		g_DataServerDB.GetStr("OWNER_GUILD",lpMsg->szCastleOwnGuild);
	}
	g_DataServerDB.Clear();
return bRet;
}

int GetCastleNpcInfo(int iMapSvrGroup,LPCSP_CSINITDATA lpMsgBody)
{
char szQuery[256];
	sprintf(szQuery,"EXEC WZ_CS_GetCastleNpcInfo %d",iMapSvrGroup);

	int iCount = 0;

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		do
		{
			lpMsgBody[iCount].iNpcNumber = g_DataServerDB.GetInt("NPC_NUMBER");
			lpMsgBody[iCount].iNpcIndex = g_DataServerDB.GetInt("NPC_INDEX");
			lpMsgBody[iCount].iNpcDfLevel = g_DataServerDB.GetInt("NPC_DF_LEVEL");
			lpMsgBody[iCount].iNpcRgLevel = g_DataServerDB.GetInt("NPC_RG_LEVEL");
			lpMsgBody[iCount].iNpcMaxHp = g_DataServerDB.GetInt("NPC_MAXHP");
			lpMsgBody[iCount].iNpcHp = g_DataServerDB.GetInt("NPC_HP");
			lpMsgBody[iCount].btNpcX = g_DataServerDB.GetInt("NPC_X");
			lpMsgBody[iCount].btNpcY = g_DataServerDB.GetInt("NPC_Y");
			lpMsgBody[iCount].btNpcDIR = g_DataServerDB.GetInt("NPC_DIR");
		}
		while((iCount++) < MAX_CS_NPC && g_DataServerDB.Fetch() != SQL_NO_DATA);
	}
	g_DataServerDB.Clear();
return iCount;
}

int GetAllGuildMarkRegInfo(int iMapSvrGroup,LPCSP_GUILDREGINFO lpMsgBody)
{
char szQuery[256];
	sprintf(szQuery,"EXEC WZ_CS_GetAllGuildMarkRegInfo %d",iMapSvrGroup);

	int iCount = 0;

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		do
		{
			g_DataServerDB.GetStr("REG_SIEGE_GUILD",lpMsgBody[iCount].szGuildName);
			lpMsgBody[iCount].iRegMarkCount = g_DataServerDB.GetInt("REG_MARKS");
			lpMsgBody[iCount].bIsGiveUp = g_DataServerDB.GetInt("IS_GIVEUP");
			lpMsgBody[iCount].btRegRank = g_DataServerDB.GetInt("SEQ_NUM");
		}
		while( (iCount++) < MAX_CS_GUILDLIST && g_DataServerDB.Fetch() != SQL_NO_DATA);
	}
	g_DataServerDB.Clear();
return iCount;
}

int FirstCreateNPC(int iMapSvrGroup,int count,LPCSP_NPCSAVEDATA lpNpc)
{
char szQuery[256];
int bRet = TRUE;
	
	sprintf(szQuery,"DELETE FROM MuCastle_NPC WHERE MAP_SVR_GROUP = %d",iMapSvrGroup);
	g_DataServerDB.Exec(szQuery);

	for(int i = 0; i < count; i++)
	{
		sprintf(szQuery,"EXEC WZ_CS_ReqNpcBuy %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
			iMapSvrGroup,lpNpc[i].iNpcNumber,lpNpc[i].iNpcIndex,
			lpNpc[i].iNpcDfLevel,lpNpc[i].iNpcRgLevel,lpNpc[i].iNpcMaxHp,
			lpNpc[i].iNpcHp,lpNpc[i].btNpcX,lpNpc[i].btNpcY,lpNpc[i].btNpcDIR);

		if(g_DataServerDB.Exec(szQuery) != TRUE)
		{
			bRet = FALSE;
			break;
		}

		g_DataServerDB.Clear();
	}

	g_DataServerDB.Clear();
return bRet;
}

int GetCalcRegGuildList(int iMapSvrGroup,LPCSP_CALCREGGUILDLIST lpMsgBody)
{
char szQuery[256];
	sprintf(szQuery,"EXEC WZ_CS_GetCalcRegGuildList %d",iMapSvrGroup);

	int iCount = -1;

	if(g_DataServerDB.Exec(szQuery) == TRUE)
	{
		iCount = 0;

		if(g_DataServerDB.Fetch() != SQL_NO_DATA)
		{
			do
			{
				g_DataServerDB.GetStr("REG_SIEGE_GUILD",lpMsgBody[iCount].szGuildName);
				lpMsgBody[iCount].iRegMarkCount = g_DataServerDB.GetInt("REG_MARKS");
				lpMsgBody[iCount].iGuildMemberCount = g_DataServerDB.GetInt("GUILD_MEMBER");
				lpMsgBody[iCount].iGuildMasterLevel = g_DataServerDB.GetInt("GM_LEVEL");
				lpMsgBody[iCount].iSeqNum = g_DataServerDB.GetInt("SEQ_NUM");
			}
			while((iCount++) < MAX_CS_GUILDLIST && g_DataServerDB.Fetch() != SQL_NO_DATA);
		}
	}
	g_DataServerDB.Clear();
return iCount;
}

int GetCsGuildUnionInfo(int count,LPCSP_CSGUILDUNIONINFO lpInfo,LPCSP_CSGUILDUNIONINFO lpOutInfo)
{
char szQuery[256];

	int iCount = 0;
	
	for(int i = 0; i < count; i++)
	{
		char szGuildName[9] = {0};
		memcpy(szGuildName,lpInfo[i].szGuildName,8);
		sprintf(szQuery,"EXEC WZ_CS_GetCsGuildUnionInfo '%s'",szGuildName);

		if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
		{
			do
			{
				g_DataServerDB.GetStr("GUILD_NAME",lpOutInfo[iCount].szGuildName);
				lpOutInfo[iCount].iCsGuildID = lpInfo[i].iCsGuildID;
			}
			while((iCount++) < MAX_CS_GUILDLIST && g_DataServerDB.Fetch() != SQL_NO_DATA);
		}
		else
		{
			g_DataServerDB.Clear();
			break;
		}

		g_DataServerDB.Clear();
	}
return iCount;
}

int SetSiegeGuildInfo(int iMapSvrGroup,int count,LPCSP_CSSAVETOTALGUILDINFO lpMsgBody)
{
char szQuery[256];
int bRet = TRUE;

	for(int i =0 ; i < count; i++)
	{
		char szGuildName[9] = {0};
		memcpy(szGuildName,lpMsgBody[i].szGuildName,8);

		sprintf(szQuery,"EXEC WZ_CS_SetSiegeGuildInfo %d, '%s', %d, %d",
			iMapSvrGroup,szGuildName,lpMsgBody[i].iCsGuildID,lpMsgBody[i].iGuildInvolved);

		if(g_DataServerDB.Exec(szQuery) == TRUE )
		{

		}
		else
		{
			bRet = FALSE;
			break;
		}
	}
	g_DataServerDB.Clear();
return bRet;
}

int CsLoadTotalGuildInfo(int iMapSvrGroup,LPCSP_CSLOADTOTALGUILDINFO lpMsgBody)
{
char szQuery[256];

	sprintf(szQuery,"EXEC WZ_CS_GetSiegeGuildInfo %d",iMapSvrGroup);

	int iCount = 0;

	if(g_DataServerDB.Exec(szQuery) && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		do
		{
			g_DataServerDB.GetStr("GUILD_NAME",lpMsgBody[iCount].szGuildName);
			lpMsgBody[iCount].iCsGuildID = g_DataServerDB.GetInt("GUILD_ID");
			lpMsgBody[iCount].iGuildInvolved = g_DataServerDB.GetInt("GUILD_INVOLVED");
		}
		while((iCount++) < MAX_CS_GUILDLIST && g_DataServerDB.Fetch() != SQL_NO_DATA);
	}
	g_DataServerDB.Clear();
return iCount;
}

int CastleNpcUpdate(int iMapSvrGroup,int count,LPCSP_NPCUPDATEDATA lpMsgBody)
{
char szQuery[256];
int bRet = TRUE;

	sprintf(szQuery,"DELETE FROM MuCastle_NPC WHERE MAP_SVR_GROUP = %d",iMapSvrGroup);
	g_DataServerDB.Exec(szQuery);

	for(int i =0; i < count; i++)
	{
		sprintf(szQuery,"EXEC WZ_CS_ReqNpcUpdate %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
			iMapSvrGroup,lpMsgBody[i].iNpcNumber,lpMsgBody[i].iNpcIndex,
				lpMsgBody[i].iNpcDfLevel,lpMsgBody[i].iNpcRgLevel,lpMsgBody[i].iNpcMaxHp,
				lpMsgBody[i].iNpcHp,lpMsgBody[i].btNpcX,lpMsgBody[i].btNpcY,lpMsgBody[i].btNpcDIR);

		if(g_DataServerDB.Exec(szQuery) == TRUE)
		{

		}
		else
		{
			bRet = FALSE;
			break;
		}

	}
	g_DataServerDB.Clear();
return bRet;
}

int CrywolfInfoLoad(int iMapSvrGroup,LPCWP_ANS_CRYWOLFINFOLOAD lpResult)
{
char szQuery[256];
int bRet = FALSE;
	sprintf(szQuery,"EXEC WZ_CW_InfoLoad %d",iMapSvrGroup);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = TRUE;
		lpResult->iOccupationState = g_DataServerDB.GetInt("CRYWOLF_OCCUFY");
		lpResult->iCrywolfState = g_DataServerDB.GetInt("CRYWOLF_STATE");
	}
	g_DataServerDB.Clear();
return bRet;
}

int CrywolfInfoSave(int iMapSvrGroup,int iOccupy,int iState)
{
char szQuery[256];
int bRet = FALSE;
	sprintf(szQuery,"EXEC WZ_CW_InfoSave %d, %d, %d",iMapSvrGroup,iState,iOccupy);

	if(g_DataServerDB.Exec(szQuery) == TRUE)
	{
		bRet = TRUE;
	}
return bRet;
}


int InsertPeriodItem(LPMSG_REQ_PERIODITEM_INSERT lpMsg)
{
char szQuery[256];
int bRet = 2;

	sprintf(szQuery,"WZ_PeriodItemInsert %d, '%s', %d, %d, %d, %d, %d, '%s'",
		lpMsg->iUserGuid,lpMsg->chUserID,lpMsg->iItemCode,lpMsg->btItemOption,lpMsg->btItemEffectType1,
		lpMsg->btItemEffectType2,lpMsg->iItemPeriodDate,lpMsg->chExpireDate);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt("QueryResult");
	}
	g_DataServerDB.Clear();
return bRet;
}

int GetPeriodItemList(int Guid,char* szName,LPERIODITEM_LIST lpItems)
{
char szQuery[256];
int count = NULL;

	sprintf(szQuery,"WZ_PeriodItemSelect %d, '%s'",Guid,szName);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		do
		{
			lpItems[count].btUsedItem = TRUE;
			lpItems[count].iItemCode = g_DataServerDB.GetInt("ItemCode");
			lpItems[count].btItemOption = g_DataServerDB.GetInt("ItemOption");
			lpItems[count].btItemEffectType1 = g_DataServerDB.GetInt("ItemEffect1");
			lpItems[count].btItemEffectType2 = g_DataServerDB.GetInt("ItemEffect2");
			lpItems[count].iItemPeriodUsedTime = g_DataServerDB.GetInt("UsedTime");
			lpItems[count].iItemPeriodLeftTime = g_DataServerDB.GetInt("LeftTime");
			count++;
		}
		while(count < SUBITEM_INFO_COUNT && g_DataServerDB.Fetch() != SQL_NO_DATA);
	}
	g_DataServerDB.Clear();

return count;
}

int UpdatePeriodItem(LPMSG_REQ_PERIODITEM_UPDATE lpMsg)
{
char szQuery[256];
int bRet = 3;

	sprintf(szQuery,"WZ_PeriodItemUpdate %d, '%s', %d, %d, %d",
		lpMsg->iUserGuid,lpMsg->chCharacterName,lpMsg->iItemCode,lpMsg->iUsedTime,lpMsg->iLeftTime);

	if(g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA)
	{
		bRet = g_DataServerDB.GetInt(0);
	}
	g_DataServerDB.Clear();
return bRet;
}

int DeletePeriodItem(int Guid, char* szName)
{
char szQuery[256];
int bRet = FALSE;

	sprintf(szQuery,"WZ_PeriodItemDelete %d, '%s'",Guid,szName);

	if(g_DataServerDB.Exec(szQuery) == TRUE)
	{
		bRet = TRUE;
	}
	g_DataServerDB.Clear();
return bRet;
}

int GetMasterInfo(LPMSG_ANS_MASTEREINFO lpMsg,char* szName)
{
int bResult = 0;
char szQuery[256];

	lpMsg->result = FALSE;
	sprintf(szQuery,"EXEC WZ_MLS_InfoLoad '%s'",szName);

	if( g_DataServerDB.Exec(szQuery) == TRUE )
	{
		bResult = TRUE;
		lpMsg->MasterLevel = 0;
		lpMsg->MasterNextExp = 0;
		if( g_DataServerDB.Fetch() != SQL_NO_DATA )
		{
			lpMsg->MasterLevel = g_DataServerDB.GetInt("MASTER_LEVEL");
			lpMsg->MasterExp = g_DataServerDB.GetInt64("ML_EXP");
			lpMsg->MasterNextExp = g_DataServerDB.GetInt64("ML_NEXTEXP");
			lpMsg->MasterPoint = g_DataServerDB.GetInt("ML_POINT");
		}
	}
	g_DataServerDB.Clear();
return bResult;
}

int GetMonsterKillInfo(LPMSG_ANS_MONSTERKILLINFO lpMsg,char* szName)
{
int bResult = FALSE;
char szQuery[256];

	sprintf(szQuery,"EXEC WZ_QMK_InfoLoad '%s'",szName);

	if( g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA )
	{
		bResult = TRUE;

		lpMsg->QuestIndex = g_DataServerDB.GetInt("QUEST_INDEX");
		lpMsg->KillInfo[0].m_MonIndex = g_DataServerDB.GetInt("MON_INDEX_1");
		lpMsg->KillInfo[0].m_KillCount = g_DataServerDB.GetInt("KILL_COUNT_1");

		lpMsg->KillInfo[1].m_MonIndex = g_DataServerDB.GetInt("MON_INDEX_2");
		lpMsg->KillInfo[1].m_KillCount = g_DataServerDB.GetInt("KILL_COUNT_2");

		lpMsg->KillInfo[2].m_MonIndex = g_DataServerDB.GetInt("MON_INDEX_3");
		lpMsg->KillInfo[2].m_KillCount = g_DataServerDB.GetInt("KILL_COUNT_3");

		lpMsg->KillInfo[3].m_MonIndex = g_DataServerDB.GetInt("MON_INDEX_4");
		lpMsg->KillInfo[3].m_KillCount = g_DataServerDB.GetInt("KILL_COUNT_4");

		lpMsg->KillInfo[4].m_MonIndex = g_DataServerDB.GetInt("MON_INDEX_5");
		lpMsg->KillInfo[4].m_KillCount = g_DataServerDB.GetInt("KILL_COUNT_5");
	}
	g_DataServerDB.Clear();

return bResult;
}

int SaveMonsterKillInfo(LPMSG_SAVE_MONSTERKILLINFO lpMsg,char* szName)
{
int bResult = FALSE;
char szQuery[256];

	sprintf(szQuery,"EXEC WZ_QMK_InfoSave '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
		szName,lpMsg->QuestIndex,lpMsg->KillInfo[0].m_MonIndex,lpMsg->KillInfo[0].m_KillCount,
			lpMsg->KillInfo[1].m_MonIndex,lpMsg->KillInfo[1].m_KillCount,
			lpMsg->KillInfo[2].m_MonIndex,lpMsg->KillInfo[2].m_KillCount,
			lpMsg->KillInfo[3].m_MonIndex,lpMsg->KillInfo[3].m_KillCount,
			lpMsg->KillInfo[4].m_MonIndex,lpMsg->KillInfo[4].m_KillCount);

	if( g_DataServerDB.Exec(szQuery) == TRUE )
	{
		bResult = TRUE;
	}
	g_DataServerDB.Clear();

return bResult;
}

int SaveMasterInfo(LPMSG_SAVE_MASTERINFO lpMsg,char* szName)
{
int bResult = FALSE;
char szQuery[256];

	sprintf(szQuery,"EXEC WZ_MLS_InfoSave '%s', %d, %I64d, %I64d, %d",
		szName,lpMsg->MasterLevel,lpMsg->MasterExp,lpMsg->MasterNextExp,lpMsg->MasterPoints);

	if( g_DataServerDB.Exec(szQuery) == TRUE )
	{
		bResult = TRUE;
	}
	g_DataServerDB.Clear();

return bResult;
}

int GetSummonerCreate(PMSG_ANS_SUMMONER_CREATE * lpMsg,char* szAccountID)
{
int bResult = FALSE;
char szQuery[256];

	sprintf(szQuery,"SELECT Summoner FROM AccountCharacter WHERE Id = '%s'",szAccountID);

	if( g_DataServerDB.Exec(szQuery) == TRUE && g_DataServerDB.Fetch() != SQL_NO_DATA )
	{
		int bSummoner = g_DataServerDB.GetInt("Summoner");

		if( bSummoner == TRUE )
		{
			bResult = TRUE;
		}
	}
	g_DataServerDB.Clear();
return bResult;
}

int SaveSummonerCreate(char* szId)
{
int bResult = FALSE;
char szQuery[256];

	sprintf(szQuery,"UPDATE AccountCharacter SET Summoner = 1 WHERE Id = '%s'",szId);

	if( g_DataServerDB.Exec(szQuery) == TRUE )
	{
		bResult = TRUE;
	}

return bResult;
}

void UpdateGameIDC(char* szId, char* szName)
{
	char szQuery[256];

	sprintf(szQuery,"UPDATE AccountCharacter SET GameIDC = '%s' WHERE Id = '%s'",
		szName,szId);

	g_DataServerDB.Exec(szQuery);
}

void UpdateQuestInfoEx(PMSG_REQ_QUESTEXINFO_SAVE* szInfo,char*szName)
{
char szQuery[256];
	sprintf(szQuery,"UPDATE T_QuestEx_Info SET memb_quest = ? WHERE memb_char = '%s'",szName);

	g_DataServerDB.WriteBlob(szQuery,szInfo->QuestInfo,sizeof(szInfo->QuestInfo));
}

void ReadQuestInfoEx(char* szName,PMSG_ANS_QUESTEXINFO* lpMsg)
{
char szQuery[256];

	lpMsg->btResult = TRUE;
	sprintf(szQuery,"SELECT memb_char FROM T_QuestEx_Info WHERE memb_char = '%s'",szName);

	if( g_DataServerDB.Exec(szQuery) != TRUE )
	{
		lpMsg->btResult = FALSE;
		g_DataServerDB.Clear();
		return;
	}

	if( g_DataServerDB.Fetch() == SQL_NO_DATA )
	{
		g_DataServerDB.Clear();

		memset(lpMsg->QuestInfo,0,sizeof(lpMsg->QuestInfo));
		sprintf(szQuery,"INSERT INTO T_QuestEx_Info (memb_char,memb_quest) VALUES ('%s',CONVERT(BINARY(260),0))",szName);
		g_DataServerDB.Exec(szQuery);
		g_DataServerDB.Clear();
	}
	else
	{
		g_DataServerDB.Clear();
		sprintf(szQuery,"SELECT memb_quest FROM T_QuestEx_Info WHERE memb_char = '%s'",szName);

		if( g_DataServerDB.ReadBlob(szQuery,lpMsg->QuestInfo) != sizeof(lpMsg->QuestInfo) )
		{
			lpMsg->btResult = FALSE;
		}
		else
		{
			lpMsg->btResult = TRUE;
		}
	}
}

void JoinGens(char* szName,char* szMasterName,int iGens,PMSG_ANS_GENS_JOINREQUEST* lpMsg)
{
char szQuery[256];

#ifdef ZTEAMDB
	sprintf(szQuery,"exec GensAddUser '%s', '%s', %d",
		szName,szMasterName,iGens);
#else
	sprintf(szQuery,"exec WZ_GensAddUser '%s', '%s', %d",
		szName,szMasterName,iGens);
#endif

	lpMsg->btResult = FALSE;

	if( !g_DataServerDB.Exec(szQuery) || g_DataServerDB.Fetch() == SQL_NO_DATA )
	{
		lpMsg->btResult = -1;
		g_DataServerDB.Clear();
		return;
	}

	lpMsg->btResult = g_DataServerDB.GetInt("Result");
	lpMsg->btGensType = g_DataServerDB.GetInt("iGens");
	lpMsg->iRank = g_DataServerDB.GetInt("iRank");

	g_DataServerDB.Clear();
}

void GetGensInfo(char* szName,PMSG_ANS_GENS_INFO* pMsg)
{
char szQuery[256];
#ifdef ZTEAMDB
	sprintf(szQuery,"SELECT memb_clan,memb_rank,memb_contribution,memb_reward FROM GensUserInfo WHERE memb_char = '%s'",szName);
#else
	sprintf(szQuery,"SELECT memb_clan,memb_rank,memb_contribution,memb_reward FROM T_Gens_UserInfo WHERE memb_char = '%s'",szName);
#endif
	if( !g_DataServerDB.Exec(szQuery) || g_DataServerDB.Fetch() == SQL_NO_DATA)
	{
		pMsg->btResult = 0;
		g_DataServerDB.Clear();
		return;
	}


	pMsg->btGens = g_DataServerDB.GetInt("memb_clan");
	pMsg->iRank = g_DataServerDB.GetInt("memb_rank");
	pMsg->iContribution = g_DataServerDB.GetInt("memb_contribution");
	pMsg->iReward = g_DataServerDB.GetInt("memb_reward");
	pMsg->btResult = TRUE;

	g_DataServerDB.Clear();
}


void UpdateGens(char* szName,int iContribution,int iReward)
{
char szQuery[256];

#ifdef ZTEAMDB
	sprintf(szQuery,"UPDATE GensUserInfo SET memb_contribution = %d, memb_reward = %d WHERE memb_char = '%s'",
		iContribution,iReward,szName);
#else
	sprintf(szQuery,"UPDATE T_Gens_UserInfo SET memb_contribution = %d, memb_reward = %d WHERE memb_char = '%s'",
		iContribution,iReward,szName);
#endif

	g_DataServerDB.Exec(szQuery);
	g_DataServerDB.Clear();
}

void LeaveGens(char* szName,int iGens,PMSG_ANS_LEAVE_GENS* lpMsg)
{
char szQuery[256];

	lpMsg->btResult = TRUE;

#ifdef ZTEAMDB
	sprintf(szQuery,"exec GensDelUser '%s', %d",szName,iGens);
#else
	sprintf(szQuery,"exec WZ_GensRemoveUser '%s', %d",szName,iGens);
#endif

	if( !g_DataServerDB.Exec(szQuery) || g_DataServerDB.Fetch() == SQL_NO_DATA )
	{
		g_DataServerDB.Clear();
		return;
	}

	lpMsg->btResult = g_DataServerDB.GetInt("Result");
	g_DataServerDB.Clear();
}

void GensServerInfo(int iServerCode,int iUpdateDays,int iPrizeDays,PMSG_ANS_GENS_SERVERINFO* lpMsg)
{
char szQuery[256];

	lpMsg->btResult = FALSE;

#ifdef ZTEAMDB
	sprintf(szQuery,"exec GensServerInfo %d,%d,%d",iServerCode,iUpdateDays,iPrizeDays);
#else
	sprintf(szQuery,"exec WZ_GensServerInfo %d,%d,%d",iServerCode,iUpdateDays,iPrizeDays);
#endif

	if( !g_DataServerDB.Exec(szQuery) || g_DataServerDB.Fetch() == SQL_NO_DATA )
	{
		g_DataServerDB.Clear();
		return;
	}

	lpMsg->btResult = TRUE;
	lpMsg->btUpdateDays = g_DataServerDB.GetInt("DaysFromUpdate");
	g_DataServerDB.Clear();
}

void ExpandWarehouse(char* szAccountID,int count)
{
char szQuery[256];

#ifdef ZTEAMDB
	sprintf(szQuery,"UPDATE AccountCharacter SET ExWarehouse = %d WHERE Id = '%s'",
		count,szAccountID);
#else
	sprintf(szQuery,"UPDATE AccountCharacter SET ExVault = %d WHERE Id = '%s'",
		count,szAccountID);
#endif

	g_DataServerDB.Exec(szQuery);
	g_DataServerDB.Clear();
}

void ExpandInventory(char* szAccountID,char* szName,int count)
{
	char szQuery[256];

	sprintf(szQuery,"UPDATE Character SET ExInventory = %d WHERE AccountID = '%s' AND Name = '%s'",
		count,szAccountID,szName);

	g_DataServerDB.Exec(szQuery);
	g_DataServerDB.Clear();
}

void MuBotSaveOption(char* szName,LPMUBOT_SETTINGS_REQ_SAVE lpMsg)
{
	char szQuery[256];
	sprintf(szQuery,"UPDATE OptionData SET MuBot = ? WHERE Name = '%s'",szName);
	g_DataServerDB.WriteBlob(szQuery,lpMsg->btDATA,sizeof(lpMsg->btDATA));
	g_DataServerDB.Clear();
}

void GetMuBotData(char* szName,LPMUBOT_SETTINGS_SEND lpMsg)
{
	char szQuery[256];
	sprintf(szQuery,"SELECT * FROM OptionData WHERE Name='%s'",szName);

	lpMsg->btDATA[0] = 0xFF;

	if(g_DataServerDB.Exec(szQuery) == TRUE )
	{
		if( g_DataServerDB.Fetch() != SQL_NO_DATA )
		{
			g_DataServerDB.Clear();

			BYTE btTemp[sizeof(lpMsg->btDATA)]={0};
			sprintf(szQuery, "SELECT MuBot FROM OptionData WHERE Name='%s'", szName);
			g_DataServerDB.ReadBlob(szQuery, btTemp);
			memcpy(lpMsg->btDATA, btTemp, sizeof(lpMsg->btDATA));
		}
	}

	g_DataServerDB.Clear();
}

void GetErtelList(char* szAccount,char* szName,LPMSG_ANS_ERTELLIST lpMsg)
{
	char szQuery[256];
	BYTE btTemp[700] = {0};

#ifdef ZTEAMDB
	sprintf(szQuery,"SELECT ErtelList FROM Ertel_Inventory WHERE UserName = '%s'",szName);
#else
	sprintf(szQuery,"SELECT ERTEL_LIST FROM T_Ertel_Inventory WHERE CHAR_NAME = '%s'",szName);
#endif

	if( g_DataServerDB.ReadBlob(szQuery,btTemp) <= 0 )
	{
		memset(lpMsg->ErtelList1,(BYTE)-1,sizeof(lpMsg->ErtelList1));
#ifdef ZTEAMDB
		sprintf(szQuery,"INSERT INTO Ertel_Inventory (UserName,ErtelList) VALUES ('%s',NULL)",szName);
#else
		sprintf(szQuery,"INSERT INTO T_Ertel_Inventory (CHAR_NAME,ERTEL_LIST) VALUES ('%s',NULL)",szName);
#endif
		g_DataServerDB.Exec(szQuery);
		g_DataServerDB.Clear();
	}
	else
	{
		memcpy(lpMsg->ErtelList1,btTemp,sizeof(lpMsg->ErtelList1));
	}

#ifdef ZTEAMDB
	sprintf(szQuery,"SELECT ErtelList FROM Ertel_Warehouse WHERE AccountID = '%s'",szAccount);
#else
	sprintf(szQuery,"SELECT ERTEL_LIST FROM T_Ertel_Warehouse WHERE ACCOUNT = '%s'",szAccount);
#endif

	if( !g_DataServerDB.ReadBlob(szQuery,btTemp) )
	{
		memset(lpMsg->ErtelList2,(BYTE)-1,sizeof(lpMsg->ErtelList2));
#ifdef ZTEAMDB
		sprintf(szQuery,"INSERT INTO Ertel_Warehouse (AccountID,ErtelList) VALUES ('%s',NULL)",szAccount);
#else
		sprintf(szQuery,"INSERT INTO T_Ertel_Warehouse (ACCOUNT,ERTEL_LIST) VALUES ('%s',NULL)",szAccount);
#endif

		g_DataServerDB.Exec(szQuery);
		g_DataServerDB.Clear();
	}
	else
	{
		memcpy(lpMsg->ErtelList2,btTemp,sizeof(lpMsg->ErtelList2));
	}
}

void SaveErtelList(char* szAccount,char* szName,LPMSG_SAVE_ERTELLIST lpMsg)
{
	char szQuery[256];
#ifdef ZTEAMDB
	sprintf(szQuery,"UPDATE Ertel_Inventory SET ErtelList =? WHERE UserName = '%s'",szName);
	g_DataServerDB.WriteBlob(szQuery,lpMsg->ErtelList1,sizeof(lpMsg->ErtelList1));

	sprintf(szQuery,"UPDATE Ertel_Warehouse SET ErtelList =? WHERE AccountID = '%s'",szAccount);
	g_DataServerDB.WriteBlob(szQuery,lpMsg->ErtelList2,sizeof(lpMsg->ErtelList2));
#else
	sprintf(szQuery,"UPDATE T_Ertel_Inventory SET ERTEL_LIST =? WHERE CHAR_NAME = '%s'",szName);
	g_DataServerDB.WriteBlob(szQuery,lpMsg->ErtelList1,sizeof(lpMsg->ErtelList1));

	sprintf(szQuery,"UPDATE T_Ertel_Warehouse SET ERTEL_LIST =? WHERE ACCOUNT = '%s'",szAccount);
	g_DataServerDB.WriteBlob(szQuery,lpMsg->ErtelList2,sizeof(lpMsg->ErtelList2));
#endif

}

void UpdateDropStatistics()
{

}

void AchievUpdateMonsterData(BYTE Type, short MonsterID, LPSTR UserName)
{
	char TableName[256] = { 0 };

	switch(Type)
	{
	case 0:
		sprintf(TableName, "web_Monster_Achiev");
		break;
	case 1:
		sprintf(TableName, "web_Monster_Ranking");
		break;
	}

	char szQuery[256];
	sprintf(szQuery, "SELECT Count FROM %s WHERE Name = '%s' AND MonsterID = %d", TableName, UserName, MonsterID);
	g_DataServerDB.Exec(szQuery);

	if( g_DataServerDB.Fetch() == SQL_NO_DATA)
	{
		g_DataServerDB.Clear();
		sprintf(szQuery, "INSERT INTO %s (Name, MonsterID, Count) VALUES ('%s', %d, %d)",
			TableName, UserName, MonsterID, 1);
		g_DataServerDB.Exec(szQuery);
	}
	else
	{
		g_DataServerDB.Clear();
		sprintf(szQuery, "UPDATE %s SET Count = Count + 1 WHERE Name = '%s' AND MonsterID = %d", TableName, UserName, MonsterID);
		g_DataServerDB.Exec(szQuery);
	}
	g_DataServerDB.Clear();
}

void AchievUpdateEventData(BYTE Type, LPSTR UserName, AchievGDUpdateEvent* lpData)
{
	char TableName[256] = { 0 };

	switch(Type)
	{
	case 0:
		sprintf(TableName, "web_Event_Achiev");
		break;
	case 1:
		sprintf(TableName, "web_Event_Ranking");
		break;
	}

	char szQuery[512];
	sprintf(szQuery, "SELECT Name FROM %s WHERE Name = '%s'", TableName, UserName);
	g_DataServerDB.Exec(szQuery);

	if( g_DataServerDB.Fetch() == SQL_NO_DATA)
	{
		g_DataServerDB.Clear();

		switch(lpData->EventType)
		{
		case 0:
			{
				sprintf(szQuery, "INSERT INTO %s (Name, BC_Exp, BC_ExtExp, BC_Score, DS_Exp, DS_Point, CC_UKill, CC_MKill, CC_TExp, CC_RExp) VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d)",
					TableName, UserName, lpData->BC_Exp, lpData->BC_ExtExp, lpData->BC_Score, 0, 0, 0, 0, 0, 0);
			}
			break;
		case 1:
			{
				sprintf(szQuery, "INSERT INTO %s (Name, BC_Exp, BC_ExtExp, BC_Score, DS_Exp, DS_Point, CC_UKill, CC_MKill, CC_TExp, CC_RExp) VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d)",
					TableName, UserName, 0, 0, 0, lpData->DS_Exp, lpData->DS_Point, 0, 0, 0, 0);
			}
			break;
		case 2:
			{
				sprintf(szQuery, "INSERT INTO %s (Name, BC_Exp, BC_ExtExp, BC_Score, DS_Exp, DS_Point, CC_UKill, CC_MKill, CC_TExp, CC_RExp) VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d)",
					TableName, UserName, 0, 0, 0, 0, 0, lpData->CC_UKill, lpData->CC_MKill, lpData->CC_TExp, lpData->CC_RExp);
			}
			break;
		}

		g_DataServerDB.Exec(szQuery);
	}
	else
	{
		g_DataServerDB.Clear();

		switch(lpData->EventType)
		{
		case 0:
			{
				sprintf(szQuery, "UPDATE %s SET BC_Exp = BC_Exp + %d, BC_ExtExp = BC_ExtExp + %d, BC_Score = BC_Score + %d WHERE Name = '%s'", 
					TableName, lpData->BC_Exp, lpData->BC_ExtExp, lpData->BC_Score, UserName);
			}
			break;
		case 1:
			{
				sprintf(szQuery, "UPDATE %s SET DS_Exp = DS_Exp + %d, DS_Point = DS_Point + %d WHERE Name = '%s'", 
					TableName, lpData->DS_Exp, lpData->DS_Point, UserName);
			}
			break;
		case 2:
			{
				sprintf(szQuery, "UPDATE %s SET CC_UKill = CC_UKill + %d, CC_MKill = CC_MKill + %d, CC_TExp = CC_TExp + %d, CC_RExp = CC_RExp + %d WHERE Name = '%s'", 
					TableName, lpData->CC_UKill, lpData->CC_MKill, lpData->CC_TExp, lpData->CC_RExp, UserName);
			}
			break;
		}

		g_DataServerDB.Exec(szQuery);
	}

	g_DataServerDB.Clear();
}

void AchievUpdateMixData(BYTE Type, LPSTR UserName, bool Success)
{
	char TableName[256] = { 0 };

	switch(Type)
	{
	case 0:
		sprintf(TableName, "web_Mix_Achiev");
		break;
	case 1:
		sprintf(TableName, "web_Mix_Ranking");
		break;
	}

	char szQuery[512];
	sprintf(szQuery, "SELECT Name FROM %s WHERE Name = '%s'", TableName, UserName);
	g_DataServerDB.Exec(szQuery);

	int SuccessCount = 0, FailCount = 0;

	if( Success )
	{
		SuccessCount++;
	}
	else
	{
		FailCount++;
	}

	if( g_DataServerDB.Fetch() == SQL_NO_DATA )
	{
		g_DataServerDB.Clear();

		sprintf(szQuery, "INSERT INTO %s (Name, Success, Fail) VALUES ('%s', %d, %d)",
			TableName, UserName, SuccessCount, FailCount);

		g_DataServerDB.Exec(szQuery);
	}
	else
	{
		g_DataServerDB.Clear();

		sprintf(szQuery, "UPDATE %s SET Success = Success + %d, Fail = Fail + %d WHERE Name = '%s'", 
			TableName, SuccessCount, FailCount, UserName);

		g_DataServerDB.Exec(szQuery);
	}

	g_DataServerDB.Clear();
}