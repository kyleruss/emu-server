#pragma once
// -------------------------------------------------------------------------------

#include "user.h"
// -------------------------------------------------------------------------------

struct BuffSkillEffectInfo
{
	int		m_SkillNumber;
	float	m_Arg1;
	float	m_Arg2;
	float	m_Arg3;
	float	m_Arg4;
	float	m_Arg5;
	float	m_Arg6;
	float	m_Arg7;
	float	m_Arg8;
	float	m_Arg9;
	float	m_Arg10;
	float	m_Arg11;
	float	m_Arg12;
};
// -------------------------------------------------------------------------------

class BuffSkillEffect  
{
public:
			BuffSkillEffect();
	virtual ~BuffSkillEffect();
	// ----
	void	Init();
	void	Load();
	void	Read(LPSTR File);
	// ----
	BuffSkillEffectInfo* GetEffect(int SkillNumber);
	// ----
private:
	std::vector<BuffSkillEffectInfo> m_EffectInfo;
	// ----
}; extern BuffSkillEffect g_BuffSkillEffect;
// -------------------------------------------------------------------------------