#include "stdafx.h"
#include "ChaosMachineManager.h"
#include "..\pugixml\pugixml.hpp"
#include "GameMain.h"
#include "DSProtocol.h"
#include "..\common\winutil.h"
#include "ChaosBox.h"
// -------------------------------------------------------------------------------

using namespace pugi;
ChaosMachineManager g_ChaosMachineManager;
// -------------------------------------------------------------------------------

ChaosRecipe::ChaosRecipe(short RecipeIndex)
{
	this->RecipeIndex = RecipeIndex;
}
// -------------------------------------------------------------------------------

ChaosRecipe::~ChaosRecipe()
{
	this->Init();
}
// -------------------------------------------------------------------------------

void ChaosRecipe::Init()
{
	this->Combinations.clear();

	if (this->Combinations.capacity() > 0)
	{
		std::vector<ChaosCombination>().swap(this->Combinations);
	}

	this->RecipeIndex = -1;
}
// -------------------------------------------------------------------------------

bool ChaosRecipe::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);

	if (Result.status != status_ok)
	{
		MsgBox("[ChaosRecipe] File %s not found!", File);
		return false;
	}

	xml_node Recipe	= Document.child("recipe");
	strncpy(this->Name, Recipe.attribute("name").as_string(), sizeof(this->Name));

	for (xml_node CombinationNode = Recipe.child("combination"); CombinationNode; CombinationNode = CombinationNode.next_sibling())
	{
		ChaosCombination lpCombination;
		strncpy(lpCombination.Name, CombinationNode.attribute("name").as_string(), sizeof(lpCombination.Name));
		lpCombination.Money = CombinationNode.child("money").text().as_int(0);
		lpCombination.SuccessRate = CombinationNode.child("successrate").text().as_int(0);
		
		for (xml_node IngredientNode = CombinationNode.child("ingredients").child("item"); IngredientNode; IngredientNode = IngredientNode.next_sibling())
		{
			ChaosIngredient lpIngredient = { 0 };
			lpIngredient.TypeStart = IngredientNode.attribute("starttype").as_int(0);
			lpIngredient.IndexStart = IngredientNode.attribute("startindex").as_int(0);
			lpIngredient.TypeEnd = IngredientNode.attribute("endtype").as_int(0);
			lpIngredient.IndexEnd = IngredientNode.attribute("endindex").as_int(0);
			lpIngredient.LevelMin = (BYTE)IngredientNode.attribute("minlevel").as_int(0);
			lpIngredient.LevelMax = (BYTE)IngredientNode.attribute("maxlevel").as_int(0);
			lpIngredient.OptionMin = IngredientNode.attribute("minoption").as_int(0);
			lpIngredient.OptionMax = IngredientNode.attribute("maxoption").as_int(0);
			lpIngredient.DurabilityMin = IngredientNode.attribute("mindur").as_int(0);
			lpIngredient.DurabilityMax = IngredientNode.attribute("maxdur").as_int(0);
			lpIngredient.CountMin = IngredientNode.attribute("mincount").as_int(0);
			lpIngredient.CountMax = IngredientNode.attribute("maxcount").as_int(0);
			strncpy(lpIngredient.Tag, CombinationNode.attribute("tag").as_string(), sizeof(lpIngredient.Tag));
			lpCombination.Ingredients.push_back(lpIngredient);
		}

		for (xml_node TalismanNode = CombinationNode.child("talismans").child("item"); TalismanNode; TalismanNode = TalismanNode.next_sibling())
		{
			ChaosTalisman lpTalisman = { 0 };
			lpTalisman.TypeStart = TalismanNode.attribute("starttype").as_int(0);
			lpTalisman.IndexStart = TalismanNode.attribute("startindex").as_int(0);
			lpTalisman.TypeEnd = TalismanNode.attribute("endtype").as_int(0);
			lpTalisman.IndexEnd = TalismanNode.attribute("endindex").as_int(0);
			lpTalisman.LevelMin = (BYTE)TalismanNode.attribute("minlevel").as_int(0);
			lpTalisman.LevelMax = (BYTE)TalismanNode.attribute("maxlevel").as_int(0);
			lpTalisman.OptionMin = TalismanNode.attribute("minoption").as_int(0);
			lpTalisman.OptionMax = TalismanNode.attribute("maxoption").as_int(0);
			lpTalisman.DurabilityMin = TalismanNode.attribute("mindur").as_int(0);
			lpTalisman.DurabilityMax = TalismanNode.attribute("maxdur").as_int(0);
			lpTalisman.AdditionalRate = TalismanNode.attribute("addrate").as_int(0);
			lpTalisman.CountMax = TalismanNode.attribute("countmax").as_int(0);
			lpCombination.Talismans.push_back(lpTalisman);
		}

		for (xml_node ProduceNode = CombinationNode.child("produces").child("item"); ProduceNode; ProduceNode = ProduceNode.next_sibling())
		{
			ChaosProduce lpProduce = { 0 };
			lpProduce.Type = ProduceNode.attribute("type").as_int(0);
			lpProduce.Index = ProduceNode.attribute("index").as_int(0);
			lpProduce.LevelMin = ProduceNode.attribute("minlevel").as_int(0);
			lpProduce.LevelMax = ProduceNode.attribute("maxlevel").as_int(0);
			lpProduce.OptionMin = ProduceNode.attribute("minoption").as_int(0);
			lpProduce.OptionMax = ProduceNode.attribute("maxoption").as_int(0);
			lpProduce.Skill = ProduceNode.attribute("skill").as_int(0);
			lpProduce.Luck = ProduceNode.attribute("luck").as_int(0);
			lpProduce.ExcellentMin = ProduceNode.attribute("excmin").as_int(0);
			lpProduce.ExcellentMax = ProduceNode.attribute("excmax").as_int(0);
			lpProduce.Ancient = ProduceNode.attribute("ancient").as_int(0);
			lpProduce.SocketMin = ProduceNode.attribute("sockmin").as_int(0);
			lpProduce.SocketMax = ProduceNode.attribute("sockmax").as_int(0);
			lpProduce.Durability = ProduceNode.attribute("dur").as_int(0);
			lpCombination.Produces.push_back(lpProduce);
		}

		this->Combinations.push_back(lpCombination);
	}

	return true;
}
// -------------------------------------------------------------------------------

