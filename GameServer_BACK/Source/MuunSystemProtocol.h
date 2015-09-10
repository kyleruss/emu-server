#pragma once

#include "..\common\zzzitem.h"

// network information
struct PMSG_USER_MUUN_EQUIPMENT_CHANGED {
    PBMSG_HEAD2 h;
    BYTE NumberH;
    BYTE NumberL;
    BYTE ItemInfo[0xc];
};

struct PMSG_PETINVENTORY_DELETE {
    PBMSG_HEAD2 h;
    BYTE IPos;
    BYTE Flag;
};

struct PMSG_MUUN_INVEN_ITEM_MOD {
    PBMSG_HEAD2 h;
    BYTE subcode;
    BYTE Pos;
    BYTE ItemInfo[MAX_ITEM_INFO];
};

struct PMSG_MUUNITEM_DUR {
    PBMSG_HEAD2 h;
    BYTE IPos;
    BYTE Dur;
};

struct PMSG_MUUN_ITEM_SELLREQUEST {
    PBMSG_HEAD2 h;
    BYTE Pos;
};

struct PMSG_MUUN_ITEM_SELLRESULT {
    PBMSG_HEAD2 h;
    BYTE Result;
    DWORD Money;
};

struct PMSG_MUUNITEM_GETREQUEST {
    PBMSG_HEAD2 h;
    BYTE NumberH;
    BYTE NumberL;
};

struct PMSG_MUUNITEM_GETRESULT {
    PBMSG_HEAD2 h;
    BYTE result;
    BYTE Data[MAX_ITEM_INFO];
};

struct PMSG_MUUNITEM_CONDITION_STATUS {
	PBMSG_HEAD2 h;
	BYTE btIPos;
	BYTE btStatus;
};

struct PMSG_MUUN_INVEN_SUB_EQUIP_OPEN {
    PBMSG_HEAD2 h;
    BYTE Result;
};

struct SDHP_REQ_DBMUUN_INVEN_LOAD {
    PBMSG_HEAD h;
    char AccountID[MAX_ACCOUNT_LEN + 1];
    char Name[MAX_ACCOUNT_LEN + 1];
    short aIndex;
};

struct _tagSDHP_ANS_DBMUUN_INVEN_LOAD {
	_tagSDHP_ANS_DBMUUN_INVEN_LOAD() {};
    PWMSG_HEAD h;
    BYTE dbItems[992]; // 16 (item size) * MUUN_INVENTORY_SIZE
    BYTE DbVersion;
    BYTE SubEquip;
    short aIndex;
};

struct _tagSDHP_REQ_DBMUUN_INVEN_SAVE {
	_tagSDHP_REQ_DBMUUN_INVEN_SAVE() {
		memset(dbInventory, (BYTE)-1, sizeof(dbInventory));
	}
    PWMSG_HEAD h;
    char AccountID[MAX_ACCOUNT_LEN + 1];
    char Name[MAX_ACCOUNT_LEN + 1];
    BYTE dbInventory[992]; // 16 (item size) * MUUN_INVENTORY_SIZE
};

struct PMSG_USEITEM_MUUN_INVEN {
    PBMSG_HEAD2 h;
    BYTE inventoryPos;
    BYTE invenrotyTarget;
    BYTE btItemUseType;
};

struct PMSG_USEITEM_MUUN_INVEN_RESULT {
    PBMSG_HEAD2 h;
    BYTE btResult;
    BYTE btItemUseType;
};