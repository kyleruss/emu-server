#include "stdafx.h"
#include "main.h"

#if (__NEWOPTION__ == 1)

typedef std::map<int,CUSTOM_OPTIONS_NAME> vtOptName;
typedef std::map<int,CUSTOM_OPTIONS_ENCHANT> vtEnchantOpt;
typedef std::map<int,CUSTOM_OPTIONS_EXCELLENT> vtExcellentOpt;
typedef std::map<int,CUSTOM_OPTIONS_LIST> vtOptList;

vtOptName m_vtOptName;
vtEnchantOpt m_vtEnchantOpt;
vtExcellentOpt m_vtExcellentOpt;
vtOptList m_vtOptList;

__ENCHANT_BONUS pBonusOffsets[8] = {
0x0096BD77, //09             ADD EAX,9
0x0096BD88, //08             ADD EAX,8
0x0096BD99, //07             ADD EAX,7
0x0096BDAA, //06             ADD EAX,6
0x0096BDBB, //05             ADD EAX,5
0x0096BDCC, //04             ADD EAX,4

0x0096BECD, //09             ADD EAX,9
0x0096BEDE, //08             ADD EAX,8
0x0096BEEF, //07             ADD EAX,7
0x0096BF00, //06             ADD EAX,6
0x0096BF11, //05             ADD EAX,5
0x0096BF22, //04             ADD EAX,4

0x0096C020, //09             ADD EAX,9
0x0096C030, //08             ADD EAX,8
0x0096C040, //07             ADD EAX,7
0x0096C050, //06             ADD EAX,6
0x0096C060, //05             ADD EAX,5
0x0096C070, //04             ADD EAX,4

0x0096C167, //09             ADD EAX,9
0x0096C177, //08             ADD EAX,8
0x0096C187, //07             ADD EAX,7
0x0096C197, //06             ADD EAX,6
0x0096C1A7, //05             ADD EAX,5
0x0096C1B7, //04             ADD EAX,4

0x0096C349, //09             ADD EAX,9
0x0096C35A, //08             ADD EAX,8
0x0096C36B, //07             ADD EAX,7
0x0096C37C, //06             ADD EAX,6
0x0096C38D, //05             ADD EAX,5
0x0096C39E, //04             ADD EAX,4

0x0096C41E, //0A             ADD EAX,0A
0x0096C42F, //09             ADD EAX,9
0x0096C440, //08             ADD EAX,8
0x0096C451, //07             ADD EAX,7
0x0096C462, //06             ADD EAX,6
0x0096C473, //05             ADD EAX,5

0x0096C4DE, //09             ADD EAX,9
0x0096C4EF, //08             ADD EAX,8
0x0096C500, //07             ADD EAX,7
0x0096C511, //06             ADD EAX,6
0x0096C522, //05             ADD EAX,5
0x0096C533, //04             ADD EAX,4

0x0096C5AD, //09             ADD EAX,9
0x0096C5BE, //08             ADD EAX,8
0x0096C5CF, //07             ADD EAX,7
0x0096C5E0, //06             ADD EAX,6
0x0096C5F1, //05             ADD EAX,5
0x0096C602, //04             ADD EAX,4
};

// set char 005C4006  /$ 55             PUSH EBP
// item convert 005BE46A

CNewOptions gNewOptions;

CNewOptions::CNewOptions()
{

}

CNewOptions::~CNewOptions()
{

}

