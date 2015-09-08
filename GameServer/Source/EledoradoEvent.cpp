#include "stdafx.h"
#include "EledoradoEvent.h"
#include "Gamemain.h"
#include "LogProc.h"
#include "user.h"
#include "TNotice.h"
#include "BuffEffectSlot.h"
#include "gObjMonster.h"

CEledoradoEvent gEledoradoEvent;


CEledoradoEvent::CEledoradoEvent()
{
	this->GoldGoblenEventStartTime = 0;
	this->TitanEventStartTime = 0;
	this->GoldDerconEventStartTime = 0;
	this->DevilLizardKingEventStartTime = 0;
	this->KanturEventStartTime = 0;
	this->RabbitEventStartTime = 0;
	this->DarkKnightEventStartTime = 0;
	this->DevilEventStartTime = 0;
	this->MonsterEventStartTime = 0;
	this->CrustEventStartTime = 0;
	this->SatirosEventStartTime = 0;
	this->TwintailEventStartTime = 0;
	this->IronKnightEventStartTime = 0;
	this->NeipinEventStartTime = 0;
	this->GreatDragonEventStartTime = 0;
	this->m_BossGoldDerconMapNumber[0] = -1;
	this->m_BossGoldDerconMapNumber[1] = -1;
	this->m_BossGoldDerconMapNumber[2] = -1;
}




CEledoradoEvent::~CEledoradoEvent()
{
	return;
}





void CEledoradoEvent::Init()
{
#if (__ABYSS__ == 1)
	m_mGoldenMobs_HP.clear();
#endif
	return;
}




void CEledoradoEvent::SetEventState(int State)
{
	this->EventState = State;
}



