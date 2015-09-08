#include "stdafx.h"
#include "GameMain.h"
#include "ResetSystem.h"
#include "..\pugixml\pugixml.hpp"
#include "MasterSkillSystem.h"
#include "MasterLevelSkillTreeSystem.h"
#include "QuestInfo.h"
#include "Gate.h"
#include "dirpath.h"

using namespace pugi;
ResetSystem g_ResetSystem;

ResetSystem::ResetSystem()
{
	this->Log = new CLogToFile(LOG_NAME_RESET, LOG_PATH_RESET, TRUE);
	this->Load();
}

ResetSystem::~ResetSystem()
{
	delete this->Log;
}

void ResetSystem::Init()
{
	this->m_NormalInfo.m_GroupList.clear();
	if( this->m_NormalInfo.m_GroupList.capacity() > 0 )
	{
		std::vector<ResetGroup>().swap(this->m_NormalInfo.m_GroupList);
	}
	this->m_GrandInfo.m_GroupList.clear();
	if( this->m_GrandInfo.m_GroupList.capacity() > 0 )
	{
		std::vector<ResetGroup>().swap(this->m_GrandInfo.m_GroupList);
	}
}

void ResetSystem::Load()
{
	this->Init();
	CDirPath path;
	path.SetFirstPath(FOLDER_DATA);

	this->Read(path.GetNewPath(FILE_CUSTOM_RESETSYSTEM));
}

