#include "stdafx.h"
#include "main.h"
#include "ItemGlow.h"

pFunctionVOID SetSpecialEffect = (pFunctionVOID)0x005FD969;
pFunctionVOID SetSpecialEffect2 = (pFunctionVOID)0x005EA359;
pFunctionVOID MainItemConvert = (pFunctionVOID)0x09D00AE5;
pFunctionVOID ItemGlow = (pFunctionVOID)0x005E88E7;
pFunctionVOID ItemGlow2 = (pFunctionVOID)0x005E9EBF;
pFunctionVOID ObjectPreview = (pFunctionVOID)0x0058267E;
pGetlpItem GetlpItem = (pGetlpItem)0x00A204EC;
pItemGetInfo ItemGetInfo = (pItemGetInfo)0x0096B894;
pFunctionDWORD GetCharsetClass = (pFunctionDWORD)0x0043734E;
pCharSetGetStruct GetCharset = (pCharSetGetStruct)0x0097CE06;
pFunctionBYTE MainGetExcText = (pFunctionBYTE)0x005B9E96;
pFunctionBYTE MainItemSpecial = (pFunctionBYTE)0x005BD8A3;

pMAIN_00544D37 MAIN_00544D37 = (pMAIN_00544D37)(0x00544D37);
pMAIN_00547964 MAIN_00547964 = (pMAIN_00547964)(0x00547964);
pMAIN_00543627 MAIN_00543627 = (pMAIN_00543627)0x00543627;
pMAIN_0077C1A8 MAIN_0077C1A8 = (pMAIN_0077C1A8)0x0077C1A8;
pFunctionVOID MAIN_0A02BE90 = (pFunctionVOID)0x0A02BE90;
pMAIN_00752200 MAIN_00752200 = (pMAIN_00752200)0x00752200;

