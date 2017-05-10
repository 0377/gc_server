//
#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

//#include "../��Ϣ����/CMD_Fish.h"
#include "common.h"
#include "Player.h"
#include "Bullet.h"
#include "Fish.h"
#include "MoveCompent.h"
#include "EffectManager.h"
#include "Effect.h"
#include "MyObjectManager.h"
#include "Define.h"
//#include "GameSessionManager.h"
//#include "common_msg_fishing.pb.h"
#include "config_define.pb.h"
#include "BaseStruct.h"
#include "CommonFun.h"
#include <list>
using namespace std;
#include <boost/asio.hpp>   
#include <boost/thread.hpp>   
#include <iostream>   

class CMyEvent;
//ˢ����Ⱥ
struct RefershTroop
{
	bool	bSendDes;               //��������
	bool	bSendTroop;             //������Ⱥ
	float	fBeginTime;             //��ʼʱ��
};

//��Ϸ������
class CTableFrameSink
{
	//�������
public:

    std::vector<CPlayer>			m_player;
    std::list <stLuaMsg *>          m_lsLuaMsg;     //��Ϣ����
	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release();
	//�ӿڲ�ѯ
	//virtual void *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	void ResetTable();

	//��λ�ӿ�
	virtual VOID RepositionSink();
	//���ýӿ�
	virtual bool Initialization();

	//��ѯ�ӿ�
public:
	//��Ϸ״̬
	//virtual bool IsUserPlaying(WORD wChairID) override;
	virtual bool IsUserPlaying(WORD wChairID);
	//��ѯ�޶�
	//virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem) override { return 0L; }
	//���ٻ���
	//virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem) override { return 0L; }
	//��ѯ�Ƿ�۷����
	//virtual bool QueryBuckleServiceCharge(WORD wChairID) override { return true; }

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, BYTE cbGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter);
	//�����¼�
	//virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize) { return true; }
	virtual bool OnDataBaseMessage(WORD, DWORD, void *, WORD)  { return true; }
	//�����¼�
	//virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) override { return true; }

	//����ӿ�
public:
	//��Ϸ��Ϣ
	//virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem) override;
	//�����Ϣ
	//virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem) override;

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore) {}

	//ʱ���¼�
	//virtual bool OnTimerTick(DWORD dwTimerID, WPARAM dwBindParameter) override { return true; }
	//�����¼�
public:
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, int GuID, bool bLookonUser);
	//�û�����
    virtual bool OnActionUserStandUp(WORD wChairID, int GuID, bool bLookonUser);
	//�û�ͬ��
	//virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)  override { return true; }
	//�û�����
	//virtual bool  OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)  override;

	/// \brief	д������ݵ����ݿ�
	///
	/// \author	lik
	/// \date	2016-05-11 20:15
	//virtual void SaveTableData();
	
	void	OnGameUpdate();
    void    OnDealLuaMsg();