BOOL CNewOptions::LoadData()
{
	VMBEGIN

	FILE* File = fopen("Data\\Custom\\ExOptions.bmd","rb");

	if( File == NULL )	{
		return false;
	}

	m_vtOptName.clear();
	m_vtEnchantOpt.clear();
	m_vtExcellentOpt.clear();
	m_vtOptList.clear();

	int i;
	CUSTOM_OPTIONS_EXHEADER pHeader = {0};
	fread(&pHeader,1,sizeof(CUSTOM_OPTIONS_EXHEADER),File);

	XorData((char*)&pHeader,sizeof(pHeader));

	if( pHeader.iOptionCount < 0 ||
		pHeader.iItemCount < 0 ||
		pHeader.iEnchantOptCount < 0 ||
		pHeader.iExcellentOptCount < 0 )
	{
		return 0;
	}

	for( i = 0; i < pHeader.iOptionCount; i++)
	{
		CUSTOM_OPTIONS_NAME pstruct = {0};
		fread(&pstruct, sizeof(pstruct), 1, File);
		XorData((char*)&pstruct,sizeof(pstruct));

		m_vtOptName.insert(std::pair<int,CUSTOM_OPTIONS_NAME>(pstruct.iIndex,pstruct));
	}

	for( i = 0; i < pHeader.iEnchantOptCount; i++)
	{
		CUSTOM_OPTIONS_ENCHANT pstruct = {0};
		fread(&pstruct, sizeof(pstruct), 1, File);
		XorData((char*)&pstruct,sizeof(pstruct));

		m_vtEnchantOpt.insert(std::pair<int,CUSTOM_OPTIONS_ENCHANT>(pstruct.iId,pstruct));
	}

	for( i = 0; i < pHeader.iExcellentOptCount; i++)
	{
		CUSTOM_OPTIONS_EXCELLENT pstruct = {0};
		fread(&pstruct, sizeof(pstruct), 1, File);
		XorData((char*)&pstruct,sizeof(pstruct));

		m_vtExcellentOpt.insert(std::pair<int,CUSTOM_OPTIONS_EXCELLENT>(pstruct.iId,pstruct));
	}

	for( i = 0; i < pHeader.iItemCount; i++)
	{
		CUSTOM_OPTIONS_LIST pstruct = {0};
		fread(&pstruct, sizeof(pstruct), 1, File);
		XorData((char*)&pstruct,sizeof(pstruct));

		m_vtOptList.insert(std::pair<int,CUSTOM_OPTIONS_LIST>(pstruct.Item,pstruct));
	}

	fclose(File);

	VMEND

	return true;
}

LPBYTE CNewOptions::GetExcellentOption(LPBYTE ItemStruct,int OptionID)
{
	LPBYTE pStruct = MainGetExcOpt((LPBYTE)this,ItemStruct,OptionID);

	if( pStruct == NULL )
		return NULL;

	int iItemID = *(DWORD*)(ItemStruct+0x04);

	CUSTOM_OPTIONS_EXCELLENT* pExcStruct = this->ExcellentOptions(iItemID);

	if( pExcStruct != NULL )
	{
		*(DWORD*)(pStruct+0x68) = pExcStruct->iValue[OptionID];
	}

return pStruct;
}

CUSTOM_OPTIONS_EXCELLENT* CNewOptions::ExcellentOptions(int iItemID)
{
	vtOptList::iterator _ItItem = m_vtOptList.find(iItemID);

	if( _ItItem == m_vtOptList.end() )
		return false;

	if( _ItItem->second.ExcellentId == -1 )
		return false;

	vtExcellentOpt::iterator _It = m_vtExcellentOpt.find(_ItItem->second.ExcellentId);

	if( _It == m_vtExcellentOpt.end() )
		return false;

	return &_It->second;
}

CUSTOM_OPTIONS_ENCHANT* CNewOptions::EnchantOptions(int iItemID)
{
	vtOptList::iterator _ItItem = m_vtOptList.find(iItemID);

	if( _ItItem == m_vtOptList.end() )
		return false;

	if( _ItItem->second.EnchantId == -1 )
		return false;

	vtEnchantOpt::iterator _It = m_vtEnchantOpt.find(_ItItem->second.EnchantId);

	if( _It == m_vtEnchantOpt.end() )
		return false;

	return &_It->second;
}

PCHAR TOOLTIP_TEXT = (PCHAR)0x85674A0;
LPDWORD TOOLTIP_TEXT_TYPE = (LPDWORD)0x8567400;

