//
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Point.h"
#include "MyObject.h"

class CPlayer : public MyObject
{
public:
	CPlayer();
	virtual ~CPlayer();

	void ClearSet(int chairid);
	//���ô���
	void SetCannonType(int n){m_nCannonType = n;}
	int GetCannonType(){return m_nCannonType;}
	//��������
	void AddWastage(LONGLONG s){m_Wastage += s;}
	LONGLONG GetWastage(){return m_Wastage;}

	void SetMultiply(int n){m_nMultiply = n;}
    //��ȡ�ӵ�����
	int GetMultiply(){return m_nMultiply;}
	//��������
	void SetCannonPos(MyPoint& pt){m_CannonPos = pt;}
	const MyPoint& GetCannonPos(){return m_CannonPos;}
	//���һ�ο�������
	void SetLastFireTick(DWORD dw){m_dwLastFireTick = dw;}
	DWORD GetLastFireTick(){return m_dwLastFireTick;}
	//������
	void SetLockFishID(DWORD id);
	DWORD GetLockFishID(){return m_dwLockFishID;}

	bool HasLocked(DWORD id);
	void ClearLockedBuffer(){LockBuffer.clear();}

	bool bLocking(){return m_bLocking;}
	void SetLocking(bool b){m_bLocking = b;}
	//�����ӵ�
	void ADDBulletCount(int n){BulletCount += n;}
	void ClearBulletCount(){BulletCount = 0;}
	int GetBulletCount(){return BulletCount;}

	void SetFired();

	int	GetCannonSetType(){return m_nCannonSetType;}
	void SetCannonSetType(int n){m_nCannonSetType = n;}
	
	void CacluteCannonPos(WORD wChairID);

	bool	CanFire(){return m_bCanFire;}
	void	SetCanFire(bool b = true){m_bCanFire = b;}

public:
	void set_guid_gateid(int guid, int gate_id);
	int get_guid() { return guid_; }
	int get_gate_id() { return gate_id_; }

	void set_chair_id(int chair_id) { chair_id_ = chair_id; }
	int get_chair_id() { return chair_id_; }

	void set_nickname(const std::string& nickname) { nickname_ = nickname; }
	const std::string& get_nickname() { return nickname_; }

protected:
	LONGLONG			m_Wastage;		//���
	int					m_nCannonType;  //��������
	int					m_nMultiply;    //�ӵ����ͣ�   �ӵ������Ƿ���� ����
	MyPoint				m_CannonPos;    //��������

	DWORD				m_dwLastFireTick;   //���һ�ο�������

	DWORD				m_dwLockFishID;     //������ID
	bool				m_bLocking;         //�Ƿ�����
	std::list<DWORD>	LockBuffer;         //buff�б�

	int					BulletCount;         //�ӵ�����

	bool				bFired;				//�Ƿ񿪻���

	bool				m_bCanFire;			//	�Ƿ���Կ���

	int					m_nCannonSetType;    //���ڼ�����

	// ������Ϣ���
	int					guid_;
	int					gate_id_;
	int					chair_id_;
	std::string			nickname_;
};

#endif