void CEledoradoEvent::Run()
{
	if ( this->m_bMenualStart != FALSE )
	{
		return;
	}

	if ( this->EventState == 0 )
	{
		return;
	}

	if ( this->GoldGoblenEventStartTime == 0 )
	{
		tm * today;
		time_t ltime;

		time(&ltime);
		today = localtime(&ltime);

		if ( today->tm_min >= 30 && today->tm_min  <= 35 )
		{
			int Time = GetTickCount();

			this->GoldGoblenEventStartTime = Time;
			this->RegenGoldGoblen();

			this->TitanEventStartTime = Time;
			this->RegenTitan();

			this->GoldDerconEventStartTime = Time;
			this->RegenGoldDercon();

			this->DevilLizardKingEventStartTime = Time;
			this->RegenDevilLizardKing();

			this->KanturEventStartTime = Time;
			this->RegenKantur();

			this->RabbitEventStartTime = Time;
			this->RegenRabbit();

			this->DarkKnightEventStartTime = Time;
			this->RegenDarkKnight();

			this->DevilEventStartTime = Time;
			this->RegenDevil();

			this->MonsterEventStartTime = Time;
			this->RegenMonster();

			this->CrustEventStartTime = Time;
			this->RegenCrust();

			this->SatirosEventStartTime = Time;
			this->RegenSatiros();

			this->TwintailEventStartTime = Time;
			this->RegenTwintail();

			this->IronKnightEventStartTime = Time;
			this->RegenIronKnight();

			this->NeipinEventStartTime = Time;
			this->RegenNeipin();

			this->GreatDragonEventStartTime = Time;
			this->RegenGreatDragon();
		}
	}
	else
	{
		DWORD Time = GetTickCount();

		if ( ( Time - this->GoldGoblenEventStartTime ) > gEledoradoGoldGoblenRegenTime * 60000 )
		{
			this->GoldGoblenEventStartTime = Time;
			this->RegenGoldGoblen();
		}

		if ( ( Time - this->TitanEventStartTime ) > gEledoradoTitanRegenTime * 60000 )
		{
			this->TitanEventStartTime = Time;
			this->RegenTitan();
		}

		if ( ( Time - this->GoldDerconEventStartTime ) > gEledoradoGoldDerconRegenTime * 60000 )
		{
			this->GoldDerconEventStartTime = Time;
			this->RegenGoldDercon();
		}

		if ( ( Time - this->DevilLizardKingEventStartTime ) > gEledoradoDevilLizardKingRegenTime * 60000 )
		{
			this->DevilLizardKingEventStartTime = Time;
			this->RegenDevilLizardKing();
		}

		if ( ( Time - this->KanturEventStartTime ) > gEledoradoDevilTantarosRegenTime * 60000 )
		{
			this->KanturEventStartTime = Time;
			this->RegenKantur();
		}

		if ( ( Time - this->RabbitEventStartTime ) > gEledoradoGoldenRabbitRegenTime * 60000 )
		{
			this->RabbitEventStartTime = Time;
			this->RegenRabbit();
		}

		if ( ( Time - this->DarkKnightEventStartTime ) > gEledoradoGoldenDarkKnightRegenTime * 60000 )
		{
			this->DarkKnightEventStartTime = Time;
			this->RegenDarkKnight();
		}

		if ( ( Time - this->DevilEventStartTime ) > gEledoradoGoldenDevilRegenTime * 60000 )
		{
			this->DevilEventStartTime = Time;
			this->RegenDevil();
		}

		if ( ( Time - this->MonsterEventStartTime ) > gEledoradoGoldenMonsterRegenTime * 60000 )
		{
			this->MonsterEventStartTime = Time;
			this->RegenMonster();
		}

		if ( ( Time - this->CrustEventStartTime ) > gEledoradoGoldenCrustRegenTime * 60000 )
		{
			this->CrustEventStartTime = Time;
			this->RegenCrust();
		}

		if ( ( Time - this->SatirosEventStartTime ) > gEledoradoGoldenSatirosRegenTime * 60000 )
		{
			this->SatirosEventStartTime = Time;
			this->RegenSatiros();
		}

		if ( ( Time - this->TwintailEventStartTime ) > gEledoradoGoldenTwintailRegenTime * 60000 )
		{
			this->TwintailEventStartTime = Time;
			this->RegenTwintail();
		}

		if ( ( Time - this->IronKnightEventStartTime ) > gEledoradoGoldenIronKnightRegenTime * 60000 )
		{
			this->IronKnightEventStartTime = Time;
			this->RegenIronKnight();
		}

		if ( ( Time - this->NeipinEventStartTime ) > gEledoradoGoldenNeipinRegenTime * 60000 )
		{
			this->NeipinEventStartTime = Time;
			this->RegenNeipin();
		}

		if ( ( Time - this->GreatDragonEventStartTime ) > gEledoradoGoldenGreatDragonRegenTime * 60000 )
		{
			this->GreatDragonEventStartTime = Time;
			this->RegenGreatDragon();
		}
	}
}







