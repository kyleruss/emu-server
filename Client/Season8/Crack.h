#pragma once

class Crack
{
public:
	void	Load();

	static void	ProcAttach();
	static void	ProcLoading();
	void ProcCrack();
	static void ProcUnprotect();
	static void ProcReturn();

	BYTE m_AttachRestoreBuff[5];

}; extern Crack g_Crack;