void GetLvl15Option(LPBYTE itemStruct,DWORD& LineCnt1,DWORD& LineCnt2)
{
	int ItemID = *(DWORD*)(itemStruct+4);
	int Level = (*(DWORD*)(itemStruct+8)/8)&15;
	bool bShopItem = (*(BYTE*)(itemStruct+0x21)&192) ? true : false;

	CUSTOM_OPTIONS_ENCHANT* lpOption = gNewOptions.EnchantOptions(ItemID);

	if( lpOption )
	{
		sprintf(&TOOLTIP_TEXT[100*LineCnt1],"\n");
		LineCnt1++;
		LineCnt2++;

		for(int i = 0; i < 6; i++)
		{
			if( lpOption->iIndex1[i] != -1 )
			{
				vtOptName::iterator _It = m_vtOptName.find(lpOption->iIndex1[i]);
	
				if( _It != m_vtOptName.end() )
				{
					sprintf(&TOOLTIP_TEXT[100*LineCnt1],_It->second.szName,lpOption->iValue1[i]);
	
					if( Level  >= i+10 )
						TOOLTIP_TEXT_TYPE[LineCnt1] = 8;
					else
						TOOLTIP_TEXT_TYPE[LineCnt1] = 10;
					LineCnt1++;
					LineCnt2++;
				}
			}
			
			if( lpOption->iIndex2[i] != -1 )
			{
				vtOptName::iterator _It = m_vtOptName.find(lpOption->iIndex2[i]);
	
				if( _It != m_vtOptName.end() )
				{			
					sprintf(&TOOLTIP_TEXT[100*LineCnt1],_It->second.szName,lpOption->iValue2[i]);
					if( Level  >= i+10 )
						TOOLTIP_TEXT_TYPE[LineCnt1] = 8;
					else
						TOOLTIP_TEXT_TYPE[LineCnt1] = 10;
					LineCnt1++;
					LineCnt2++;
				}
			}
		}
	}

	if( bShopItem )
	{
		sprintf(&TOOLTIP_TEXT[100*LineCnt1],"ShopItem");
		TOOLTIP_TEXT_TYPE[LineCnt1] = 3;
		LineCnt1++;
		LineCnt2++;
	}
}

DWORD __GetLvl15Option_MOV = 0x007E19A8;

void __declspec(naked) __GetLvl15Option()
{
	__asm
	{
		LEA EAX,DWORD PTR SS:[EBP-0x6C]
		PUSH EAX
		LEA EAX,DWORD PTR SS:[EBP-0x78]
		PUSH EAX
		MOV EAX,DWORD PTR SS:[EBP+0x10]
		PUSH EAX
		CALL GetLvl15Option
		ADD ESP,12
		MOV EAX,DWORD PTR SS:[EBP+0x10]
		MOVZX EAX,WORD PTR DS:[EAX+4]
		JMP DWORD PTR DS:[__GetLvl15Option_MOV]
	}
}


int CalcWizRise(LPBYTE itemStruct)	{
	int WizRise = 0;

	int ItemID = *(DWORD*)(itemStruct+4);
	LPBYTE itemInfo = ItemGetInfo(itemStruct);
	BYTE Level = (*(DWORD*)(itemStruct+8)/8)&15;
	WizRise = *(BYTE*)(itemInfo+0x248);

	if( *(BYTE*)(itemInfo+0x248) )	{
		if( (*(BYTE*)(itemStruct+0x20)&0x3F) && *(WORD*)(itemInfo+0x232) )	{
			if( ItemID == ITEMGET(5,7))	{
				WizRise += 25;
			}else{
				WizRise += (WizRise*25) / *(WORD*)(itemInfo+0x232) + 5;
			}
		}

		int tmpLevel = (Level>9) ? 9 : Level;
		WizRise += tmpLevel*3;

		switch(Level-9)
		{
		case 6: WizRise += 30;
		case 5: WizRise += 23;
		case 4:	WizRise += 17;
		case 3:	WizRise += 12;
		case 2:	WizRise += 8;
		case 1:	WizRise += 5;
		}

		WizRise /= 2;

		if( ItemID < ITEMGET(2,0) || ItemID >= ITEMGET(3,0) )	{
			WizRise += Level*2;
		}
	}
return WizRise;
}

DWORD __FixWizRise_MOV = 0x007E2AD2;

void __declspec(naked) __FixWizRise()
{
	__asm
	{
		MOV EAX,DWORD PTR SS:[EBP+0x1C]
		PUSH EAX
		CALL CalcWizRise
		ADD ESP,4
		JMP DWORD PTR DS:[__FixWizRise_MOV]
	}
}

DWORD __FixWizRise2_MOV = 0x009C9634;

void __declspec(naked) __FixWizRise2()
{
	__asm
	{
		MOV EAX,DWORD PTR SS:[EBP-0x3F4]
		PUSH EAX
		CALL CalcWizRise
		ADD ESP,4
		JMP DWORD PTR DS:[__FixWizRise2_MOV]
	}
}

#endif