void ResetSystem::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);

	if (Result.status != status_ok)
	{
		MsgBox("[ResetSystem] File %s not found!", File);
		return;
	}

	xml_node ResetSystem = Document.child("resetsystem");
	xml_node Node, SubNode, Category, GroupList, GroupSub, RewardNode, ItemNode;

	int ClassCounter = 0;

	Category = ResetSystem.child("normal");

	this->m_NormalInfo.m_Enabled = Category.child("enabled").text().as_int(0);

	this->m_NormalInfo.m_NPC.Class				= Category.child("npc").attribute("id").as_int(0);
	this->m_NormalInfo.m_NPC.MapNumber			= Category.child("npc").attribute("map").as_int(0);
	this->m_NormalInfo.m_NPC.X					= Category.child("npc").attribute("x").as_int(0);
	this->m_NormalInfo.m_NPC.Y					= Category.child("npc").attribute("y").as_int(0);

	GroupList = Category.child("grouplist");

	for (Node = GroupList.child("group"); Node; Node = Node.next_sibling())
	{
		ResetGroup NewGroup;

		NewGroup.MinLevel = Node.attribute("minlevel").as_int(0);
		NewGroup.MaxLevel = Node.attribute("maxlevel").as_int(0);
		NewGroup.MinReset = Node.attribute("minreset").as_int(0);
		NewGroup.MaxReset = Node.attribute("maxreset").as_int(0);
		NewGroup.Money = Node.attribute("money").as_int(0);
		NewGroup.WCoinC = Node.attribute("wcoinc").as_int(0);
		NewGroup.WCoinP = Node.attribute("wcoinp").as_int(0);
		NewGroup.WCoinG = Node.attribute("wcoing").as_int(0); 
		NewGroup.GensP = Node.attribute("gensp").as_int(0);
		NewGroup.Credits = Node.attribute("credits").as_int(0);
		NewGroup.Delay = Node.attribute("delay").as_int(0);

	//	for (SubNode = Node.child("group"); SubNode; SubNode = SubNode.next_sibling())
//		{
			NewGroup.Cleaning.Stats			= Node.child("cleaning").child("stats").text().as_int(0);
			NewGroup.Cleaning.Points		= Node.child("cleaning").child("points").text().as_int(0);
			NewGroup.Cleaning.Skills		= Node.child("cleaning").child("skills").text().as_int(0);
			NewGroup.Cleaning.Equipment		= Node.child("cleaning").child("equipment").text().as_int(0);
			NewGroup.Cleaning.Inventory		= Node.child("cleaning").child("inventory").text().as_int(0);
			NewGroup.Cleaning.Shop			= Node.child("cleaning").child("shop").text().as_int(0);
			NewGroup.Cleaning.MasterLevel	= Node.child("cleaning").child("masterlevel").text().as_int(0);
			NewGroup.Cleaning.MasterPoints	= Node.child("cleaning").child("masterpoints").text().as_int(0);
			NewGroup.Cleaning.MasterSkills	= Node.child("cleaning").child("masterskills").text().as_int(0);
			NewGroup.Cleaning.MarlonQuest	= Node.child("cleaning").child("marlonquest").text().as_int(0);

			NewGroup.Checks.Equipment		= Node.child("checks").child("equipment").text().as_int(0);
			NewGroup.Checks.Inventory		= Node.child("checks").child("inventory").text().as_int(0);
			NewGroup.Checks.Shop			= Node.child("checks").child("shop").text().as_int(0);

			GroupSub = Node.child("reward");
			NewGroup.PointMode = GroupSub.attribute("pointmode").as_int(0);

			for (RewardNode = GroupSub.child("class"); RewardNode; RewardNode = RewardNode.next_sibling())
			{
				int ClassCode = RewardNode.attribute("id").as_int(-1);

				if (ClassCode < 0 || ClassCode > 6)
				{
					MsgBox("[ResetSystem] Syntax error in reward node, param: id (%d)", ClassCode);
					this->Log->Output("[ResetSystem] Syntax error in reward node, param: id (%d)", ClassCode);
					continue;
				}

				NewGroup.RewardList[ClassCode].Class = ClassCode;
				NewGroup.RewardList[ClassCode].Point = RewardNode.attribute("point").as_int(0);
				NewGroup.RewardList[ClassCode].WCoinC = RewardNode.attribute("wcoinc").as_int(0);
				NewGroup.RewardList[ClassCode].WCoinP = RewardNode.attribute("wcoinp").as_int(0);
				NewGroup.RewardList[ClassCode].WCoinG = RewardNode.attribute("wcoing").as_int(0);
				NewGroup.RewardList[ClassCode].GensP = RewardNode.attribute("gensp").as_int(0);
				NewGroup.RewardList[ClassCode].Credits = RewardNode.attribute("wcoing").as_int(0);
			}

			GroupSub = Node.child("itemslist");
			
			for (ItemNode = GroupSub.child("item"); ItemNode; ItemNode = ItemNode.next_sibling())
			{
				ResetItem NewItem;
				NewItem.Type = ItemNode.attribute("type").as_int(0);
				NewItem.Index = ItemNode.attribute("index").as_int(0);
				NewItem.MinLevel = ItemNode.attribute("minlvl").as_int(0);
				NewItem.MaxLevel = ItemNode.attribute("maxlvl").as_int(0);
				NewItem.MinDurability = ItemNode.attribute("mindur").as_int(0);
				NewItem.MaxDurability = ItemNode.attribute("maxdur").as_int(0);
				NewItem.MinOption = ItemNode.attribute("minopt").as_int(0);
				NewItem.MaxOption = ItemNode.attribute("maxopt").as_int(0);
				NewItem.Luck = ItemNode.attribute("luck").as_int(0);
				NewItem.Skill = ItemNode.attribute("skill").as_int(0);
				NewItem.Excellent = ItemNode.attribute("excellent").as_int(-1);
				NewItem.Ancient = ItemNode.attribute("ancient").as_int(-1);
				NewItem.Socket = ItemNode.attribute("socket").as_int(-1);
				NewItem.Element = ItemNode.attribute("element").as_int(-1);
				NewItem.Count = ItemNode.attribute("count").as_int(0);
				
				if (IsValidItemNode(NewItem))
				{
					NewGroup.ItemList.push_back(NewItem);
				}
			}
	//	}

		m_NormalInfo.m_GroupList.push_back(NewGroup);
	}

	Category = ResetSystem.child("grand");

	this->m_GrandInfo.m_Enabled	= Category.child("enabled").text().as_int(0);

	this->m_GrandInfo.m_NPC.Class				= Category.child("npc").attribute("id").as_int(0);
	this->m_GrandInfo.m_NPC.MapNumber			= Category.child("npc").attribute("map").as_int(0);
	this->m_GrandInfo.m_NPC.X					= Category.child("npc").attribute("x").as_int(0);
	this->m_GrandInfo.m_NPC.Y					= Category.child("npc").attribute("y").as_int(0);

	GroupList = Category.child("grouplist");

	for (Node = GroupList.child("group"); Node; Node = Node.next_sibling())
	{
		ResetGroup NewGroup;

		NewGroup.MinLevel = Node.attribute("minlevel").as_int(0);
		NewGroup.MaxLevel = Node.attribute("maxlevel").as_int(0);
		NewGroup.MinReset = Node.attribute("minreset").as_int(0);
		NewGroup.MaxReset = Node.attribute("maxreset").as_int(0);
		NewGroup.Money = Node.attribute("money").as_int(0);
		NewGroup.WCoinC = Node.attribute("wcoinc").as_int(0);
		NewGroup.WCoinP = Node.attribute("wcoinp").as_int(0);
		NewGroup.WCoinG = Node.attribute("wcoing").as_int(0);
		NewGroup.GensP = Node.attribute("gensp").as_int(0);
		NewGroup.Credits = Node.attribute("credits").as_int(0);
		NewGroup.Delay = Node.attribute("delay").as_int(0);

	//	for (SubNode = Node.child("group"); SubNode; SubNode = SubNode.next_sibling())
	//	{
			NewGroup.Cleaning.Stats			= Node.child("cleaning").child("stats").text().as_int(0);
			NewGroup.Cleaning.Points		= Node.child("cleaning").child("points").text().as_int(0);
			NewGroup.Cleaning.Skills		= Node.child("cleaning").child("skills").text().as_int(0);
			NewGroup.Cleaning.Equipment		= Node.child("cleaning").child("equipment").text().as_int(0);
			NewGroup.Cleaning.Inventory		= Node.child("cleaning").child("inventory").text().as_int(0);
			NewGroup.Cleaning.Shop			= Node.child("cleaning").child("shop").text().as_int(0);
			NewGroup.Cleaning.MasterLevel	= Node.child("cleaning").child("masterlevel").text().as_int(0);
			NewGroup.Cleaning.MasterPoints	= Node.child("cleaning").child("masterpoints").text().as_int(0);
			NewGroup.Cleaning.MasterSkills	= Node.child("cleaning").child("masterskills").text().as_int(0);
			NewGroup.Cleaning.MarlonQuest	= Node.child("cleaning").child("marlonquest").text().as_int(0);

			NewGroup.Checks.Equipment		= Node.child("checks").child("equipment").text().as_int(0);
			NewGroup.Checks.Inventory		= Node.child("checks").child("inventory").text().as_int(0);
			NewGroup.Checks.Shop			= Node.child("checks").child("shop").text().as_int(0);

			GroupSub = Node.child("reward");

			NewGroup.PointMode = GroupSub.attribute("pointmode").as_int(0);

			for (RewardNode = GroupSub.child("class"); RewardNode; RewardNode = RewardNode.next_sibling())
			{
				int ClassCode = RewardNode.attribute("id").as_int(-1);

				if (ClassCode < 0 || ClassCode > 6)
				{
					MsgBox("[ResetSystem] Syntax error in reward node, param: id (%d)", ClassCode);
					this->Log->Output("[ResetSystem] Syntax error in reward node, param: id (%d)", ClassCode);
					continue;
				}
				
				NewGroup.RewardList[ClassCode].Class = ClassCode;
				NewGroup.RewardList[ClassCode].Point = RewardNode.attribute("point").as_int(0);
				NewGroup.RewardList[ClassCode].WCoinC = RewardNode.attribute("wcoinc").as_int(0);
				NewGroup.RewardList[ClassCode].WCoinP = RewardNode.attribute("wcoinp").as_int(0);
				NewGroup.RewardList[ClassCode].WCoinG = RewardNode.attribute("wcoing").as_int(0);
				NewGroup.RewardList[ClassCode].GensP = RewardNode.attribute("gensp").as_int(0);
				NewGroup.RewardList[ClassCode].Credits = RewardNode.attribute("wcoing").as_int(0);
			}

			GroupSub = SubNode.child("itemslist");
			
			for (ItemNode = GroupSub.child("item"); ItemNode; ItemNode = ItemNode.next_sibling())
			{
				ResetItem NewItem;
				NewItem.Type = ItemNode.attribute("type").as_int(0);
				NewItem.Index = ItemNode.attribute("index").as_int(0);
				NewItem.MinLevel = ItemNode.attribute("minlvl").as_int(0);
				NewItem.MaxLevel = ItemNode.attribute("maxlvl").as_int(0);
				NewItem.MinDurability = ItemNode.attribute("mindur").as_int(0);
				NewItem.MaxDurability = ItemNode.attribute("maxdur").as_int(0);
				NewItem.MinOption = ItemNode.attribute("minopt").as_int(0);
				NewItem.MaxOption = ItemNode.attribute("maxopt").as_int(0);
				NewItem.Luck = ItemNode.attribute("luck").as_int(0);
				NewItem.Skill = ItemNode.attribute("skill").as_int(0);
				NewItem.Excellent = ItemNode.attribute("excellent").as_int(-1);
				NewItem.Ancient = ItemNode.attribute("ancient").as_int(-1);
				NewItem.Socket = ItemNode.attribute("socket").as_int(-1);
				NewItem.Element = ItemNode.attribute("element").as_int(-1);
				NewItem.Count = ItemNode.attribute("count").as_int(0);
				
				if (IsValidItemNode(NewItem))
				{
					NewGroup.ItemList.push_back(NewItem);
				}
			}
	//	}

		m_GrandInfo.m_GroupList.push_back(NewGroup);
	}

	this->Log->Output("Configuration loaded (total normal groups: %d, total grand groups: %d)", 
		m_NormalInfo.m_GroupList.size(), m_GrandInfo.m_GroupList.size());
}

