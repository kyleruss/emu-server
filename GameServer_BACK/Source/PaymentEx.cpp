#include "stdafx.h"
#include "PaymentEx.h"
#include "..\pugixml\pugixml.hpp"

#ifdef STREAM

using namespace pugi;
PaymentEx g_PaymentEx;

PaymentEx::PaymentEx()
{

}

PaymentEx::~PaymentEx()
{

}

void PaymentEx::Init()
{
	this->m_PremiumInfo.clear();
	if( this->m_PremiumInfo.capacity() > 0 )
	{
		std::vector<PaymentExPremium>().swap(this->m_PremiumInfo);
	}
	ZeroMemory(this->m_BalanceCommand, sizeof(this->m_BalanceCommand));
}

void PaymentEx::Load()
{
	this->Init();
	this->Read(FILE_CUSTOM_PAYMENTEX);
}

void PaymentEx::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if( Result.status != status_ok )
	{
		MsgBox("[PaymentEx] File %s not found!", File);
		return;
	}
	// ----
	xml_node PaymentExList = Document.child("paymentex");
	// ----
	memcpy(this->m_BalanceCommand, PaymentExList.child("balance").text().as_string(), sizeof(this->m_BalanceCommand)-1);
	// ----
	for( xml_node Item = PaymentExList.child("premium").child("item"); Item; Item = Item.next_sibling() )
	{
		PaymentExPremium lpItem = { 0 };
		lpItem.Price = Item.attribute("price").as_int();
		lpItem.Duration = Item.attribute("duration").as_int();
		memcpy(lpItem.Command, Item.text().as_string(), sizeof(lpItem.Command)-1);
		this->m_PremiumInfo.push_back(lpItem);
	}
}

void PaymentEx::CommandProc(LPOBJ lpUser, LPSTR Text)
{
	if( !lpUser || lpUser->Connected != PLAYER_PLAYING )
	{
		return;
	}

	if( !strcmp(this->m_BalanceCommand, Text) )
	{
		this->DisplayBalance(lpUser);
		return;
	}

	int InfoIndex = -1;

	for( int i = 0; i < this->m_PremiumInfo.size(); i++ )
	{
		if( !strcmp(this->m_PremiumInfo[i].Command, Text) )
		{
			InfoIndex = i;
			break;
		}
	}

	if( InfoIndex == (int)-1 )
	{
		MsgOutput(lpUser->m_Index, "Syntax error in command");
		return;
	}

	PaymentExPremium* lpInfo = &this->m_PremiumInfo[InfoIndex];

	if( lpInfo->Price > lpUser->m_Credits )
	{
		MsgOutput(lpUser->m_Index, "You need more credits for this command");
		return;
	}

	lpUser->m_Credits -= lpInfo->Price;
	this->DisplayBalance(lpUser);
	this->GDUpdatePremium(lpUser, lpInfo->Duration);
}

void PaymentEx::DisplayBalance(LPOBJ lpUser)
{
	if( !lpUser || lpUser->Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	char Text[90] = { 0 };
	sprintf(Text, "Balance: %d", lpUser->m_Credits);
	MsgOutput(lpUser->m_Index, Text);
}

void PaymentEx::GDUpdatePremium(LPOBJ lpUser, int Duration)
{
	if( !lpUser || lpUser->Connected != PLAYER_PLAYING )
	{
		return;
	}
	// ----
	GDPaymentExUpdatePremium lpRequest = { 0 };
	PHeadSubSetB((LPBYTE)&lpRequest, 0xFD, 0x05, sizeof(lpRequest));
	lpRequest.Duration = Duration;
	DataSend(lpUser->m_Index, (LPBYTE)&lpRequest, lpRequest.Head.size);
}

#endif