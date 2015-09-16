#include "stdafx.h"
#include "MonsterHerd.h"
#include "resource.h"
#include "GameServer.h"
#include "Gamemain.h"
#include "Giocp.h"
#include "logproc.h"
#include "RingAttackEvent.h"
#include "CWhatsUpDummyServer.h"
#include "DBSockMng.h"
#include "DSProtocol.h"
#include "EDSProtocol.h"
#include "SProtocol.h"
#include "ChaosCastle.h"
#include "Protocol.h"
#include "AcceptIP.h"
#include "user.h"
#include "closeplayer.h"
#include "MapServerManager.h"
#include "protocol.h"
#include "Shop.h"
#include "Weapon.h"
#include "CastleSiegeSync.h"
#include "BloodCastle.h"
#include "EledoradoEvent.h"
#include "EventManagement.h"
#include "CashShop.h"
#include "TServerInfoDisplayer.h"
#include "TMonsterAI.h"
#include "Kanturu.h"
#include "CastleDeepEvent.h"
#include "CastleSiege.h"
#include "Crywolf.h"
#include "IllusionTempleEvent.h"
#include "Notice.h"
#include "Event.h"
#include "Raklion.h"
#include "DoubleGoerEvent.h"
#include "ImperialGuardian.h"
#include "GameEvent.h"
#include "MuTimer.h"
#include "RepackVendor.h"
#include <signal.h>

#if (ENABLETEST_ZTLUA == 1)
#include "ZTLuaManager.h"
#endif
// -------------------------------------------------------------------------------

int					gServerReady;
int					gCloseMsg;
int					gCloseMsgTime;
int					gCurPaintPlayer;
int					gCurPaintType;
int					gCurPaintMapNumber;
CRingAttackEvent	g_RingAttackEvent;
CXMasAttackEvent	g_XMasAttackEvent;
CWhatsUpDummyServer gWhatsUpDummyServer;
// -------------------------------------------------------------------------------

HINSTANCE hInst;
HWND ghWnd;
char szTitle[100];
char szWindowClass[64];
// -------------------------------------------------------------------------------

BOOL GameServerStart(void);
int InitInstance(HINSTANCE, int);	// Init GameServer
ATOM MyRegisterClass(HINSTANCE);	// Register Window Class
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);// Brain of the GameServer
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
// -------------------------------------------------------------------------------

typedef struct _MINIDUMP_EXCEPTION_INFORMATION 
{
    DWORD ThreadId;
    PEXCEPTION_POINTERS ExceptionPointers;
    BOOL ClientPointers;
} MINIDUMP_EXCEPTION_INFORMATION, *PMINIDUMP_EXCEPTION_INFORMATION;
// -------------------------------------------------------------------------------

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, DWORD DumpType, PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, void* UserStreamParam, void* CallbackParam);
LPTOP_LEVEL_EXCEPTION_FILTER pOldFilter;
// -------------------------------------------------------------------------------