bool ResetSystem::IsValidItemNode(ResetItem NewItem)
{
    if (NewItem.Type < 0 || NewItem.Type > 15)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: type (%d)", NewItem.Type);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: type (%d)", NewItem.Type);
        return false;
    }
    
    if (NewItem.Index < 0 || NewItem.Index > 512)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: index (%d)", NewItem.Index);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: index (%d)", NewItem.Index);
        return false;
    }
    
    if (NewItem.MinLevel < 0 || NewItem.MaxLevel > 15)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: minlvl (%d)", NewItem.MinLevel);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: minlvl (%d)", NewItem.MinLevel);
        return false;
    }
    
    if (NewItem.MaxLevel < 0 || NewItem.MaxLevel > 15)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: maxlvl (%d)", NewItem.MaxLevel);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: maxlvl (%d)", NewItem.MaxLevel);
        return false;
    }
    
    if (NewItem.MinDurability < 0 || NewItem.MinDurability > 255)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: mindur (%d)", NewItem.MinDurability);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: mindur (%d)", NewItem.MinDurability);
        return false;
    }
    
    if (NewItem.MaxDurability < 0 || NewItem.MaxDurability > 255)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: maxdur (%d)", NewItem.MaxDurability);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: maxdur (%d)", NewItem.MaxDurability);
        return false;
    }
    
    if (NewItem.MinOption < 0 || NewItem.MinOption > 7)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: minopt (%d)", NewItem.MinOption);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: minopt (%d)", NewItem.MinOption);
        return false;
    }
    
    if (NewItem.MaxOption < 0 || NewItem.MaxOption > 7)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: maxopt (%d)", NewItem.MaxOption);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: maxopt (%d)", NewItem.MaxOption);
        return false;
    }

	if (NewItem.Luck < -1 || NewItem.Luck > 1)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: luck (%d)", NewItem.Luck);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: luck (%d)", NewItem.Luck);
        return false;
    }

	if (NewItem.Skill < -1 || NewItem.Skill > 1)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: luck (%d)", NewItem.Luck);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: luck (%d)", NewItem.Luck);
        return false;
    }

	if (NewItem.Excellent < -1 || NewItem.Excellent > 63)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: excellent (%d)", NewItem.Excellent);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: excellent (%d)", NewItem.Excellent);
        return false;
    }

	if (NewItem.Ancient < -1 || NewItem.Ancient > 2)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: ancient (%d)", NewItem.Ancient);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: ancient (%d)", NewItem.Ancient);
        return false;
    }
	
	if (NewItem.Socket < -1 || NewItem.Socket > 5)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: socket (%d)", NewItem.Socket);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: socket (%d)", NewItem.Socket);
        return false;
    }

	if (NewItem.Element < -1 || NewItem.Element > 5)
    {
        MsgBox("[ResetSystem] Syntax error in item node, param: element (%d)", NewItem.Element);
        this->Log->Output("[ResetSystem] Syntax error in item node, param: element (%d)", NewItem.Element);
        return false;
    }

	return true;
}

