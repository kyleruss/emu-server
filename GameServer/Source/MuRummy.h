#pragma once

#include "MuRummyProtocol.h"
#include "user.h"

class CMuRummyCardInfo
{
public:
    CMuRummyCardInfo();
    ~CMuRummyCardInfo();

    void SetColor(int iColor) { this->m_iColor = iColor; };
    void SetNumber(int iNumber) { this->m_iNumber = iNumber; };
    void SetState(int iState) { this->m_iState = iState; };
    void SetSlotNum(int iSlotNum) { this->m_iSlotNum = iSlotNum; };
    int GetColor() { return this->m_iColor; };
    int GetNumber() { return this->m_iNumber; };
    int GetState() { return this->m_iState; };
    int GetSlotNum() { return this->m_iSlotNum; };

private:
    int m_iColor;
    int m_iNumber;
    int m_iState;
    int m_iSlotNum;
};

class CMuRummyInfo
{
public:
    CMuRummyInfo();
    virtual ~CMuRummyInfo();

    void Init();
    void Release();
    void Clear();

    CMuRummyCardInfo* GetCardInfo();
    BYTE* GetSlotIndexInfo();
    void SetSlotIndexInfo(int, int); //hidden?

    void SetScore(int iScore) { this->m_iScore = iScore; };
    void SetCardDeckCnt(int iCardDeckCnt) { this->m_iCardDeckCnt = iCardDeckCnt; };
    void SetGetPlayCard(bool bPlayCard) { this->m_bPlayCard = bPlayCard; };
    void SetMuRummyInfoLoad(bool bMuRummyInfoLoad) { this->m_bMuRummyInfoLoad = bMuRummyInfoLoad; };
    void SetWaitReward(bool bWaitReward) { this->m_bWaitReward = bWaitReward; };

    int GetScore() { return this->m_iScore; };
    int GetCardDeckCnt() { return this->m_iCardDeckCnt; };
    bool IsGetPlayCard() { return this->m_bPlayCard; };
    bool IsMuRummyInfoLoad() { return this->m_bMuRummyInfoLoad; };
    bool IsWaitReward() { return this->m_bWaitReward; };

private:
    int m_iScore;
    int m_iCardDeckCnt;
    bool m_bPlayCard;
    bool m_bMuRummyInfoLoad;
    bool m_bWaitReward;
    CMuRummyCardInfo* m_pCCardInfo;
    LPBYTE m_pSlotCardIndex;
};

class CMuRummyMng
{
public:
    CMuRummyMng();
    ~CMuRummyMng();

    void LoadPreFixData(LPSTR lpszFileName);
    void ProbabilityLoadData();
    int IsMuRummyEventOn();
    int GetRewardScore1();
    int GetRewardScore2();
    void SetCardDeck(CMuRummyInfo* pMuRummyInfo);
    void CardShuffle(CMuRummyCardInfo* pCCardInfo);
    bool SetPlayCardInfo(CMuRummyInfo* pMuRummyInfo, _tagMuRummyCardInfo* pOutCardInfo);
    BYTE GetTempCardShowSlot(LPBYTE pSlotCardIndex);
    BYTE CardSlotMove(CMuRummyInfo* pMuRummyInfo, int sSlot, int tSlot, CMuRummyCardInfo* pCOutCardInfo, LPOBJ obj);
    BYTE CardSlotReMove(CMuRummyInfo* pCMuRummyInfo, int sSlot);
    BYTE CardMatchCheck(CMuRummyInfo* pCMuRummyInfo, int& iOutScore, _tagMuRummyCardUpdateDS* pOutCardUpdateDS, LPOBJ obj);
    void CardSortAscending(CMuRummyCardInfo* pCCardInfo);
    int IsCardSameColor(CMuRummyCardInfo* pCCardInfo);
    int IsCardNumber(CMuRummyCardInfo* pCCardInfo);
    bool FillEmptySlotCard(CMuRummyInfo* pMuRummyInfo, _tagMuRummyCardInfo* pOutCardInfo, LPOBJ obj);
    
	void CGReqMuRummyEventOpen(_tagPMSG_REQ_MURUMMY_EVENT_OPEN* lpMsg, int aIndex);
    void CGReqMuRummyStart(_tagPMSG_REQ_MURUMMY_INFO* lpMsg, int aIndex);
    void CGReqCardReveal(_tagPMSG_REQ_REVEAL_CARD* lpMsg, int aIndex);
    void CGReqCardMove(_tagPMSG_REQ_CARD_MOVE* lpMsg, int aIndex);
    void CGReqCardReMove(_tagPMSG_REQ_CARD_REMOVE* lpMsg, int aIndex);
    void CGReqCardMatch(_tagPMSG_REQ_CARD_MATCH* lpMsg, int aIndex);
    void CGReqMuRummyEnd(_tagPMSG_REQ_MURUMMY_END* lpMsg, int aIndex);
    void GCSendCardList(int aIndex);
    void GCSendMsg(int aIndex, BYTE btNotiIndex, int iValue);
    void GDReqCardInfo(LPOBJ obj);
    void GDAnsCardInfo(_tagPMSG_ANS_MURUMMY_SELECT_DS* lpMsg);
    void GDReqCardInfoInsert(LPOBJ obj);
    void GDReqCardInfoUpdate(LPOBJ obj, CMuRummyCardInfo* pCCardInfo, int iSeq);
    void GDReqScoreUpdate(LPOBJ obj, WORD wScore, _tagMuRummyCardUpdateDS* lpMsg);
    void GDReqScoreDelete(LPOBJ obj);
    void GDReqSlotInfoUpdate(LPOBJ obj, BYTE btSeq, BYTE btSlotNum, BYTE btStatus);
    void GDReqMuRummyInfoUpdate(LPOBJ obj);
    void GDReqMuRummyDBLog(LPOBJ obj, int iScore);

    void Slot5Log(LPOBJ obj, int iLogType);
    void Slot3Log(LPOBJ obj, int iLogType);
    char GetColorName(int iColorNum);
 
private:
    _SYSTEMTIME m_tmEventEndDate;
//    CProbabilityItemBag* m_pRewardItemBag1;
 //   CProbabilityItemBag* m_pRewardItemBag2;
    int m_iRewardScore1;
    int m_iRewardScore2;
};

extern CMuRummyMng g_CMuRummyMng;