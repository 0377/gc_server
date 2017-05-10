#include "common.h"
#include "MyObjectManager.h"
#include "MyObjectFactory.h"
#include <algorithm>
#include <atlstr.h>

#define SAFE_DELETE(x) { if (NULL != (x)) { delete (x); (x) = NULL; } }

MyObjMgr::MyObjMgr()
{
}

MyObjMgr::~MyObjMgr()
{
	Clear();
}

void MyObjMgr::Lock(void)
{
	m_lock.lock();
}

void MyObjMgr::Unlock(void)
{
	m_lock.unlock();
}

MyObject* MyObjMgr::Find(DWORD nID)
{
	std::lock_guard<std::recursive_mutex> l(m_lock);
	obj_table_iter it = m_mapObject.find(nID);
	if(it != m_mapObject.end())
		return it->second;

	return NULL;
}

void MyObjMgr::Add(MyObject* pObj)
{
	if (pObj == NULL)
	{
//		ASSERT(FALSE);
		return;
	}
	std::lock_guard<std::recursive_mutex> l(m_lock);
	pObj->SetMgr(this);
// #if defined(_DEBUG) && defined(TRACE_SERVICE_HEAD_FILE)
// 	if (m_mapObject.find(pObj->GetId()) == m_mapObject.end())
// 	{
// 		CString str;
// 		str.Format(_T("pObject[%d] �Ѵ���"), pObj->GetId());
// 		CTraceService::TraceString(str, TraceLevel_Debug);
// 	}
// #endif // _DEBUG
	//ASSERT(m_mapObject.find(pObj->GetId()) == m_mapObject.end());
	m_mapObject[pObj->GetId()] = pObj;
}

void MyObjMgr::Remove(MyObject* pObj)
{
	Remove(pObj->GetId());
}

void MyObjMgr::Remove(DWORD nID)
{
	std::lock_guard<std::recursive_mutex> l(m_lock);
	obj_table_iter it = m_mapObject.find(nID);
	if(it != m_mapObject.end())
	{
		MyObject* pObj = it->second;
		m_mapObject.erase(it);
		pObj->ClearComponent();
		MyObjFactory::GetInstance()->Recovery(pObj->GetObjType(), pObj);
	}
}

obj_table_iter MyObjMgr::Begin()
{
	return m_mapObject.begin();
}

obj_table_iter MyObjMgr::End()
{
	return m_mapObject.end();
}

void MyObjMgr::OnUpdate(UINT32 ms)
{
	for(obj_table_iter it = m_mapObject.begin(); it != m_mapObject.end(); ++it)
	{
		(it->second)->OnUpdate(ms);
	}
}

void MyObjMgr::Clear()
{
	std::lock_guard<std::recursive_mutex> l(m_lock);
	for(obj_table_iter it = m_mapObject.begin(); it != m_mapObject.end(); ++it)
	{
		it->second->ClearComponent();
		MyObjFactory::GetInstance()->Recovery((it->second)->GetObjType(), it->second);
	}
	m_mapObject.clear();
}

int MyObjMgr::CountObject()
{
	std::lock_guard<std::recursive_mutex> l(m_lock);
	return m_mapObject.size();
}