LONG CALLBACK make_minidump(EXCEPTION_POINTERS* e)
{
	HMODULE hDbgDll = LoadLibrary("DBGHELP.DLL");

	if( hDbgDll )
	{
		MINIDUMPWRITEDUMP _MiniDumpWrite = (MINIDUMPWRITEDUMP)GetProcAddress(hDbgDll,"MiniDumpWriteDump");

		if( _MiniDumpWrite )
		{
			char szDmpName[64] = {0};
			SYSTEMTIME pTime;
			GetLocalTime(&pTime);

			sprintf(szDmpName,"%s%d-%d-%d_%dh%dm%ds.dmp",
				LOG_PATH_CRASH, pTime.wYear,pTime.wMonth,pTime.wDay,pTime.wHour,pTime.wMinute,pTime.wSecond);

			HANDLE hFile = CreateFile(szDmpName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,128,NULL);

			if( hFile != INVALID_HANDLE_VALUE )
			{
				_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

				ExInfo.ThreadId = ::GetCurrentThreadId();
				ExInfo.ExceptionPointers = e;
				ExInfo.ClientPointers = NULL;

				BOOL bDump = _MiniDumpWrite(GetCurrentProcess(),GetCurrentProcessId(),hFile,0x26,&ExInfo,NULL,NULL);

				if( bDump )
				{
					CloseHandle(hFile);
					return true;
				}
			}

			CloseHandle(hFile);
		}
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
// -------------------------------------------------------------------------------

void SetUnhandledFilter()
{
	SetErrorMode(SEM_FAILCRITICALERRORS);
	pOldFilter = SetUnhandledExceptionFilter(&make_minidump);
}
// -------------------------------------------------------------------------------

void UnSetUnhandledFilter()
{
	SetUnhandledExceptionFilter(pOldFilter);
}
// -------------------------------------------------------------------------------

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetUnhandledFilter();

	MSG msg;
	HACCEL hAccelTable;

	LoadString(hInstance, IDS_APP_TITLE, szTitle, 100);
	LoadString(hInstance, IDS_GAMESERVER, szWindowClass, 100);
	MyRegisterClass(hInstance);

	InitCommonControls();

	if (!InitInstance(hInstance, nCmdShow))
	{
		return 0;
	}

	ShowWindow(ghWnd, SW_SHOWNORMAL);// Showm GameServer Window

	hAccelTable=LoadAccelerators(hInstance,(LPCSTR)IDA_GAMESERVER);// Load Accelerators

	GiocpInit();
	GameMainInit(ghWnd);
	
	GameServerPort			= GetPrivateProfileIntA("Connection", "ServerPort", 55901, gDirPath.GetNewPath(FILE_SERVER_INFO));
	GetPrivateProfileStringA("Connection", "JoinServerIP", "127.0.0.1", JoinServerIP, sizeof(JoinServerIP), gDirPath.GetNewPath(FILE_SERVER_INFO));
	JoinServerPort			= GetPrivateProfileIntA("Connection", "JoinServerPort", 55970, gDirPath.GetNewPath(FILE_SERVER_INFO));
	GetPrivateProfileStringA("Connection", "DataServer1IP", "127.0.0.1", DataServerIP, sizeof(DataServerIP), gDirPath.GetNewPath(FILE_SERVER_INFO));
	DataServerPort			= GetPrivateProfileIntA("Connection", "DataServer1Port", 55960, gDirPath.GetNewPath(FILE_SERVER_INFO));
	GetPrivateProfileStringA("Connection", "DataServer2IP", "127.0.0.1", DataServerIP2, sizeof(DataServerIP2), gDirPath.GetNewPath(FILE_SERVER_INFO));
	DataServerPort2			= GetPrivateProfileIntA("Connection", "DataServer2Port", DataServerPort+2, gDirPath.GetNewPath(FILE_SERVER_INFO));
	GetPrivateProfileStringA("Connection", "ExDBServerIP", "127.0.0.1", ExDBServerIP, sizeof(ExDBServerIP), gDirPath.GetNewPath(FILE_SERVER_INFO));
	ExDbPort				= GetPrivateProfileIntA("Connection", "ExDBServerPort", 55906, gDirPath.GetNewPath(FILE_SERVER_INFO));
	GetPrivateProfileStringA("Connection", "RankingServerIP", "127.0.0.1", RankingServerIP, sizeof(RankingServerIP), gDirPath.GetNewPath(FILE_SERVER_INFO));
	RankingServerPort		= GetPrivateProfileIntA("Connection", "RankingServerPort", 44455, gDirPath.GetNewPath(FILE_SERVER_INFO));
	GetPrivateProfileStringA("Connection", "EventServerIP", "127.0.0.1", EventServerIP, sizeof(EventServerIP), gDirPath.GetNewPath(FILE_SERVER_INFO));
	EventServerPort			= GetPrivateProfileIntA("Connection", "EventServerPort", 44456, gDirPath.GetNewPath(FILE_SERVER_INFO));

	gWhatsUpDummyServer.Start(ghWnd, GameServerPort+1);
	AllServerStart();

	while(GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (!TranslateAccelerator(msg.hwnd,hAccelTable,&msg))
		{
		    TranslateMessage(&msg);
		    DispatchMessage(&msg);
		}
	}

	g_MuTimer.SetTimerEnd();
	UnSetUnhandledFilter();

	return msg.wParam;
}
// -------------------------------------------------------------------------------
	
BOOL AllServerStart(void)
{
	
	if (JoinServerConnected==0)
	{
		if (GMJoinServerConnect(JoinServerIP, WM_GM_JOIN_CLIENT_MSG_PROC) == 0)
		{	
			// LoginServer connect fail
			MsgBox(lMsg.Get(MSGGET(1, 144)));
			return 0;
		}
	}

	/*if (IsDevilSquareEventConnected==0)
	{
		if (GMRankingServerConnect(RankingServerIP, WM_GM_RANKING_CLIENT_MSG_PROC) == 0)
		{
			MsgBox("Ranking Server connect fail!");
			return 0;
		}
		IsDevilSquareEventConnected=1;
	}*/

	if (IsEventChipServerConnected==0)
	{
		if (GMEventChipServerConnect(ExDBServerIP, WM_GM_EVENTCHIP_CLIENT_MSG_PROC) == 0)
		{
			MsgBox("Event Chip Server connect fail!");
			return 0;
		}
		IsEventChipServerConnected=1;
	}

	cDBSMng.Connect();
	return TRUE;
}
// -------------------------------------------------------------------------------

BOOL GameServerStart(void) 
{
	if (GameMainServerCreate(WM_GM_SERVER_MSG_PROC, WM_GM_CLIENT_MSG_PROC) == FALSE )
	{
		MsgBox(lMsg.Get(MSGGET( 1, 142) ));
		return FALSE;
	}


	CreateGIocp(GameServerPort);

	SetTimer(ghWnd, WM_LOG_PAINT, 2000, NULL);
	SetTimer(ghWnd, WM_CONNECT_DATASERVER, 10000, NULL);

	g_MuTimer.SetMuTimer();
	return TRUE;
}
// -------------------------------------------------------------------------------

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_NORMAL_V1);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDR_MENU;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON_MINIMIZED);

	return RegisterClassEx(&wcex);
}
// -------------------------------------------------------------------------------

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

	HWND hWnd;
	hInst=hInstance;

