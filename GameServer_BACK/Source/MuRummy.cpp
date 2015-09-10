#include "stdafx.h"
#include "MuRummy.h"
#include "LargeRand.h"
#include "DBSockMng.h"
//#include "PeriodItemEx.h"

int g_bMuRummyEventOn = true;
int g_MuRummyEventDBLog = false;
CMuRummyMng g_CMuRummyMng;

CMuRummyCardInfo::CMuRummyCardInfo()
{
	this->m_iColor = 0;
	this->m_iNumber = 0;
	this->m_iState = 0;
	this->m_iSlotNum = -1;
}

CMuRummyCardInfo::~CMuRummyCardInfo()
{

}

CMuRummyInfo::CMuRummyInfo()
{
	this->Init();
}

CMuRummyInfo::~CMuRummyInfo()
{
	if (this->m_pCCardInfo)
	{
		delete []this->m_pCCardInfo;
		this->m_pCCardInfo = NULL;
	}

	if (this->m_pSlotCardIndex)
	{
		delete this->m_pSlotCardIndex;
		this->m_pSlotCardIndex = NULL;
	}
}

void CMuRummyInfo::Init()
{
	this->m_pCCardInfo = 0;
	this->m_pSlotCardIndex = 0;
	this->m_iScore = 0;
	this->m_iCardDeckCnt = 0;
	this->m_bPlayCard = 0;
	this->m_bMuRummyInfoLoad = 0;

	if (!this->m_pCCardInfo)
	{
		this->m_pCCardInfo = new CMuRummyCardInfo[MURUMMY_MAX_CARD];
	}

	if (!this->m_pSlotCardIndex)
	{
		this->m_pSlotCardIndex = new BYTE[MURUMMY_MAX_SLOT];
	}
}

void CMuRummyInfo::Release()
{

}

void CMuRummyInfo::Clear()
{
	for (int i = 0; i < MURUMMY_MAX_CARD; i++)
	{
		this->m_pCCardInfo[i].SetColor(0);
		this->m_pCCardInfo[i].SetNumber(0);
		this->m_pCCardInfo[i].SetState(0);
		this->m_pCCardInfo[i].SetSlotNum(-1);
	}

	for (int i = 0; i < MURUMMY_MAX_SLOT; i++)
	{
		this->m_pSlotCardIndex[i] = -1;
	}

	this->m_iScore = 0;
	this->m_iCardDeckCnt = 0;
	this->m_bPlayCard = 0;
	this->m_bWaitReward = 0;
}

CMuRummyCardInfo* CMuRummyInfo::GetCardInfo()
{
	if (!this->m_pCCardInfo)
	{
		return 0;
	}

	return this->m_pCCardInfo;
}

LPBYTE CMuRummyInfo::GetSlotIndexInfo()
{
	if (!this->m_pSlotCardIndex)
	{
		return 0;
	}

	return this->m_pSlotCardIndex;
}

CMuRummyMng::CMuRummyMng()
{
	//this->m_pRewardItemBag1 = 0;
	//this->m_pRewardItemBag2 = 0;
	this->m_iRewardScore1 = 0;
	this->m_iRewardScore2 = 0;
}

CMuRummyMng::~CMuRummyMng()
{
	//del m_pRewardItemBag1, m_pRewardItemBag2
}

void CMuRummyMng::LoadPreFixData(LPSTR lpszFileName)
{
	this->m_tmEventEndDate.wYear = GetPrivateProfileIntA("GameServerInfo", "MuRummyEventEndYear", 0, lpszFileName);
	this->m_tmEventEndDate.wMonth = GetPrivateProfileIntA("GameServerInfo", "MuRummyEventEndMonth", 0, lpszFileName);
	this->m_tmEventEndDate.wDay = GetPrivateProfileIntA("GameServerInfo", "MuRummyEventEndDay", 0, lpszFileName);
	this->m_tmEventEndDate.wHour = GetPrivateProfileIntA("GameServerInfo", "MuRummyEventEndHour", 0, lpszFileName);
	this->m_tmEventEndDate.wMinute = GetPrivateProfileIntA("GameServerInfo", "MuRummyEventEndMinute", 0, lpszFileName);
	this->m_iRewardScore1 = GetPrivateProfileIntA("GameServerInfo", "MuRummyRewardScore1", 0, lpszFileName);
	this->m_iRewardScore2 = GetPrivateProfileIntA("GameServerInfo", "MuRummyRewardScore2", 0, lpszFileName);
  
	if (this->m_iRewardScore1 <= 0 || this->m_iRewardScore2 <= 0)
	{
		if (this->IsMuRummyEventOn() == 1)
		{
			MessageBoxA(0, "[CommonSever.cfg] Check MuRummyRewardScore", "Error", 0);
		}
	}
}

void CMuRummyMng::ProbabilityLoadData()
{
	//eventitembag loading
}

int CMuRummyMng::IsMuRummyEventOn()
{
	return g_bMuRummyEventOn;
}

int CMuRummyMng::GetRewardScore1()
{
	return this->m_iRewardScore1;
}

int CMuRummyMng::GetRewardScore2()
{
	return this->m_iRewardScore2;
}

void CMuRummyMng::SetCardDeck(CMuRummyInfo* pMuRummyInfo)
{
	int nCardArr = 0;

	if (!pMuRummyInfo)
	{
		return;
	}

	CMuRummyCardInfo* pCCardInfo = pMuRummyInfo->GetCardInfo();

	if (!pCCardInfo)
    {
		return;
	}

	for (int nCardColor = 0; nCardColor < MURUMMY_MAX_CARD_COLOR; nCardColor++)
    {
		for (int nCardNum = 0; nCardNum < MURUMMY_MAX_SLOT; nCardNum++)
        {
			if (nCardArr < 0 || nCardArr > MURUMMY_MAX_CARD - 1)
			{
				return;
			}

			pCCardInfo[nCardArr].SetColor(nCardColor + 1);
			pCCardInfo[nCardArr].SetNumber(nCardNum + 1);
			pCCardInfo[nCardArr].SetState(0);
			pCCardInfo[nCardArr].SetSlotNum(0);
			nCardArr++;
		}
	}

	LPBYTE pSlotIndexInfo = pMuRummyInfo->GetSlotIndexInfo();

	if (!pSlotIndexInfo)
    {
		return;
	}

	for (int i = 0; i < MURUMMY_MAX_SLOT; i++)
	{
		pSlotIndexInfo[i] = -1;
	}
    
	pMuRummyInfo->SetCardDeckCnt(MURUMMY_MAX_CARD);
}

void CMuRummyMng::CardShuffle(CMuRummyCardInfo* pCCardInfo)
{
	CMuRummyCardInfo CardInfoTmp;

	for (int nCnt = 0; nCnt < MURUMMY_MAX_CARD; nCnt++)
	{
		DWORD iIndex = nCnt + GetLargeRand() % (MURUMMY_MAX_CARD - nCnt);

		if (iIndex > MURUMMY_MAX_CARD - 1)
		{
			return;
		}

		CardInfoTmp.SetColor(pCCardInfo[nCnt].GetColor());
		CardInfoTmp.SetNumber(pCCardInfo[nCnt].GetNumber());
		CardInfoTmp.SetState(pCCardInfo[nCnt].GetState());
		CardInfoTmp.SetSlotNum(pCCardInfo[nCnt].GetSlotNum());

		pCCardInfo[nCnt].SetColor(pCCardInfo[iIndex].GetColor());
		pCCardInfo[nCnt].SetNumber(pCCardInfo[iIndex].GetNumber());
		pCCardInfo[nCnt].SetState(pCCardInfo[iIndex].GetState());
		pCCardInfo[nCnt].SetSlotNum(pCCardInfo[iIndex].GetSlotNum());

		pCCardInfo[iIndex].SetColor(CardInfoTmp.GetColor());
		pCCardInfo[iIndex].SetNumber(CardInfoTmp.GetNumber());
		pCCardInfo[iIndex].SetState(CardInfoTmp.GetState());
		pCCardInfo[iIndex].SetSlotNum(CardInfoTmp.GetSlotNum());
	}
}