void ChaosRecipe::ActStart(LPOBJ lpUser, ChaosRecipe* lpRecipe, ChaosCombination* lpCombination)
{
	lpUser->ChaosSuccessRate += lpCombination->SuccessRate;

	int ProduceIndex = rand() % lpCombination->Produces.size();
	ChaosProduce* lpProduce = &lpCombination->Produces[ProduceIndex];

	if (lpProduce == NULL)
	{
		LogAddC(2, "[DEBUG] Produce not found");
		return;
	}
	
	if (lpUser->Money < lpCombination->Money)
	{
		ChaosRecipe::ActFail(lpUser, lpRecipe, lpCombination, CB_NOT_ENOUGH_ZEN);
		return;
	}

	DWORD Random = rand() % 100;
	lpUser->ChaosLock = true;
	lpUser->Money -= lpCombination->Money;
	GCMoneySend(lpUser->m_Index, lpUser->Money);

	if (Random > lpUser->ChaosSuccessRate)
	{
		ChaosRecipe::ActFail(lpUser, lpRecipe, lpCombination);
	}
	else
	{
		ChaosRecipe::ActSuccess(lpUser, lpRecipe, lpCombination, lpProduce);
	}	
}
// -------------------------------------------------------------------------------

bool ChaosRecipe::ActCheck(LPOBJ lpUser, ChaosRecipe* lpRecipe, ChaosCombination* lpCombination)
{
	return false;
}
// -------------------------------------------------------------------------------

