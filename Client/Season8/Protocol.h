#pragma once

struct PMSG_CHECK_ANTICHEAT
{
	BYTE c;
	BYTE size;
	BYTE headcode;
	BYTE type;
#if (CHEATGUARD_DOWNGRADE == 0)
	WORD IsSpeedEditor;
#endif
	WORD wWord1;
	WORD wAttackSpeed1;
	WORD wWord2;
	WORD wAttackSpeed2;
};

class CStreamPacketEngine_Client
{

public:

	CStreamPacketEngine_Client()	// Good
	{
		this->Clear();
	};

	virtual ~CStreamPacketEngine_Client()	// Good
	{
		return;
	};

	void Clear()	// Good 
	{
		this->m_wSize = 0;
	};

protected:

	void XorData(int iStart, int iEnd, int iDir)	// Good
	{
		if( iStart >= 2048 ||
			iEnd >= 2048 )
			return;

		unsigned char byXorFilter[32];

		byXorFilter[0] = 0xAB;
		byXorFilter[1] = 0x11;
		byXorFilter[2] = 0xCD;
		byXorFilter[3] = 0xFE;
		byXorFilter[4] = 0x18;
		byXorFilter[5] = 0x23;
		byXorFilter[6] = 0xC5;
		byXorFilter[7] = 0xA3;
		byXorFilter[8] = 0xCA;
		byXorFilter[9] = 0x33;
		byXorFilter[10] = 0xC1;
		byXorFilter[11] = 0xCC;
		byXorFilter[12] = 0x66;
		byXorFilter[13] = 0x67;
		byXorFilter[14] = 0x21;
		byXorFilter[15] = 0xF3;
		byXorFilter[16] = 0x32;
		byXorFilter[17] = 0x12;
		byXorFilter[18] = 0x15;
		byXorFilter[19] = 0x35;
		byXorFilter[20] = 0x29;
		byXorFilter[21] = 0xFF;
		byXorFilter[22] = 0xFE;
		byXorFilter[23] = 0x1D;
		byXorFilter[24] = 0x44;
		byXorFilter[25] = 0xEF;
		byXorFilter[26] = 0xCD;
		byXorFilter[27] = 0x41;
		byXorFilter[28] = 0x26;
		byXorFilter[29] = 0x3C;
		byXorFilter[30] = 0x4E;
		byXorFilter[31] = 0x4D;

		for ( int i = iStart ; i != iEnd ; i+=iDir )
		{
			this->m_byBuffer[i] ^= this->m_byBuffer[i-1] ^ byXorFilter[ i%32 ];
		}
	}

public:

	BOOL AddData(void* pSrc, WORD wSize )	// Good 
	{
		if ( ((this->m_wSize + wSize) >=  2048) ||  (wSize == 0) )	{
			return FALSE;
		}

		memcpy((void*)&this->m_byBuffer[this->m_wSize], pSrc, wSize);
		this->m_wSize += wSize;
		return TRUE;
	}

	BOOL InsertPacket(void* pTar)	// Good
	{
		WORD wSize;
		BYTE byTemp[2048];

		

		switch ( this->m_byBuffer[0] )
		{
			case 0xC1:
				wSize = this->m_byBuffer[1];
				break;

			case 0xC2:
				wSize = this->m_byBuffer[1] * 256 + this->m_byBuffer[2];
				break;

			default:
				return true;
				break;
		}

		if ( this->m_wSize < wSize )
		{
			return 2;
		}


		this->XorData( (this->m_byBuffer[0] == 0xC1 ? 2 : 3)+1,wSize, 1);
		memcpy(pTar, this->m_byBuffer, wSize);
		this->m_wSize-=wSize;
		memcpy(byTemp, &this->m_byBuffer[wSize], this->m_wSize);
		memcpy(this->m_byBuffer, byTemp, this->m_wSize);
		return false;
	}


			

private:

	WORD m_wSize;	// 4
	BYTE m_byBuffer[2048];	// 6
	
};

struct PBMSG_HEAD
{
	BYTE c;
	BYTE size;
	BYTE headcode;
};

struct PBMSG_HEAD2
{
	BYTE c;
	BYTE size;
	BYTE headcode;
	BYTE subcode;
};

struct PWMSG_HEAD
{
	BYTE c;
	BYTE sizeH;
	BYTE sizeL;
	BYTE headcode;
};

struct PWMSG_HEAD2
{
	BYTE c;
	BYTE sizeH;
	BYTE sizeL;
	BYTE headcode;
	BYTE subcode;
};


struct PMSG_REQ_SERVLIST
{
	PBMSG_HEAD2 h;
	char szHash[32];
};

struct PMSG_SEND_HWID
{
	PBMSG_HEAD h;
	char szHash[32];
};

struct PMSG_CHARMAPJOINRESULT
{
	PBMSG_HEAD h;	// C3:F3:03
	BYTE subcode;	// 3
	BYTE MapX;	// 4
	BYTE MapY;	// 5
	BYTE MapNumber;	// 6
	BYTE Dir;	// 7

	//__int64 Exp;	// 8
	//__int64 NextExp;	// C

	BYTE Exp[8];
	BYTE NextExp[8];

	WORD LevelUpPoint;	// 10
	WORD Str;	// 12
	WORD Dex;	// 14
	WORD Vit;	// 16
	WORD Energy;	// 18
	WORD Life;	// 1A
	WORD MaxLife;	// 1C
	WORD Mana;	// 1E
	WORD MaxMana;	// 20
	WORD wShield;	// 22
	WORD wMaxShield;	// 24
	WORD BP;
	WORD MaxBP;	// 26
	WORD Unk;	// 28
	int Money;	// 2C
	BYTE PkLevel;	// 30
	BYTE CtlCode;	// 31
	short AddPoint;	// 32
	short MaxAddPoint;	// 34
	WORD Leadership;	// 36
	WORD wMinusPoint;	// 38
	WORD wMaxMinusPoint;	// 3A
	BYTE btInventoryExpansion;
#if (__FIX_HP__==1)
	int HP;
	int MAXHP;
	int MP;
	int MAXMP;
	int SD;
	int MAXSD;
#endif
};