bool CMuRummyMng::SetPlayCardInfo(CMuRummyInfo* pMuRummyInfo, _tagMuRummyCardInfo* pOutCardInfo)
{
	int nCardCnt = 0;

	if (!pMuRummyInfo)
	{
		return false;
	}

	CMuRummyCardInfo* pCCardInfo = pMuRummyInfo->GetCardInfo();

	if (!pCCardInfo)
	{
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL [%s, %d]", __FILE__, __LINE__);
		return false;
	}

	LPBYTE pSlotIndexInfo = pMuRummyInfo->GetSlotIndexInfo();

	if (!pSlotIndexInfo )
    {
		LogAddTD("[MuRummy][Error] pSlotIndexInfo is NULL [%s, %d]", __FILE__, __LINE__);
		return false;
	}

	for (int nCnt = 0; nCnt < MURUMMY_MAX_SLOT; nCnt++)
    {
		int nCardArr = pSlotIndexInfo[nCnt];

		if (nCardArr < 0 || nCardArr > MURUMMY_MAX_CARD - 1)
		{
			continue;
		}

		if (pCCardInfo[nCardArr].GetState() == 1)
		{
			if (nCardCnt < 0 || nCardCnt > 4)
			{
				return false;
			}

			pOutCardInfo[nCardCnt].btColor = pCCardInfo[nCardArr].GetColor();
            pOutCardInfo[nCardCnt].btNumber = pCCardInfo[nCardArr].GetNumber();
            pOutCardInfo[nCardCnt].btSlotNum = pCCardInfo[nCardArr].GetSlotNum();
			nCardCnt++;
		}
	}

	return true;
}

BYTE CMuRummyMng::GetTempCardShowSlot(LPBYTE pSlotCardIndex)
{
	int iSlotCnt = 0;
	
	for (int i = 0; i < 8; i++)
	{
		if (pSlotCardIndex[i] != (BYTE)-1 )
		{
			if (i >= 5)
			{
				return -2;
			}

			iSlotCnt++;
		}
	}

	if (iSlotCnt >= 5)
	{
		return -1;
	}

    for (int i = 0; i < 5; i++ )
    {
		if (pSlotCardIndex[i] == (BYTE)-1)
		{
			return i;
		}
    }

	return -1;
}