void ChaosRecipe::ActFail(LPOBJ lpUser, ChaosRecipe* lpRecipe, ChaosCombination* lpCombination, BYTE ErrorCode)
{
	PMSG_CHAOSMIXRESULT lpResult;
	PHeadSetB((LPBYTE)&lpResult.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	lpResult.Result = ErrorCode;
	
//	ChaosBoxWingMixItemDown(lpObj);
	g_ChaosBox.ChaosBoxInit(lpUser);
	GCUserChaosBoxSend(lpUser, 0);
	DataSend(lpUser->m_Index, (LPBYTE)&lpResult, lpResult.h.size);

	lpUser->ChaosLock = false;
	LogAddTD("[ChaosRecipe] [%s] [%s] Combination has failed (Recipe: %s, Combination: %s, Rate: %d)",
		lpUser->AccountID, lpUser->Name, lpRecipe->Name, lpCombination->Name, 
		lpUser->ChaosSuccessRate);
}
// -------------------------------------------------------------------------------

void ChaosRecipe::ActFail(LPOBJ lpUser, ChaosRecipe* lpRecipe, ChaosCombination* lpCombination)
{
	ChaosRecipe::ActFail(lpUser, lpRecipe, lpCombination, CB_ERROR);
}
// -------------------------------------------------------------------------------

void ChaosRecipe::ActSuccess(LPOBJ lpUser, ChaosRecipe* lpRecipe, ChaosCombination* lpCombination, ChaosProduce* lpProduce)
{
	BYTE Level = 0;
	BYTE Durability = 0;
	BYTE Option = 0;

	if (lpProduce->LevelMax == lpProduce->LevelMin)
	{
		Level = lpProduce->LevelMax;
	}
	else if (lpProduce->LevelMin < lpProduce->LevelMax)
	{
		int Sub = (lpProduce->LevelMax - lpProduce->LevelMin) + 1;
		Level = lpProduce->LevelMin + (rand() % Sub);
	}
	else
	{
		Level = lpProduce->LevelMin;
		LogAddTD("[ChaosRecipe] [%s] [%s] (Warning) Produce have wrong level (Recipe: %s, Combination: %s)",
			lpUser->AccountID, lpUser->Name, lpRecipe->Name, lpCombination->Name);
	}

	if (Level > 15)
	{
		Level = 15;
	}

	if (lpProduce->OptionMax == lpProduce->OptionMin )
	{
		Option = lpProduce->OptionMax;
	}
	else if (lpProduce->OptionMin < lpProduce->OptionMax )
	{
		int Sub = (lpProduce->OptionMax - lpProduce->OptionMin) + 1;
		Option = lpProduce->OptionMin + (rand() % Sub);
	}
	else
	{
		Durability = lpProduce->OptionMin;
		LogAddTD("[ChaosRecipe] [%s] [%s] (Warning) Produce have wrong option (Recipe: %s, Combination: %s)",
			lpUser->AccountID, lpUser->Name, lpRecipe->Name, lpCombination->Name);
	}

	if (Option > 7)
	{
		Option = 7;
	}

	ItemSerialCreateSend(lpUser->m_Index, 255, 0, 0, ITEMGET(lpProduce->Type, lpProduce->Index), Level, 
		lpProduce->Durability, lpProduce->Skill, lpProduce->Luck, Option, -1, 0, 0);
	gObjInventoryCommit(lpUser->m_Index);

	LogAddTD("[ChaosRecipe] [%s] [%s] Combination has succeeded (Recipe: %s, Combination: %s, Rate: %d)",
		lpUser->AccountID, lpUser->Name, lpRecipe->Name, lpCombination->Name, lpUser->ChaosSuccessRate);
}
// -------------------------------------------------------------------------------


bool ChaosRecipe::CheckIngredient(LPOBJ lpUser, short ChaosPos, ChaosIngredient* Ingredient)
{
	if (!lpUser->pChaosBox[ChaosPos].IsItem())
	{
		return false;
	}
	
	if (lpUser->pChaosBox[ChaosPos].m_Type < ITEMGET(Ingredient->TypeStart, Ingredient->IndexStart)
		|| lpUser->pChaosBox[ChaosPos].m_Type > ITEMGET(Ingredient->TypeEnd, Ingredient->IndexEnd))
	{
		return false;
	}

	if (lpUser->pChaosBox[ChaosPos].m_Level < Ingredient->LevelMin && Ingredient->LevelMin != (BYTE)-1
		|| lpUser->pChaosBox[ChaosPos].m_Level > Ingredient->LevelMax && Ingredient->LevelMax != (BYTE)-1)
	{
		return false;
	}

	if (lpUser->pChaosBox[ChaosPos].m_Option3 < Ingredient->OptionMin && Ingredient->OptionMin != (BYTE)-1
		|| lpUser->pChaosBox[ChaosPos].m_Option3 > Ingredient->OptionMax && Ingredient->OptionMax != (BYTE)-1)
	{
		return false;
	}

	if (lpUser->pChaosBox[ChaosPos].m_Durability < Ingredient->DurabilityMin && Ingredient->DurabilityMin != (BYTE)-1
		|| lpUser->pChaosBox[ChaosPos].m_Durability > Ingredient->DurabilityMax && Ingredient->DurabilityMax != (BYTE)-1)
	{
		return false;
	}

	return true;
}
// -------------------------------------------------------------------------------

bool ChaosRecipe::CheckTalisman(LPOBJ lpUser, short ChaosPos, ChaosTalisman* lpTalisman)
{
	if (!lpUser->pChaosBox[ChaosPos].IsItem())
	{
		return false;
	}

	if (lpUser->pChaosBox[ChaosPos].m_Type < ITEMGET(lpTalisman->TypeStart, lpTalisman->IndexStart)
		|| lpUser->pChaosBox[ChaosPos].m_Type > ITEMGET(lpTalisman->TypeEnd, lpTalisman->IndexEnd))
	{
		return false;
	}

	if (lpUser->pChaosBox[ChaosPos].m_Level < lpTalisman->LevelMin && lpTalisman->LevelMin != (BYTE)-1
		|| lpUser->pChaosBox[ChaosPos].m_Level > lpTalisman->LevelMax && lpTalisman->LevelMax != (BYTE)-1)
	{
		return false;
	}

	if (lpUser->pChaosBox[ChaosPos].m_Option3 < lpTalisman->OptionMin && lpTalisman->OptionMin != (BYTE)-1
		|| lpUser->pChaosBox[ChaosPos].m_Option3 > lpTalisman->OptionMax && lpTalisman->OptionMax != (BYTE)-1)
	{
		return false;
	}

	if (lpUser->pChaosBox[ChaosPos].m_Durability < lpTalisman->DurabilityMin && lpTalisman->DurabilityMin != (BYTE)-1
		|| lpUser->pChaosBox[ChaosPos].m_Durability > lpTalisman->DurabilityMax && lpTalisman->DurabilityMax != (BYTE)-1)
	{
		return false;
	}

	return true;
}
// -------------------------------------------------------------------------------

bool ChaosRecipe::ApplyTalisman(LPOBJ lpUser, short ChaosPos, ChaosRecipe* lpRecipe, ChaosCombination* lpCombination, ChaosTalisman* lpTalisman)
{
	if (!lpUser->pChaosBox[ChaosPos].IsItem())
	{
		return false;
	}

	if (lpUser->pChaosBox[ChaosPos].m_Type < ITEMGET(lpTalisman->TypeStart, lpTalisman->IndexStart)
		|| lpUser->pChaosBox[ChaosPos].m_Type > ITEMGET(lpTalisman->TypeEnd, lpTalisman->IndexEnd))
	{
		return false;
	}

	if (lpUser->pChaosBox[ChaosPos].m_Level < lpTalisman->LevelMin && lpTalisman->LevelMin != (BYTE)-1
		|| lpUser->pChaosBox[ChaosPos].m_Level > lpTalisman->LevelMax && lpTalisman->LevelMax != (BYTE)-1)
	{
		return false;
	}

	if (lpUser->pChaosBox[ChaosPos].m_Option3 < lpTalisman->OptionMin && lpTalisman->OptionMin != (BYTE)-1
		|| lpUser->pChaosBox[ChaosPos].m_Option3 > lpTalisman->OptionMax && lpTalisman->OptionMax != (BYTE)-1)
	{
		return false;
	}

	if (lpUser->pChaosBox[ChaosPos].m_Durability < lpTalisman->DurabilityMin && lpTalisman->DurabilityMin != (BYTE)-1
		|| lpUser->pChaosBox[ChaosPos].m_Durability > lpTalisman->DurabilityMax && lpTalisman->DurabilityMax != (BYTE)-1)
	{
		return false;
	}

	BYTE RateIncrease = 0;
	
	if (lpTalisman->AdditionalRate == (BYTE)-1)
	{
		RateIncrease += lpUser->pChaosBox[ChaosPos].m_Durability;
	}
	else
	{
		RateIncrease += lpTalisman->AdditionalRate;
	}

	lpUser->ChaosSuccessRate += RateIncrease;
	LogAddTD("[ChaosRecipe] [%s] [%s] Talisman has been applied (Recipe: %s, Combination: %s, Talisman: %d-%d, Increase: %d)",
		lpUser->AccountID, lpUser->Name, lpRecipe->Name, lpCombination->Name, 
		lpUser->pChaosBox[ChaosPos].m_Type / 512, lpUser->pChaosBox[ChaosPos].m_Type % 512, RateIncrease);
	return true;
}
// -------------------------------------------------------------------------------

ChaosMachineManager::ChaosMachineManager()
{

}
// -------------------------------------------------------------------------------

ChaosMachineManager::~ChaosMachineManager()
{

}
// -------------------------------------------------------------------------------

void ChaosMachineManager::Init()
{
	this->Recipes.clear();

	if (this->Recipes.capacity() > 0)
	{
		std::vector<ChaosRecipe*>().swap(this->Recipes);
	}
}
// -------------------------------------------------------------------------------

void ChaosMachineManager::Load()
{
	this->Init();
	this->Read(gDirPath.GetNewPath(FILE_CHAOSMACHINE));
}
// -------------------------------------------------------------------------------

void ChaosMachineManager::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);

	if (Result.status != status_ok)
	{
		MsgBox("[ChaosMachineManager] File %s not found!", File);
		return;
	}

	xml_node Recipe	= Document.child("chaosmachine");

	for (xml_node CombinationNode = Recipe.child("mixlist").child("recipe"); CombinationNode; CombinationNode = CombinationNode.next_sibling())
	{
		char FileName[MAX_PATH] = { 0 };
		sprintf(FileName, "%s%s", gDirPath.GetNewPath(PATH_CHAOSMACHINE), CombinationNode.text().as_string());

		ChaosRecipe* lpRecipe = new ChaosRecipe(CombinationNode.attribute("index").as_int(0));
		
		if (!lpRecipe->Read(FileName))
		{
			continue;
		}

		this->Recipes.push_back(lpRecipe);
	}
}
// -------------------------------------------------------------------------------