void CEledoradoEvent::RegenGoldGoblen()
{
	int n=0;
	int MapNumber;
	int Map[2]={MAP_INDEX_RORENCIA,MAP_INDEX_NORIA};
	int SelMap = -1;

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 78 )
		{
			gObj[n].Live = TRUE;

			if( SelMap == -1 )
			{
				MapNumber = Map[rand()%2];
			}
			else
			{
				MapNumber = SelMap;
			}

			gObj[n].MapNumber = MapNumber;


			while ( gMSetBase.GetBoxPosition(MapNumber, 50, 50, 200, 200, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make GoldGoblen : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
	}
}



void CEledoradoEvent::RegenTitan()
{
	int n=0;
	int MapNumber;

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 53 )	// Golden Titan
		{
			gObj[n].Live = TRUE;
			MapNumber = 2;
			gObj[n].MapNumber = MapNumber;

			while ( gMSetBase.GetBoxPosition(MapNumber, 50, 50, 200, 200, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			this->m_BossTitanMapNumber = gObj[n].MapNumber;
			this->m_BossTitanMapX = gObj[n].X;
			this->m_BossTitanMapY = gObj[n].Y;

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make GoldTaitan : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
		else if ( gObj[n].Class == 54 )
		{
			gObj[n].Live = TRUE;
			MapNumber = this->m_BossTitanMapNumber;
			gObj[n].MapNumber = MapNumber;

			gMSetBase.GetBoxPosition(MapNumber, this->m_BossTitanMapX-4, this->m_BossTitanMapY-4,
				this->m_BossTitanMapX+4, this->m_BossTitanMapY+4, gObj[n].X, gObj[n].Y);

						gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;
		}
	}
}


void CEledoradoEvent::RegenGoldDercon()
{
	int n=0;
	int MapNumber=-1;
	int Map[3]={MAP_INDEX_RORENCIA,MAP_INDEX_DEVIAS,MAP_INDEX_NORIA};
	int SelMap = -1;
	int count = 0;

	this->CheckGoldDercon(MapNumber);

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 79 )
		{
			gObj[n].Live = TRUE;

			if ( SelMap == -1 )	
			{
				MapNumber = Map[rand()%3];
			}
			else
			{
				MapNumber = SelMap;
			}

			gObj[n].MapNumber = MapNumber;

			while ( gMSetBase.GetBoxPosition(MapNumber, 80, 80, 170, 170, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make GoldDercon : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			if ( SelMap == -1 )
			{
				char szTemp[256];
				strcpy(szTemp, " ");
				strcat(szTemp, gMapName[MapNumber]);
				strcat(szTemp, lMsg.Get(MSGGET(4, 134)));

				TNotice::AllSendServerMsg(szTemp);
			}

			this->m_BossGoldDerconMapNumber[count] = MapNumber;
			gObj[n].m_BossGoldDerconMapNumber = count;
			count++;
			SelMap = MapNumber;

			gObjMonsterHitDamageInit(&gObj[n]);
		}
	}

	this->CheckGoldDercon(MapNumber);
}



void CEledoradoEvent::RegenDevilLizardKing()
{
	int n=0;
	int MapNumber;

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		MapNumber = 7;
		if ( gObj[n].Class == 80 )	// Devil Lizard King
		{
			gObj[n].Live = TRUE;
			gObj[n].MapNumber = MapNumber;

			while ( gMSetBase.GetBoxPosition(MapNumber, 50, 10, 250, 200, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			this->m_BossDevilLizardKingMapNumber = gObj[n].MapNumber;
			this->m_BossDevilLizardKingMapX = gObj[n].X;
			this->m_BossDevilLizardKingMapY = gObj[n].Y;

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make GoldLizarKing : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
		else if ( gObj[n].Class == 81 ) // Golden Velparie
		{
			gObj[n].Live = TRUE;
			MapNumber = this->m_BossDevilLizardKingMapNumber;
			gObj[n].MapNumber = MapNumber;

			while( gMSetBase.GetBoxPosition(MapNumber, 15,50, 250, 200, gObj[n].X, gObj[n].Y ) == 0 )
			{

			}

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;
		}
	}
}

void CEledoradoEvent::RegenKantur()
{
	int n=0;
	int MapNumber;

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 82 )	// Gold Tantalos
		{
			gObj[n].Live = TRUE;
			MapNumber = 8;
			gObj[n].MapNumber = MapNumber;

			while ( gMSetBase.GetBoxPosition(MapNumber, 50, 50, 200, 200, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			this->m_BossKanturMapNumber = gObj[n].MapNumber;
			this->m_BossKanturMapX = gObj[n].X;
			this->m_BossKanturMapY = gObj[n].Y;

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;

			gObjClearBuffEffect(&gObj[n],CLEAR_TYPE_DIEREGEN);

			gObj[n].m_ViewState = 0;
			gObj[n].Teleport = 0;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make GoldTantarus : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
		else if ( gObj[n].Class == 83 ) // Gold Iron Wheel
		{
			gObj[n].Live = TRUE;
			MapNumber = this->m_BossKanturMapNumber;
			gObj[n].MapNumber = MapNumber;


			gMSetBase.GetBoxPosition(MapNumber, this->m_BossKanturMapX-10, this->m_BossKanturMapY-10,
				this->m_BossKanturMapX+10, this->m_BossKanturMapY+10, gObj[n].X, gObj[n].Y);

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;
		}
	}
}

void CEledoradoEvent::CheckGoldDercon(int MapNumber)
{
	if ( EventState == 0 )
		return;

	BOOL EventOn = FALSE;
	int EventClearMapNumber = -1;

	for ( int i=0;i<3;i++)
	{
		if ( m_BossGoldDerconMapNumber[i] != -1 )
		{
			if ( m_BossGoldDerconMapNumber[i] == MapNumber )
			{
				EventOn = TRUE;
			}
			else
			{
				EventClearMapNumber = this->m_BossGoldDerconMapNumber[i];
			}
		}
	}

	if( EventClearMapNumber != -1 )
	{
		GCMapEventStateSend(EventClearMapNumber, 0, 3);
	}

	if ( EventOn != FALSE )
	{
		GCMapEventStateSend(MapNumber, 1, 3);
	}
	else
	{
		GCMapEventStateSend(MapNumber, 0, 3);
	}
}



void CEledoradoEvent::Start_Menual()
{
	this->SetMenualStart(TRUE);

	LogAddTD("¡Û¡Ü[Event Management] [Start] EledoradoEvent Event!");

	DWORD TickCount = GetTickCount();

	this->GoldGoblenEventStartTime = TickCount;
	this->RegenGoldGoblen();

	this->TitanEventStartTime = TickCount;
	this->RegenTitan();

	this->GoldDerconEventStartTime = TickCount;
	this->RegenGoldDercon();

	this->DevilLizardKingEventStartTime = TickCount;
	this->RegenDevilLizardKing();

	this->KanturEventStartTime = TickCount;
	this->RegenKantur();

	this->RabbitEventStartTime = TickCount;
	this->RegenRabbit();

	this->DarkKnightEventStartTime = TickCount;
	this->RegenDarkKnight();

	this->DevilEventStartTime = TickCount;
	this->RegenDevil();

	this->MonsterEventStartTime = TickCount;
	this->RegenMonster();

	this->CrustEventStartTime = TickCount;
	this->RegenCrust();

	this->SatirosEventStartTime = TickCount;
	this->RegenSatiros();

	this->TwintailEventStartTime = TickCount;
	this->RegenTwintail();

	this->IronKnightEventStartTime = TickCount;
	this->RegenIronKnight();

	this->NeipinEventStartTime = TickCount;
	this->RegenNeipin();

	this->GreatDragonEventStartTime = TickCount;
	this->RegenGreatDragon();
}
void CEledoradoEvent::End_Menual()
{
	this->SetMenualStart(FALSE);
}

void CEledoradoEvent::RegenRabbit()
{
	int n=0;
	int MapNumber;

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 502 )
		{
			gObj[n].Live = TRUE;
			MapNumber = MAP_INDEX_ELBELAND;
			gObj[n].MapNumber = MapNumber;

			while ( gMSetBase.GetBoxPosition(MapNumber, 1, 1, 255, 255, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make Golden rabbit : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
	}
}

void CEledoradoEvent::RegenDarkKnight()
{
	int n=0;
	int MapNumber;

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 493 )
		{
			gObj[n].Live = TRUE;
			MapNumber = MAP_INDEX_DUNGEON;
			gObj[n].MapNumber = MapNumber;

			while ( gMSetBase.GetBoxPosition(MapNumber, 1, 1, 255, 255, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make Golden DarkKnight : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
	}
}
void CEledoradoEvent::RegenDevil()
{
	int n=0;
	int MapNumber;

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 494 )
		{
			gObj[n].Live = TRUE;
			MapNumber = MAP_INDEX_LOSTTOWER;
			gObj[n].MapNumber = MapNumber;

			while ( gMSetBase.GetBoxPosition(MapNumber, 1, 1, 255, 255, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make Golden Devil : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
	}
}
void CEledoradoEvent::RegenMonster()
{
	int n=0;
	int MapNumber;

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 495 )
		{
			gObj[n].Live = TRUE;
			MapNumber = MAP_INDEX_AIDA;
			gObj[n].MapNumber = MapNumber;


			while ( gMSetBase.GetBoxPosition(MapNumber, 1, 1, 255, 255, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make Golden Monster : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
	}
}

void CEledoradoEvent::RegenCrust()
{
	int n=0;
	int MapNumber;

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 496 )
		{
			gObj[n].Live = TRUE;
			MapNumber = MAP_INDEX_ICARUS;
			gObj[n].MapNumber = MapNumber;

			while ( gMSetBase.GetBoxPosition(MapNumber, 1, 1, 255, 255, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make Golden Crust : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
	}
}

void CEledoradoEvent::RegenSatiros()
{
	int n=0;
	int MapNumber;

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 497 )
		{
			gObj[n].Live = TRUE;
			MapNumber = MAP_INDEX_KANTURU1;
			gObj[n].MapNumber = MapNumber;

			while ( gMSetBase.GetBoxPosition(MapNumber, 1, 1, 255, 255, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make Golden Satiros : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
	}
}
void CEledoradoEvent::RegenTwintail()
{
	int n=0;
	int MapNumber;

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 498 )
		{
			gObj[n].Live = TRUE;
			MapNumber = MAP_INDEX_KANTURU2;
			gObj[n].MapNumber = MapNumber;

			while ( gMSetBase.GetBoxPosition(MapNumber, 1, 1, 255, 255, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make Golden Twintail : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
	}
}

void CEledoradoEvent::RegenIronKnight()
{
	int n=0;
	int MapNumber;

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 499 )
		{
			gObj[n].Live = TRUE;
			MapNumber = MAP_INDEX_RAKLION;
			gObj[n].MapNumber = MapNumber;

			while ( gMSetBase.GetBoxPosition(MapNumber, 1, 1, 255, 255, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make Golden IronKnight : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
	}
}


void CEledoradoEvent::RegenNeipin()
{
	int n=0;
	int MapNumber;

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 500 )
		{
			gObj[n].Live = TRUE;
			MapNumber = MAP_INDEX_SWAMPOFCALMNESS;
			gObj[n].MapNumber = MapNumber;

			while ( gMSetBase.GetBoxPosition(MapNumber, 1, 1, 255, 255, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make Golden Neipin : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
	}
}
void CEledoradoEvent::RegenGreatDragon()
{
	int n=0;
	int MapNumber;
	int Map[2] = {MAP_INDEX_KANTURU1,MAP_INDEX_RAKLION};

	for (n=0;n<OBJ_MAXMONSTER;n++)
	{
		if ( gObj[n].Class == 501 )
		{
			gObj[n].Live = TRUE;
			MapNumber = Map[rand()%2];
			gObj[n].MapNumber = MapNumber;

			while ( gMSetBase.GetBoxPosition(MapNumber, 1, 1, 255, 255, gObj[n].X, gObj[n].Y) == 0 )
			{

			}

			gObj[n].Life = gObj[n].MaxLife;
			gObj[n].TX = gObj[n].X;
			gObj[n].TY = gObj[n].Y;
			gObj[n].MTX = gObj[n].X;
			gObj[n].MTY = gObj[n].Y;
			gObj[n].StartX = gObj[n].X;
			gObj[n].StartY = gObj[n].Y;
			gObj[n].m_State = 1;
			gObj[n].PathCount = 0;

			LogAddTD("Make Golden Great Dragon : %d, %d,%d",
				MapNumber, gObj[n].X, gObj[n].Y);

			gObjMonsterHitDamageInit(&gObj[n]);
		}
	}
}