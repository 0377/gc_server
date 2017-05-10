////
#ifndef _MY_COMPONENT_H_
#define _MY_COMPONENT_H_

#include <memory>
#include <Windows.h>

class MyObject;
class MyComponent;
//����¼�
class CComEvent
{
public:
	CComEvent(){};
	virtual ~CComEvent(){};
	void SetID(UINT32 id){id_ = id;}
	UINT32 GetID() const{return id_;};

	INT64	GetParam1()const{return param1_;}
	void	SetParam1(INT64 param){param1_ = param;}

	void*	GetParam2()const{return param2_;}
	void	SetParam2(void* param){param2_ = param;}

	//�¼��ķ�����
	MyComponent*	GetSender()const {return sender_;}
	void				SetSender(MyComponent* sender){sender_ = sender;}

protected:
	UINT32				id_;            //�¼�ID 
	MyComponent*	sender_;            //���ָ��
	INT64				param1_;        //���Ͳ���
	void*				param2_;        //�¼��Խ�
};

enum MyComponentType
{
	ECF_NONE = 0,
	ECF_MOVE,		//�ƶ����
	ECF_VISUAL,		//���ӻ����                     //ǰ��ʹ��
	ECF_EFFECTMGR,	//����Ч��������
	ECF_BUFFERMGR,	//�£գƣƣţҹ�����
};
//���
class MyComponent
{
public:
	MyComponent():owner_(0){};
	virtual ~MyComponent(){};

	const UINT32 GetID() const {return id_;};
	virtual const UINT32 GetFamilyID() const{return id_ >> 8 ;};

	//���ӵ�����󱻵���
	virtual void OnAttach(){};
	//�Ӷ����Ƴ�ǰ������
	virtual void OnDetach(){};

	//��Ӧʱ������
	virtual void OnUpdate(int ms){};

	//��Ӧ�����Ϣ
	virtual void OnCCEvent(CComEvent*){};

	void SetOwner(MyObject* owner){owner_ = owner;};
	MyObject* GetOwner() const {return owner_;};

protected:
	//�����¼�,���¼���������Ӧ
	void RaiseEvent(CComEvent*);
	void RaiseEvent(UINT32 idEvent, INT64 nParam1 = 0, void* pParam2 = 0);
	//Ͷ���¼�,���¼������Ӻ���Ӧ
	void PostEvent(std::auto_ptr<CComEvent> &evnt);
	void PostEvent(UINT32 idEvent, INT64 nParam1 = 0, void* pParam2 = 0);
private:
	void SetID(UINT32 id){id_ = id;}
	friend class MyComponentFactory;
private:
	UINT32 id_;                 //ID
	MyObject * owner_;          //�����������
};



#endif


