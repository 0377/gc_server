#include "common.h"
#include "MoveCompent.h"
#include "MathAide.h"
#include "PathManager.h"
//#include "../��Ϣ����/CMD_Fish.h"
#include "MyObject.h"
#include "GameConfig.h"
#include <math.h>

MoveCompent::MoveCompent()
:m_bPause(false)
,m_fSpeed(1.0f)
,m_nPathID(0)
,m_bEndPath(false)
,m_fDelay(0.0f)
,m_bBeginMove(false)
,m_bRebound(true)
,m_dwTargetID(0)
,m_pObjMgr(NULL)
,m_bTroop(false)
{
	SetPosition(-5000, -5000);
}
//����
void MoveCompent::OnDetach()
{
	m_bPause = false;
	m_fSpeed = 1.0f;
	m_nPathID = 0;
	m_bEndPath = false;
	m_fDelay = 0.0f;
	m_bBeginMove = false;
	m_bRebound = true;
	m_dwTargetID = 0;
	m_pObjMgr = NULL;
	SetPosition(-5000, -5000);
}
//�¼�����
void MoveCompent::OnCCEvent(CComEvent* pEvent)
{
	if(pEvent != NULL)
	{
		switch(pEvent->GetID())
		{
		case EME_STATE_CHANGED:
			{
				if(pEvent->GetParam1() >= EOS_DEAD)
				{
					SetPause(true);
				}
				break;
			}
		}
	}
}
//·���ƶ���ʼ��
void MoveByPath::InitMove()
{
	//��ȡ·��
	MovePoints* pPath = PathManager::GetInstance()->GetPathData(GetPathID(), bTroop());
	if(pPath != NULL)
	{
		m_fDuration = pPath->size();
	}
	//��ʼ�ƶ�����
	m_Elaspe = 0.0f;
	m_bEndPath = false;
}
//·���ƶ�
void MoveByPath::OnUpdate(int ms)
{
 	if(m_bPause || m_bEndPath) return;

	MovePoints* pPath = PathManager::GetInstance()->GetPathData(GetPathID(), bTroop());
	if(pPath == NULL) return;

	if(ms < 0) ms = 1000/GAME_FPS;

	float fdt =  ms / 1000.0f;
	//��ѯ��ǰ�ٶȣ��ӵ�EME_QUERY_SPEED_MUL�� ����
	MyObject* pOwner = GetOwner();
	if(pOwner != NULL)
	{
		CComEvent se;
		se.SetID(EME_QUERY_SPEED_MUL);
		se.SetParam1(0);
		se.SetParam2(&fdt);

		pOwner->ProcessCCEvent(&se);
	}
	//��ʱ�ж�
	if(m_fDelay > 0)
	{
		m_fDelay -= fdt;
		return;
	}
	//�Ƿ�ʼ�ƶ�
	if(m_bBeginMove == false && m_Elaspe > 0)
	{
		m_bBeginMove = true;
	}
	//�Ѿ��ƶ��ľ��� + ʱ�� X �ٶ�
	m_Elaspe += fdt * GetSpeed();
	//�趨��ʼ�����ꣿ
	CMovePoint mp(MyPoint(-5000, -5000), 0.0f);
	//��ȡ�ƶ��ڵ�ٷֱ�
	float time = min(1.0f, (m_Elaspe / m_fDuration));
	float fDiff;
	//�õ��ڵ�ƫ��ֵ��
	float fIndex = time * pPath->size();
	//ȡ���ڵ�
	int index = fIndex;
	//ƫ��
	fDiff = fIndex - index;
	//�ж��Ƿ񵽴��յ�򳬹�
	if (index >= pPath->size())
	{
		index = pPath->size() - 1;
	}
	else if(index < 0 || fDiff < 0)
	{
		index = 0;
		fDiff = 0;
	}
	//���С�����ڵ���-1 �ƶ�
	if (index < pPath->size() - 1)
	{
		//��ȡ��ǰ�ڵ����꼰��һ�ڵ�����
		CMovePoint move_point1 = pPath->at(index);
		CMovePoint move_point2 = pPath->at(index+1);
		//�������� �����
		mp.m_Position = move_point1.m_Position*(1.0-fDiff)+ move_point2.m_Position*fDiff;
		mp.m_Direction = move_point1.m_Direction*(1.0-fDiff)+ move_point2.m_Direction*fDiff;
		//��ȡ���շ���
		if (std::abs(move_point1.m_Direction-move_point2.m_Direction) > M_PI)
		{
			mp.m_Direction = move_point1.m_Direction;
		}
	}
	else
	{//�����յ�
		mp = pPath->at(index);
		m_bEndPath = true;
	}
	//��������
 	SetPosition(mp.m_Position + m_Offest);
	//���÷���
 	SetDirection(mp.m_Direction);
}
//�����ƶ�
void MoveByDirection::OnUpdate(int ms)
{
	if(m_bPause || m_bEndPath) return;

	if(ms < 0) ms = 1000/GAME_FPS;
	//�ж��Ƿ��������Ŀ��
	if(m_pObjMgr != NULL && m_dwTargetID != 0)
	{
		MyObject* pObj = m_pObjMgr->Find(m_dwTargetID);
		if(pObj != NULL && pObj->GetState() < EOS_DEAD && pObj->InSideScreen())
		{
			//����������10���¶�λ����С��10���ٶ�λ
			if(CMathAide::CalcDistance(pObj->GetPosition().x_, pObj->GetPosition().y_, GetPostion().x_, GetPostion().y_) > 10)
			{
				//ͨ������Ŀ�����Լ�����ǶȻ�ȡ����
				SetDirection(CMathAide::CalcAngle(pObj->GetPosition().x_, pObj->GetPosition().y_, GetPostion().x_, GetPostion().y_));
				//��ʼ�ƶ�
				InitMove();
			}
			else
			{
				//���������뷽��
				SetPosition(pObj->GetPosition());
				SetDirection(pObj->GetDirection());
				return;
			}
		}
		else
		{
			//Ŀ��Ϊ��
			m_dwTargetID = 0;
		}
	}

	float fdt =  ms / 1000.0f;
	//��ѯ��ǰ�ٶȣ��ӵ�EME_QUERY_SPEED_MUL������
	MyObject* pOwner = GetOwner();
	if(pOwner != NULL)
	{
		CComEvent se;
		se.SetID(EME_QUERY_SPEED_MUL);
		se.SetParam1(0);
		se.SetParam2(&fdt);

		pOwner->ProcessCCEvent(&se);
	}
	//��ʱ�ж�
	if(m_fDelay > 0)
	{
		m_fDelay -= fdt;
		return;
	}
	//�Ƿ�����ƶ�
	if(m_bBeginMove == false)
	{
		m_bBeginMove = true;
	}
	//��ȡ��ǰ����
	MyPoint pt(GetPostion());

	pt.x_ += m_fSpeed* dx_ * fdt;
	pt.y_ += m_fSpeed* dy_ * fdt;
	//��ȡĬ�Ͽ�ȸ߶�
	float fWidth = CGameConfig::GetInstance()->nDefaultWidth;
	float fHeigth = CGameConfig::GetInstance()->nDefaultHeight;
	//�Ƿ񷴵�
	if(Rebound())
	{
		//���㷴��
		if (pt.x_ < 0.0f) { pt.x_ = 0 + (0 - pt.x_); dx_ = -dx_; angle_ =  - angle_; }
		if (pt.x_ > fWidth)  {pt.x_ = fWidth - (pt.x_ - fWidth); dx_ = -dx_; angle_ =  - angle_;}

		if (pt.y_ < 0.0f) { pt.y_ = 0 + (0 - pt.y_); dy_ = -dy_; angle_ = M_PI - angle_;}
		if (pt.y_ > fHeigth)  {pt.y_ = fHeigth - (pt.y_ - fHeigth); dy_ = -dy_; angle_ = M_PI - angle_;}
	}
	else
	{
		//�������Ƿ񵽱���
		if(pt.x_ < 0 || pt.x_ > fWidth || pt.y_ < 0 || pt.y_ > fHeigth)
			m_bEndPath = true;
	}

	if(pOwner != NULL)//����(���ϱ��Ѿ��жϹ��Ƿ���ڣ�
	{
		//���÷���
		SetDirection(pOwner->GetObjType() == EOT_FISH ? angle_ - M_PI_2 : angle_);
	}
	//��������
	SetPosition(pt);
}
//��ʼ���ƶ�
void MoveByDirection::InitMove()
{
	angle_ = GetDirection();
	dx_ = cosf(angle_ - M_PI_2);
	dy_ = sinf(angle_ - M_PI_2);
	m_bEndPath = false;
}