BYTE CMuRummyMng::CardSlotMove(CMuRummyInfo* pCMuRummyInfo, int sSlot, int tSlot, CMuRummyCardInfo* pCOutCardInfo, LPOBJ obj)
{
	CMuRummyCardInfo* pCCardInfo = pCMuRummyInfo->GetCardInfo();

	if (!pCCardInfo)
	{
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL [%s][%s] [%s, %d]", 
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return -1;
	}

	LPBYTE pSlotIndexInfo = pCMuRummyInfo->GetSlotIndexInfo();

	if (!pSlotIndexInfo)
	{
		LogAddTD("[MuRummy][Error] pSlotIndexInfo is NULL [%s][%s] [%s, %d]",
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return -1;
	}

	BYTE sCardArr = pSlotIndexInfo[sSlot];
    BYTE tCardArr = pSlotIndexInfo[tSlot];

	if (sCardArr == (BYTE)-1)
    {
		return -1;
	}

	if (tCardArr != (BYTE)-1)
    {
		LogAddTD("[MuRummy][Error] slot Not Empty [%s][%s] %d ", 
			obj->AccountID, obj->Name, tCardArr);
		return -2;
	}

	if (sCardArr < 0 || sCardArr > MURUMMY_MAX_CARD - 1)
	{
		LogAddTD("[MuRummy][Error] CardInfo Index is Overflow [%s][%s][%d] [%s, %d]",
			obj->AccountID, obj->Name, sCardArr, __FILE__, __LINE__);
         return -2;
	}

	if (pCCardInfo[sCardArr].GetState() == 1)
	{
		if (sSlot < 5 && tSlot >= 5 )
        {
			LogAddTD("[%s][%s][Mu Rummy] Slot[5] - Move Slot[%d] %c : %d",
				obj->AccountID, obj->Name, 
				pCCardInfo[sCardArr].GetSlotNum() + 1,
                this->GetColorName(pCCardInfo[sCardArr].GetColor() - 1),
                pCCardInfo[sCardArr].GetNumber());
		}

		if (sSlot >= 5 && tSlot < 5)
        {
			LogAddTD("[%s][%s][Mu Rummy] Slot[3] - Move Slot[%d] %c : %d",
				obj->AccountID, obj->Name,
                pCCardInfo[sCardArr].GetSlotNum() - 4,
                this->GetColorName(pCCardInfo[sCardArr].GetColor() - 1),
                pCCardInfo[sCardArr].GetNumber());
        }

		pSlotIndexInfo[sSlot] = -1;
        pSlotIndexInfo[tSlot] = sCardArr;
		pCCardInfo[sCardArr].SetSlotNum(tSlot);
		memcpy(pCOutCardInfo, &pCCardInfo[sCardArr], sizeof(CMuRummyCardInfo));
		
		if (sSlot < 5 && tSlot >= 5 )
		{
			LogAddTD("[%s][%s][Mu Rummy] Slot[3] - Register Slot[%d] %c : %d",
				obj->AccountID, obj->Name,
                pCCardInfo[sCardArr].GetSlotNum() - 4,
                this->GetColorName(pCCardInfo[sCardArr].GetColor() - 1),
                pCCardInfo[sCardArr].GetNumber());
            this->Slot3Log(obj, 0);
		}

		if (sSlot >= 5 && tSlot < 5)
        {
			LogAddTD("[%s][%s][Mu Rummy] Slot[5] - Register Slot[%d] %c : %d",
				obj->AccountID, obj->Name,
                pCCardInfo[sCardArr].GetSlotNum() + 1,
                this->GetColorName(pCCardInfo[sCardArr].GetColor() - 1),
                pCCardInfo[sCardArr].GetNumber());
            this->Slot5Log(obj, 0);
		}

		return 0;
	}

	return -1;
}

BYTE CMuRummyMng::CardSlotReMove(CMuRummyInfo* pCMuRummyInfo, int sSlot)
{
	CMuRummyCardInfo* pCCardInfo = pCMuRummyInfo->GetCardInfo();

	if (!pCCardInfo )
	{	 
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL SlotNum [%d] [%s, %d]", sSlot, __FILE__, __LINE__);
		return -1;
	}

	LPBYTE pSlotIndexInfo = pCMuRummyInfo->GetSlotIndexInfo();
  
	if (!pSlotIndexInfo )
	{
		LogAddTD("[MuRummy][Error] pSlotIndexInfo is NULL SlotNum [%d] [%s, %d]", sSlot, __FILE__, __LINE__);
		return -1;
	}

    if (pSlotIndexInfo[sSlot] == (BYTE)-1)
    {
        LogAddTD("[MuRummy][Error] Slot is Empty SlotNum [%d] [%s, %d]", sSlot, __FILE__, __LINE__);
        return -1;
    }

    BYTE sCardArr = pSlotIndexInfo[sSlot];

	if (pCCardInfo[sCardArr].GetSlotNum() != sSlot)
	{
		LogAddTD("[MuRummy][Error] Slot Num diff [%s, %d]", __FILE__, __LINE__);
		return -1;
	}

    pCCardInfo[sCardArr].SetSlotNum(-1);
    pCCardInfo[sCardArr].SetState(2);
    pSlotIndexInfo[sSlot] = -1;
    return sCardArr;
}

BYTE CMuRummyMng::CardMatchCheck(CMuRummyInfo* pCMuRummyInfo, int& iOutScore, _tagMuRummyCardUpdateDS* pOutCardUpdateDS, LPOBJ obj)
{
	//Need recheck structure, maybe not valid return

	CMuRummyCardInfo* pCCardInfo = pCMuRummyInfo->GetCardInfo();
	
	if (!pCCardInfo)
	{
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL [%s][%s] [%s, %d]", 
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return -1;
	}

	LPBYTE pSlotIndexInfo = pCMuRummyInfo->GetSlotIndexInfo();
	
	if (!pSlotIndexInfo)
	{
		LogAddTD("[MuRummy][Error] pSlotIndexInfo is NULL [%s][%s] [%s, %d]",
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return -1;
	}

	CMuRummyCardInfo CCardInfoTmp[3];

	int nChkCnt = 0;

	for (int nCnt = 5; nCnt < MURUMMY_MAX_SLOT; nCnt++)
	{
		if (pSlotIndexInfo[nCnt] != (BYTE)-1)
		{
			CCardInfoTmp[nChkCnt].SetColor(pCCardInfo[pSlotIndexInfo[nCnt]].GetColor());
			CCardInfoTmp[nChkCnt].SetNumber(pCCardInfo[pSlotIndexInfo[nCnt]].GetNumber());
			CCardInfoTmp[nChkCnt].SetState(pCCardInfo[pSlotIndexInfo[nCnt]].GetState());
			CCardInfoTmp[nChkCnt].SetSlotNum(pCCardInfo[pSlotIndexInfo[nCnt]].GetSlotNum());
			nChkCnt++;
		}
	}

	if (nChkCnt != 3)
	{
		return -1;
	}

	this->Slot3Log(obj, 1);
	int nRetCardColor = this->IsCardSameColor(CCardInfoTmp);
	int nRetCardNum = this->IsCardNumber(CCardInfoTmp);

	if (nRetCardColor == 1 && nRetCardNum == 3)
	{
		iOutScore = 10 * (CCardInfoTmp->GetNumber() - 1) + 10;
	}

	if (nRetCardColor == 2 && nRetCardNum == 3)
	{
		iOutScore = 10 * (CCardInfoTmp->GetNumber() - 1) + 50;
	}

	if (nRetCardColor == 1 && nRetCardNum == 4)
	{
		iOutScore = 10 * (CCardInfoTmp->GetNumber() - 1) + 20;
	}

	int iCnt = 0;

    for (int i = 5; i < MURUMMY_MAX_SLOT; i++)
    {
		if (pSlotIndexInfo[i] == (BYTE)-1 )
		{
			continue;
		}

		if (pSlotIndexInfo[i] < 0 || pSlotIndexInfo[i] > MURUMMY_MAX_CARD - 1)
		{
			LogAddTD("[MuRummy][Error] Combine CardInfo Index is Overflow [%s][%s][%d] [%s, %d]",
				obj->AccountID, obj->Name, pSlotIndexInfo[i], __FILE__, __LINE__);
			return -2;
		}

        pCCardInfo[pSlotIndexInfo[i]].SetSlotNum( -1);
        pCCardInfo[pSlotIndexInfo[i]].SetState(3);

		//maybe need init here SlotIndexInfo[i]

		pOutCardUpdateDS[iCnt].btSeq = pSlotIndexInfo[i];
        pOutCardUpdateDS[iCnt].btSlotNum = pCCardInfo[pSlotIndexInfo[i]].GetSlotNum();
        pOutCardUpdateDS[iCnt].btStatus = pCCardInfo[pSlotIndexInfo[i]].GetState();

        pSlotIndexInfo[i] = -1;
		iCnt++;
	}

	int nSlotNum = 5;

	for (int n = 0; n < 5; n++)
	{
		if (pSlotIndexInfo[n] != (BYTE)-1 )
		{
			continue;
		}

		if (nSlotNum < 0 || nSlotNum > MURUMMY_MAX_SLOT - 1)
		{
			return -2;
		}

		pSlotIndexInfo[n] = pSlotIndexInfo[nSlotNum];

		if (pSlotIndexInfo[n] < 0 || pSlotIndexInfo[n] > MURUMMY_MAX_CARD - 1)
		{
			LogAddTD("[MuRummy][Error] CardInfo Index is Overflow [%s][%s][%d] [%s, %d]",
				obj->AccountID, obj->Name, pSlotIndexInfo[n], __FILE__, __LINE__);
			return -2;
		}
		
		pCCardInfo[pSlotIndexInfo[n]].SetSlotNum(n);
		pSlotIndexInfo[nSlotNum] = -1;
		nSlotNum++;
	}

	return 0;
}

void CMuRummyMng::CardSortAscending(CMuRummyCardInfo* pCCardInfo)
{
	CMuRummyCardInfo CCardInfoTmp;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 3 - i - 1; j++)
		{
			if (pCCardInfo[j].GetNumber() > pCCardInfo[j + 1].GetNumber())
			{
				CCardInfoTmp.SetColor(pCCardInfo[j].GetColor());
				CCardInfoTmp.SetNumber(pCCardInfo[j].GetNumber());
				CCardInfoTmp.SetState(pCCardInfo[j].GetState());
				CCardInfoTmp.SetSlotNum(pCCardInfo[j].GetSlotNum());

				pCCardInfo[j].SetColor(pCCardInfo[j + 1].GetColor());
				pCCardInfo[j].SetNumber(pCCardInfo[j + 1].GetNumber());
				pCCardInfo[j].SetState(pCCardInfo[j + 1].GetState());
				pCCardInfo[j].SetSlotNum(pCCardInfo[j + 1].GetSlotNum());

				pCCardInfo[j + 1].SetColor(CCardInfoTmp.GetColor());
				pCCardInfo[j + 1].SetNumber(CCardInfoTmp.GetNumber());
				pCCardInfo[j + 1].SetState(CCardInfoTmp.GetState());
				pCCardInfo[j + 1].SetSlotNum(CCardInfoTmp.GetSlotNum());
			}
		}
	}
}

int CMuRummyMng::IsCardSameColor(CMuRummyCardInfo* pCCardInfo)
{
	for (int i = 0; i < 2; i++)
	{
		if (pCCardInfo[i].GetColor() != pCCardInfo[i + 1].GetColor())
		{
			return 1;
		}
	}

	return 2;
}

int CMuRummyMng::IsCardNumber(CMuRummyCardInfo* pCCardInfo)
{
	for (int i = 0; ; i++)
	{
		if (i >= 2 )
		{
			return 4;
		}

		if (pCCardInfo[i].GetNumber() != pCCardInfo[i + 1].GetNumber())
		{
			break;
		}
	}

	this->CardSortAscending(pCCardInfo);

	for (int j = 0; j < 2; j++)
	{
		if ( pCCardInfo[j + 1].GetNumber() - pCCardInfo[j].GetNumber() != 1)
		{
			return 0;
		}
	}

	return 3;
}