bool ResetSystem::ProcDialog(LPOBJ lpUser, LPOBJ lpNpc)
{
	if( this->m_NormalInfo.m_Enabled )
	{
		if(		this->m_NormalInfo.m_NPC.Class == lpNpc->Class
			&&	this->m_NormalInfo.m_NPC.MapNumber == lpNpc->MapNumber
			&&	this->m_NormalInfo.m_NPC.X == lpNpc->X
			&&	this->m_NormalInfo.m_NPC.Y == lpNpc->Y )
		{
			this->GC_OpenDialog(lpUser, ResetType::Normal);
			return true;
		}
	}

	if( this->m_GrandInfo.m_Enabled )
	{
		if(		this->m_GrandInfo.m_NPC.Class == lpNpc->Class
			&&	this->m_GrandInfo.m_NPC.MapNumber == lpNpc->MapNumber
			&&	this->m_GrandInfo.m_NPC.X == lpNpc->X
			&&	this->m_GrandInfo.m_NPC.Y == lpNpc->Y )
		{
			this->GC_OpenDialog(lpUser, ResetType::Grand);
			return true;
		}
	}

	return false;
}

bool ResetSystem::ProcReset(LPOBJ lpUser, ResetType::T Type, BYTE PayType, BYTE ProcWait)
{
	PacketAnswerStart lpAnswer;
	lpAnswer.Head.set((LPBYTE)&lpAnswer, 0xFE, 0x06, sizeof(lpAnswer));
	lpAnswer.Type = (BYTE)Type;
	lpAnswer.PayType = PayType;
	//lpAnswer.WaitProc = lpUser->m_ResetWait;
	lpAnswer.Result = true;

	ResetGroup* ResetGroup = GetResetGroup(lpUser, Type);

	if (ResetGroup == NULL)
	{
		this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset - logoutput) Group for this user not found",
			lpUser->AccountID, lpUser->Name);
		LogAddC(2, "[ResetSystem] [%s] [%s] (ProcReset - logc) Group for this user not found",
			lpUser->AccountID, lpUser->Name);
		//lpAnswer.Result = false;
		//DataSend(lpUser->m_Index, (LPBYTE)&lpAnswer, lpAnswer.Head.size);
		return false;
	}
	
	if (!this->CheckRequirements(lpUser, ResetGroup, Type, true, PayType))
	{
		//lpAnswer.Result = false;
		//DataSend(lpUser->m_Index, (LPBYTE)&lpAnswer, lpAnswer.Head.size);
		return false;
	}

	if (PayType == 0 && lpUser->m_ResetWait == 0)
	{
		lpUser->m_ResetWaitTick = GetTickCount();
		lpUser->m_ResetWait = 1;
	}

	if (PayType == 0 && lpUser->m_ResetWait == 1)
	{
		DWORD RemainMin = (GetTickCount() - lpUser->m_ResetWaitTick) / 1000 / 60;

		if (RemainMin >= ResetGroup->Delay)
		{
			lpUser->m_ResetWait = 2;
		}
		else
		{
			this->GC_MessageBox(lpUser, "Reset Master", "Please wait %d min.", ResetGroup->Delay - RemainMin);
			return false;
		}

	}

	if (PayType == 1 && lpUser->m_ResetWait == 2)
	{
		this->GC_MessageBox(lpUser, "Reset Master", "You have already finished delay");
		return false;
	}

	lpUser->m_ResetWaitTick = 0;
	lpUser->m_ResetWait = 0;

	this->GC_MessageBox(lpUser, "Reset Master", "Reset successful!\nYou now have %d resets\nRedirecting..", lpUser->iResetCount+1);

	int InputReset = lpUser->iResetCount;	

	if (Type == ResetType::Normal)
	{
		InputReset = lpUser->iResetCount;
	}
	else if (Type == ResetType::Grand)
	{
		//InputReset = lpUser->iGrandResetCount;
	}

	if (InputReset < 1)
	{
		InputReset = 1;
	}

	if (PayType == 0)
	{
		lpUser->Money -= this->GetResetMoney(lpUser, ResetGroup, Type, true);
		GCMoneySend(lpUser->m_Index, lpUser->Money);
		this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) Money has been withdrawn, amount: %d", 
			lpUser->AccountID, lpUser->Name, this->GetResetMoney(lpUser, ResetGroup, Type, true));
	}
	else if (PayType == 1)
	{
		lpUser->m_Credits -= ResetGroup->Credits;;
	}
	else if (PayType == 2)
	{

	}


	lpUser->Level		= 1;
	lpUser->Experience	= 0;

	if (ResetGroup->Cleaning.Stats)
	{
		lpUser->Strength	= DCInfo.DefClass[lpUser->Class].Strength;
		lpUser->Dexterity	= DCInfo.DefClass[lpUser->Class].Dexterity;
		lpUser->Energy		= DCInfo.DefClass[lpUser->Class].Energy;
		lpUser->Vitality	= DCInfo.DefClass[lpUser->Class].Vitality;
		lpUser->Leadership	= DCInfo.DefClass[lpUser->Class].Leadership;
		this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) Stats has been reseted", 
			lpUser->AccountID, lpUser->Name);
	}

	if (ResetGroup->Cleaning.Points)
	{
		lpUser->LevelUpPoint = 0;
		this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) Points has been reseted", 
			lpUser->AccountID, lpUser->Name);
	}

	if (ResetGroup->Cleaning.Skills && ResetGroup->Cleaning.MasterSkills )
	{
		for (int n = 0; n < MAGIC_SIZE;n++)
		{
			if (lpUser->Magic[n].IsMagic() == TRUE)
			{
				lpUser->Magic[n].Clear();
			}
		}
		this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) All skills has been reseted", 
			lpUser->AccountID, lpUser->Name);
	}
	else if (ResetGroup->Cleaning.Skills)
	{
		for (int n = 0; n < MAGIC_SIZE;n++)
		{
			if (lpUser->Magic[n].IsMagic() == TRUE && g_MasterSkillSystem.CheckMasterLevelSkill(lpUser->Magic[n].m_Skill) == FALSE)
			{
				lpUser->Magic[n].Clear();
			}
		}
		this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) Normal skills has been reseted", 
			lpUser->AccountID, lpUser->Name);
	}

	if (ResetGroup->Cleaning.Equipment)
	{
		for (int ItemPos = 0; ItemPos < INVETORY_WEAR_SIZE; ItemPos++ )
		{
			if (lpUser->pInventory[ItemPos].IsItem() )
			{
				gObjInventoryItemSet(lpUser->m_Index, ItemPos, -1);
				lpUser->pInventory[ItemPos].Clear();
			}
		}
		this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) Equipment has been reseted", 
			lpUser->AccountID, lpUser->Name);
	}

	if (ResetGroup->Cleaning.Inventory)
	{
		for (int ItemPos = INVETORY_WEAR_SIZE; ItemPos < MAIN_INVENTORY_SIZE; ItemPos++ )
		{
			if (lpUser->pInventory[ItemPos].IsItem() )
			{
				gObjInventoryItemSet(lpUser->m_Index, ItemPos, -1);
				lpUser->pInventory[ItemPos].Clear();
			}
		}
		this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) Inventory has been reseted", 
			lpUser->AccountID, lpUser->Name);
	}

	if (ResetGroup->Cleaning.Shop)
	{
		for (int ItemPos = MAIN_INVENTORY_SIZE; ItemPos < INVENTORY_SIZE; ItemPos++ )
		{
			if (lpUser->pInventory[ItemPos].IsItem() )
			{
				gObjInventoryItemSet(lpUser->m_Index, ItemPos, -1);
				lpUser->pInventory[ItemPos].Clear();
			}
		}
		this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) Shop has been reseted", 
			lpUser->AccountID, lpUser->Name);
	}

	if (ResetGroup->Cleaning.MasterLevel)
	{
		lpUser->MasterLevel = 0;
		lpUser->MasterExperience = 0;
		g_MasterExp.SetNextExp(lpUser);
		this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) MasterLevel has been reseted", 
			lpUser->AccountID, lpUser->Name);
	}
	
	if (ResetGroup->Cleaning.MasterPoints)
	{
		lpUser->MasterPoint = 0;
		this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) MasterPoints has been reseted", 
			lpUser->AccountID, lpUser->Name);
	}

	if (ResetGroup->Cleaning.MarlonQuest)
	{
		g_QuestInfo.ReSetQuestState(lpUser, 2);
		g_QuestInfo.ReSetQuestState(lpUser, 3);
		this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) MarlonQuest has been reseted", 
			lpUser->AccountID, lpUser->Name);
	}
	
	if (Type == ResetType::Normal)
	{
		lpUser->iResetCount++;

		if (ResetGroup->PointMode == 0)
		{
			lpUser->LevelUpPoint += ResetGroup->RewardList[lpUser->Class].Point * lpUser->iResetCount;
			this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) Added %d points", 
				lpUser->AccountID, lpUser->Name, ResetGroup->RewardList[lpUser->Class].Point * lpUser->iResetCount);
		}
		else if (ResetGroup->PointMode == 1)
		{
			lpUser->LevelUpPoint += ResetGroup->RewardList[lpUser->Class].Point;
			this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) Added %d points", 
				lpUser->AccountID, lpUser->Name, ResetGroup->RewardList[lpUser->Class].Point);
		}
	}
	else if (Type == ResetType::Grand)
	{
		//lpUser->iGrandResetCount++;
		lpUser->iResetCount = 0;

		if (ResetGroup->PointMode == 0)
		{
			lpUser->LevelUpPoint += ResetGroup->RewardList[lpUser->Class].Point * (lpUser->iResetCount + 1);
			this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) Added %d points", 
				lpUser->AccountID, lpUser->Name, ResetGroup->RewardList[lpUser->Class].Point * lpUser->iResetCount);
		}
		else if (ResetGroup->PointMode == 1)
		{
			lpUser->LevelUpPoint += ResetGroup->RewardList[lpUser->Class].Point;
			this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) Added %d points", 
				lpUser->AccountID, lpUser->Name, ResetGroup->RewardList[lpUser->Class].Point);
		}
	}

	lpUser->m_GensInfo.iContribution += ResetGroup->RewardList[lpUser->Class].GensP;
	lpUser->m_Credits += ResetGroup->RewardList[lpUser->Class].Credits;

	int Gate = -1;
	BYTE X, Y;
	short Level;
	BYTE MapNumber, Dir;

	if( lpUser->Class == CLASS_ELF )
	{
		Gate = 27;
	}
	else if( lpUser->Class == CLASS_SUMMONER )
	{
		Gate = 267;
	}
	else
	{
		Gate = 17;
	}

	gGateC.GetGate(Gate, (BYTE&)X, (BYTE&)Y, (BYTE&)MapNumber, (BYTE&)Dir, (short&)Level);

	lpUser->MapNumber	= MapNumber;
	lpUser->X			= X;
	lpUser->Y			= Y;
	lpUser->Dir			= Dir;

	gObjCloseSet(lpUser->m_Index, 1);
	
	this->Log->Output("[ResetSystem] [%s] [%s] (ProcReset) Reset has been finished (Reset: %d)", 
		lpUser->AccountID, lpUser->Name, lpUser->iResetCount);

	return true;
}