void MySetSpecialEffect(LPBYTE arg1,DWORD arg2,float* arg3,LPBYTE arg4,DWORD arg5,DWORD arg6,LPBYTE arg7,LPBYTE arg8,DWORD arg9,DWORD arg10,DWORD arg11)
{
#if (__CUSTOM_WINGS__==1)
	if( Is4thWing(arg2-ITEM_START) )	{
		LPBYTE pItemClass = GetlpItem(*(DWORD*)(0x11F92BC),arg2);

		*(float*)(pItemClass+0x48) = *arg3;
		*(float*)(pItemClass+0x4C) = *arg3;
		*(float*)(pItemClass+0x50) = *arg3;

		MySetSpecialEffect2(pItemClass,arg1,arg2,arg4,arg9,arg10,arg11);

		if( arg2 == ITEMGET(12,440)+ITEM_START )	{

			float fRand = rand()%100/100.0f;
			float fCOLOR[3] = {0.1f,0.05f,0};
			float fCOLOR2[3] = {0.4f,0.0f,0.0f};
			float fCOLOR3[3] = {0.3f,0.0f,0.0f};
			float fCOLOR4[3] = {0.2f,0.1f,0.0f};

			float fBUFF1[3] = {0.0f};
			float fBUFF2[3] = {0.0f};

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*1,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D02,&fBUFF2[0],6.0f+fRand*0.5f,&fCOLOR2[0],arg1,0,0);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*5,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_00752200(0x7D49,&fBUFF2[0],(LPBYTE)(arg1+0x108),&fCOLOR4[0],0,1.0f,0);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*21,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_00752200(0x7D49,&fBUFF2[0],(LPBYTE)(arg1+0x108),&fCOLOR4[0],0,1.0f,0);

			for(int i = 0; i <= 30; i++)
			{
				MAIN_00543627(pItemClass,ITEM_CONST3+0x30*i,&fBUFF1[0],&fBUFF2[0],1);
				MAIN_0077C1A8(0x7D02,&fBUFF2[0],2.0f+fRand*0.5f,&fCOLOR3[0],arg1,0,0);	
			}

			for(int i = 26; i <= 30; i++)
			{
				MAIN_00543627(pItemClass,ITEM_CONST3+0x30*i,&fBUFF1[0],&fBUFF2[0],1);
				MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR[0],1,arg1,-1,0,0,0,0,-1);
			}
			
			for(int i = 10; i <= 15; i++)
			{
				MAIN_00543627(pItemClass,ITEM_CONST3+0x30*i,&fBUFF1[0],&fBUFF2[0],1);
				MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR[0],1,arg1,-1,0,0,0,0,-1);
			}
		}
		else if( arg2 == ITEMGET(12,441)+ITEM_START )
		{
			float fRand = rand()%100/100.0f;
			float fCOLOR1[3] = {1.0f,0.0f,0.0f};
			float fCOLOR2[3] = {0.5f,0.0f,0.0f};
			float fCOLOR3[3] = {0.4f,0.0f,0.3f};

			float fBUFF1[3] = {0.0f};
			float fBUFF2[3] = {0.0f};
			
			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*4,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D83,&fBUFF2[0],3.5f,&fCOLOR3[0],arg1,0,0);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*7,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D83,&fBUFF2[0],3.5f,&fCOLOR3[0],arg1,0,0);
			
			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*12,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D83,&fBUFF2[0],2.5f,&fCOLOR3[0],arg1,0,0);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*6,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D83,&fBUFF2[0],2.5f,&fCOLOR3[0],arg1,0,0);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*9,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D83,&fBUFF2[0],1.0f+fRand*1.5f,&fCOLOR3[0],arg1,0,0);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*18,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D83,&fBUFF2[0],1.0f+fRand*1.5f,&fCOLOR3[0],arg1,0,0);

			if( rand()%15 > 12 )
			{
				MAIN_00543627(pItemClass,ITEM_CONST3+0x30*9,&fBUFF1[0],&fBUFF2[0],1);
				MAIN_00752200(0x7D71,&fBUFF2[0],(LPBYTE)(arg1+0x118),&fCOLOR1[0],4,1.0f,0);

				MAIN_00543627(pItemClass,ITEM_CONST3+0x30*18,&fBUFF1[0],&fBUFF2[0],1);
				MAIN_00752200(0x7D71,&fBUFF2[0],(LPBYTE)(arg1+0x118),&fCOLOR1[0],4,1.0f,0);
			}
			
			if( rand()%15 > 12 )
			{
				MAIN_00543627(pItemClass,ITEM_CONST3+0x30*6,&fBUFF1[0],&fBUFF2[0],1);
				MAIN_00752200(0x7D71,&fBUFF2[0],(LPBYTE)(arg1+0x118),&fCOLOR1[0],4,1.0f,0);

				MAIN_00543627(pItemClass,ITEM_CONST3+0x30*12,&fBUFF1[0],&fBUFF2[0],1);
				MAIN_00752200(0x7D71,&fBUFF2[0],(LPBYTE)(arg1+0x118),&fCOLOR1[0],4,1.0f,0);
			}
		}
		else if( arg2 == ITEMGET(12,442)+ITEM_START )
		{
			float fRand = rand()%100/100.0f;
			float fCOLOR1[3] = {0.0f,0.0f,0.5f};
			float fCOLOR2[3] = {0.0f,0.0f,0.7f};
			float fCOLOR3[3] = {0.2f,0.0f,0.6f};

			float fBUFF1[3] = {0.0f};
			float fBUFF2[3] = {0.0f};

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*0,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D02,&fBUFF2[0],6.0f+fRand,&fCOLOR1[0],arg1,0,0);

			if( rand()%15 > 12 )
			{
				MAIN_00543627(pItemClass,ITEM_CONST3+0x30*7,&fBUFF1[0],&fBUFF2[0],1);
				MAIN_00752200(0x7D83,&fBUFF2[0],(LPBYTE)(arg1+0x108),&fCOLOR2[0],4,1.0f,0);

				MAIN_00543627(pItemClass,ITEM_CONST3+0x30*21,&fBUFF1[0],&fBUFF2[0],1);
				MAIN_00752200(0x7D83,&fBUFF2[0],(LPBYTE)(arg1+0x108),&fCOLOR2[0],4,1.0f,0);
			}

			if( rand()%3 == 0 )
			{
				BYTE Pos[4] = {7,9,11,13};
				for(int i = 0; i < 4; i++)
				{
					MAIN_00543627(pItemClass,ITEM_CONST3+0x30*Pos[i],&fBUFF1[0],&fBUFF2[0],1);
					MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR3[0],1,arg1,-1,0,0,0,0,-1);
				}

				BYTE Pos2[4] = {20,22,24,26};
				for(int i = 0; i < 4; i++)
				{
					MAIN_00543627(pItemClass,ITEM_CONST3+0x30*Pos2[i],&fBUFF1[0],&fBUFF2[0],1);
					MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR3[0],1,arg1,-1,0,0,0,0,-1);
				}
			}

			for(int i = 7; i <= 13; i++)
			{
				MAIN_00543627(pItemClass,ITEM_CONST3+0x30*i,&fBUFF1[0],&fBUFF2[0],1);
				MAIN_0077C1A8(0x7D02,&fBUFF2[0],2.0f+fRand*0.5f,&fCOLOR3[0],arg1,0,0);	
			}

			for(int i = 21; i <= 27; i++)
			{
				MAIN_00543627(pItemClass,ITEM_CONST3+0x30*i,&fBUFF1[0],&fBUFF2[0],1);
				MAIN_0077C1A8(0x7D02,&fBUFF2[0],2.0f+fRand*0.5f,&fCOLOR3[0],arg1,0,0);	
			}
		}
		else if( arg2 == ITEMGET(12,443)+ITEM_START )
		{
			float fRand = rand()%100/100.0f;

			float fCOLOR1[3] = {0.2f,0.0f,0.0f};
			float fCOLOR2[3] = {0.1f,0.0f,0.2f};
			float fCOLOR3[3] = {0.4f,0.4f,0.0f};

			float fBUFF1[3] = {0.0f};
			float fBUFF2[3] = {0.0f};

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*2,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D02,&fBUFF2[0],1.5f+fRand,&fCOLOR3[0],arg1,0,0);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*6,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D02,&fBUFF2[0],1.5f+fRand,&fCOLOR3[0],arg1,0,0);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*1,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D03,&fBUFF2[0],4.5f+fRand,&fCOLOR2[0],arg1,0,0);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*5,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D03,&fBUFF2[0],4.5f+fRand,&fCOLOR2[0],arg1,0,0);

			for(int i = 0; i <= 9; i++)
			{
				if( i%2 == 0 )
				{
					MAIN_00543627(pItemClass,ITEM_CONST3+0x30*i,&fBUFF1[0],&fBUFF2[0],1);
					MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR2[0],1,arg1,-1,0,0,0,0,-1);
				}
			}

			if( rand()%100 < 40 )
			{
				for(int i = 0; i <= 9; i++)
				{
					MAIN_00543627(pItemClass,ITEM_CONST3+0x30*i,&fBUFF1[0],&fBUFF2[0],1);
					MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR1[0],1,arg1,-1,0,0,0,0,-1);
				}
			}
		}
		else if( arg2 == ITEMGET(12,444)+ITEM_START )
		{

			float fRand = rand()%100/100.0f;

			float fCOLOR1[3] = {0.3f,0.1f,0.0f};
			float fCOLOR2[3] = {0.3f,0.1f,0.0f};
			float fCOLOR3[3] = {0.2f,0.0f,0.0f};
			float fCOLOR4[3] = {0.8f,0.0f,0.0f};

			float fBUFF1[3] = {0.0f};
			float fBUFF2[3] = {0.0f};

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*6,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D03,&fBUFF2[0],6.0f+fRand,&fCOLOR2[0],arg1,0,0);
			MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR1[0],1,arg1,-1,0,0,0,0,-1);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*14,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D03,&fBUFF2[0],6.0f+fRand,&fCOLOR2[0],arg1,0,0);
			MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR1[0],1,arg1,-1,0,0,0,0,-1);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*8,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D03,&fBUFF2[0],2.0f+fRand,&fCOLOR3[0],arg1,0,0);
			MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR1[0],1,arg1,-1,0,0,0,0,-1);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*16,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D03,&fBUFF2[0],2.0f+fRand,&fCOLOR3[0],arg1,0,0);
			MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR1[0],1,arg1,-1,0,0,0,0,-1);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*3,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D02,&fBUFF2[0],1.0f+fRand,&fCOLOR1[0],arg1,0,0);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*11,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D02,&fBUFF2[0],1.0f+fRand,&fCOLOR1[0],arg1,0,0);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*4,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D03,&fBUFF2[0],0.5f+fRand,&fCOLOR4[0],arg1,0,0);
	
			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*12,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D03,&fBUFF2[0],0.5f+fRand,&fCOLOR4[0],arg1,0,0);
		}
		else if( arg2 == ITEMGET(12,445)+ITEM_START)
		{
			float fRand = rand()%100/100.0f;
			float fCOLOR1[3] = {0.8f,0.0f,0.2f};
			float fCOLOR2[3] = {0.2f,0.0f,0.2f};
			float fCOLOR3[3] = {0.4f,0.0f,0.1f};
			float fBUFF1[3] = {0.0f};
			float fBUFF2[3] = {0.0f};

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*10,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D03,&fBUFF2[0],6.0f+fRand,&fCOLOR2[0],arg1,0,0);
			MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR3[0],1,arg1,-1,0,0,0,0,-1);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*23,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D03,&fBUFF2[0],6.0f+fRand,&fCOLOR2[0],arg1,0,0);
			MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR3[0],1,arg1,-1,0,0,0,0,-1);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*8,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D03,&fBUFF2[0],4.5f+fRand,&fCOLOR2[0],arg1,0,0);

			MAIN_00543627(pItemClass,ITEM_CONST3+0x30*21,&fBUFF1[0],&fBUFF2[0],1);
			MAIN_0077C1A8(0x7D03,&fBUFF2[0],4.5f+fRand,&fCOLOR2[0],arg1,0,0);

			if( rand()%15 > 12 )
			{
				MAIN_00543627(pItemClass,ITEM_CONST3+0x30*8,&fBUFF1[0],&fBUFF2[0],1);
				MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR3[0],1,arg1,-1,0,0,0,0,-1);

				MAIN_00543627(pItemClass,ITEM_CONST3+0x30*21,&fBUFF1[0],&fBUFF2[0],1);
				MAIN_0A02BE90(0x185,&fBUFF2[0],arg1+0x118,&fCOLOR3[0],1,arg1,-1,0,0,0,0,-1);
			}
		}
		return;
	}