bool ChaosMachineManager::StartMix(LPOBJ lpUser, short RecipeIndex)
{
	lpUser->ChaosSuccessRate = 0;

	for (int Recipe = 0; Recipe < this->Recipes.size(); Recipe++)
	{
		if (this->Recipes[Recipe]->RecipeIndex == RecipeIndex)
		{
			int ActiveCombination = -1;

			//Select active combination
			for (int Combination = 0; Combination < this->Recipes[Recipe]->Combinations.size(); Combination++)
			{
				BYTE ItemCounter[CHAOS_BOX_SIZE] = { 0 };
				BYTE TalismanCounter[CHAOS_BOX_SIZE] = { 0 };

				for (int Pos = 0; Pos < CHAOS_BOX_SIZE; Pos++)
				{
					if (!lpUser->pChaosBox[Pos].IsItem())
					{
						continue;
					}

					for (int n = 0; n < this->Recipes[Recipe]->Combinations[Combination].Ingredients.size(); n++)
					{
						if (n >= CHAOS_BOX_SIZE-1)
						{
							break;
						}

						ChaosIngredient* lpIngredient = &this->Recipes[Recipe]->Combinations[Combination].Ingredients[n];

						if (ChaosRecipe::CheckIngredient(lpUser, Pos, lpIngredient))
						{
							ItemCounter[n]++;
						}
					}
				}

				bool CountRange = true;

				for (int n = 0; n < this->Recipes[Recipe]->Combinations[Combination].Ingredients.size(); n++)
				{
					if (ItemCounter[n] < this->Recipes[Recipe]->Combinations[Combination].Ingredients[n].CountMin
						|| ItemCounter[n] > this->Recipes[Recipe]->Combinations[Combination].Ingredients[n].CountMax)
					{
						LogAddC(2, "[DEBUG] Wrong range (%d:%d, %d/%d:%d)",
							this->Recipes[Recipe]->Combinations[Combination].Ingredients[n].TypeStart,
							this->Recipes[Recipe]->Combinations[Combination].Ingredients[n].IndexStart,
							ItemCounter[n],
							this->Recipes[Recipe]->Combinations[Combination].Ingredients[n].CountMin,
							this->Recipes[Recipe]->Combinations[Combination].Ingredients[n].CountMax);
						CountRange = false;
						break;
					}
				}
				
				if (CountRange)
				{
					ActiveCombination = Combination;
					break;
				}
			}

			if (ActiveCombination == -1)
			{
				LogAddC(2, "[DEBUG] Combination not found");
				return false;
			}

			ChaosCombination* lpCombination = &this->Recipes[Recipe]->Combinations[ActiveCombination];

			if (lpCombination->Produces.size() <= 0)
			{
				LogAddC(2, "[DEBUG] Produces list not filled");
				return false;
			}

			//Talisman effect apply
			BYTE TalismanCounter[CHAOS_BOX_SIZE] = { 0 };

			for (int Pos = 0; Pos < CHAOS_BOX_SIZE; Pos++)
			{
				if (!lpUser->pChaosBox[Pos].IsItem())
				{
					continue;
				}

				for (int n = 0; n < this->Recipes[Recipe]->Combinations[ActiveCombination].Talismans.size(); n++)
				{
					if (n >= CHAOS_BOX_SIZE-1)
					{
						break;
					}

					ChaosTalisman* lpTalisman = &this->Recipes[Recipe]->Combinations[ActiveCombination].Talismans[n];

					if (ChaosRecipe::CheckTalisman(lpUser, Pos, lpTalisman))
					{
						if (TalismanCounter[n] < lpTalisman->CountMax)
						{
							ChaosRecipe::ApplyTalisman(lpUser, Pos, this->Recipes[Recipe], 
								&this->Recipes[Recipe]->Combinations[ActiveCombination], lpTalisman);
							TalismanCounter[n]++;
						}
						else
						{
							ChaosRecipe::ActFail(lpUser, this->Recipes[Recipe], &this->Recipes[Recipe]->Combinations[ActiveCombination], 0xF0);
							return 0;
						}
					}
				}
			}

			ChaosRecipe::ActStart(lpUser, this->Recipes[Recipe], lpCombination);
			return true;
		}
	}
	return false;
}
// -------------------------------------------------------------------------------