struct PMSG_REFILL
{
	PBMSG_HEAD h;	// C1:26
	BYTE IPos;	// 3
	BYTE LifeH;	// 4
	BYTE LifeL;	// 5
	BYTE Flag;	// 6
	BYTE btShieldH;	// 7
	BYTE btShieldL;	// 8
#if (__FIX_HP__==1)
	int HP;
	int SD;
#endif
};

struct PMSG_MASTERINFORESULT
{
	PBMSG_HEAD2 h;
	WORD Level;

	BYTE Exp[8];
	BYTE NextExp[8];

	WORD Points;

	WORD MaxLife;
	WORD MaxMana;
	WORD MaxShield;
	WORD MaxBP;

#if(__FIX_HP__==1)
	int MAXHP;
	int MAXMP;
	int MAXSD;
#endif
};

struct PMSG_MASTER_LEVELUP
{
	PBMSG_HEAD2 h;
	WORD Level;
	WORD LevelAdd;
	WORD Points;
	WORD MaxMoints;
	WORD MaxHP;
	WORD MaxMana;
	WORD MaxShield;
	WORD MaxBP;

#if (__FIX_HP__==1)
	int MAXHP;
	int MAXMP;
	int MAXSD;
#endif
};

struct PMSG_LEVELUP
{
	PBMSG_HEAD h;	// C1:F3:05
	BYTE subcode;	// 3
	WORD Level;	// 4
	WORD LevelUpPoint;	// 6
	WORD MaxLife;	// 8
	WORD MaxMana;	// A
	WORD wMaxShield;	// C
	WORD MaxBP;	// E
	short AddPoint;	// 10
	short MaxAddPoint;	// 12
	short MinusPoint;	// 14
	short MaxMinusPoint;	// 16

#if(__FIX_HP__==1)
	int MAXHP;
	int MAXMP;
	int MAXSD;
#endif
};

struct PMSG_MANASEND
{
	PBMSG_HEAD h;	// C1:27
	BYTE IPos;	// 3
	BYTE ManaH;	// 4
	BYTE ManaL;	// 5
	BYTE BPH;	// 6
	BYTE BPL;	// 7
#if (__FIX_HP__==1)
	int MP;
#endif
};

struct PMSG_SENDEFFECT
{
	PBMSG_HEAD h;	// C1:2D
	WORD wOptionType;	// 4
	WORD wEffectType;	// 6
	BYTE byEffectOption;	// 8
	int iLeftTime;	// C
	BYTE iEffect;
	int iEffectValue;
};

struct PMSG_LVPOINTADDRESULT
{
	PBMSG_HEAD h;	// C1:F3:06
	BYTE subcode;	// 3
	BYTE ResultType;	// 4
	WORD MaxLifeAndMana;	// 6
	WORD wMaxShield;	// 8
	WORD MaxBP;	// A
	WORD NewValue;
	WORD LvlUpPt;
#if (__FIX_HP__==1)
	int MAXHPANDMANA;
	int MAXSD;
#endif
};

struct PMSG_CHARREGEN {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE MapX;
	BYTE MapY;
	BYTE MapNumber;
	BYTE Dir;
	WORD Life;
	WORD Mana;
	WORD wShield;
	WORD BP;
	BYTE Exp[8];
	DWORD Money;

#if (__FIX_HP__==1)
	int iLife;
	int iMana;
	int iShield;
#endif
};

#pragma pack(push)
#pragma pack(1)

struct PMSG_SETSTATS
{
	PBMSG_HEAD2 h;	// C1:F3:06
	WORD Str;
	WORD Dex;
	WORD Vit;
	WORD Ene;
	WORD Lea;
	int MaxLife;
	int MaxMana;
	int wMaxShield;	// 8
	WORD MaxBP;	// A
	WORD LvlUpPt;
};
#pragma pack(pop)

typedef struct
{
	PBMSG_HEAD h;	// C1:02
	char id[10];	// 3
	char chatmsg[100];	// D  
}PMSG_GLOBALPOST_WHISPER;

struct PMSG_SHOPITEMCOUNT
{
	PWMSG_HEAD h;	// C2:31
	BYTE Type;	// 4
	BYTE count;	// 5
};

struct __shop_price
{
	BYTE btPosAndTax;
	int iPrice;
};

BOOL ProtocolCore(BYTE HeadCode,LPBYTE pData,int iSize,int bCheck);
BOOL RecvBuffInfo(PMSG_SENDEFFECT* lpMsg);
BOOL PointAdd(LPBYTE aRecv,LPBYTE pPlayer);
void SetStats(LPBYTE aRecv,LPBYTE pPlayer);
BOOL customPost(PMSG_GLOBALPOST_WHISPER* lpMsg);
void SetShopPrices(LPBYTE pData);
DWORD GetItemValue(LPBYTE itemInfo,int type);

#pragma pack(push, 1)
struct ClientTest
{
	PBMSG_HEAD2 h;
	char Text[100];
	DWORD Value;
};
#pragma pack(pop)

class Protocol
{
public:
	void		Load();
	static void	DataRecv(DWORD Case, LPBYTE Data, int Len, int aIndex);
	static int	DataSend_(SOCKET Socket, const char* Data, int Len, int Flags);
	bool		DataSend(LPBYTE Data, int Size);
	SOCKET		m_ActiveSocket;
}; extern Protocol g_Protocol;