#endif

	SetSpecialEffect(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11);
}

void MySetSpecialEffect2(LPBYTE arg1,LPBYTE arg2,DWORD arg3,LPBYTE arg4,DWORD arg5,DWORD arg6,DWORD arg7)
{
#if (__CUSTOM_WINGS__ == 1)
	if( arg3 == ITEMGET(12,440)+ITEM_START )
	{
		MAIN_00547964(arg1,2,*(float*)(arg2+0xAC),*(DWORD*)(arg2+0x58),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),*(float*)(arg2+0x80),*(DWORD*)(arg2+0x4C),-1);
		return;
	}
	else if( arg3 == ITEMGET(12,441)+ITEM_START )
	{
		*(float*)(arg1+0x48) = 0.4f;
		*(float*)(arg1+0x4C) = 0.4f;
		*(float*)(arg1+0x50) = 0.4f;

		MAIN_00547964(arg1,0x42,*(float*)(arg2+0xAC),*(DWORD*)(arg2+0x58),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),*(float*)(arg2+0x80),*(DWORD*)(arg2+0x4C),-1);
		return;
	}
	else if( arg3 == ITEMGET(12,442)+ITEM_START )
	{
		MAIN_00547964(arg1,2,*(float*)(arg2+0xAC),*(DWORD*)(arg2+0x58),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),*(float*)(arg2+0x80),*(DWORD*)(arg2+0x4C),-1);
		return;
	}
	else if( arg3 == ITEMGET(12,443)+ITEM_START )
	{
		*(float*)(arg1+0x48) = 1.0f;
		*(float*)(arg1+0x4C) = 1.0f;
		*(float*)(arg1+0x50) = 1.0f;

		MAIN_00547964(arg1,0x42,*(float*)(arg2+0xAC),*(DWORD*)(arg2+0x58),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),*(float*)(arg2+0x80),*(DWORD*)(arg2+0x4C),-1);
		return;
	}
	else if( arg3 == ITEMGET(12,444)+ITEM_START)
	{
		MAIN_00547964(arg1,2,*(float*)(arg2+0xAC),*(DWORD*)(arg2+0x58),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),*(float*)(arg2+0x80),*(DWORD*)(arg2+0x4C),-1);
		return;
	}
	else if( arg3 == ITEMGET(12,445)+ITEM_START)
	{
		MAIN_00547964(arg1,2,*(float*)(arg2+0xAC),*(DWORD*)(arg2+0x58),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),*(float*)(arg2+0x80),*(DWORD*)(arg2+0x4C),-1);
		return;
	}