void ResetSystem::GC_OpenDialog(LPOBJ lpUser, ResetType::T Type)
{
	ResetGroup* Group = this->GetResetGroup(lpUser, Type);

	if (Group == NULL)
	{
		this->Log->Output("[ResetSystem] [%s] [%s] (OpenDialog) Group for this user not found",
			lpUser->AccountID, lpUser->Name);
		LogAddC(2, "[ResetSystem] [%s] [%s] (OpenDialog) Group for this user not found",
			lpUser->AccountID, lpUser->Name);
		return;
	}

	PacketResetDialog lpRequest;

	lpRequest.Head.set((LPBYTE)&lpRequest, 0xFE, 0x04, sizeof(PacketResetDialog));
	lpRequest.Type = (BYTE)Type;
	lpRequest.NextReset = lpUser->iResetCount + 1;
	lpRequest.RewardPoint = Group->RewardList[lpUser->Class].Point * lpRequest.NextReset;
	lpRequest.ReqLevel = Group->MinLevel;
	lpRequest.ReqMoney = Group->Money * lpRequest.NextReset;
	lpRequest.Delay = Group->Delay;
	lpRequest.Credits = Group->Credits;

	if (lpUser->m_Credits < lpRequest.Credits || lpRequest.Credits < 1)
	{
		lpRequest.AllowCredits = false;
	}
	else
	{
		lpRequest.AllowCredits = true;
	}
	
	DataSend(lpUser->m_Index, (LPBYTE)&lpRequest, lpRequest.Head.size);
}

