#pragma once

#include "N3UIBase.h"

class CUICmd : public CN3UIBase  
{
public:
	CN3UIBase*	m_pBtn_Exit;			//������

	CN3UIBase*	m_pBtn_Act;			//�ൿ
	CN3UIBase*	m_pBtn_Act_Walk;	//�ȱ�
	CN3UIBase*	m_pBtn_Act_Run;		//�޸���
	CN3UIBase*	m_pBtn_Act_Stop;	//����
	CN3UIBase*	m_pBtn_Act_Attack;	//����
	
	CN3UIBase*	m_pBtn_Act_StandUp; // �Ͼ��.
	CN3UIBase*	m_pBtn_Act_SitDown;	// �ɱ�

	CN3UIBase*	m_pBtn_Camera;			//ī�޶�
	CN3UIBase*	m_pBtn_Inventory;		//������ â 
	CN3UIBase*	m_pBtn_Party_Invite;	//��Ƽ �ʴ�
	CN3UIBase*	m_pBtn_Party_Disband;	//��Ƽ Ż��
	CN3UIBase*  m_pBtn_Option;			//�ɼ�
	CN3UIBase*	m_pBtn_Quest;			//����Ʈ
	CN3UIBase*	m_pBtn_Character;		//�ڱ� ����â   
	CN3UIBase*	m_pBtn_Skill;			//��ųƮ�� �Ǵ� ����â 
	CN3UIBase*	m_pBtn_Belong;			//�Ҽ� ���� 
	CN3UIBase*	m_pBtn_Map;				// �̴ϸ�

public:
	bool OnKeyPress(int iKey);
//	void	SetVisibleOptButtons(bool bVisible);
//	void	SetVisibleActButtons(bool bVisible);

	bool	Load(HANDLE hFile);
	bool	ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg);
	void	UpdatePartyButtons(bool bIAmLeader, bool bIAmMemberOfParty, int iMemberIndex, const class CPlayerBase* pTarget);

	CUICmd();
	virtual ~CUICmd();
};