#if (ZEONWINDOW == TRUE)
    hWnd = CreateWindowEx(
        0,
        szWindowClass,
        szTitle,
        WS_OVERLAPPED+WS_CAPTION+WS_SYSMENU+WS_MINIMIZEBOX+WS_VISIBLE,
        CW_USEDEFAULT, 0, GAMESERVER_WIDTH, GAMESERVER_HEIGHT,
        NULL, NULL, hInstance, NULL);
#else
	hWnd = CreateWindowEx(
        0,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT,0,
        NULL, NULL, hInstance, NULL);
#endif
	
	if (hWnd == 0) 
	{
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	ghWnd=hWnd;
	return TRUE;
}
// -------------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT wmId, wmEvent;

	switch (message) 
	{
		case WM_CREATE:
			return 0;
		case WM_COMMAND:

			wmId    = wParam & 0xFFFF & 0xFFFF; 
			wmEvent = (wParam >> 16) & 0xFFFF;

			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUT_BOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   SendMessage(hWnd, WM_CLOSE, 0, 0);
				   break;
				case IDM_CLOSE_PLAYER:
					{
						Ccloseplayer closeplayer;
						closeplayer.Load("closeplayer.txt");
					}
					break;
				case IDM_ALL_USER_CLOSE:
					gObjAllDisconnect();
					break;
				case IDM_ALL_USER_LOGOUT:
					gObjAllLogOut();
					break;
				case IDM_ALL_POSITION_RELOAD:
					gMSetBase.MonsterSetBaseReload(LOAD_ALLOBJ);
					break;
				case IDM_MONSTER_POSITION_RELOAD:
					gMSetBase.MonsterSetBaseReload(LOAD_MONSTERLIST);
					break;
				case IDM_GOLDEN_POSITION_RELOAD:
					gMSetBase.MonsterSetBaseReload(LOAD_ELDORADOLIST);
					break;
				case IDM_NPC_POSITION_RELOAD:
					gMSetBase.MonsterSetBaseReload(LOAD_NPCLIST);
					break;
				case IDM_SHOP_RELOAD:
					g_ShopManager.Reload();
					break;
				case IDM_MAP_SERVER_INFO_RELOAD:
					g_MapServerManager.LoadData(gDirPath.GetNewPath(FILE_WORLD_MAPSERVER));
					break;
#if (GS_CASTLE == 1)
				case IDM_CASTLE_SIEGE_RELOAD:
					if(g_CastleSiege.LoadData(gDirPath.GetNewPath(FILE_EVENT_CASTLESIEGE)) != FALSE)
					{
						g_CastleSiege.LoadPreFixData(gDirPath.GetNewPath(FILE_SERVER_COMMON));
						g_CastleSiege.SetDataLoadState(2);
					}
					break;
#endif
				case IDM_OPTION_RELOAD:
					ReadCommonServerInfo();
					break;
				case IDM_RELOAD_SCRIPTS:
#if (ENABLETEST_ZTLUA == 1)
					g_LuaBackup.Reload();
#endif
					break;
				case IDM_VERSION_RELOAD:
					ReadCommonlocInfo();
					break;
				case IDM_CASHSHOP_OPTION_RELOAD:
					g_CashShop.CashShopOptioNReload();
					break;
				case IDM_LOG_PAINT:
					gCurPaintType=0;
					break;
				case IDM_MAP1:
					gCurPaintMapNumber = 0;	// Lorencia
					gCurPaintType=1;
					break;
				case IDM_MAP2:
					gCurPaintType=1;
					gCurPaintMapNumber = 1;	// Dungeon
					break;
				case IDM_MAP3:
					gCurPaintType=1;
					gCurPaintMapNumber = 2;	// Devias
					break;
				case IDM_MAP4:
					gCurPaintType=1;
					gCurPaintMapNumber = 3;	// Noria
					break;
				case IDM_CRYWOLF:
					gCurPaintType=1;
					gCurPaintMapNumber = 34;	// Crywolf
					break;
				case IDM_DOUBLEGOER1:
					gCurPaintType=1;
					gCurPaintMapNumber = 68;	// Double Goer
					break;
				case IDM_DRAGON_EVENT:
					if (  DragonEvent->GetState() != 0 )
						DragonEvent->End();
					else
						DragonEvent->Start();
					break;
				case IDM_FIRECRACKER:
					if ( gFireCrackerEvent != 0 )
						gFireCrackerEvent = 0;
					else
						gFireCrackerEvent=1;
					break;
				case IDM_5_MINUTE_CLOSE:
					if ( gCloseMsg == 0 )
					{
						gCloseMsgTime = 300;
						gCloseMsg=1;
						AllSendServerMsg( lMsg.Get(MSGGET(4, 104)) );
					}
					break;
				case IDM_3_MINUTE_CLOSE:
					if ( gCloseMsg == 0 )
					{
						gCloseMsgTime = 180;
						gCloseMsg=2;
						AllSendServerMsg(lMsg.Get(MSGGET(4, 105)) );
					}
					break;
				case IDM_1_MINUTE_CLOSE:
					if ( gCloseMsg == 0 )
					{
						gCloseMsgTime = 60;
						gCloseMsg=3;
						AllSendServerMsg(lMsg.Get(MSGGET(4, 106)) );
					}
					break;
				case IDM_RELOAD_ALL_EVENT:
					ReadEventInfo(MU_EVENT_ALL );
					break;
				case IDM_RELOAD_DEVILSQUARE:
					ReadEventInfo(MU_EVENT_DEVILSQUARE );
					break;
				case IDM_RELOAD_BLOODCASTLE:
					ReadEventInfo(MU_EVENT_BLOODCASTLE );
					break;
				case IDM_RELOAD_UNDERWORLD_ATTACK:
					ReadEventInfo(MU_EVENT_ATTACKEVENTTROOP );
					break;
				case IDM_RELOAD_GOLDENTROOP_ATTACK:
					ReadEventInfo(MU_EVENT_GOLDENTROOP );
					break;
				case IDM_RELOAD_WHITEMAGE_ATTACK:
					ReadEventInfo(MU_EVENT_WHITEMAGETROOP );
					break;
				case IDM_RELOAD_LOVE_PANGPANG:
					ReadEventInfo(MU_EVENT_LOVEPANGPANG );
					break;
				case IDM_RELOAD_FIRECRACKER:
					ReadEventInfo(MU_EVENT_FIRECRACKER );
					break;
				case IDM_RELOAD_XMAS_STAR:
					ReadEventInfo(MU_EVENT_XMASSTAR );
					break;
				case IDM_RELOAD_HEART_OF_LOVE:
					ReadEventInfo(MU_EVENT_HEARTOFLOVE );
					break;
				case IDM_RELOAD_NPC_SAY_HAPPY_NEW_YEAR:
					ReadEventInfo(MU_EVENT_SAY_HAPPYNEWYEAR );
					break;
				case IDM_RELOAD_NPC_SAY_MERRY_XMAS:
					ReadEventInfo(MU_EVENT_SAY_MERRYXMAS );
					break;
				case IDM_RELOAD_CHAOSCASTLE:
					ReadEventInfo(MU_EVENT_CHAOSCASTLE );
					break;
				case IDM_RELOAD_CHRISTMAS_RIBBONBOX:
					ReadEventInfo(MU_EVENT_CHRISTMAS_RIBBONBOX );
					break;
				case IDM_RELOAD_VALENTINE_DAY_CHOCOLATE_BOX:
					ReadEventInfo(MU_EVENT_VALENTINESDAY_CHOCOLATEBOX );
					break;
				case IDM_RELOAD_WHITE_DAY_CANDY_BOX:
					ReadEventInfo(MU_EVENT_WHITEDAY_CANDYBOX  );
					break;
				case IDM_RELOAD_ALL_ETC_OPTION:
					ReadGameEtcInfo(MU_ETC_ALL );
					break;
				case IDM_RELOAD_CREATE_CHARACTER:
					ReadGameEtcInfo(MU_ETC_CREATECHARACTER );
					break;
				case IDM_RELOAD_GUILD:
					ReadGameEtcInfo(MU_ETC_GUILD );
					break;
				case IDM_RELOAD_TRADE:
					ReadGameEtcInfo(MU_ETC_TRADE );
					break;
				case IDM_RELOAD_CHAOSBOX:
					ReadGameEtcInfo(MU_ETC_USECHAOSBOX );
					break;
				case IDM_RELOAD_PERSONAL_SHOP:
					ReadGameEtcInfo(MU_ETC_PERSONALSHOP );
					break;
				case IDM_RELOAD_PK_ITEM_DROP:
					ReadGameEtcInfo(MU_ETC_PKITEMDROP );
					break;
				case IDM_RELOAD_ITEM_DROP_RATE:
					ReadGameEtcInfo(MU_ETC_ITEMDROPRATE );
					break;
				case IDM_RELOAD_SPEEDHACK:
					ReadGameEtcInfo(MU_ETC_SPEEDHACK );
					break;
				case IDM_RELOAD_GAMEGUARD_CHECKSUM_CHECK:
					ReadGameEtcInfo(MU_ETC_GAMEGUARD );
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
					break;
			}
			break;
		case WM_GM_SERVER_MSG_PROC:
			GMServerMsgProc(wParam, lParam);
			break;
		case WM_GM_CLIENT_MSG_PROC:
			{
				int wp = wParam;
				int lp = lParam;
				GMClientMsgProc(wParam, lParam);
			}
			break;
		case WM_GM_JOIN_CLIENT_MSG_PROC:
			GMJoinClientMsgProc(wParam, lParam);
			break;
		case WM_GM_RANKING_CLIENT_MSG_PROC:
			GMRankingClientMsgProc(wParam, lParam);
			break;
		case WM_GM_EVENTCHIP_CLIENT_MSG_PROC:
			GMEventChipClientMsgProc(wParam, lParam);
			break;
		case WM_GM_DATA1_CLIENT_MSG_PROC:
			cDBSMng.MsgProc(0, wParam, lParam);
			break;
		case WM_GM_DATA2_CLIENT_MSG_PROC:
			cDBSMng.MsgProc(1, wParam, lParam);
			break;
		case WM_GM_EXDATA_CLIENT_MSG_PROC:
			ExDataClientMsgProc(wParam, lParam);
			break;
		case WM_START_SERVER:
			if ( gServerReady == 2 )
				GameServerStart();
			break;
		case WM_TIMER:
			switch ( wParam )
			{
				case WM_GUILD_LIST_REQUEST:
					DGGuildListRequest();
					KillTimer(ghWnd, WM_GUILD_LIST_REQUEST);
					break;
				case WM_LOG_PAINT:
					{
						RECT rect;
						HDC hdc = GetDC(hWnd);
						GetClientRect(hWnd, &rect);
						FillRect(hdc, &rect, (HBRUSH)GetStockObject(0));
						ReleaseDC(hWnd, hdc);

						if ( gCurPaintType == 0)
						{
							if ( LogTextPaint != NULL )
							{
								LogTextPaint(hWnd);
							}
						}
						
						gObjViewportPaint(hWnd, gCurPaintPlayer);
						g_ServerInfoDisplayer.Run(hWnd);
						GJNotifyMaxUserCount();

#if (__CUSTOM__ == 1)
					g_Notice.SendOnPoint();
#endif
					}
					break;
				case WM_MONSTER_AND_MSG_PROC:
					MonsterAndMsgProc();
					break;
				case WM_MOVE_MONSTER_PROC:
					MoveMonsterProc();
#if (GS_CASTLE==0 || GS_SPEC == 1)
					g_Kanturu.UserMonsterCountCheck();
#endif
					break;
				case WM_EVENT_RUN_PROC:
					g_BloodCastle.Run();
					g_RingAttackEvent.Run();
					g_ChaosCastle.Run();
					g_IllusionTemple.Run();
#if (GS_CASTLE==1)
					g_CastleSiege.Run();
					g_CastleDeepEvent.Run();
					g_CsNPC_Weapon.WeaponAttackProc();
					g_Crywolf.Run();
#endif
#if (GS_CASTLE == 0 || GS_SPEC == 1)
					g_Kanturu.Run();
					g_CsNPC_Weapon.WeaponAttackProc();
#endif
					g_Raklion.Run();
					g_XMasAttackEvent.Run();
					g_ImperialGuardian.Run();
					//gDuelSystem.Run();
					gDoubleGoer.Run();
					break;
				case WM_EVENT_RUN_PROC2:
					//g_ArcaWar.Run();
					break;
				case WM_DUEL_RUN_PROC:
					//g_DuelManager.Run();
					break;
				case WM_AI_MONSTER_MOVE_PROC:
					TMonsterAI::MonsterMoveProc();
					break;
				case WM_AI_MONSTER_PROC:
					TMonsterAI::MonsterAIProc();
					break;
				case WM_FIRST_MSG_PROCESS:
					{
						int n;
						for ( n=0;n<OBJMAX;n++)
						{
							gObjStateSetCreate(n);
							gObjViewportListDestroy(n);
							gObjViewportListCreate(n);
							gObjViewportListProtocol(n);

							if( n >= OBJ_STARTUSERINDEX )
							{
								gObjUnionUpdateProc(n);

								if ( gDoPShopOpen != FALSE )
								{
									PShop_ViewportListRegenarate(n);

									if (gObjIsConnected(n) != PLAYER_EMPTY )
									{
										gObj[n].m_bPShopItemChange = false;
									}
								}
							}
						}
						gObjSetState();
						GameServerInfoSend();
					}
					break;
				case WM_SECOND_MSG_PROCESS:
					{
						for (int n = 0;n<MAX_NUMBER_MAP;n++)
						{
							MapC[n].WeatherVariationProcess();
						}
						wsGServer.m_SendSec = wsGServer.m_SendSecTmp;
						wsGServer.m_RecvSec = wsGServer.m_RecvSecTmp;
						wsGServer.m_SendSecTmp = 0;
						wsGServer.m_RecvSecTmp = 0;
						DragonEvent->Run();
						AttackEvent->Run();
						gEledoradoEvent.Run();
						g_EventManager.Run();
#if (CUSTOM_OLYMP==1)
						g_CustomOlymp.Run();
#endif
					}
					break;
				case WM_GS_CLOSE:
					if ( gCloseMsg != 0 )
					{
						gCloseMsgTime--;

						if ( gCloseMsgTime <= 1 )
						{
							if ( gCloseMsgTime == 1 )
							{
								AllSendServerMsg(lMsg.Get(MSGGET(1, 193)));
								LogAddC(2, lMsg.Get(MSGGET(1, 193)));
							}
						}
						else
						{
							if ( (gCloseMsgTime % 10) == 0 )
							{
								char szTemp[256];

								wsprintf( szTemp, lMsg.Get(MSGGET(1, 194)), gCloseMsgTime);
								AllSendServerMsg(szTemp);
								LogAddC(2, szTemp);
							}
						}

						if ( gCloseMsgTime < 0 )
						{
							KillTimer(ghWnd, WM_GS_CLOSE);
							gObjAllLogOut();
							gCloseMsgTime = 0;
							gCloseMsg = 0;
						}
					}
					gObjSecondProc();
#if (GS_CASTLE == 1)
					if(cDBSMng.GetActiveDS() > -1)
					{
						if(g_CastleSiege.GetDataLoadState() == 2)
						{
							g_CastleSiege.DataRequest();
						}

#if (CUSTOM_OLYMP==1)
						if(g_CustomOlymp.GetDataLoadState() == 2 )
						{
							g_CustomOlymp.DataRequest();
						}
					
#endif
					}
					g_CastleSiege.SendCastleStateSync();
					g_CastleSiegeSync.AdjustTributeMoney();
					g_Crywolf.CrywolfSecondAct();
#endif
#if (GS_CASTLE == 0 || GS_SPEC == 1)
					g_CastleSiegeSync.AdjustTributeMoney();

#if(__GENS_SYSTEM__ == 1)
					gGensSystem.UpdateRun();
#endif
#endif
					break;
				case WM_SET_DATE:
					gSetDate();
					WhisperCash.TimeCheckCashDelete();
					break;
				case WM_CONNECT_DATASERVER:
					cDBSMng.Connect();
					g_CashShop.CheckShopServerConnectState();
					gObjCheckAllUserDuelStop();
					g_CashShop.ItemCheckWorker();
					break;
				case WM_LOG_DATE_CHANGE:
#if (GS_CASTLE == 1)
					if(LogDateChange() == TRUE)	
					{
						g_iCastleItemMixLimit = 1;
					}
#else
					LogDateChange();
#endif
					break;
			}
			break;
		case WM_CLOSE:
			if (MessageBox(ghWnd, "GameServer close?", "Close", MB_YESNO|MB_APPLMODAL) == IDYES)
			{
				DestroyWindow(hWnd);
			}
			break;
		case WM_DESTROY:
			GameMainFree();
			GiocpDelete();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
// -------------------------------------------------------------------------------

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if ( ( wParam & 0xFFFF & 0xFFFF ) == IDOK || (wParam & 0xFFFF & 0xFFFF ) == IDCANCEL) 
			{
				EndDialog(hDlg, (wParam& 0xFFFF & 0xFFFF ));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
// -------------------------------------------------------------------------------