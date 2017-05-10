//
#ifndef _MY_OBJECT_H_
#define _MY_OBJECT_H_

#include <set>
#include <list>
#include <map>
#include <memory>
#include <string.h>
#include <Windows.h>
#include "Size.h"
#include "Point.h"

class MyComponent;
class CComEvent;
class MyObjMgr;

enum ObjectType
{
	EOT_NONE = 0,
	EOT_PLAYER,
	EOT_BULLET,
	EOT_FISH,
};

enum ObjState
{
	EOS_LIVE = 0,
	EOS_HIT,
	EOS_DEAD,
	EOS_DESTORY,
	EOS_LIGHTING,
};

enum MyEvent
{
	EME_STATE_CHANGED = 0,		//״̬�仯
	EME_QUERY_SPEED_MUL,		//��ѯ�ٶȱ���
	EME_QUERY_ADDMUL,			//��ѯ�������ӵı���
};

class MyObject
{
public:
	MyObject();
	virtual ~MyObject();

public:
	//���úͻ�ȡId
	DWORD GetId()const{return id_;};
	void SetId(DWORD newId){id_ = newId;};

	int GetObjType()const{return objType_;}
	void SetObjType(int objType){objType_ = objType;}

	//��Ӧʱ������
	virtual void OnUpdate(int msElapsed);

	void SetMgr(MyObjMgr* mgr){m_Mgr = mgr;}
	MyObjMgr* GetMgr(){return m_Mgr;}

	MyPoint GetPosition();

	float GetDirection();
	
	/// \brief	��ǰ����ķ���
	///
	/// \author	lik
	/// \date	2016-05-12 22:18
	///
	/// \return	The score.
	LONGLONG GetScore(){return m_Score;}
	void SetScore(LONGLONG sc){m_Score = sc;}
	void AddScore(LONGLONG sc){m_Score += sc;}

	float	GetProbability(){return m_fProbability;}
	void SetProbability(float f){m_fProbability = f;}

	DWORD GetCreateTick(){return m_dwCreateTick;}
	void SetCreateTick(DWORD tk){m_dwCreateTick = tk;}

	bool InSideScreen();

protected:
	MyObjMgr* m_Mgr;
	DWORD id_;
	int objType_;

	friend class ClientObjectFactory;

protected:
	typedef std::map< const UINT32, MyComponent* >	Component_Table_t;
	typedef std::list< CComEvent* > CCEvent_Queue_t;

	Component_Table_t components_;
	CCEvent_Queue_t ccevent_queue_;
	
	LONGLONG m_Score;

	float		m_fProbability;

	DWORD	m_dwCreateTick;

	int	m_nState;

public:
	void ProcessCCEvent(CComEvent*);//��ʱ������¼�
	void ProcessCCEvent(UINT32 idEvent, INT64 nParam1 = 0, void* pParam2 = 0);

	void PushCCEvent(std::auto_ptr<CComEvent>& evnt);//�ӳٴ�����¼�
	void PushCCEvent(UINT32 idEvent, INT64 nParam1 = 0, void* pParam2 = 0);

	MyComponent* GetComponent(const UINT32& familyID);
	void SetComponent( MyComponent* newComponent);

	bool DelComponent(const UINT32& familyID);//ɾ��ָ�����������ҵ����ɹ�ɾ���򷵻أ�����壬�Ҳ����򷵻أ�����
	void ClearComponent();

	void SetState(int st, MyObject* pobj = NULL);
	int GetState();

	void SetTypeID(int n){m_nTypeID = n;}
	int GetTypeID(){return m_nTypeID;}

protected:
	int			m_nTypeID;

};



#endif