//protected:
    bool	OnTimeSync(CS_stTimeSync* msg);
	//bool	OnChangeScore(void* pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
    bool	OnChangeCannon(CS_stChangeCannon* msg);
    bool	OnFire(CS_stFire* msg);

	void	CatchFish(CBullet* pBullet, CFish* pFish, int nCatch, int* nCatched);

	void	SendCatchFish(CBullet* pBullet, CFish*pFish, LONGLONG score);

	void	DistrubFish(float fdt);

	void	ResetSceneDistrub();

	void	SendFish(CFish* pFish, WORD wChairID = INVALID_CHAIR);
	void	SendBullet(CBullet* pBullet, WORD wChairID = INVALID_CHAIR, bool bNew = false);

	void	SendSceneInfo(WORD wChairID);
	void	SendPlayerInfo(WORD wChairID);
	void	SendCannonSet(WORD wChairID);
	void	SendGameConfig(WORD wChairID);

	void	ReturnBulletScore(WORD wChairID);

	void	SendAllowFire(WORD wChairID);

	void	OnProduceFish(CMyEvent* pEvent);

	void	OnAddBuffer(CMyEvent* pEvent);

	void	OnAdwardEvent(CMyEvent* pEvent);

	void	OnCannonSetChange(CMyEvent* pEvent);

	void	OnCatchFishBroadCast(CMyEvent* pEvent);

	void	OnFirstFire(CMyEvent* pEvent);

	void	OnMulChange(CMyEvent* pEvent);

    void	LockFish(WORD wChairID);

    bool    OnLockFish(CS_stLockFish* msg);

    bool	OnNetCast(CS_stNetcast* msg);

    bool	OnChangeCannonSet(CS_stChangeCannonSet* msg);

	bool	HasRealPlayer();

	void	AddBuffer(int btp, float parm, float ft);

    stLuaMsg * GetMsg();
    void    PushMsg(stLuaMsg *);

	int		CountPlayer();

	//void	RecordGameScore(IServerUserItem * pIServerUserItem, tagScoreInfo & ScoreInfo);

	//bool	ImitationRealPlayer(IServerUserItem* pUser);
	public:
	static void	LoadConfig();

    bool	OnTreasureEND(CS_stTreasureEnd* msg);

    int    GetPlayerGuID(WORD wChairID);
    bool    GetOnePlayerGuID(int &GuID);

	// ������Ϣ
	//template<typename T> void send2client_pb(WORD wChairID, T* pb)
	//{
	//	if (INVALID_CHAIR == wChairID)
	//	{
	//		broadcast2client_pb(pb);
	//		return;
	//	}

	//	if (wChairID >= m_player.size())
	//	{
	//		LOG_WARN("wChairID %d out of range[0,%d)", wChairID, m_player.size());
	//		return;
	//	}

	//	int guid = m_player[wChairID].get_guid();
	//	if (guid == 0)
	//	{
	//		LOG_WARN("wChairID %d guid=0", wChairID);
	//		return;
	//	}

	//	GameSessionManager::instance()->send2client_pb(guid, m_player[wChairID].get_gate_id(), pb);
	//}

	//template<typename T> void broadcast2client_pb(T* pb)
	//{
	//	for (auto& player : m_player)
	//	{
	//		int guid = player.get_guid();
	//		if (guid != 0)
	//			GameSessionManager::instance()->send2client_pb(guid, player.get_gate_id(), pb);
	//	}
	//}

	void set_guid_gateid(int chair_id, int guid, int gate_id);

    void set_nickname(int chair_id, const char* nickname);
    void set_money(int chair_id, LONGLONG lvalue);
	void set_table_id(int table_id) { m_table_id = table_id; }
	int get_table_id() { return m_table_id; }
    CLock get_list_lock() { return m_LockLuaMsg; }

protected:
	DWORD							m_dwLastTick;						//��һ��ɨ��ʱ��
	float							m_fSceneTime;						//����ʱ��
	int								m_nCurScene;						//��ǰ����
	MyObjMgr						m_FishManager;						//�������
	MyObjMgr						m_BulletManager;					//�ӵ�������		

	bool							m_bAllowFire;						//���Կ���
	float							m_fPauseTime;						//��ͣʱ��
	std::vector<SYSTEMTIME>			m_SystemTimeStart;					//��ʼʱ��

	int								m_nSpecialCount;       //����������

	std::list<DWORD>				m_CanLockList;         //�������б�
	std::vector<float>				m_vDistrubFishTime;    //����ʱ��
	std::vector<RefershTroop>		m_vDistrubTroop;       //������Ⱥ

	std::vector<MyPoint>			m_NearFishPos;        //������

	int								m_nFishCount;         //������

    boost::asio::deadline_timer     m_Timer;

	SCORE							user_win_scores_[MAX_TABLE_CHAIR];// �û�������Ӯ
	SCORE							user_revenues_[MAX_TABLE_CHAIR];// �û����ܳ�˰
	SCORE							user_score_pools_[MAX_TABLE_CHAIR];// �����޹ʳԵ����û�����

    bool							m_bRun;			//�Ƿ�����

	int								m_table_id;     //����ID

    CLock                           m_LockLuaMsg;   //��Ϣ������
};

//////////////////////////////////////////////////////////////////////////

#endif