bool CMuRummyMng::FillEmptySlotCard(CMuRummyInfo* pMuRummyInfo, _tagMuRummyCardInfo* pOutCardInfo, LPOBJ obj)
{
	CMuRummyCardInfo* pCCardInfo = pMuRummyInfo->GetCardInfo();

	if (!pCCardInfo)
	{
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL [%s, %d]", __FILE__, __LINE__);
		return false;
	}

	LPBYTE pSlotIndexInfo = pMuRummyInfo->GetSlotIndexInfo();

	if (!pSlotIndexInfo)
	{
		LogAddTD("[MuRummy][Error] pSlotIndexInfo is NULL [%s, %d]", __FILE__, __LINE__);
		return false;
	}

	int nCardArr = 0;
    
	for (int i = 0; i < 5; i++)
    {
		if (pSlotIndexInfo[i] != (BYTE)-1)
        {
			continue;
		}

		if (nCardArr < 0 || nCardArr > 4)
		{
			return false;
		}

		int nCardDeckCnt = pMuRummyInfo->GetCardDeckCnt();
        int nCardSeq = MURUMMY_MAX_CARD - nCardDeckCnt;

		if (nCardSeq >= MURUMMY_MAX_CARD )
        {
			LogAddTD("[MuRummy][Error] Card Cnt Over [%d] [%s, %d]", nCardSeq, __FILE__, __LINE__);
            return false;
        }

		pCCardInfo[nCardSeq].SetSlotNum(i);
        pCCardInfo[nCardSeq].SetState(1);
        pSlotIndexInfo[i] = nCardSeq;
        pOutCardInfo[nCardArr].btColor = pCCardInfo[nCardSeq].GetColor();
        pOutCardInfo[nCardArr].btNumber = pCCardInfo[nCardSeq].GetNumber();
        pOutCardInfo[nCardArr].btSlotNum = pCCardInfo[nCardSeq].GetSlotNum();
        pMuRummyInfo->SetCardDeckCnt(nCardDeckCnt - 1);
        nCardArr++;

		if (pMuRummyInfo->GetCardDeckCnt() < 0)
        {
            LogAddTD("[MuRummy][Error] CardDeck cnt [%d] [%s, %d]", pMuRummyInfo->GetCardDeckCnt(), 
				__FILE__, __LINE__);
            return false;
        }

        this->GDReqSlotInfoUpdate(obj, nCardSeq, pCCardInfo[nCardSeq].GetSlotNum(), pCCardInfo[nCardSeq].GetState());
        
		if (!pMuRummyInfo->GetCardDeckCnt())
		{
           return true;
		}
	}
	
	return true;
}
    
void CMuRummyMng::CGReqMuRummyEventOpen(_tagPMSG_REQ_MURUMMY_EVENT_OPEN* lpMsg, int aIndex)
{	
	_tagPMSG_ANS_MURUMMY_EVENT_OPEN pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0x4D, 0xF, sizeof(_tagPMSG_ANS_MURUMMY_EVENT_OPEN));
	pMsg.btResult = -1;

	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}

	if (!gObjIsConnected(aIndex))
	{
		return;
	}
	
	LPOBJ lpObj = &gObj[aIndex];
	
	if (!lpObj)
	{
		return;
	}

	if (lpObj->CloseType != -1) 
	{
		return;
	}

	if (lpObj->m_IfState.use > 0 && lpObj->m_IfState.type != 3)
	{
		 LogAddTD("[MuRummy][Error] [%s][%s][%d] : not event ofen",
			 lpObj->AccountID,lpObj->Name, lpObj->m_IfState.type);
		 return;
	}

	if (lpObj->m_bPShopOpen == 1 || lpObj->ChaosLock == 1)
    {
		LogAddTD("[MuRummy][Error] [%s][%s][%d] : not event ofen",
			 lpObj->AccountID,lpObj->Name, lpObj->m_IfState.type);
		 return;
	}

	CMuRummyInfo* pMuRummyInfo;// = lpObj->m_pCMuRummyInfo;

	if (!pMuRummyInfo)
	{
		LogAddTD("[MuRummy][Error] pMuRummyInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return;
	}

	if (pMuRummyInfo->IsWaitReward())
	{
		LogAddTD("[MuRummy][DebugLog][%s][%s] Waiting Item Reward(Req Open)", lpObj->AccountID, lpObj->Name);
		GCServerMsgStringSend(lMsg.Get(3539), lpObj->m_Index, 1);
	}
	else if (this->IsMuRummyEventOn())
	{
		CTime tExpireTime; //maybe special initilizator
		DWORD dwExpireSec = 0;//g_PeriodItemEx.GetLeftDate(tExpireTime.GetTime());
		pMsg.btEventTime1 = DWORD((DWORD(dwExpireSec >> 32) >> 16) & 0xFFFF) >> 8;
		pMsg.btEventTime2 = DWORD((DWORD(dwExpireSec >> 32) >> 16) & 0xFFFF) & 0xFF;
		pMsg.btEventTime3 = DWORD((DWORD(dwExpireSec >> 32) & 0xFFFF) & 0xFFFF) >> 8;
		pMsg.btEventTime4 = DWORD((DWORD(dwExpireSec >> 32) & 0xFFFF) & 0xFFFF) & 0xFF;
		
		if (!pMuRummyInfo->IsGetPlayCard())
		{
			 pMsg.btResult = 1;
             DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
			 return;
		}

		pMsg.btResult = 2;
        DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
        this->CGReqMuRummyStart(0, aIndex);

		LogAddTD("[MuRummy] [%s][%s] : Mini Game Open", lpObj->AccountID, lpObj->Name);
	}
}

