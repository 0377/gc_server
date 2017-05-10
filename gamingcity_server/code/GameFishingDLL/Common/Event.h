//
#ifndef __MY_EVENT_H__
#define __MY_EVENT_H__

#include <string>

typedef std::string EventID;

class CMyEvent
{
public:
	CMyEvent(const EventID& name = "", void* param = 0, void* source = 0, void* target = 0)
		:strName(name)
		,pParam(param)
		,pSource(source)
		,pTarget(target)
	{}

	virtual ~CMyEvent(){}

	void SetName(const EventID& name){strName = name;}
	const EventID& GetName(){return strName;}
	void SetParam(void* param){pParam = param;}
	void* GetParam(){return pParam;}
	void SetSource(void* source){pSource = source;}
	void* GetSource(){return pSource;}
	void SetTarget(void* target){pTarget = target;}
	void* GetTarget(){return pTarget;}
protected:
	EventID strName;                //�¼���
	void* pParam;                   //����
	void* pSource;                  //Դ
	void* pTarget;                  //Ŀ��
};


#endif//__CLIENT_EVENT_H__