void ResetSystem::GC_MessageBox(LPOBJ lpUser, LPSTR Title, LPSTR Text, ...)
{
	char szBuffer[512] = "";

	va_list pArguments;
	va_start(pArguments, Text);
	vsprintf(szBuffer, Text, pArguments);
	va_end(pArguments);

	PacketResetMessageBox lpRequest = { 0 };
	lpRequest.Head.set((LPBYTE)&lpRequest, 0xFE, 0x05, sizeof(PacketResetMessageBox));
	memcpy(lpRequest.Title, Title, sizeof(lpRequest.Title)-1);
	memcpy(lpRequest.Text, szBuffer, sizeof(lpRequest.Text)-1);

	DataSend(lpUser->m_Index, (LPBYTE)&lpRequest, lpRequest.Head.size);
}

void ResetSystem::CG_RequestStart(int UserIndex, PacketResetStart* lpRequest)
{
	if (!gObjIsConnected(UserIndex))
	{
		return;
	}

	LPOBJ lpUser = &gObj[UserIndex];

	this->ProcReset(lpUser, (ResetType::T)lpRequest->Type, lpRequest->PayType, lpRequest->ProcWait);
}

bool ResetSystem::CheckRequirements(LPOBJ lpUser, ResetGroup* Group, ResetType::T Type, bool NotifyMessage, BYTE PayType)
{
	int ItemPos;

	if (Group->Checks.Equipment)
	{
		for( ItemPos = 0; ItemPos < INVETORY_WEAR_SIZE; ItemPos++ )
		{
			if( lpUser->pInventory[ItemPos].IsItem() )
			{
				this->Log->Output("[ResetSystem] [%s] [%s] (CheckRequirements) Equipment check fail",
					lpUser->AccountID, lpUser->Name);
				if (NotifyMessage)
				{
					this->GC_MessageBox(lpUser, "Reset Master", "Hey %s!\nI think you forgot\nremove equipment...", lpUser->Name);
				}
				return false;
			}
		}
	}

	if (Group->Checks.Inventory)
	{
		for( ItemPos = INVETORY_WEAR_SIZE; ItemPos < MAIN_INVENTORY_SIZE; ItemPos++ )
		{
			if( lpUser->pInventory[ItemPos].IsItem() )
			{
				this->Log->Output("[ResetSystem] [%s] [%s] (CheckRequirements) Inventory check fail",
					lpUser->AccountID, lpUser->Name);
				if (NotifyMessage)
				{
				this->GC_MessageBox(lpUser, "Reset Master", "%s, maybe you forgot\nclean out inventory?", lpUser->Name);
				}
		return false;
			}
		}
	}

	if (Group->Checks.Shop)
	{
		for( ItemPos = MAIN_INVENTORY_SIZE; ItemPos < INVENTORY_SIZE; ItemPos++ )
		{
			if( lpUser->pInventory[ItemPos].IsItem() )
			{
				this->Log->Output("[ResetSystem] [%s] [%s] (CheckRequirements) Shop check fail",
					lpUser->AccountID, lpUser->Name);
				if (NotifyMessage)
				{
				this->GC_MessageBox(lpUser, "Reset Master", "%s, please clean your\npersonal shop.", lpUser->Name);
				}
		return false;
			}
		}
	}

	if (lpUser->Level < Group->MinLevel || lpUser->Level > Group->MaxLevel)
	{
		this->Log->Output("[ResetSystem] [%s] [%s] (CheckRequirements) Level is not valid for this reset (min: %d, max: %d, user: %d)",
			lpUser->AccountID, lpUser->Name, Group->MinLevel, Group->MaxLevel, lpUser->Level);
		if (NotifyMessage)
				{
		this->GC_MessageBox(lpUser, "Reset Master", "Great %s!\nYou need %d level\nfor this reset.", lpUser->Name, Group->MinLevel);
		}
		return false;
	}
	
	if (PayType == 0)
	{
		if (lpUser->Money < this->GetResetMoney(lpUser, Group, Type, true))
		{
			this->Log->Output("[ResetSystem] [%s] [%s] (CheckRequirements) Not have enough Money (need: %d, user: %d)",
				lpUser->AccountID, lpUser->Name, Group->Money, lpUser->Money);
			if (NotifyMessage)
			{
				this->GC_MessageBox(lpUser, "Reset Master", "Hey %s!\nYou need %d Money\nfor this reset.", lpUser->Name, Group->Money * (lpUser->iResetCount + 1));
			}
			return false;
		}	
	}
	else if (PayType == 1)
	{
		if (lpUser->m_Credits < Group->Credits)
	{
		this->Log->Output("[ResetSystem] [%s] [%s] (CheckRequirements) Not have enough Credits (need: %d, user: %d)",
			lpUser->AccountID, lpUser->Name, Group->Credits, lpUser->m_Credits);
		if (NotifyMessage)
				{
		this->GC_MessageBox(lpUser, "Reset Master", "Hello, %s!\nYou need %d Credits for\n this reset. Now you have: %d", 
			lpUser->Name, Group->Credits, lpUser->m_Credits);
		}
		return false;
	}	
	}
	else if (PayType == 2)
	{
		if (lpUser->m_wcCashPoint < Group->WCoinC)
	{
		this->Log->Output("[ResetSystem] [%s] [%s] (CheckRequirements) Not have enough WCoinC (need: %d, user: %d)",
			lpUser->AccountID, lpUser->Name, Group->WCoinC, lpUser->m_wcCashPoint);
		if (NotifyMessage)
				{
		this->GC_MessageBox(lpUser, "Reset Master", "Hey, %s!\nYou need %d WCoinC\nfor this reset.", lpUser->Name, Group->WCoinC);
		}
		return false;
	}	
	}

	
	
	/*if (lpUser->m_wpCashPoint < Group->WCoinP)
	{
		this->Log->Output("[ResetSystem] [%s] [%s] (CheckRequirements) Not have enough WCoinP (need: %d, user: %d)",
			lpUser->AccountID, lpUser->Name, Group->WCoinP, lpUser->m_wpCashPoint);
		if (NotifyMessage)
				{
		this->GC_MessageBox(lpUser, "Reset Master", "Great %s!\nYou need %d WCoinP\nfor this reset.", lpUser->Name, Group->WCoinP);
		}
		return false;
	}

	if (lpUser->m_wgCashPoint < Group->WCoinG)
	{
		this->Log->Output("[ResetSystem] [%s] [%s] (CheckRequirements) Not have enough WCoinG (need: %d, user: %d)",
			lpUser->AccountID, lpUser->Name, Group->WCoinG, lpUser->m_wgCashPoint);
		if (NotifyMessage)
				{
		this->GC_MessageBox(lpUser, "Reset Master", "Great %s!\nYou need %d Goblin Point\nfor this reset.", lpUser->Name, Group->WCoinG);
		}
		return false;
	}*/

	if (lpUser->m_GensInfo.iContribution < Group->GensP)
	{
		this->Log->Output("[ResetSystem] [%s] [%s] (CheckRequirements) Not have enough GensP (need: %d, user: %d)",
			lpUser->AccountID, lpUser->Name, Group->GensP, lpUser->m_GensInfo.iContribution);
		if (NotifyMessage)
				{
		this->GC_MessageBox(lpUser, "Reset Master", "Great %s!\nYou need %d Contribution\nfor this reset.", lpUser->Name, Group->GensP);
		}
		return false;
	}

	//Missed credits check

	if (Group->ItemList.size() > 0)
	{
		for (int i = 0; i < Group->ItemList.size(); i++)
		{
			if (this->GetItemInventoryPos(lpUser, Group->ItemList[i]) == (BYTE)-1)
			{
				this->Log->Output("[ResetSystem] [%s] [%s] (CheckRequirements) Not have required item (type: %d, index: %d, minlevel: %d, maxlevel: %d, mindur: %d, maxdur: %d, minoption: %d, maxoption: %d, luck: %d, skill: %d, excellent: %d, ancient: %d, socket: %d, element: %d, count: %d)",
					lpUser->AccountID, lpUser->Name, Group->ItemList[i].Type, Group->ItemList[i].Index, Group->ItemList[i].MinLevel, Group->ItemList[i].MaxLevel, Group->ItemList[i].MinDurability, Group->ItemList[i].MaxDurability, Group->ItemList[i].MinOption, Group->ItemList[i].MaxOption,
					Group->ItemList[i].Luck, Group->ItemList[i].Skill, Group->ItemList[i].Excellent, Group->ItemList[i].Ancient, Group->ItemList[i].Socket, Group->ItemList[i].Element, Group->ItemList[i].Count);
				return false;
			}
		}
	}

	return true;
}

