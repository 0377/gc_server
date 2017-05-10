#include "common.h"
#include "Effect.h"
#include "Bullet.h"
#include "Player.h"
#include "MyObjectManager.h"
#include "EffectManager.h"
#include "BufferManager.h"
#include "MathAide.h"
#include "EventMgr.h"
#include "MoveCompent.h"
#include "GameConfig.h"
//#include "../��Ϣ����/CMD_Fish.h"

CEffect::CEffect()
:m_nType(ETP_ADDMONEY)
{
	m_nParam.resize(2);
	ClearParam();
}

CEffect::~CEffect()
{
}

void CEffect::ClearParam()
{
	for(int i = 0; i < m_nParam.size(); ++i)
	{
		m_nParam[i] = 0;
	}
}

int CEffect::GetParam(int pos)
{
	if(pos >= m_nParam.size()) return 0;

	return m_nParam[pos];
}

void CEffect::SetParam(int pos, int p)
{
	if(pos > m_nParam.size()) return;

	m_nParam[pos] = p;
}

CEffectAddMoney::CEffectAddMoney()
:CEffect()
{
	m_nParam.resize(3);
	ClearParam();
	lSco = 0;
}
LONGLONG CEffectAddMoney::Execute(MyObject* pSelf, MyObject* pTarget, std::list<MyObject*>& list, bool bPretreating)
{
	if(pSelf == NULL) return 0;

	LONGLONG lScore = 0;
	int mul = 1;

	if(lSco == 0)
	{
		lSco =  GetParam(2) > GetParam(1) ? RandInt(GetParam(1), GetParam(2)) : GetParam(1);
	}

	if(GetParam(0) == 0) 
	{
		mul = 1;
	}
	else if(pTarget != NULL)
	{
		mul = pTarget->GetScore();
	}

	int n = -1;
	CComEvent se;
	se.SetID(EME_QUERY_ADDMUL);
	se.SetParam1(0);
	se.SetParam2(&n);
	pSelf->ProcessCCEvent(&se);

	if(n != -1)
	{
		lSco = CGameConfig::GetInstance()->nAddMulBegin;

		if(n + lSco > GetParam(2))
			n = GetParam(2) - lSco;

		if(!bPretreating)
			CGameConfig::GetInstance()->nAddMulCur = 0;
	}
	else
		n = 0;

	lScore = (lSco + n) * mul;

	if(pTarget->GetObjType() == EOT_BULLET && ((CBullet*)pTarget)->bDouble())
		lScore *= 2;

	return lScore;
}

CEffectKill::CEffectKill()
:CEffect()
{
	m_nParam.resize(3);
	ClearParam();
}

LONGLONG CEffectKill::Execute(MyObject* pSelf, MyObject* pTarget, std::list<MyObject*>& list, bool bPretreating)
{
	if(pSelf == NULL) return 0;

	LONGLONG score = 0;

	if(!bPretreating)
		RaiseEvent("AddChain", this, pSelf);		//�޴����Ӧ 

	MyObjMgr* pMgr = pSelf->GetMgr();
	if(pMgr != NULL)
	{
		pMgr->Lock();
		obj_table_iter ifs = pMgr->Begin();
		while(ifs != pMgr->End())
		{
			MyObject* pObj = ifs->second;
			if(pObj != pSelf)
			{
				EffectMgr* pEm = (EffectMgr*)pObj->GetComponent(ECF_EFFECTMGR);
				MoveCompent* pMove = (MoveCompent*)pObj->GetComponent(ECF_MOVE);
				if(pEm != NULL && pMove != NULL)
				{
					if(GetParam(0) == 0 && pObj->InSideScreen() && pMove->HasBeginMove())//������Ϊ��ʱ��ʾɱ��ȫ������
					{
						score += pEm->Execute(pTarget, list, bPretreating);
					}
					else if(GetParam(0) == 1 && pObj->InSideScreen() && pMove->HasBeginMove())//������Ϊ��ʱ��ʾɱ��ָ����Χ�ڵ��㣬��������ʾ�뾶
					{
						if(CMathAide::CalcDistance(pSelf->GetPosition().x_, pSelf->GetPosition().y_, pObj->GetPosition().x_, pObj->GetPosition().y_) <= GetParam(1))
							score += pEm->Execute(pTarget, list, bPretreating);
					}
					else if(GetParam(0) == 2 && pObj->InSideScreen() && pMove->HasBeginMove())//������Ϊ��ʱ��ʾɱ��ָ�����͵��㣬��������ʾָ������
					{
						if(pObj->GetTypeID() == GetParam(1) && ((CFish*)pObj)->GetFishType() == ESFT_NORMAL)
							score += pEm->Execute(pTarget, list, bPretreating);
					}
					else if(GetParam(0) == 3)//������Ϊ��ʱ��ʾɱ��ͬһ����ˢ�������㡣
					{
						if(((CFish*)pObj)->GetRefershID() == ((CFish*)pSelf)->GetRefershID())
							score += pEm->Execute(pTarget, list, bPretreating);
					}
				}
			}

			++ifs;
		}
		pMgr->Unlock();
	}

	if(score / pTarget->GetScore() > GetParam(2))
		score = pTarget->GetScore() * GetParam(2);

	return score;
}

