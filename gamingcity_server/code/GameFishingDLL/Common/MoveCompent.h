////
#ifndef __MOVE_COMPENT_H__
#define __MOVE_COMPENT_H__

#include "MyComponent.h"
#include "MyObjectManager.h"
#include "MovePoint.h"

enum MoveCompentType
{
	EMCT_PATH = (ECF_MOVE<<8),	//��·���ƶ�
	EMCT_DIRECTION,				//��ָ�������ƶ�
	EMCT_TARGET,				//��Ŀ�귽���ƶ�
};

class MoveCompent : public MyComponent
{
public:
	MoveCompent();
	virtual ~MoveCompent(){}

	virtual const UINT32 GetFamilyID() const{return ECF_MOVE;}

	MyPoint GetPostion(){return m_data.m_Position;}
	void SetPosition(const MyPoint &pt){m_data.m_Position = pt;}
	void SetPosition(int x, int y){m_data.m_Position.x_ = x; m_data.m_Position.y_ = y;}

	float GetDirection(){return m_data.m_Direction;}
	void SetDirection(float dr){m_data.m_Direction = dr;}

	void SetSpeed(float sp){m_fSpeed = sp;}
	float GetSpeed(){return m_fSpeed;}

	void SetPause(bool bPause = true){m_bPause = bPause;}
	bool IsPaused(){return m_bPause;}

	virtual void OnUpdate(int ms) = NULL;

	void SetPathID(int pid, bool bt = false){m_nPathID = pid; m_bTroop = bt;}
	int GetPathID(){return m_nPathID;}

	bool bTroop(){return m_bTroop;}
	virtual void InitMove() = NULL;

	bool IsEndPath(){ return m_bEndPath; }
	bool SetEndPath(bool be){m_bEndPath = be;}

	const MyPoint& GetOffest(){return m_Offest;}
	void SetOffest(MyPoint& pt){m_Offest = pt;}

	virtual void OnCCEvent(CComEvent*);

	void SetDelay(float f){m_fDelay = f;}
	float GetDelay(){return m_fDelay;}

	bool HasBeginMove(){return m_bBeginMove;}

	bool Rebound(){return m_bRebound;}
	void SetRebound(bool b){m_bRebound = b;}

	DWORD GetTargetID(){return m_dwTargetID;}
	void SetTarget(MyObjMgr* pm, DWORD id){m_pObjMgr = pm; m_dwTargetID = id;}

	virtual void OnDetach();

protected:
	CMovePoint	m_data;				//����
	float		m_fSpeed;			//�ٶ�
	bool		m_bPause;			//�Ƿ���ͣ
	int			m_nPathID;			//·��ID
	bool		m_bEndPath;			//�Ƿ��ƶ����յ�
	MyPoint		m_Offest;			//ƫ��
	float		m_fDelay;			//��ʱ
	bool		m_bBeginMove;		//��ʼ�ƶ�
	bool		m_bRebound;			//�Ƿ񷴵�
	DWORD		m_dwTargetID;		//Ŀ��ID
	MyObjMgr*	m_pObjMgr;			//�������
	bool		m_bTroop;			//�Ƿ����
};

class MoveByPath : public MoveCompent
{
public:
	virtual void OnUpdate(int ms);

	virtual void InitMove();

protected:
	float				m_fDuration;	//�ܽ����
	float				m_Elaspe;		//����
};

class MoveByDirection : public MoveCompent
{
public:
	virtual void OnUpdate(int ms);

	virtual void InitMove();

protected:
	float	angle_;		//�Ƕ�
	float	dx_;		
	float	dy_;
};

#endif

