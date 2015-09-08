#pragma once

#define PVPBALANCE_DEBUG	0

struct PvPBalanceInfo {
	BYTE SourceClassType;
	BYTE TargetClassType;
	short IncreaseDamagePercent;
	short DecreaseDamagePercent;
	short IncreaseAttackRatePercent;
	short DecreaseAttackRatePercent;
	short IncreaseDefenseRatePercent;
	short DecreaseDefenseRatePercent;
};

class PvPBalance {
public:
	PvPBalance();
	~PvPBalance();

	void Load();
	void Init();
	void Read(const char* File);
	
	PvPBalanceInfo* getInfo(BYTE SourceClassType, BYTE TargetClassType);

	void modifyDamage(short UserIndex, short TargetIndex, int& Value);
	void modifyAttackRate(short UserIndex, short TargetIndex, float& Value);
	void modifyDefenseRate(short UserIndex, short TargetIndex, float& Value);

private:
	bool m_isLoaded;
	std::vector<PvPBalanceInfo> m_ClassInfo;
};

extern PvPBalance g_PvPBalance;