CEffectAddBuffer::CEffectAddBuffer()
:CEffect()
{
	m_nParam.resize(5);
	ClearParam();
}

LONGLONG CEffectAddBuffer::Execute(MyObject* pSelf, MyObject* pTarget, std::list<MyObject*>& list, bool bPretreating)
{
	if(pSelf == NULL || bPretreating) return 0;

	RaiseEvent("AddBuffer", this, pSelf);

	MyObjMgr* pMgr = pSelf->GetMgr();
	if(pMgr != NULL)
	{
		obj_table_iter ifs = pMgr->Begin();
		while(ifs != pMgr->End())
		{
			MyObject* pObj = (MyObject*)ifs->second;

			if(pObj != pSelf/* && pObj->InSideScreen()*/)
			{
				BufferMgr* pBM = (BufferMgr*)pObj->GetComponent(ECF_BUFFERMGR);
				if(pBM != NULL)
				{
					if(GetParam(0) == 0)//������Ϊ��ʱ��ʾȫ������
					{
						pBM->Add(GetParam(2), GetParam(3), GetParam(4));
					}
					else if(GetParam(0) == 1)//������Ϊ��ʱ��ʾָ����Χ�ڵ��㣬��������ʾ�뾶
					{
						if(CMathAide::CalcDistance(pSelf->GetPosition().x_, pSelf->GetPosition().y_, pObj->GetPosition().x_, pObj->GetPosition().y_) <= GetParam(1))
							pBM->Add(GetParam(2), GetParam(3), GetParam(4));
					}
					else if(GetParam(0) == 2)//������Ϊ��ʱ��ʾָ�����͵��㣬��������ʾָ������
					{
						if(pObj->GetTypeID() == GetParam(1))
							pBM->Add(GetParam(2), GetParam(3), GetParam(4));
					}
				}
			}

			++ifs;
		}
	}

	return 0;
}

CEffectProduce::CEffectProduce()
:CEffect()
{
	m_nParam.resize(4);
	ClearParam();
}

LONGLONG CEffectProduce::Execute(MyObject* pSelf, MyObject* pTarget, std::list<MyObject*>& list, bool bPretreating)
{
	if(pSelf == NULL || bPretreating) return 0;

	RaiseEvent("ProduceFish", this, pSelf);
	
	return 0;
}

CEffectBlackWater::CEffectBlackWater()
:CEffect()
{
	m_nParam.resize(0);
	m_nParam.clear();
}

LONGLONG CEffectBlackWater::Execute(MyObject* pSelf, MyObject* pTarget, std::list<MyObject*>& list, bool bPretreating)
{
	if(pSelf == NULL || bPretreating) return 0;

	RaiseEvent("BlackWater", this, pSelf);

	return 0;
}

CEffectAward::CEffectAward()
{
	m_nParam.resize(4);
	ClearParam();
}

LONGLONG CEffectAward::Execute(MyObject* pSelf, MyObject* pTarget, std::list<MyObject*>& list, bool bPretreating)
{
	if(pSelf == NULL) return 0;

	LONGLONG lScore = 0;
	//�����Ԥ������ �����ӽ�Ǯ����
	if(GetParam(1) == 0 && bPretreating)
	{
		if(GetParam(2) == 0)
			lScore = GetParam(3);
		else if(pTarget != NULL)
			lScore = pTarget->GetScore() * GetParam(3);
	}
	//���������� �������¼�
	if(!bPretreating)
		RaiseEvent("AdwardEvent", this, pSelf, pTarget);

	return lScore;
}