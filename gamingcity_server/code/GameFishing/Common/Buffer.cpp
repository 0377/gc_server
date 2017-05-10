#include "stdafx.h"
#include "Buffer.h"
#include "MyComponent.h"
#include "Player.h"
#include "EventMgr.h"
#include "Bullet.h"
#include "GameConfig.h"
//#include "../��Ϣ����/CMD_Fish.h"

CBuffer::CBuffer()
:m_fLife(0.0f)
,m_BTP(EBT_NONE)
,m_param(0.0f)
{

}

CBuffer::~CBuffer()
{
	Clear();
}

bool CBuffer::OnUpdate(int ms)
{
	if(m_fLife > 0.0f)
		m_fLife -= ms / 1000.0f;
	
	return m_fLife == -1.0f || m_fLife > 0.0f;
}

CSpeedBuffer::CSpeedBuffer()
:CBuffer()
{
	SetType(EBT_CHANGESPEED);
}

CSpeedBuffer::~CSpeedBuffer()
{
	Clear();
}

void CSpeedBuffer::Clear()
{
	m_param = 1.0f;
}

void CSpeedBuffer::OnCCEvent(CComEvent* pEvent)
{
	if(pEvent->GetID() == EME_QUERY_SPEED_MUL)
	{
		float* pSpeed = (float*)pEvent->GetParam2();
		*pSpeed = (*pSpeed) * m_param;
	}
}

CDoubleCannon::CDoubleCannon()
:CBuffer()
{
	SetType(EBT_DOUBLE_CANNON);
}

CDoubleCannon::~CDoubleCannon()
{
	Clear();
}

void CDoubleCannon::Clear()
{
	CPlayer* pOwner = (CPlayer*)m_pOwner;
	if(pOwner != NULL)
	{
		int n = pOwner->GetCannonSetType();
		if(n < CGameConfig::GetInstance()->CannonSetArray.size())
		{
			n = CGameConfig::GetInstance()->CannonSetArray[n].nNormalID;
		}
		pOwner->SetCannonSetType(n);
	}
	RaiseEvent("CannonSetChanaged", pOwner);
}

void CDoubleCannon::OnCCEvent(CComEvent*)
{

}

void CDoubleCannon::SetOwner(MyObject* pobj)
{
	CBuffer::SetOwner(pobj);
	if(pobj != NULL)
	{
		CPlayer* pOwner = (CPlayer*)pobj;
		if(pOwner != NULL)
		{
			int n = pOwner->GetCannonSetType();
			if(n < CGameConfig::GetInstance()->CannonSetArray.size())
			{
				n = CGameConfig::GetInstance()->CannonSetArray[n].nDoubleID;
			}
			pOwner->SetCannonSetType(n);
			RaiseEvent("CannonSetChanaged", pOwner);
		}
	}
}

CIonCannon::CIonCannon()
:CBuffer()
{
	SetType(EBT_ION_CANNON);
}

CIonCannon::~CIonCannon()
{
	Clear();
}

void CIonCannon::Clear()
{
	CPlayer* pOwner = (CPlayer*)m_pOwner;
	if(pOwner != NULL)
	{
		int n = pOwner->GetCannonSetType();
		if(n < CGameConfig::GetInstance()->CannonSetArray.size())
		{
			n = CGameConfig::GetInstance()->CannonSetArray[n].nNormalID;
		}
		pOwner->SetCannonSetType(n);
	}
	RaiseEvent("CannonSetChanaged", pOwner);
}

void CIonCannon::OnCCEvent(CComEvent*)
{

}

void CIonCannon::SetOwner(MyObject* pobj)
{
	CBuffer::SetOwner(pobj);
	if(pobj != NULL)
	{
		CPlayer* pOwner = (CPlayer*)pobj;
		if(pOwner != NULL)
		{
			int n = pOwner->GetCannonSetType();
			if(n < CGameConfig::GetInstance()->CannonSetArray.size())
			{
				n = CGameConfig::GetInstance()->CannonSetArray[n].nIonID;
			}
			pOwner->SetCannonSetType(n);
			RaiseEvent("CannonSetChanaged", pOwner);
		}
	}
}

CAddMulByHit::CAddMulByHit()
:CBuffer()
{
	SetType(EBT_ADDMUL_BYHIT);
	m_param = 0;
}

CAddMulByHit::~CAddMulByHit()
{
	Clear();
}

void CAddMulByHit::Clear()
{
	nCurMul = CGameConfig::GetInstance()->nAddMulCur;
}

void CAddMulByHit::OnCCEvent(CComEvent* pEvent)
{
	if(pEvent != NULL)
	{
		if(pEvent->GetID() == EME_STATE_CHANGED && pEvent->GetParam1() == EOS_HIT)
		{
			CBullet* pBullet = (CBullet*)pEvent->GetParam2();
			if(pBullet != NULL && pBullet->GetScore() == CGameConfig::GetInstance()->m_MaxCannon)
			{
				++nCurMul;

				++CGameConfig::GetInstance()->nAddMulCur;

				RaiseEvent("FishMulChange", m_pOwner);
			}
		}
		else if(pEvent->GetID() == EME_QUERY_ADDMUL)
		{
			nCurMul = max(nCurMul, CGameConfig::GetInstance()->nAddMulCur);
			int* pMul = (int*)pEvent->GetParam2();
			*pMul = nCurMul;
		}
	}
}

void CAddMulByHit::SetOwner(MyObject* pobj)
{
	CBuffer::SetOwner(pobj);
}


