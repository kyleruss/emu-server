#pragma once

#include "..\include\prodef.h"
/*
struct PMSG_MINIMAP_SEND_NPC
{
	PBMSG_HEAD2 h;
	BYTE btNum;
	BYTE btGroup;
	BYTE btType;
	BYTE btAction;
	BYTE btX;
	BYTE btY;
	char szName[31];
};

struct MINIMAP_USER_INFO
{
	char szName[11];
	BYTE btMapNumber;
	BYTE btX;
	BYTE btY;
};

struct PMSG_MINIMAP_SEND_PARTY
{
	PBMSG_HEAD2 h;
	BYTE btCount;
	MINIMAP_USER_INFO User[5];
};

struct MINIMAP_OBJECT
{
	BYTE btType;
	BYTE btX;
	BYTE btY;
	char szDescription[31];
};

typedef std::multimap<int,MINIMAP_OBJECT> mapMinimap;

class CMiniMap
{
public:
	CMiniMap();
	virtual ~CMiniMap();

	void Init();
	void LoadMinimapInfo(const char* lpszFileName);
	void SendMinimap(int aIndex);
	void SendParty(int aIndex);
	void SetOpenMinimap(int aIndex);
	void SetCloseMinimap(int aIndex);

private:
	int m_bLoaded;
	mapMinimap m_NpcObjects;
	mapMinimap m_GateObjects;
};

extern CMiniMap g_MiniMap;*/

struct _stSendNPCInfo
{
    int nNpcClass;
    int nIdentNo;
    BYTE btTag;
    BYTE btSync;
    BYTE btMapNumber;
    BYTE btPosX;
    BYTE btPosY;
    char szName[31];
};

struct _stSendPortalInfo
{
    int nIdentNo;
    BYTE btPosX;
    BYTE btPosY;
    BYTE btMapNumber;
    char szName[31];
};

struct _PMSG_REQ_NPC_POSITION
{
    PBMSG_HEAD2 h;
    BYTE btType;
};

struct _PMSG_ANS_NPC_POSITION
{
    PBMSG_HEAD2 h;
    BYTE btIdentNo;
    BYTE btIsNpc;
    BYTE btTag;
    BYTE btType;
    BYTE btPosX;
    BYTE btPosY;
    char szName[31];
};

struct stMemberPosInfo
{
    stMemberPosInfo()
	{
		memset(this->szName, 0, sizeof(this->szName));
		this->btPosX = 0;
		this->btPosY = 0;
		this->btMapNumber = 0;
	};

    char szName[11];
    BYTE btMapNumber;
    BYTE btPosX;
    BYTE btPosY;
};

struct PMSG_MEMBER_POS_INFO_SEND
{
    PMSG_MEMBER_POS_INFO_SEND()
	{

	};
	
    PBMSG_HEAD2 h;
    BYTE btCount;
    stMemberPosInfo stPosInfo[5];
};

class Minimap
{
public:
	Minimap();
	virtual ~Minimap();

	void Init();
	void Load();
	void Read(LPSTR File);

	void CGReqSendMemberPosInfoStart(int UserIndex);
	void CGReqSendMemberPosInfoStop(int UserIndex);
	void GCReqSendNpcPosInfo(_PMSG_REQ_NPC_POSITION* lpRecv, int UserIndex);

	void GCPartyMemberPosition(int nIndex);
	void SendNpcCoordinate(int nDestUserIndex, BYTE btMapIndex, BYTE btAddType);
	void SendPortalCoordinateMoveNpc(BYTE btBeforeMapNumber, BYTE btAfterMapNumber, BYTE btBeforeX, BYTE btBeforeY, BYTE btX, BYTE btY, int nNpcClass);
	void SendPortalCoordinate(int nDestUserIndex, BYTE btMapNumber);

private:
	std::vector<_stSendNPCInfo> m_vtSendNpcInfo;
	std::vector<_stSendPortalInfo> m_vtSendPortalInfo;
};

extern Minimap g_Minimap;