void CMuRummyMng::CGReqMuRummyStart(_tagPMSG_REQ_MURUMMY_INFO* lpMsg, int aIndex)
{
	_tagPMSG_ANS_MURUMMY_INFO pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0x4D, 0x10, sizeof(_tagPMSG_ANS_MURUMMY_INFO));

	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}

	if (!gObjIsConnected(aIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (!lpObj)
    {
		LogAddTD("[MuRummy][Error] lpObj is NULL [%s, %d]", __FILE__, __LINE__);
		return;
    }
	
	if (lpObj->CloseType != -1) 
	{
		return;
	}

	CMuRummyInfo* pMuRummyInfo;// = lpObj->m_pCMuRummyInfo;

	if (!pMuRummyInfo->IsMuRummyInfoLoad())
	{
		LogAddTD("[MuRummy][Error] IsMuRummyInfoLoad fail [%s][%s]", lpObj->AccountID, lpObj->Name);
        return;
	}

	_tagMuRummyCardInfo stCardInfo[3];
	bool bNewGame = false;

	if (pMuRummyInfo->IsGetPlayCard())
	{
		if (this->SetPlayCardInfo(pMuRummyInfo, stCardInfo))
		{
			LogAddTD("[%s][%s][Mu Rummy] Event Open", lpObj->AccountID, lpObj->Name);
			LogAddTD("[%s][%s][Mu Rummy] Event Card Cnt(%d)", lpObj->AccountID, lpObj->Name, pMuRummyInfo->GetCardDeckCnt());
			LogAddTD("[%s][%s][Mu Rummy] Event Point(%d)", lpObj->AccountID, lpObj->Name, pMuRummyInfo->GetScore());
			this->Slot5Log(lpObj, 1);
			this->Slot3Log(lpObj, 1);
			memcpy(pMsg.stMuRummyCardInfo, stCardInfo, sizeof(stCardInfo));
		}
		else
		{
			LogAddTD("[MuRummy][Error] SetPlayCardInfo fail [%s][%s]", lpObj->AccountID, lpObj->Name);
		}
	}
	else
	{
		bool bItem = 0;

        for (int x = 0; x < 32; x++)
        {
			//if ( lpObj->pEventInventory[x].IsItem() == 1 
			//	&& lpObj->pEventInventory[x].m_Type == 7384 )
			{
				LogAddTD("[%s][%s][Mu Rummy] Event Start", lpObj->AccountID, lpObj->Name);
//				LogAddTD("[%s][%s][Mu Rummy] Event Card register success (%u)", lpObj->AccountID, lpObj->Name, lpObj->pEventInventory[x].GetNumber());
				//LogAddEvent("MuRummyEvent:MiniGameStart", lpObj);
				//gObjEventInventoryDeleteItem(aIndex, x);
				//GCEventInventoryItemDeleteSend(aIndex, x, 1);
				bItem = true;
				break;
			}
        }

        if (!bItem)
        {
			LogAddTD("[%s][%s][Mu Rummy] Event Start Fali (0)", lpObj->AccountID, lpObj->Name);
			this->GCSendMsg(aIndex, 0, 0);
			return;
        }

        pMuRummyInfo->Clear();
        pMuRummyInfo->SetGetPlayCard(true);
        this->SetCardDeck(pMuRummyInfo);
        this->CardShuffle(pMuRummyInfo->GetCardInfo());
        this->GDReqCardInfoInsert(lpObj);
        bNewGame = true;
	}

	pMsg.btCardDeckCnt = pMuRummyInfo->GetCardDeckCnt();
    pMsg.btTotScoreH = SET_NUMBERH(pMuRummyInfo->GetScore());
    pMsg.btTotScoreL = SET_NUMBERL(pMuRummyInfo->GetScore());
    DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

    if (bNewGame)
	{
		this->GCSendMsg( aIndex, 1, 0);
	}
}
    
void CMuRummyMng::CGReqCardReveal(_tagPMSG_REQ_REVEAL_CARD* lpMsg, int aIndex)
{
	_tagPMSG_ANS_REVEAL_CARD pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0x4D, 0x11, sizeof(_tagPMSG_ANS_REVEAL_CARD));

	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}

	if (!gObjIsConnected(aIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (!lpObj)
    {
		LogAddTD("[MuRummy][Error] lpObj is NULL [%s, %d]", __FILE__, __LINE__);
		return;
    }
	
	if (lpObj->CloseType != -1) 
	{
		return;
	}

	CMuRummyInfo* pMuRummyInfo;// = lpObj->m_pCMuRummyInfo;

	if (!pMuRummyInfo)
	{
		LogAddTD("[MuRummy][Error] pMuRummyInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return;
	}

	if (!pMuRummyInfo->GetCardInfo())
	{
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return;
	}

	if (!pMuRummyInfo->GetCardDeckCnt())
	{
		this->GCSendMsg(aIndex, 2, 0);
		return;
	}

	LPBYTE pSlotIndexInfo = pMuRummyInfo->GetSlotIndexInfo();

	if (!pSlotIndexInfo)
    {
		 LogAddTD("[MuRummy][Error] pSlotIndexInfo is NULL [%s][%s] [%s, %d]",
			 lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		 return;
	}

	BYTE btSlotNum = -1;
    btSlotNum = this->GetTempCardShowSlot(pSlotIndexInfo);
    
	if (btSlotNum == (BYTE)-1)
	{
		this->GCSendMsg(aIndex, 2, 0);
		return;
	}

	if (btSlotNum == (BYTE)-2)
	{
		this->GCSendMsg(aIndex, 3, 0);
		return;
	}

	_tagMuRummyCardInfo stCardInfo[3];
	int iBeforeCardCnt = pMuRummyInfo->GetCardDeckCnt();

	if (this->FillEmptySlotCard(pMuRummyInfo, stCardInfo, lpObj))
	{
		 LogAddTD("[%s][%s][Mu Rummy] Event Card Cnt(%d) (%d)",
			 lpObj->AccountID, lpObj->Name, iBeforeCardCnt, pMuRummyInfo->GetCardDeckCnt());
		 this->Slot5Log(lpObj, 0);
		 memcpy(pMsg.stMuRummyCardInfo, stCardInfo, sizeof(stCardInfo));
		 pMsg.btCardDeckCnt = pMuRummyInfo->GetCardDeckCnt();
         DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
	}
}
    
void CMuRummyMng::CGReqCardMove(_tagPMSG_REQ_CARD_MOVE* lpMsg, int aIndex)
{
	_tagPMSG_ANS_CARD_MOVE pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0x4D, 0x12, sizeof(_tagPMSG_ANS_CARD_MOVE));

	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}

	if (!gObjIsConnected(aIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->CloseType != -1) 
	{
		return;
	}

	int sSlot = lpMsg->sSlot;
    int tSlot = lpMsg->tSlot;

	if (sSlot < 0 || sSlot >= MURUMMY_MAX_SLOT)
    {
		return;
	}

	if (tSlot < 0 || tSlot >= MURUMMY_MAX_SLOT)
    {
		return;
	}

	if (sSlot == tSlot)
	{
		return;
	}

	CMuRummyInfo* pCMuRummyInfo;// = lpObj->m_pCMuRummyInfo;

	if (!pCMuRummyInfo)
	{
		LogAddTD("[MuRummy][Error] pCMuRummyInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return;
	}

	CMuRummyCardInfo pCOutCardInfo;
	BYTE Ret = this->CardSlotMove(pCMuRummyInfo, sSlot, tSlot, &pCOutCardInfo, lpObj);

	if (Ret == (BYTE)-1)
	{
		return;
	}
	else if (Ret == (BYTE)-2)
	{
		sSlot = (BYTE)-1;
	}

	pMsg.sSlot = sSlot;
    pMsg.tSlot = pCOutCardInfo.GetSlotNum();
    pMsg.btColor = pCOutCardInfo.GetColor();
    pMsg.btNumber = pCOutCardInfo.GetNumber();
    DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}
    
void CMuRummyMng::CGReqCardReMove(_tagPMSG_REQ_CARD_REMOVE* lpMsg, int aIndex)
{
	_tagPMSG_ANS_CARD_REMOVE pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0x4D, 0x13, sizeof(_tagPMSG_ANS_CARD_REMOVE));

	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}

	if (!gObjIsConnected(aIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->CloseType != -1) 
	{
		return;
	}

	int sSlot = lpMsg->btSlot;

	if (sSlot >= MURUMMY_MAX_SLOT)
    {
		LogAddTD("[MuRummy][Error] dump fail Slot Num [%d] [%s][%s] [%s, %d]",
			sSlot, lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return;
	}

	CMuRummyInfo* pCMuRummyInfo;// = lpObj->m_pCMuRummyInfo;

	if (!pCMuRummyInfo)
	{
		LogAddTD("[MuRummy][Error] pCMuRummyInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return;
	}

	BYTE Ret = this->CardSlotReMove(pCMuRummyInfo, sSlot);

	if (Ret == (BYTE)-1)
	{
		LogAddTD("[MuRummy][Error] dump Card fail [%s][%s] ", 
			lpObj->AccountID, lpObj->Name);
		return;
	}

	CMuRummyCardInfo* pCCardInfo = pCMuRummyInfo->GetCardInfo();

	if (!pCCardInfo)
	{
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return;
	}

	this->GDReqCardInfoUpdate(lpObj, &pCCardInfo[Ret], Ret);
	pMsg.btResult = sSlot;
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

    this->GCSendMsg(aIndex, 4, 0);
	LogAddTD("[%s][%s][Mu Rummy] Slot[5] - Junk Slot[%d] %c : %d",
		lpObj->AccountID, lpObj->Name, sSlot,
		this->GetColorName(pCCardInfo[Ret].GetColor() - 1), pCCardInfo[Ret].GetNumber());
	this->Slot5Log(lpObj, 0);
}
    
void CMuRummyMng::CGReqCardMatch(_tagPMSG_REQ_CARD_MATCH* lpMsg, int aIndex)
{
	_tagPMSG_ANS_CARD_MATCH pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0x4D, 0x14, sizeof(_tagPMSG_ANS_CARD_MATCH));

	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}

	if (!gObjIsConnected(aIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	CMuRummyInfo* pCMuRummyInfo;// = gObj[aIndex].m_pCMuRummyInfo;

	if (!pCMuRummyInfo)
    {
		LogAddTD("[MuRummy][Error] pCMuRummyInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return;
	}

	int nScore = 0;
	_tagMuRummyCardUpdateDS stCardUpdateDS[3];
	BYTE btRet = this->CardMatchCheck(pCMuRummyInfo, nScore, stCardUpdateDS, lpObj);
    CMuRummyCardInfo* pCCardInfo = pCMuRummyInfo->GetCardInfo();

	if (!pCCardInfo)
    {
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL [%s, %d]", __FILE__, __LINE__);
		return;
	}

	if (btRet)
	{
		this->GCSendMsg(aIndex, 6, 0);
		LogAddTD("[%s][%s][Mu Rummy] Slot[3] - Combine Fail", lpObj->AccountID, lpObj->Name);
	}
	else
	{
		pCMuRummyInfo->SetScore(nScore + pCMuRummyInfo->GetScore());
        
		pMsg.btResult = 1;
        pMsg.btScoreH = SET_NUMBERH(nScore);
        pMsg.btScoreL = SET_NUMBERL(nScore);
        pMsg.btTotScoreH = SET_NUMBERH(pCMuRummyInfo->GetScore());
        pMsg.btTotScoreL = SET_NUMBERL(pCMuRummyInfo->GetScore());
        DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
          
		this->GCSendMsg(aIndex, 5, 0);
        this->GDReqScoreUpdate(lpObj, pCMuRummyInfo->GetScore(), stCardUpdateDS);

		LogAddTD("[%s][%s][Mu Rummy] Slot[3] - Combine Success Gain Point(%d) (%d)",
			lpObj->AccountID,lpObj->Name, nScore, pCMuRummyInfo->GetScore());
	}

	this->GCSendCardList(aIndex);
}
    
void CMuRummyMng::CGReqMuRummyEnd(_tagPMSG_REQ_MURUMMY_END* lpMsg, int aIndex)
{
	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}

	if (!gObjIsConnected(aIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->CloseType != -1) 
	{
		return;
	}

	CMuRummyInfo* pCMuRummyInfo;// = lpObj->m_pCMuRummyInfo;

	if (!pCMuRummyInfo)
	{
		LogAddTD("[MuRummy][Error] pCMuRummyInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return;
	}

	if (pCMuRummyInfo->IsWaitReward())
	{
		LogAddTD("[MuRummy][DebugLog][%s][%s] Waiting Item Reward(Req End)", 
			lpObj->AccountID, lpObj->Name);
        GCServerMsgStringSend(lMsg.Get(3539), lpObj->m_Index, 1);
		return;
	}

	if (pCMuRummyInfo->GetScore() >= this->GetRewardScore1() && !CheckInventoryEmptySpace(lpObj, 4, 2))
	{
		this->GCSendMsg(aIndex, 7, 0);
		LogAddTD("[%s][%s][Mu Rummy] Event Reward fail (1)", lpObj->AccountID, lpObj->Name);
		return;
	}

	if (!gObjCheckMaxZen(aIndex, MURUMMY_REWARD_MONEY))
	{
		this->GCSendMsg(aIndex, 11, 0);
        LogAddTD("[%s][%s][Mu Rummy] Event Reward fail (0)", lpObj->AccountID, lpObj->Name);
		return;
	}

	if (pCMuRummyInfo->GetScore() >= this->GetRewardScore1())
	{
		if (pCMuRummyInfo->GetScore() >= this->GetRewardScore2())
		{
			//this->m_pRewardItemBag2->InsertInvenRewardItem(lpObj->m_Index, 226, lpObj->X, lpObj->Y);
		}
		else
		{
			//this->m_pRewardItemBag1->InsertInvenRewardItem(lpObj->m_Index, 226, lpObj->X, lpObj->Y);
		}

		pCMuRummyInfo->SetWaitReward(true);
	}

	this->GCSendMsg(aIndex, 10, 0);
    gObj[aIndex].Money += MURUMMY_REWARD_MONEY;
    GCMoneySend(aIndex, gObj[aIndex].Money);

    _tagPMSG_ANS_MURUMMY_END pMsg;
    pMsg.btResult = 1;
    pMsg.h.set((LPBYTE)&pMsg, 0x4D, 0x15, sizeof(_tagPMSG_ANS_MURUMMY_END));
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);

	this->GCSendMsg(aIndex, 8, 0);
	LogAddTD("[%s][%s][Mu Rummy] Event Reward Result [Point : %d, RewardZen : %d]",
		lpObj->AccountID, lpObj->Name, pCMuRummyInfo->GetScore(), MURUMMY_REWARD_MONEY);  
	LogAddTD("[%s][%s][Mu Rummy] Event end", lpObj->AccountID, lpObj->Name);
	//LogAddEvent("MuRummyEvent:MiniGameEnd", lpObj, ":Score[%d]", pCMuRummyInfo->GetScore());
	this->GDReqMuRummyDBLog(lpObj, pCMuRummyInfo->GetScore());

	pCMuRummyInfo->Clear();
	this->GDReqScoreDelete(lpObj);
}
    
void CMuRummyMng::GCSendCardList(int aIndex)
{
	_tagPMSG_SEND_SLOTCARD_LIST pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0x4D, 0x16, sizeof(_tagPMSG_SEND_SLOTCARD_LIST));

	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}

	if (!gObjIsConnected(aIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	CMuRummyInfo* pCMuRummyInfo;// = gObj[aIndex].m_pCMuRummyInfo;

	if (!pCMuRummyInfo)
	{
		LogAddTD("[MuRummy][Error] pCMuRummyInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return;
	}

	CMuRummyCardInfo* pCCardInfo = pCMuRummyInfo->GetCardInfo();
	
	if (!pCCardInfo)
	{
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return;
	}

	LPBYTE pSlotIndexInfo = pCMuRummyInfo->GetSlotIndexInfo();

	if (!pSlotIndexInfo)
	{
		return;
	}

	int nSlotNum = 0;
    
	for (int i = 0; i < MURUMMY_MAX_SLOT; i++)
	{
		if (pSlotIndexInfo[i] != (BYTE)-1)
		{
			if (nSlotNum < 0 || nSlotNum > 4)
			{
				return;
			}

			pMsg.stMuRummyCardInfo[nSlotNum].btColor = pCCardInfo[pSlotIndexInfo[i]].GetColor();
			pMsg.stMuRummyCardInfo[nSlotNum].btNumber = pCCardInfo[pSlotIndexInfo[i]].GetNumber();
			pMsg.stMuRummyCardInfo[nSlotNum].btSlotNum = pCCardInfo[pSlotIndexInfo[i]].GetSlotNum();
			nSlotNum++;
		}
	}

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}
    
void CMuRummyMng::GCSendMsg(int aIndex, BYTE btNotiIndex, int iValue)
{
	_tagPMSG_SEND_MURUMMY_MSG pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0x4D, 0x17, sizeof(_tagPMSG_SEND_MURUMMY_MSG));

	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}

	if (!gObjIsConnected(aIndex))
	{
		return;
	}

	//LPOBJ lpObj = &gObj[aIndex];
    
	pMsg.btNotiIndex = btNotiIndex;
	pMsg.btValueH = SET_NUMBERH(iValue);
	pMsg.btValueL = SET_NUMBERL(iValue);
    
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}
    
void CMuRummyMng::GDReqCardInfo(LPOBJ obj)
{
	if (!OBJMAX_RANGE(obj->m_Index))
	{
		return;
	}

	if (!gObjIsConnected(obj->m_Index))
	{
		return;
	}

	if (!this->IsMuRummyEventOn())
	{
		return;
	}

	_tagPMSG_REQ_MURUMMY_SELECT_DS pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xE8, 0x00, sizeof(_tagPMSG_SEND_MURUMMY_MSG));

	memcpy(pMsg.AccountID, obj->AccountID, sizeof(pMsg.AccountID));
	memcpy(pMsg.Name, obj->Name, sizeof(pMsg.Name));
	pMsg.aIndex = obj->m_Index;

	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size); 
}
    
void CMuRummyMng::GDAnsCardInfo(_tagPMSG_ANS_MURUMMY_SELECT_DS* lpMsg)
{
	int aIndex = lpMsg->aIndex;

	if (!OBJMAX_RANGE(aIndex))
	{
		return;
	}

	if (!gObjIsConnected(aIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (!lpObj)
	{
		//LogAddTD("[MuRummy][Error] lpObj is NULL [%s][%s] [%s, %d]", 121, 132, __FILE__, __LINE__);
		LogAddTD("[MuRummy][Error] lpObj is NULL [%s][%s] [%s, %d]", 'NONE', 'NONE', __FILE__, __LINE__);
		return;
	}

	CMuRummyInfo* pMuRummyInfo;// = lpObj->m_pCMuRummyInfo;

	if (!pMuRummyInfo)
	{
		LogAddTD("[MuRummy][Error] pMuRummyInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return;
	}

	CMuRummyCardInfo* pCCardInfo = pMuRummyInfo->GetCardInfo();

	if (!pCCardInfo)
	{
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL [%s][%s] [%s, %d]",
			lpObj->AccountID, lpObj->Name, __FILE__, __LINE__);
		return;
	}

	LPBYTE pSlotIndexInfo = pMuRummyInfo->GetSlotIndexInfo();

	if (!pSlotIndexInfo)
	{
		return;
	}

	if (!this->IsMuRummyEventOn())
	{
		return;
	}

	_tagPMSG_SEND_MURUMMYEVENT_ON pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0x4D, 0x18, sizeof(_tagPMSG_SEND_MURUMMYEVENT_ON));

	if (!lpMsg->btResult)
	{
		pMuRummyInfo->SetGetPlayCard(false);
		pMuRummyInfo->SetMuRummyInfoLoad(true);
        DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	pMuRummyInfo->SetScore(lpMsg->wScore);

	for (int i = 0; i < MURUMMY_MAX_SLOT; i++)
	{
		pSlotIndexInfo[i] = (BYTE)-1;
	}

	int iCardCnt = 0;

	for (int nCardArr = 0; nCardArr < MURUMMY_MAX_CARD; nCardArr++)
	{
		int nArr = lpMsg->stMuRummyCardInfoDS[nCardArr].btSeq;
		int iSlotNum = lpMsg->stMuRummyCardInfoDS[nCardArr].btSlotNum;

		pCCardInfo[nArr].SetColor(lpMsg->stMuRummyCardInfoDS[nCardArr].btColor);
		pCCardInfo[nArr].SetNumber(lpMsg->stMuRummyCardInfoDS[nCardArr].btNumber);
 		pCCardInfo[nArr].SetState(lpMsg->stMuRummyCardInfoDS[nCardArr].btStatus);
		pCCardInfo[nArr].SetSlotNum(iSlotNum);

		if (iSlotNum > -1 && iSlotNum < MURUMMY_MAX_SLOT && pCCardInfo[nArr].GetState() == 1 )
		{ 
			pSlotIndexInfo[iSlotNum] = nArr;
		}
		 
		if (!pCCardInfo[nArr].GetState())
		{
			iCardCnt++;
		}
		 
		LogAddTD("[MuRummy] MuRummy Card Info Load [%s][%s] Seq:[%d] [%d][%c][%d][%d]",
			lpObj->AccountID, lpObj->Name, nArr, pCCardInfo[nArr].GetSlotNum(),
			this->GetColorName(pCCardInfo[nArr].GetColor()),
			pCCardInfo[nArr].GetNumber(), pCCardInfo[nArr].GetState());
	}
}

void CMuRummyMng::GDReqCardInfoInsert(LPOBJ obj)
{
	if (!OBJMAX_RANGE(obj->m_Index))
	{
		return;
	}

	if (!gObjIsConnected(obj->m_Index))
	{
		return;
	}

	if (!this->IsMuRummyEventOn())
	{
		return;
	}

	CMuRummyInfo* pMuRummyInfo;// = obj->m_pCMuRummyInfo;

	if (!pMuRummyInfo)
	{
		LogAddTD("[MuRummy][Error] pMuRummyInfo is NULL [%s][%s] [%s, %d]",
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return;
	}

	CMuRummyCardInfo* pCCardInfo = pMuRummyInfo->GetCardInfo();

	if (!pCCardInfo)
	{
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL [%s][%s] [%s, %d]",
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return;
	}

	LPBYTE pSlotIndexInfo = pMuRummyInfo->GetSlotIndexInfo();

	if (!pSlotIndexInfo)
	{
		LogAddTD("[MuRummy][Error] pSlotIndexInfo is NULL [%s][%s] [%s, %d]",
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return;
	}

	_tagPMSG_REQ_MURUMMY_INSERT_DS pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xE8, 0x01, sizeof(_tagPMSG_REQ_MURUMMY_INSERT_DS));

	memcpy(pMsg.AccountID, obj->AccountID, sizeof(pMsg.AccountID));
	memcpy(pMsg.Name, obj->Name, sizeof(pMsg.Name));
	pMsg.aIndex = obj->m_Index;

	for (int i = 0; i < MURUMMY_MAX_CARD; i++)
	{
		pMsg.stMuRummyCardInfoDS[i].btColor = pCCardInfo[i].GetColor();
		pMsg.stMuRummyCardInfoDS[i].btNumber = pCCardInfo[i].GetNumber();
        pMsg.stMuRummyCardInfoDS[i].btSlotNum = pCCardInfo[i].GetSlotNum();
		pMsg.stMuRummyCardInfoDS[i].btStatus = pCCardInfo[i].GetState();
		pMsg.stMuRummyCardInfoDS[i].btSeq = i;
	}

	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}
    
	
void CMuRummyMng::GDReqCardInfoUpdate(LPOBJ obj, CMuRummyCardInfo* pCCardInfo, int iSeq)
{
	_tagPMSG_REQ_MURUMMY_UPDATE_DS pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xE8, 0x03, sizeof(_tagPMSG_REQ_MURUMMY_INSERT_DS));
	memcpy(pMsg.AccountID, obj->AccountID, sizeof(pMsg.AccountID));
	memcpy(pMsg.Name, obj->Name, sizeof(pMsg.Name));
	pMsg.btSlotNum = pCCardInfo->GetSlotNum();
	pMsg.btStatus = pCCardInfo->GetState();
	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}
    
void CMuRummyMng::GDReqScoreUpdate(LPOBJ obj, WORD wScore, _tagMuRummyCardUpdateDS* pCardUpdateDS)
{
	_tagPMSG_REQ_MURUMMY_SCORE_UPDATE_DS pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xE8, 0x02, sizeof(_tagPMSG_REQ_MURUMMY_SCORE_UPDATE_DS));
	memcpy(pMsg.AccountID, obj->AccountID, sizeof(pMsg.AccountID));
	memcpy(pMsg.Name, obj->Name, sizeof(pMsg.Name));
	memcpy(pMsg.stCardUpdateDS, &pCardUpdateDS->btSlotNum, sizeof(pMsg.stCardUpdateDS));
	pMsg.wScore = wScore;
	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}
    
void CMuRummyMng::GDReqScoreDelete(LPOBJ obj)
{
	_tagPMSG_REQ_MURUMMY_DELETE_DS pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xE8, 0x04, sizeof(_tagPMSG_REQ_MURUMMY_SCORE_UPDATE_DS));
	memcpy(pMsg.AccountID, obj->AccountID, sizeof(pMsg.AccountID));
	memcpy(pMsg.Name, obj->Name, sizeof(pMsg.Name));
	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}
    
void CMuRummyMng::GDReqSlotInfoUpdate(LPOBJ obj, BYTE btSeq, BYTE btSlotNum, BYTE btStatus)
{
	_tagPMSG_REQ_MURUMMY_SLOTUPDATE_DS pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xE8, 0x05, sizeof(_tagPMSG_REQ_MURUMMY_SLOTUPDATE_DS));

	if (btSlotNum < 0 || btSlotNum > 4)
	{
		LogAddTD("[MuRummy][Error] btSlotNum[%d]  [%s][%s] [%s, %d]",
			btSlotNum, obj->AccountID, obj->Name, __FILE__, __LINE__);
		return;
	}

	pMsg.stCardUpdateDS.btSeq = btSeq;
    pMsg.stCardUpdateDS.btStatus = btStatus;
    pMsg.stCardUpdateDS.btSlotNum = btSlotNum;
    memcpy(pMsg.AccountID, obj->AccountID, sizeof(pMsg.AccountID));
	memcpy(pMsg.Name, obj->Name, sizeof(pMsg.Name));
	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}
    
void CMuRummyMng::GDReqMuRummyInfoUpdate(LPOBJ obj)
{
	if (!OBJMAX_RANGE(obj->m_Index))
	{
		return;
	}

	if (!gObjIsConnected(obj->m_Index))
	{
		return;
	}

	if (!this->IsMuRummyEventOn())
	{
		return;
	}

	CMuRummyInfo* pMuRummyInfo;// = obj->m_pCMuRummyInfo;

    if (!pMuRummyInfo)
    {
		LogAddTD("[MuRummy][Error] pMuRummyInfo is NULL [%s][%s] [%s, %d]",
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return;
	}

	CMuRummyCardInfo* pCCardInfo = pMuRummyInfo->GetCardInfo();

	if (!pCCardInfo)
    {
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL [%s][%s] [%s, %d]",
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return;
	}

	_tagPMSG_REQ_MURUMMY_INFO_UPDATE_DS pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xE8, 0x06, sizeof(_tagPMSG_REQ_MURUMMY_INFO_UPDATE_DS));
	memcpy(pMsg.AccountID, obj->AccountID, sizeof(pMsg.AccountID));
	memcpy(pMsg.Name, obj->Name, sizeof(pMsg.Name));
	pMsg.wScore = pMuRummyInfo->GetScore();
	
	for (int i = 0; i < MURUMMY_MAX_CARD; i++)
	{
		pMsg.stMuRummyCardUpdateDS[i].btSlotNum = pCCardInfo[i].GetSlotNum();
		pMsg.stMuRummyCardUpdateDS[i].btStatus = pCCardInfo[i].GetState();
		pMsg.stMuRummyCardUpdateDS[i].btSeq = i;

		if (pCCardInfo[i].GetColor() != (BYTE)-1)
		{
			LogAddTD("[MuRummy] MuRummy Info Save [%s][%s] [%d][%c][%d][%d] ",
				obj->AccountID, obj->Name, pCCardInfo[i].GetSlotNum(),
				this->GetColorName(pCCardInfo[i].GetColor()),
				pCCardInfo[i].GetNumber(), pCCardInfo[i].GetState());

		}
	}

	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}
    
void CMuRummyMng::GDReqMuRummyDBLog(LPOBJ obj, int iScore)
{
	if (!OBJMAX_RANGE(obj->m_Index))
	{
		return;
	}

	if (!gObjIsConnected(obj->m_Index))
	{
		return;
	}

	if (!this->IsMuRummyEventOn() || !g_MuRummyEventDBLog || !iScore)
	{
		return;
	}

	_tagPMSG_REQ_MURUMMY_LOG_INSERT_DS pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xE8, 0x07, sizeof(_tagPMSG_REQ_MURUMMY_LOG_INSERT_DS));
	memcpy(pMsg.AccountID, obj->AccountID, sizeof(pMsg.AccountID));
	memcpy(pMsg.Name, obj->Name, sizeof(pMsg.Name));
	pMsg.wScore = iScore;
	cDBSMng.Send((PCHAR)&pMsg, pMsg.h.size);
}
    