bool ResetSystem::CheckRequirements(LPOBJ lpUser, ResetType::T Type)
{
	ResetGroup* Group = this->GetResetGroup(lpUser, Type);
	
	if (Group == NULL)
	{
		this->Log->Output("[ResetSystem] [%s] [%s] (CheckRequirements) Group for this user not found",
			lpUser->AccountID, lpUser->Name);
		LogAddC(2, "[ResetSystem] [%s] [%s] (CheckRequirements) Group for this user not found",
			lpUser->AccountID, lpUser->Name);
		return false;
	}

	return true;
}

BYTE ResetSystem::GetItemInventoryPos(LPOBJ lpUser, ResetItem Item)
{
	for (BYTE i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++)
	{
		if( !lpUser->pInventory[i].IsItem() 
			|| lpUser->pInventory[i].m_Type != ITEMGET(Item.Type, Item.Index))
		{
			continue;
		}
	}
	return (BYTE)-1;
}

int ResetSystem::GetResetMoney(LPOBJ lpUser, ResetGroup* Group, ResetType::T Type, bool ForNextReset)
{
	int ReturnMoney = Group->Money * (lpUser->iResetCount + ForNextReset);

	ReturnMoney -= ReturnMoney * m_ObjBill[lpUser->m_Index].GetResetDiscount() / 100;

	if (ReturnMoney < 0)
	{
		LogAddC(2, "[ResetSystem] [%s] [%s] (GetResetMoney) Calculated wrong value (Reset: %d, Value: %d)",
			lpUser->AccountID, lpUser->Name, lpUser->iResetCount + ForNextReset, ReturnMoney);
		return 0;
	}

	return ReturnMoney;
}