#endif

#if (__ONLYMU__)

	if( (arg3 == ITEMGET(7,154)+ITEM_START) && !(arg5&0x40000) )
	{
		if( *(BYTE*)(arg1+0x99) == 1 )
		{
			glColor3fv((const GLfloat*)(arg1+0x48));
			MAIN_00544D37(arg1,2,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),-1);
		}
		else{
			MAIN_00547964(arg1,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),*(DWORD*)(arg2+0x4C),-1);
		}
		return;
	}
	else if( (arg3 == ITEMGET(7,156)+ITEM_START) && !(arg5&0x40000) )
	{
		if( *(BYTE*)(arg1+0x99) == 1 )
		{
			glColor3fv((const GLfloat*)(arg1+0x48));
			MAIN_00544D37(arg1,0,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),-1);
			MAIN_00544D37(arg1,1,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),-1);
		}
		else{
			MAIN_00547964(arg1,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),*(DWORD*)(arg2+0x4C),-1);
		}
		return;
	}
	else if( arg3 == (ITEMGET(8,154)+ITEM_START) && !(arg5&0x40000) )
	{
		if( *(BYTE*)(arg1+0x99) == 1)
		{
			glColor3fv((const GLfloat*)(arg1+0x48));
			int id = 0x7766;

			MAIN_00544D37(arg1,0,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),id);
			for(int i = 1; i < *(short*)(arg1+0x24); i++)
			{
				MAIN_00544D37(arg1,i,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),-1);
			}
		}
		else
		{
			MAIN_00547964(arg1,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),*(DWORD*)(arg2+0x4C),-1);
		}
		return;
	}
	else if( arg3 == (ITEMGET(8,155)+ITEM_START) && !(arg5&0x40000) )
	{
		if( *(BYTE*)(arg1+0x99) == 1)
		{
			MAIN_00544D37(arg1,0,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),-1);
			MAIN_00544D37(arg1,1,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),-1);
		}
		else
		{
			MAIN_00547964(arg1,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),*(DWORD*)(arg2+0x4C),-1);
		}
		return;
	}
	else if( arg3 == (ITEMGET(9,154)+ITEM_START) && !(arg5&0x40000) )
	{
		if( *(BYTE*)(arg1+0x99) == 1)
		{
			glColor3fv((const GLfloat*)(arg1+0x48));
			int id = 0x776F;

			MAIN_00544D37(arg1,0,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),id);
			for(int i = 1; i < *(short*)(arg1+0x24); i++)
			{
				MAIN_00544D37(arg1,i,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),-1);
			}
		}
		else
		{
			MAIN_00547964(arg1,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),*(DWORD*)(arg2+0x4C),-1);
		}
		return;
	}
	else if( arg3 == (ITEMGET(9,155)+ITEM_START) && !(arg5&0x40000) )
	{
		if( *(BYTE*)(arg1+0x99) == 1)
		{
			MAIN_00544D37(arg1,0,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),-1);
			MAIN_00544D37(arg1,1,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),-1);
		}
		else
		{
			MAIN_00547964(arg1,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),*(DWORD*)(arg2+0x4C),-1);
		}
		return;
	}
	else if( arg3 == (ITEMGET(10,154)+ITEM_START) && !(arg5&0x40000) )
	{
		if( *(BYTE*)(arg1+0x99) == 1)
		{
			glColor3fv((const GLfloat*)(arg1+0x48));
			MAIN_00544D37(arg1,0,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),-1);
		}
		else
		{
			MAIN_00547964(arg1,2,*(float*)(arg2+0xA8),*(DWORD*)(arg2+0x54),*(float*)(arg2+0x74),*(float*)(arg2+0x78),*(float*)(arg2+0x7C),*(DWORD*)(arg2+0x4C),-1);
		}
		return;
	}
