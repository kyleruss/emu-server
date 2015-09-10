#pragma once
// -------------------------------------------------------------------------------

#include "..\common\winutil.h"
// -------------------------------------------------------------------------------

struct _tagPMSG_ANS_ARCA_BATTLE_JOIN
{
    PBMSG_HEAD2 h;
    BYTE		btResult;
	// ----
	_tagPMSG_ANS_ARCA_BATTLE_JOIN::_tagPMSG_ANS_ARCA_BATTLE_JOIN() //Line: 110
	{
		this->btResult = 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagPMSG_ANS_ARCA_BATTLE_ENTER
{
	PBMSG_HEAD2 h;
    BYTE		btResult;
	// ----
    _tagPMSG_ANS_ARCA_BATTLE_ENTER::_tagPMSG_ANS_ARCA_BATTLE_ENTER() //Line: 129
	{
		this->btResult = 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagPMSG_ARCA_BATTLE_OBELISK_INFO
{
	PBMSG_HEAD2 h;
    BYTE		m_btObeliskAttribute;
    BYTE		m_btAuraState[3];
	// ----
	_tagPMSG_ARCA_BATTLE_OBELISK_INFO::_tagPMSG_ARCA_BATTLE_OBELISK_INFO() //Line: 147
	{
		this->m_btObeliskAttribute = 0;
		// ----
		for( int i = 0; i < 3; i++ )
		{
			this->m_btAuraState[i] = 0;
		}
	};
};
// -------------------------------------------------------------------------------

struct _tagPMSG_ARCA_BATTLE_STATE
{
    PBMSG_HEAD2 h;
    BYTE		m_btArcaBattleState;
	// ----
	_tagPMSG_ARCA_BATTLE_STATE::_tagPMSG_ARCA_BATTLE_STATE() //Line: 167
	{
		this->m_btArcaBattleState = 0;
	};
};
// -------------------------------------------------------------------------------

struct _stAuraInfo
{
    int		m_iAuraPosX;
    int		m_iAuraPosY;
	// ----
	_stAuraInfo::_stAuraInfo() //Line: 182
	{
		this->m_iAuraPosX = -1;
		this->m_iAuraPosY = -1;
	};
};
// -------------------------------------------------------------------------------

struct _stArcaBattleObeliskInfo
{
    int		iGroupNumber;
    int		iAttrKind;
    int		iMapIndex;
    int		iPosX;
    int		iPosY;
    _stAuraInfo m_StAuraInfo[3];
	// ----
	_stArcaBattleObeliskInfo::_stArcaBattleObeliskInfo() //Line: 203
	{
		this->iGroupNumber	= -1;
		this->iAttrKind		= -1;
		this->iMapIndex		= -1;
		this->iPosX			= -1;
		this->iPosY			= -1;
	};
};
// -------------------------------------------------------------------------------

struct _tagPMSG_ARCA_BATTLE_PLAY_INFO
{
    PBMSG_HEAD2 h;
    WORD		wGuildGroupNum;
    _stArcaBattleObeliskInfo m_stObeliskInfo[5];
	// ----
	_tagPMSG_ARCA_BATTLE_PLAY_INFO::_tagPMSG_ARCA_BATTLE_PLAY_INFO() //Line: 222
	{
		this->wGuildGroupNum = 0;
	};
};
// -------------------------------------------------------------------------------

struct _stABObeliskOccupy
{
    WORD	wObeliskKind;
    char	szGuildName[9];
	// ----
	_stABObeliskOccupy::_stABObeliskOccupy() //Line: 237
	{
		this->wObeliskKind = 0;
		memset(this->szGuildName, 0, 9);
	};
};
// -------------------------------------------------------------------------------

struct _stObeliskLife
{
    BYTE	btLife;
    BYTE	btAttribute;
	// ----
	_stObeliskLife::_stObeliskLife() //Line: 265
	{
		this->btLife		= 0;
		this->btAttribute	= 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagPMSG_REQ_AB_GUILD_JOIN_SELECT_DS
{
    
    PBMSG_HEAD2	h;
    char		szGuildMaster[11];
    WORD		wNumber;
	// ----
	_tagPMSG_REQ_AB_GUILD_JOIN_SELECT_DS::_tagPMSG_REQ_AB_GUILD_JOIN_SELECT_DS() //Line: 307
	{
		memset(this->szGuildMaster, 0, 11);
		this->wNumber = 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagPMSG_REQ_ARCA_BATTLE_GUILD_JOIN_DS
{
    PBMSG_HEAD2	h;
    char		szGuildMaster[11];
    char		szGuildName[9];
    DWORD		dwGuild;
    WORD		wNumber;
	// ----
	_tagPMSG_REQ_ARCA_BATTLE_GUILD_JOIN_DS::_tagPMSG_REQ_ARCA_BATTLE_GUILD_JOIN_DS() //Line: 325
	{
		memset(this->szGuildMaster, 0, 11);
		memset(this->szGuildName, 0, 9);
		this->dwGuild = 0;
		this->wNumber = 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagPMSG_REQ_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS
{
    PBMSG_HEAD2 h;
    char		szCharName[11];
    char		szGuildName[9];
    DWORD		dwGuild;
    WORD		wNumber;
	// ----
	_tagPMSG_REQ_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS::_tagPMSG_REQ_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS() //Line: 360
	{
		memset(this->szCharName, 0, 11);
		memset(this->szGuildName, 0, 9);
		this->dwGuild = 0;
		this->wNumber = 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagPMSG_REQ_ARCA_BATTLE_ENTER_DS
{
    PBMSG_HEAD2 h;
    char		szCharName[11];
    BYTE		btEnterSeq;
    WORD		wNumber;
	// ----
	_tagPMSG_REQ_ARCA_BATTLE_ENTER_DS::_tagPMSG_REQ_ARCA_BATTLE_ENTER_DS() //Line: 393
	{
		memset(this->szCharName, 0, 11);
		this->wNumber = 0;
	};
};
// -------------------------------------------------------------------------------

struct _stABWinGuildInfoDS
{
    char	szGuildName[9];
    DWORD	dwGuild;
    WORD	wOccupyObelisk;
    WORD	wObeliskGroup;
	// ----
	_stABWinGuildInfoDS::_stABWinGuildInfoDS() //Line: 451
	{
		memset(this->szGuildName, 0, 9);
		this->dwGuild			= 0;
		this->wOccupyObelisk	= 0;
		this->wObeliskGroup		= 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagPMSG_REQ_AB_WIN_GUILD_INFO_INSERT_DS
{
    PBMSG_HEAD2 h;
    BYTE		btGuildCnt;
    WORD		wMapSvrNum;
    _stABWinGuildInfoDS m_stABWinGuildInfoDS[5];
	// ----
	_tagPMSG_REQ_AB_WIN_GUILD_INFO_INSERT_DS::_tagPMSG_REQ_AB_WIN_GUILD_INFO_INSERT_DS() //Line: 471
	{
		this->btGuildCnt = 0;
		this->wMapSvrNum = 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagPMSG_REQ_AB_WIN_GUILD_INFO_DS
{
    PBMSG_HEAD2 h;
    WORD		wNumber;
	// ----
	_tagPMSG_REQ_AB_WIN_GUILD_INFO_DS::_tagPMSG_REQ_AB_WIN_GUILD_INFO_DS() //Line: 508
	{
		this->wNumber = 0;
	};
};
// -------------------------------------------------------------------------------

struct _tagPMSG_REQ_REMOVE_ALL_GUILD_BUFF_DS
{
    PBMSG_HEAD2 h;
    WORD wMapSvrNum;
};
// -------------------------------------------------------------------------------

struct _tagPMSG_ANS_REMOVE_GUILD_BUFF_DS
{
    PBMSG_HEAD2 h;
    char szGuildName[9];
};
// -------------------------------------------------------------------------------

struct _tagPMSG_SEND_AB_WIN_GUILD_INFO
{
    PBMSG_HEAD2 h;
    char szGuildName[9];
};
// -------------------------------------------------------------------------------

struct _tagPMSG_REQ_AB_PROC_STATE_DS
{
    PBMSG_HEAD2 h;
};
// -------------------------------------------------------------------------------

struct _tagPMSG_REQ_ARCA_BATTLE_INFO_DS
{
    PBMSG_HEAD2 h;
};
// -------------------------------------------------------------------------------

struct _tagPMSG_REQ_AB_PROC_INSERT_DS
{
    PBMSG_HEAD2 h;
    WORD wMapSvrNum;
    BYTE btProcState;
};
// -------------------------------------------------------------------------------

struct _tagPMSG_REQ_AB_JOIN_MEMBER_UNDER_DS
{
    PBMSG_HEAD2 h;
    WORD wMapSvrNum;
};
// -------------------------------------------------------------------------------

struct _tagPMSG_REQ_AB_JOIN_CANCEL_DS
{
    PBMSG_HEAD2 h;
    BYTE btMinGuildMemNum;
    WORD wMapSvrNum;
};
// -------------------------------------------------------------------------------

struct _tagPMSG_SEND_AB_JOIN_CANCEL
{
    PBMSG_HEAD2 h;
};
// -------------------------------------------------------------------------------

struct _stCancelGuildNames
{
    char szGuildNames[9];
};
// -------------------------------------------------------------------------------

struct _tagPMSG_ANS_AB_JOIN_CANCEL_DS
{
    PBMSG_HEAD2 h;
    BYTE btGuildCnt;
    _stCancelGuildNames CancelGuildNames[6];
};
// -------------------------------------------------------------------------------

struct _tagPMSG_ANS_AB_REG_MEMBER_CNT_DS
{
    PBMSG_HEAD2 h;
    WORD wNumber;
    BYTE btRegMemCnt;
};
// -------------------------------------------------------------------------------

struct _tagPMSG_ANS_AB_REG_MEMBER_CNT
{
    PBMSG_HEAD2 h;
    BYTE btResult;
    BYTE btRegMemCnt;
};
// -------------------------------------------------------------------------------