ResetGroup* ResetSystem::GetResetGroup(LPOBJ lpUser, ResetType::T Type)
{
	if (Type == ResetType::Normal)
	{
		for (int i = 0; i < this->m_NormalInfo.m_GroupList.size(); i++)
		{
			if (lpUser->iResetCount < this->m_NormalInfo.m_GroupList[i].MinReset
				|| lpUser->iResetCount + 1 > this->m_NormalInfo.m_GroupList[i].MaxReset)
			{
				//this->Log->Output("group not found, resetcount: %s, min reset: %s, max reset: %s", lpUser->iResetCount, this->m_NormalInfo.m_GroupList[i].MinReset, 
					//this->m_NormalInfo.m_GroupList[i].MaxReset);
				continue;
			}

			this->Log->Output("about to return normal");
			return &this->m_NormalInfo.m_GroupList[i];
		}
	}

	else if (Type == ResetType::Grand)
	{
		//this->Log->Output("type is GRAND");
		for (int i = 0; i < this->m_GrandInfo.m_GroupList.size(); i++)
		{
			if (lpUser->iResetCount < this->m_GrandInfo.m_GroupList[i].MinReset
				|| lpUser->iResetCount + 1 > this->m_GrandInfo.m_GroupList[i].MaxReset)
			{
			//	LogAddC(3, "group not found, resetcount: %s, min reset: %s, max reset: %s", lpUser->iResetCount, this->m_GrandInfo.m_GroupList[i].MinReset, 
				//	this->m_GrandInfo.m_GroupList[i].MaxReset);
				continue;
			}

			LogAddC(0, "type (grand) is unrecognized");
			return &this->m_GrandInfo.m_GroupList[i];
		}
	}

	this->Log->Output("type is unrecognized..");
	return NULL;
}