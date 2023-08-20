#pragma once

enum e_PerTradeState	{												// Item personal transaction status..
							PER_TRADE_STATE_NONE = 0,					// naught..
							PER_TRADE_STATE_WAIT_FOR_REQ,				// Make a request to the other person and wait for a response.
							PER_TRADE_STATE_WAIT_FOR_MY_DECISION_AGREE_OR_DISAGREE,	// Receiving a request from the other person and waiting for my decision..
							PER_TRADE_STATE_NORMAL,						// Item Personal Transaction Normal condition..
							PER_TRADE_STATE_ADD_AND_WAIT_FROM_SERVER,	// Add item and wait for response from server..
							PER_TRARE_STATE_EDITTING,					// Editing item personal transaction amount or number etc..
							PER_TRADE_STATE_MY_TRADE_DECISION_DONE,		// Hold down my trade button...
						};

enum e_PerTradeResultCode	{										// Define the code that unlocks the item trade..
								PER_TRADE_RESULT_MY_DISAGREE = 0,	// I, who received the transaction request, canceled the transaction request..
								PER_TRADE_RESULT_OTHER_DISAGREE,	// The counterparty who received the transaction request cancels the transaction request.
								PER_TRADE_RESULT_MY_CANCEL,			// I, who applied for a transaction, canceled the transaction request..


							};

enum e_PerTradeProceedCode	{										// Defines the state in which item trading continues.
								PER_TRADE_RESULT_MY_AGREE = 0,		// I, who received the transaction request, accepted the transaction request..
								PER_TRADE_RESULT_OTHER_AGREE,		// The counterparty to whom the transaction was requested approves the transaction request.



							};

enum e_PerTradeItemKindBackup	{
									PER_TRADE_ITEM_MONEY = 0,		// ADD in the personal trading window before is money.
									PER_TRADE_ITEM_OTHER,			// This is an item that was ADD in the personal trading window before.
								};

const DWORD dwGold = 900000000;	// hmm...

class CUIManager;
class CUIPerTradeDlg;
class CUITradeEditDlg;

#include "GameBase.h"

class CSubProcPerTrade : public CGameBase
{
protected:
	int					m_iOtherID;
	int					m_iGoldOffsetBackup;

	std::string			m_szMsg;	// MessageBox key

public:
	CUIPerTradeDlg*				m_pUIPerTradeDlg;
	CUITradeEditDlg*			m_pUITradeEditDlg;

	e_PerTradeState				m_ePerTradeState;
	e_PerTradeItemKindBackup	m_ePerTradeItemKindBackup;

public:
	CSubProcPerTrade();
	virtual ~CSubProcPerTrade();

	void	Release();

	void	InitPerTradeDlg(CUIManager* pUIManager);

	void	EnterWaitMsgFromServerStatePerTradeReq();			// The state in which I applied for an item transaction to someone else..
	void	EnterWaitMsgFromServerStatePerTradeReq(std::string szName);			// The state in which I applied for an item transaction to someone else..
	void	EnterWaitMyDecisionToPerTrade(int iOtherID);		// The state in which I received an item transaction request from someone else..
	void	LeavePerTradeState(e_PerTradeResultCode ePTRC);		// The code to release the item transaction status..
	void	ProcessProceed(e_PerTradeProceedCode ePTPC);		// Defines the state in which item trading continues.

	void	SecureCodeBegin() const;									// protection code..

	void	PerTradeCoreStart();
	void	PerTradeCoreInvDisable();

	void	RequestItemCountEdit();
	void	ItemCountEditOK();
	void	ItemCountEditCancel();

	void	FinalizePerTrade();									// Literally the final aftermath...
	void	PerTradeCompleteSuccess();							// Personal transaction final success..
	void	PerTradeCompleteCancel();							// Cancellation of personal transaction..

	void	PerTradeMyDecision();								// I decided to trade...
	void	PerTradeOtherDecision();							// Someone else decided to make a deal.

	void	SecureJobStuffByMyDecision();

	void	ReceiveMsgPerTradeReq(int iOtherID);
	void	ReceiveMsgPerTradeAgree(BYTE bResult);
	void	ReceiveMsgPerTradeAdd(BYTE bResult);
	void	ReceiveMsgPerTradeOtherAdd(int iItemID, int iCount, int iDurability);
	void	ReceiveMsgPerTradeOtherDecide();
	void	ReceiveMsgPerTradeDoneSuccessBegin(int iTotalGold);
	void	ReceiveMsgPerTradeDoneItemMove(BYTE bItemPos, int iItemID, int iCount, int iDurability);
	void	ReceiveMsgPerTradeDoneSuccessEnd();
	void	ReceiveMsgPerTradeDoneFail();
	void	ReceiveMsgPerTradeCancel();

	// Item Count OK..
	void	ItemCountOK();
	void	ItemCountCancel();
};
