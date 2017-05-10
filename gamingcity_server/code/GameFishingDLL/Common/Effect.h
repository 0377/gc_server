//
#ifndef __EFFECT_H__
#define __EFFECT_H__

#include <WTypes.h>
#include <list>
#include <vector>

class MyObject;

enum EffectType
{
	ETP_ADDMONEY = 0,			//���ӽ��
	ETP_KILL,					//ɱ��������
	ETP_ADDBUFFER,				//����BUFFER
	ETP_PRODUCE,				//����������
	ETP_BLACKWATER,				//������ī֭Ч��
	ETP_AWARD,					//�齱
};

class CEffect
{
public:
	CEffect();
	virtual ~CEffect();

	EffectType GetEffectType(){return m_nType;}
	void SetEffectType(EffectType etp){m_nType = etp;}

	int GetParam(int pos);
	void SetParam(int pos, int p);

	void ClearParam();

	int GetParamSize(){return m_nParam.size();}

	virtual LONGLONG Execute(MyObject* pSelf, MyObject* pTarget, std::list<MyObject*>& list, bool bPretreating) = 0;

protected:
	EffectType			m_nType;
	std::vector<int>	m_nParam;
};

//���ӽ��
//������Ϊ��ʱ��ʾ���ӹ̶��Ľ��������������ʾǮ��
//������Ϊ��ʱ��ʾ����һ��������Ǯ������������ʾ����
class CEffectAddMoney : public CEffect
{
public:
	CEffectAddMoney();
	virtual LONGLONG Execute(MyObject* pSelf, MyObject* pTarget, std::list<MyObject*>& list, bool bPretreating);
	LONGLONG lSco;
};


//ɱ��ɱ��������
//������Ϊ��ʱ��ʾɱ��ȫ������
//������Ϊ��ʱ��ʾɱ��ָ����Χ�ڵ��㣬��������ʾ�뾶
//������Ϊ��ʱ��ʾɱ��ָ�����͵��㣬��������ʾָ������
//������Ϊ��ʱ��ʾɱ��ͬһ����ˢ�������㡣
class CEffectKill : public CEffect
{
public:
	CEffectKill();
	virtual LONGLONG Execute(MyObject* pSelf, MyObject* pTarget, std::list<MyObject*>& list, bool bPretreating);
};

//���ӣ£գƣ�
//��������ʾҪ���ӵģ£գƣƵķ�Χ�� ����ʾȫ������,����ʾ��Χ�ڵ���,����ʾָ�����͵���
//������Ϊ��ʱ��ʾɱ��ָ����Χ�ڵ��㣬��������ʾ�뾶;������Ϊ��ʱ��ʾָ�����͵��㣬��������ʾָ������
//��������ʾҪ���ӵģ£գƣƣţ�����
//��������ʾҪ���ӵģ£գƣƣţҵĲ���
//��������ʾҪ���ӵģ£գƣƣţҵ�ʱ��
class CEffectAddBuffer : public CEffect
{
public:
	CEffectAddBuffer();
	virtual LONGLONG Execute(MyObject* pSelf, MyObject* pTarget, std::list<MyObject*>& list, bool bPretreating);
};

//������
//��������ʾҪ���ɵ���ģɣ�
//��������ʾҪ���ɵ��������
//��������ʾÿ������Ҫ���ɵ��������
//��������ʾÿ������֮���ʱ����
class CEffectProduce : public CEffect
{
public:
	CEffectProduce();
	virtual LONGLONG Execute(MyObject* pSelf, MyObject* pTarget, std::list<MyObject*>& list, bool bPretreating);
};

//����ī֭Ч��
class CEffectBlackWater : public CEffect
{
public:
	CEffectBlackWater();
	virtual	LONGLONG Execute(MyObject* pSelf, MyObject* pTarget, std::list<MyObject*>& list, bool bPretreating);
};

//�齱Ч��չʾ
//��������ʾ����, 0-7
//��������ʾʵ��Ч�� ���ӽ�ҡ������ӣ£գƣƣţ�
//������ �ڼӽ��ʱ����Ϊ����ʾ�ӹ̶���Ǯ����������ʾǮ������
//					 ��Ϊ����ʾ�ӱ���Ǯ����������ʾǮ�ı���
//�ڼӣ£գƣƣţ�ʱ������ʾ�£գƣƣţ����͡�����ʾ�£գƣƣţ�ʱ��
class CEffectAward : public CEffect
{
public:
	CEffectAward();
	virtual LONGLONG Execute(MyObject* pSelf, MyObject* pTarget, std::list<MyObject*>& list, bool bPretreating);
};
#endif