#endif

	SetSpecialEffect2(arg1,arg2,arg3,arg4,arg5);
}

void ItemConvertEx(LPBYTE itemStruct,DWORD Op1,DWORD Op2,DWORD Op3)
{
	MainItemConvert(itemStruct,Op1,Op2,Op3);

	if( *(short*)(itemStruct + 0x2C) > 400 ) //Excellent + Ancient fix
	{
		*(short*)(itemStruct + 0x2C) = 400;
	}

#if (__CUSTOM_WINGS__ ==1)

	if( Is4thWing(*(DWORD*)(itemStruct+4)) )	{
		DWORD ItemID = *(DWORD*)(itemStruct+4);
		LPBYTE itemInfo = ItemGetInfo(itemStruct);
		
		BYTE ExOption = Op2&0x3F;
		BYTE Level = (Op1/8)&15;
		BYTE Option = (Op1&3)+ ((Op2&64)/64)* 4;

		LPWORD lpOption = (LPWORD)(itemStruct+0x30);
		LPBYTE lpOptionValue = (LPBYTE)(itemStruct+0x40);
		*(WORD*)(itemStruct+0x16) = *(BYTE*)(itemInfo+0x240);
		*(WORD*)(itemStruct+0x2C) = 330;

		//*(BYTE*)(itemStruct+0x20) = 0;

		if( *(WORD*)(itemStruct+0x16) )	{
			int tmpLevel = (Level>9) ? 9 : Level;
			*(WORD*)(itemStruct+0x16) += tmpLevel*5;

			switch(Level-9)	{
#if (__NEWOPTION__==1)
			case 6: *(WORD*)(itemStruct+0x16) += 30;
			case 5: *(WORD*)(itemStruct+0x16) += 23;
			case 4: *(WORD*)(itemStruct+0x16) += 17;
			case 3: *(WORD*)(itemStruct+0x16) += 12;
			case 2: *(WORD*)(itemStruct+0x16) += 8;
			case 1: *(WORD*)(itemStruct+0x16) += 5;
#else
			case 6: *(WORD*)(itemStruct+0x16) += 9;
			case 5: *(WORD*)(itemStruct+0x16) += 8;
			case 4: *(WORD*)(itemStruct+0x16) += 7;
			case 3: *(WORD*)(itemStruct+0x16) += 6;
			case 2: *(WORD*)(itemStruct+0x16) += 5;
			case 1: *(WORD*)(itemStruct+0x16) += 4;
#endif
			}
		}

		if( ItemID == ITEMGET(12,440)  || ItemID ==ITEMGET(12,441) || ItemID == ITEMGET(12,444) )
		{
			if( Option != 0 )
			{
				lpOption[*(BYTE*)(itemStruct+0x2E)] = 6;
				lpOptionValue[*(BYTE*)(itemStruct+0x2E)] = Option;
				
				if( (ExOption&0x10) )
				{
					lpOption[*(BYTE*)(itemStruct+0x2E)] = 0;
					lpOptionValue[*(BYTE*)(itemStruct+0x2E)] = Option*4;
				}
				else if( (ExOption&0x20) )
				{
					lpOption[*(BYTE*)(itemStruct+0x2E)] = 4;
					lpOptionValue[*(BYTE*)(itemStruct+0x2E)] = Option*4;
				}
				*(BYTE*)(itemStruct+0x2E) += 1;

			}
		}
		else if( ItemID == ITEMGET(12,442) )
		{
			if( Option != 0 )
			{
				lpOption[*(BYTE*)(itemStruct+0x2E)] = 6;
				lpOptionValue[*(BYTE*)(itemStruct+0x2E)] = Option;

				if( (ExOption&0x10) )
				{
					lpOption[*(BYTE*)(itemStruct+0x2E)] = 1;
					lpOptionValue[*(BYTE*)(itemStruct+0x2E)] = Option*4;
				}
				else if( (ExOption&0x20) )
				{
					lpOption[*(BYTE*)(itemStruct+0x2E)] = 4;
					lpOptionValue[*(BYTE*)(itemStruct+0x2E)] = Option*4;
				}

				*(BYTE*)(itemStruct+0x2E) += 1;
			}
		}
		else if( ItemID == ITEMGET(12,443) )
		{
			if( Option != 0 )
			{
				lpOption[*(BYTE*)(itemStruct+0x2E)] = 6;
				lpOptionValue[*(BYTE*)(itemStruct+0x2E)] = Option;

				if( (ExOption&0x10) )
				{
					lpOption[*(BYTE*)(itemStruct+0x2E)] = 0;
					lpOptionValue[*(BYTE*)(itemStruct+0x2E)] = Option*4;
				}
				else if( (ExOption&0x20) )
				{
					lpOption[*(BYTE*)(itemStruct+0x2E)] = 1;
					lpOptionValue[*(BYTE*)(itemStruct+0x2E)] = Option*4;
				}

				*(BYTE*)(itemStruct+0x2E) += 1;
			}
		}
		else if( ItemID == ITEMGET(12,445) )
		{
			if( Option != 0 )
			{
				lpOption[*(BYTE*)(itemStruct+0x2E)] = 6;
				lpOptionValue[*(BYTE*)(itemStruct+0x2E)] = Option;

				if( (ExOption&0x10) )
				{
					lpOption[*(BYTE*)(itemStruct+0x2E)] = 1;
					lpOptionValue[*(BYTE*)(itemStruct+0x2E)] = Option*4;
				}
				else if( (ExOption&0x20) )
				{
					lpOption[*(BYTE*)(itemStruct+0x2E)] = 2;
					lpOptionValue[*(BYTE*)(itemStruct+0x2E)] = Option*4;
				}

				*(BYTE*)(itemStruct+0x2E) += 1;
			}
		}
	}
#endif
}

