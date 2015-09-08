#pragma once
// -------------------------------------------------------------------------------

#include "user.h"
// -------------------------------------------------------------------------------

namespace ChaosProduceTag
{
	enum T
	{
		Type = 0,
		Index = 1,
		Level = 2,
		Durability = 3,
		Skill = 4,
		Luck = 5,
		Option = 6,
		Excellent = 7,
		Ancient = 8,
		Socket = 9,
		Count = 10,
	};
};
// -------------------------------------------------------------------------------

struct ChaosProduce
{
	BYTE	Type;
	short	Index;
	BYTE	LevelMin;
	BYTE	LevelMax;
	BYTE	OptionMin;
	BYTE	OptionMax;
	BYTE	Skill;
	BYTE	Luck;
	BYTE	ExcellentMin;
	BYTE	ExcellentMax;
	BYTE	Ancient;
	BYTE	SocketMin;
	BYTE	SocketMax;
	BYTE	Durability;
};
// -------------------------------------------------------------------------------

struct ChaosIngredient
{
	BYTE	TypeStart;
	short	IndexStart;
	BYTE	TypeEnd;
	short	IndexEnd;
	BYTE	LevelMin;
	BYTE	LevelMax;
	BYTE	OptionMin;
	BYTE	OptionMax;
	BYTE	DurabilityMin;
	BYTE	DurabilityMax;
	BYTE	CountMin;
	BYTE	CountMax;
	char	Tag[50];
};
// -------------------------------------------------------------------------------

struct ChaosTalisman
{
	BYTE	TypeStart;
	short	IndexStart;
	BYTE	TypeEnd;
	short	IndexEnd;
	BYTE	LevelMin;
	BYTE	LevelMax;
	BYTE	OptionMin;
	BYTE	OptionMax;
	BYTE	DurabilityMin;
	BYTE	DurabilityMax;
	BYTE	AdditionalRate; //-1: by durability, 0+: value
	BYTE	CountMax; //-1: none, 0+: value
};
// -------------------------------------------------------------------------------

struct ChaosCombination
{
	char Name[255];
	BYTE SuccessRate;
	int Money;
	std::vector<ChaosIngredient> Ingredients;
	std::vector<ChaosProduce> Produces;
	std::vector<ChaosTalisman> Talismans;
};
// -------------------------------------------------------------------------------

class ChaosRecipe
{
public:
			ChaosRecipe(short RecipeIndex);
	virtual ~ChaosRecipe();
 
	void Init();
	bool Read(LPSTR File);
 
	static void	ActStart(LPOBJ lpUser, ChaosRecipe* lpRecipe, ChaosCombination* lpCombination);
	static bool	ActCheck(LPOBJ lpUser, ChaosRecipe* lpRecipe, ChaosCombination* lpCombination);
	static void ActFail(LPOBJ lpUser, ChaosRecipe* lpRecipe, ChaosCombination* lpCombination);
	static void ActFail(LPOBJ lpUser, ChaosRecipe* lpRecipe, ChaosCombination* lpCombination, BYTE ErrorCode);
	static void	ActSuccess(LPOBJ lpUser, ChaosRecipe* lpRecipe, ChaosCombination* lpCombination, ChaosProduce* lpProduce);
 
	static bool	CheckIngredient(LPOBJ lpUser, short ChaosPos, ChaosIngredient* lpIngredient);
	static bool	CheckTalisman(LPOBJ lpUser, short ChaosPos, ChaosTalisman* lpTalisman);
	static bool	ApplyTalisman(LPOBJ lpUser, short ChaosPos, ChaosRecipe* lpRecipe, ChaosCombination* lpCombination, ChaosTalisman* lpTalisman);
public:
	char Name[255];
	short RecipeIndex;
	std::vector<ChaosCombination> Combinations;
};
// -------------------------------------------------------------------------------

class ChaosMachineManager
{
public:
			ChaosMachineManager();
	virtual ~ChaosMachineManager();
 
	void	Init();
	void	Load();
	void	Read(LPSTR File);
 
	bool	StartMix(LPOBJ lpUser, short RecipeIndex);
private:
	std::vector<ChaosRecipe*> Recipes;
	// ----
}; extern ChaosMachineManager g_ChaosMachineManager;
// -------------------------------------------------------------------------------