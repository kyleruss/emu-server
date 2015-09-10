#pragma once

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

class CSendNPCInfo  
{
public:
	CSendNPCInfo();
	virtual ~CSendNPCInfo();

	void LoadScript(LPSTR File);

    int SendNpcCoordinate(int nDestUserIndex, BYTE btMapIndex, BYTE btAddType);
    int SendPortalCoordinate( int nDestUserIndex, BYTE btMapNumber);
    int SendPortalCoordinateMoveNpc(BYTE btBeforeMapNumber, BYTE btAfterMapNumber, BYTE btBeforeX, BYTE btBeforeY, BYTE btX, BYTE btY, int nNpcClass);

private:
	bool m_bLoadScript;
	std::vector<_stSendNPCInfo> m_vtSendNpcInfo;
    std::vector<_stSendPortalInfo> m_vtSendPortalInfo;
};

extern CSendNPCInfo g_sendNpcInfo;