BYTE IsItemSpecial(LPBYTE Item)
{
	if( Item == NULL )
		return false;

	int ItemID = *(DWORD*)(Item+4);

	switch( ItemID )
	{
	default:
		return MainItemSpecial(Item);
		break;
	}
}

void myGlow(DWORD ResourceId, DWORD uk1, DWORD uk2, FRGB *cl, BYTE BalrogMode)
{
	if (!g_ItemGlow.setColor(ResourceId - MU_ItemObjectOffset, cl)) {
		ItemGlow(ResourceId, uk1, uk2, cl, BalrogMode);
	}
	
}

void myGlow2(DWORD ResourceId, DWORD uk1, DWORD uk2, FRGB *cl, BYTE BalrogMode)
{
	if (!g_ItemGlow.setColor(ResourceId - MU_ItemObjectOffset, cl)) {
		ItemGlow2(ResourceId, uk1, uk2, cl, BalrogMode);
	}
	
}

BOOL SetItemGlow(int itemID,FRGB* cl)
{
	return false;
}

DWORD __FixExchangeSymbolPrice_MOV = 0x0058EC29;
DWORD __FIxExchangeSymbolPrice_MOVOK = 0x0058EBFE;
DWORD __FixExchangeSymbolPrice_END = 0x0058F897;

