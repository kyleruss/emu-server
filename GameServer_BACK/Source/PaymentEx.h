#pragma once

#ifdef STREAM

#include "..\common\winutil.h"

struct PaymentExPremium
{
	int		Duration;
	int		Price;
	char	Command[25];
};

struct GDPaymentExUpdatePremium
{
	PBMSG_HEAD2		Head;
	DWORD			Duration;
};

class PaymentEx  
{
public:
	PaymentEx();
	virtual ~PaymentEx();

	void	Init();
	void	Load();
	void	Read(LPSTR File);
	
	void	CommandProc(LPOBJ lpUser, LPSTR Text);
	
	void	DisplayBalance(LPOBJ lpUser);
	void	GDUpdatePremium(LPOBJ lpUser, int Duration);
private:
	std::vector<PaymentExPremium> m_PremiumInfo;
	char	m_BalanceCommand[25];

}; extern PaymentEx g_PaymentEx;

#endif