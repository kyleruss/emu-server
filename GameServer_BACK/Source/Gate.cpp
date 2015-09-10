#include "stdafx.h"
#include "gate.h"
#include "logproc.h"
#include "GameMain.h"
#include "GameServer.h"
#include "..\common\WzMemScript.h"
#include "..\include\readscript.h"
#include "Raklion.h"
#include "..\pugixml\pugixml.hpp"
using namespace pugi;
CGate gGateC;


CGate::CGate ()
{
	return;
}


CGate::~CGate()
{
	return;
}


void CGate::Init()
{
	int n;

	for (n=0;n<MAX_GATES;n++)
	{
		m_This[n] = 0xFFFF;	// -1 NULL
		m_Level[n]=0;
	}
}


void CGate::Load(char* filename)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(filename);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[MoveGate] Loading fail [%s]", filename);
		return;
	}
	// ----
	xml_node GateList = Document.child("movegate");
	// ----
	for( xml_node Gate = GateList.child("gate"); Gate; Gate = Gate.next_sibling() )
	{
		int GateNumber = Gate.attribute("id").as_int(-1);
		// ----
		if( GateNumber < 0 || GateNumber >= MAX_GATES )
		{
			MsgBox("[MoveGate] GateNumber is wrong [%d]", GateNumber);
			return;
		}
		// ----

			m_This[GateNumber]	= GateNumber;
			m_Flag[GateNumber]	= Gate.attribute("flag").as_int();
			m_MapNumber[GateNumber] = Gate.attribute("map").as_int();
			m_Sx[GateNumber] = Gate.attribute("x1").as_int();
			m_Sy[GateNumber] = Gate.attribute("y1").as_int();
			m_Ex[GateNumber] = Gate.attribute("x2").as_int();
			m_Ey[GateNumber] = Gate.attribute("y2").as_int();
			m_TargetGate[GateNumber] = Gate.attribute("target").as_int();
			m_Dir[GateNumber] = Gate.attribute("dir").as_int();
			m_Level[GateNumber] = Gate.attribute("minlvl").as_int();
			m_MaxLevel[GateNumber] = Gate.attribute("maxlvl").as_int();
	}
	// ----
	/*int Token;
	int number;
	
	this->Init();

	SMDFile=fopen(filename, "r");
	
	if (SMDFile==0)
	{
		LogAdd("[%s] gate information load error", filename);
		return;
	}

	while ( true )
	{
		Token = GetToken();

		if ( Token == 2 )
		{
			break;
		}
		else
		{
			if ( Token == 1 )
			{

				number=(int)TokenNumber;
				m_This[number]=number;

				Token=GetToken();
				m_Flag[number]=(BYTE)TokenNumber;

				Token=GetToken();
				m_MapNumber[number]=(BYTE)TokenNumber;

				Token=GetToken();
				m_Sx[number]=(BYTE)TokenNumber;

				Token=GetToken();
				m_Sy[number]=(BYTE)TokenNumber;

				Token=GetToken();
				m_Ex[number]=(BYTE)TokenNumber;

				Token=GetToken();
				m_Ey[number]=(BYTE)TokenNumber;

				Token=GetToken();
				m_TargetGate[number]=(WORD)TokenNumber;

				Token=GetToken();
				m_Dir[number]=(BYTE)TokenNumber;

				Token=GetToken();
				m_Level[number]=(WORD)TokenNumber;
			}
		}
	}

	fclose(SMDFile);*/
	LogAdd("[%s] MoveGate has been loaded", filename);
}

BOOL CGate::IsGate(int GateNumber)
{
	if ( (GateNumber < 0) || (GateNumber > MAX_GATES-1) )
	{
		LogAdd(lMsg.Get(MSGGET(1, 195)), GateNumber, __FILE__, __LINE__);
		return 0;
	}

	if ( m_This[GateNumber] == (WORD)-1 )
	{
		LogAdd( lMsg.Get(MSGGET(1, 196)), __FILE__, __LINE__);
		return 0;
	}

	return 1;
}



int CGate::GetGate(int mgt, BYTE& x, BYTE& y, BYTE& MapNumber, BYTE& dir, short& Level)
{
	int gt;
	int tx;
	int ty;
	int loopcount;
	BYTE attr;
	
	if (IsGate(mgt) == 0)
	{
		return -1;
	}

	gt = m_TargetGate[mgt];

	if (gt == 0)
	{
		gt = mgt;
	}

	if (MapNumber < 0 || MapNumber >= MAX_NUMBER_MAP) {
		LogAddDebug("[%s] Wrong map number, maybe async in packet reading!", __FUNCTION__);
		return -1;
	}

	loopcount = 10;

	while ( loopcount-- != 0 )
	{
		// For X coords
		if ( ( m_Ex[gt] - m_Sx[gt]) > 0 )
		{
			tx = m_Sx[gt] + ( rand() % ( m_Ex[gt] - m_Sx[gt] ) );	// Choose any X position between XBegin and XEnd
		}
		else
		{
			tx = m_Sx[gt];	// If there is amistake while putting values in gate.txt
		}
		// For Y coords
		if ( ( m_Ey[gt] - m_Sy[gt]) > 0 )
		{
			ty = m_Sy[gt] + ( rand() % ( m_Ey[gt] - m_Sy[gt] ) );	// Choose any Y position between YBegin and YEnd
		}
		else
		{
			ty = m_Sy[gt];	// If there is amistake while putting values in gate.txt
		}

		attr = MapC[MapNumber].GetAttr(tx, ty);

		if ( (attr&4) != 4 && (attr&2) != 2 && (attr&8) != 8 )
		{
			break;
		}
	}

	x = tx;
	y = ty;
	MapNumber = m_MapNumber[gt];
	dir = m_Dir[gt];
	Level = m_Level[gt];

	return gt;
}
			

int CGate::GetLevel(int GateNumber)
{
	if (GateNumber < 0 || GateNumber > MAX_GATES-1 )
	{
		LogAdd(lMsg.Get(MSGGET(1, 195)), GateNumber, __FILE__, __LINE__);
		return -1;
	}

	return m_Level[GateNumber];
}



BOOL CGate::IsInGate(int aIndex, int GateNumber)
{
	LPOBJ lpObj = &gObj[aIndex];

	if ( IsGate(GateNumber) == FALSE )
	{
		return false;
	}

	int level = m_Level[GateNumber];

	if ( lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_FIGHTER )
	{
		if ( level > 0 )
		{
			level = level / 3 * 2;
		}
	}

	if ( lpObj->Level < level )
	{
		return FALSE;
	}

	if ( lpObj->MapNumber != m_MapNumber[GateNumber] )
	{
		return FALSE;
	}

	if ( lpObj->X < m_Sx[GateNumber]-5 || lpObj->X > m_Ex[GateNumber]+5 || lpObj->Y < m_Sy[GateNumber]-5 || lpObj->Y > m_Ey[GateNumber]+5 )
	{
		return FALSE;
	}

	return true;
}

BOOL CGate::CheckGateLevel(int aIndex, int GateNumber)
{
	LPOBJ lpObj = &gObj[aIndex];

	if ( IsGate(GateNumber) == FALSE )
	{
		return false;
	}

	int level = m_Level[GateNumber];

	if ( lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_FIGHTER )
	{
		if ( level > 0 )
		{
			level = (level/3)*2;
		}
	}

	if ( lpObj->Level < level )
	{
		return false;
	}

	return true;
}