void __declspec( naked )__FixExchangeSymbolPrice()
{
	__asm
	{
		MOV EAX,DWORD PTR SS:[EBP+0x8]
		CMP DWORD PTR DS:[EAX+0x4],0x1C64
		JNZ move
		MOV EAX, DWORD PTR DS:[EAX+0x08]
		SAR EAX, 3
		AND EAX, 15
		CMP EAX, 1
		JE level1
		CMP EAX, 2
		JE level2
		CMP EAX, 3
		JE level3
		JMP DWORD PTR DS:[__FIxExchangeSymbolPrice_MOVOK];

level1:
		MOV DWORD PTR SS:[EBP-0x10],100000000
		AND DWORD PTR SS:[EBP-0xC],0
		JMP DWORD PTR DS:[__FixExchangeSymbolPrice_END]
level2:
		MOV DWORD PTR SS:[EBP-0x10],500000000
		AND DWORD PTR SS:[EBP-0xC],0
		JMP DWORD PTR DS:[__FixExchangeSymbolPrice_END]
level3:
		MOV DWORD PTR SS:[EBP-0x10],1000000000
		AND DWORD PTR SS:[EBP-0xC],0
		JMP DWORD PTR DS:[__FixExchangeSymbolPrice_END]
move:
		JMP DWORD PTR DS:[__FixExchangeSymbolPrice_MOV]
	}
}

DWORD _MySetSetEffect_MOV = 0x0058C29C;
DWORD _MySetSetEffect_MOVOK = 0x0058C2C9;

void __declspec( naked ) _MySetSetEffect()
{
	__asm
	{
		MOV EDX,DWORD PTR SS:[EBP+0x08]
		MOVSX EAX,WORD PTR DS:[EDX+0x1AC]
		CMP EAX, MU_GetItemObject(11, 150)
		JL move
		CMP EAX, MU_GetItemObject(11, 166)
		JG move
		CMP DWORD PTR DS:[0x7EEC384],9
		JLE move
		MOV ECX,DWORD PTR SS:[EBP+0x0C]
		LEA EDX,DWORD PTR DS:[ECX+0xAC]
		PUSH EDX
		PUSH EAX
		CALL SetItemGlow
		ADD ESP,8
		JMP DWORD PTR DS:[_MySetSetEffect_MOVOK]
move:
		MOV EDX,DWORD PTR SS:[EBP+0x08]
		MOVSX EAX,WORD PTR DS:[EDX+0x1AC]
		JMP DWORD PTR DS:[_MySetSetEffect_MOV]
	}
}

#if (__CUSTOM_WINGS__ == 1)

BOOL Is4thWing(DWORD ItemID)
{
	switch(ItemID)
	{
	case ITEMGET(12,440):
	case ITEMGET(12,441):
	case ITEMGET(12,442):
	case ITEMGET(12,443):
	case ITEMGET(12,444):
	case ITEMGET(12,445):
		return TRUE;
	default:
		return FALSE;
	}
}

void MyObjectPreview(DWORD arg1,LPBYTE arg2,LPBYTE arg3,LPBYTE arg4)
{
	ObjectPreview(arg1,arg2,arg3,arg4);

#if (__CUSTOM_WINGS__ == 1)
	LPBYTE lpItem = NULL;

	if( arg3 == NULL )
	{
		lpItem = GetCharset((LPBYTE)GetCharsetClass(),arg1);
	}
	else
	{
		lpItem = arg3;
	}

	if( arg2[16]&16 )
	{
		*(WORD*)(lpItem+0x218) = ITEM_START+ITEMGET(12,440)+arg2[16]%16;
	}
#endif
}

DWORD __Fix4thWingsDisplay_MOV = 0x007E3272;

void __declspec( naked ) __Fix4thWingsDisplay()	{
	__asm
	{
		MOV EAX,DWORD PTR SS:[EBP+0x1C]
		PUSH DWORD PTR DS:[EAX+4]
		CALL Is4thWing
		ADD ESP,4
		TEST EAX,EAX
		JE original
		MOV EAX,DWORD PTR SS:[EBP+0x1C]
		CMP DWORD PTR DS:[EAX+4],ITEMGET(12,444)
		JNZ all
		MOV EAX,DWORD PTR SS:[EBP+0x20]
		IMUL EAX,EAX,3
		ADD EAX,30
		JMP move
all:
		MOV EAX,DWORD PTR SS:[EBP+0x20]
		IMUL EAX,EAX,3
		ADD EAX,39
move:
		JMP DWORD PTR DS:[__Fix4thWingsDisplay_MOV]
original:
		MOV EAX,DWORD PTR SS:[EBP+0x20]
		LEA EAX,DWORD PTR DS:[EAX+EAX+39]
		JMP DWORD PTR DS:[__Fix4thWingsDisplay_MOV]
	}
}

DWORD __Fix4thWingsPos_MOVOK = 0x005C66DB;
DWORD __Fix4thWingsPos_MOV = 0x005C670C;