void CMuRummyMng::Slot5Log(LPOBJ obj, int iLogType)
{
	if (!OBJMAX_RANGE(obj->m_Index))
	{
		return;
	}

	CMuRummyInfo* pMuRummyInfo;// = obj->m_pCMuRummyInfo;

	if (!pMuRummyInfo)
	{
		LogAddTD("[MuRummy][Error] pMuRummyInfo is NULL [%s][%s] [%s, %d]",
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return;
	}

	CMuRummyCardInfo* pCCardInfo = pMuRummyInfo->GetCardInfo();

	if (!pCCardInfo)
	{
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL [%s][%s] [%s, %d]",
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return;
	}

	LPBYTE pSlotIndexInfo = pMuRummyInfo->GetSlotIndexInfo();

	if (!pSlotIndexInfo)
	{
		LogAddTD("[MuRummy][Error] pSlotIndexInfo is NULL [%s][%s] [%s, %d]",
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return;
	}

	char szSlotInfo[1024];
	char szTemp[1024];
	_tagMuRummyCardInfo stMuRummyCardInfo[5];

	for (int n = 0; n < 5; n++)
	{
		if (pSlotIndexInfo[n] == (BYTE)-1)
		{
			strcat(szTemp, "None");
		}
		else
		{
			sprintf(szSlotInfo, "%c(%d)", 
				this->GetColorName(pCCardInfo[pSlotIndexInfo[n]].GetColor() - 1),  
				pCCardInfo[pSlotIndexInfo[n]].GetNumber());
			strcat(szTemp, szSlotInfo);
		}

		if (n != 4)
		{
			strcat(szTemp, " ,");
		}
	}

	if (iLogType)
	{
		LogAddTD("[%s][%s][Mu Rummy] Slot[5] - Register Result [ %s ]", obj->AccountID, obj->Name, szTemp);
	}
    else
	{
		LogAddTD("[%s][%s][Mu Rummy] Slot[5] - [ %s ]", obj->AccountID, obj->Name, szTemp);
	}
}
    
void CMuRummyMng::Slot3Log(LPOBJ obj, int iLogType)
{
	if (!OBJMAX_RANGE(obj->m_Index))
	{
		return;
	}

	CMuRummyInfo* pMuRummyInfo;// = obj->m_pCMuRummyInfo;

	if (!pMuRummyInfo)
	{
		LogAddTD("[MuRummy][Error] pMuRummyInfo is NULL [%s][%s] [%s, %d]",
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return;
	}

	CMuRummyCardInfo* pCCardInfo = pMuRummyInfo->GetCardInfo();

	if (!pCCardInfo)
	{
		LogAddTD("[MuRummy][Error] pCCardInfo is NULL [%s][%s] [%s, %d]",
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return;
	}

	LPBYTE pSlotIndexInfo = pMuRummyInfo->GetSlotIndexInfo();

	if (!pSlotIndexInfo)
	{
		LogAddTD("[MuRummy][Error] pSlotIndexInfo is NULL [%s][%s] [%s, %d]",
			obj->AccountID, obj->Name, __FILE__, __LINE__);
		return;
	}

	char szSlotInfo[1024];
	char szTemp[1024];
	_tagMuRummyCardInfo stMuRummyCardInfo[5];

	for (int n = 5; n < MURUMMY_MAX_SLOT; n++)
	{
		if (pSlotIndexInfo[n] == (BYTE)-1)
		{
			strcat(szTemp, "None");
		}
		else
		{
			sprintf(szSlotInfo, "%c(%d)", 
				this->GetColorName(pCCardInfo[pSlotIndexInfo[n]].GetColor() - 1),  
				pCCardInfo[pSlotIndexInfo[n]].GetNumber());
			strcat(szTemp, szSlotInfo);
		}

		if (n != 7)
		{
			strcat(szTemp, " ,");
		}
	}

	if (iLogType)
	{
		LogAddTD("[%s][%s][Mu Rummy] Slot[3] - Register Result [ %s ]", obj->AccountID, obj->Name, szTemp);
	}
    else
	{
		LogAddTD("[%s][%s][Mu Rummy] Slot[3] - [ %s ]", obj->AccountID, obj->Name, szTemp);
	}
}

char CMuRummyMng::GetColorName(int iColorNum)
{
	char szColor[] = { 'R', 'B', 'Y', 'E' };

	if (iColorNum < 0 || iColorNum > 3)
	{
		return szColor[3];
	}

	return szColor[iColorNum];
}