void __declspec(naked) __Fix4thWingsPos()
{
	__asm
	{
		CMP DWORD PTR SS:[EBP+8],0x1CF3
		JE move
		CMP DWORD PTR SS:[EBP+8],(ITEMGET(12,441)+ITEM_START)
		JE move
		CMP DWORD PTR SS:[EBP+8],(ITEMGET(12,442)+ITEM_START)
		JE move
		CMP DWORD PTR SS:[EBP+8],(ITEMGET(12,445)+ITEM_START)
		JE move
		JMP DWORD PTR DS:[__Fix4thWingsPos_MOV]
move:
		JMP DWORD PTR DS:[__Fix4thWingsPos_MOVOK]
	}
}

DWORD __Fix4thWingsPrice1_MOVOK = 0x0058D661;
DWORD __Fix4thWingsPrice1_MOV = 0x0058D647;

void __declspec(naked) __Fix4thWingsPrice1()
{
	__asm
	{
		PUSH DWORD PTR DS:[EAX+4]
		CALL Is4thWing
		ADD ESP,4
		TEST EAX,EAX
		JE move
		JMP DWORD PTR DS:[__Fix4thWingsPrice1_MOVOK]
move:
		MOV EAX,DWORD PTR SS:[EBP+0x08]
		CMP DWORD PTR DS:[EAX+0x04],0x1832
		JMP DWORD PTR DS:[__Fix4thWingsPrice1_MOV]
	}
}

DWORD __Fix4thWingsPrice2_MOVOK = 0x0058D866;
DWORD __Fix4thWingsPrice2_MOV = 0x0058D84C;

void __declspec(naked) __Fix4thWingsPrice2()
{
	__asm
	{
		PUSH DWORD PTR DS:[EAX+4]
		CALL Is4thWing
		ADD ESP,4
		TEST EAX,EAX
		JE move
		JMP DWORD PTR DS:[__Fix4thWingsPrice2_MOVOK]
move:
		MOV EAX,DWORD PTR SS:[EBP+0x08]
		CMP DWORD PTR DS:[EAX+0x04],0x1832
		JMP DWORD PTR DS:[__Fix4thWingsPrice2_MOV]
	}
}

DWORD __Fix4thWingsPrice3_MOVOK = 0x0058DA43;
DWORD __Fix4thWingsPrice3_MOV = 0x0058DA15;

void __declspec(naked) __Fix4thWingsPrice3()
{
	__asm
	{
		PUSH DWORD PTR DS:[EAX+4]
		CALL Is4thWing
		ADD ESP,4
		TEST EAX,EAX
		JE move
		JMP DWORD PTR DS:[__Fix4thWingsPrice3_MOVOK]
move:
		MOV EAX,DWORD PTR SS:[EBP+0x08]
		CMP DWORD PTR DS:[EAX+0x04],0x1831
		JMP DWORD PTR DS:[__Fix4thWingsPrice3_MOV]
	}
}

DWORD __Fix4thWingsHPOption_MOV = 0x0096D8FD;
DWORD __Fix4thWingsHPOption_MOVOK = 0x0096D8EB;

void __declspec(naked) __Fix4thWingsHPOption()
{
	__asm
	{
		CMP BYTE PTR SS:[EBP-0x80],7
		JE newopt
		JMP DWORD PTR DS:[__Fix4thWingsHPOption_MOV]
newopt:
		MOV EAX,DWORD PTR SS:[EBP-0x7C]
		MOV EAX,DWORD PTR DS:[EAX+8]
		SAR EAX,0x3
		AND EAX,0x0F
		ADD EAX,1
		MOV DWORD PTR SS:[EBP-0x78],EAX
		MOV EAX,DWORD PTR SS:[EBP-0x78]
		MOV ECX,DWORD PTR SS:[EBP-0x74]
		MOV ECX,DWORD PTR DS:[ECX+0x68]
		IMUL EAX,ECX
		JMP DWORD PTR DS:[__Fix4thWingsHPOption_MOVOK]
	}
}

#endif

DWORD __FixExcellentSocket_MOVOK = 0x0096D57F;
DWORD __FixExcellentSocket_MOV = 0x0096D609;

void __declspec(naked) __FixExcellentSocket()
{
	__asm
	{
		MOVZX EAX, AL
		TEST EAX, EAX
		JNZ ok
		PUSH 2
		MOV ECX, DWORD PTR SS:[EBP-0x20]
		MOV EAX, 0x00512F2B
		CALL EAX
		MOVZX EAX,AL
		TEST EAX,EAX
		JNZ ok
		JMP DWORD PTR DS:[__FixExcellentSocket_MOV]
ok:
		JMP DWORD PTR DS:[__FixExcellentSocket_MOVOK]
	}
}