//
#include "common.h"
#include "TableFrameSink.h"
#include "GameConfig.h"
#include "MathAide.h"
#include "GameConfig.h"
#include "CommonLogic.h"
#include "PathManager.h"
#include "EventMgr.h"
#include <math.h>
#include <MMSystem.h>
//#include "ServerControl.h"
#include "IDGenerator.h"
#include "BufferManager.h"
#include "MyComponentFactory.h"

#include "GameLog.h"
#include "TableManager.h"
#include <codecvt>

#define IDI_GAMELOOP	1

#define TIME_GAMELOOP	1000/GAME_FPS

#define MAX_LIFE_TIME	30000
extern "C" {
#include"lua.hpp"
#include "lauxlib.h"
#include "lualib.h"
}
extern lua_State* g_LuaL;
extern CLock g_LuaLock;
extern boost::shared_ptr< boost::asio::io_service >  g_IoService;
//extern GameServerConfig				g_pbServerConfig;
//���캯��
CTableFrameSink::CTableFrameSink()
    : m_Timer(*g_IoService)
{
	m_nFishCount = 0;
	m_bRun = false;

	m_table_id = 0;
    Initialization();
	TableManager::instance()->add_table(this);
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
    //m_Timer.cancel();
	TableManager::instance()->remove_table(this);
}

VOID  CTableFrameSink::Release()
{
	/*for (uint16_t chair_id = 0; chair_id < MAX_TABLE_CHAIR; ++chair_id) {
		auto pUser = m_pITableFrame->GetTableUserItem(chair_id);
		if (pUser) {
		m_pITableFrame->DecUserRebateAndSaveToDB(pUser, 0);
		}
		}
		if(m_pITableFrame->IsGameStarted())
		m_pITableFrame->DismissGame();

		m_player.clear();
		m_SystemTimeStart.clear();
		m_NearFishPos.clear();*/
}

//��ʼ��
bool  CTableFrameSink::Initialization()
{
	Bind_Event_Handler("ProduceFish", CTableFrameSink, OnProduceFish);
	Bind_Event_Handler("CannonSetChanaged", CTableFrameSink, OnCannonSetChange);
	Bind_Event_Handler("AddBuffer", CTableFrameSink, OnAddBuffer);
	Bind_Event_Handler("CatchFishBroadCast", CTableFrameSink, OnCatchFishBroadCast);
	Bind_Event_Handler("FirstFire", CTableFrameSink, OnFirstFire);
	Bind_Event_Handler("AdwardEvent", CTableFrameSink, OnAdwardEvent);
	Bind_Event_Handler("FishMulChange", CTableFrameSink, OnMulChange);

	m_player.resize(GAME_PLAYER);
	m_SystemTimeStart.resize(GAME_PLAYER);
	m_NearFishPos.resize(GAME_PLAYER);

	memset(user_win_scores_, 0, sizeof(user_win_scores_));
	memset(user_revenues_, 0, sizeof(user_revenues_));
	memset(user_score_pools_, 0, sizeof(user_score_pools_));

	return true;
}

void CTableFrameSink::LoadConfig()
{
	std::string path = "../data/fishing/";// g_pbServerConfig.data_path();
    printf("��ʼ��������...\n");
	//LOG_DEBUG("��ʼ��������...");
	DWORD dwStartTick = ::GetTickCount();

	CGameConfig::GetInstance()->LoadSystemConfig(path + "System.xml");

	CGameConfig::GetInstance()->LoadBoundBox(path + "BoundingBox.xml");

	CGameConfig::GetInstance()->LoadFish(path + "Fish.xml");


	PathManager::GetInstance()->LoadNormalPath(path + "path.xml");

	PathManager::GetInstance()->LoadTroop(path + "TroopSet.xml");

	CGameConfig::GetInstance()->LoadCannonSet(path + "CannonSet.xml");
	CGameConfig::GetInstance()->LoadBulletSet(path + "BulletSet.xml");

	CGameConfig::GetInstance()->LoadScenes(path + "Scene.xml");

	CGameConfig::GetInstance()->LoadSpecialFish(path + "Special.xml");

    dwStartTick = ::GetTickCount() - dwStartTick;
    printf("������� �ܼƺ�ʱ%g��\n", dwStartTick / 1000.f);
	//LOG_DEBUG("������� �ܼƺ�ʱ%g��", dwStartTick / 1000.f);
}
//��������
void CTableFrameSink::ResetTable()
{

	m_FishManager.Clear();

	m_BulletManager.Clear();

	m_fPauseTime = 0.0f;

	m_nSpecialCount = 0;

	m_nFishCount = 0;

	for (WORD i = 0; i < GAME_PLAYER; ++i)
	{
		m_player[i].ClearSet(i);
	}
}
//��λ����
void  CTableFrameSink::RepositionSink()
{
	ResetTable();
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, int GuID, bool bLookonUser)
{
	if (!bLookonUser)
	{
		if (wChairID >= GAME_PLAYER)
		{
			return false;
		}
		m_player[wChairID].ClearSet(wChairID);

		user_revenues_[wChairID] = 0;
		user_win_scores_[wChairID] = 0;
        
		//��ȡBUFF������
		BufferMgr* pBMgr = (BufferMgr*)m_player[wChairID].GetComponent(ECF_BUFFERMGR);
		if (pBMgr == NULL)
		{
			pBMgr = (BufferMgr*)CreateComponent(EBCT_BUFFERMGR);
			if (pBMgr != NULL)
				m_player[wChairID].SetComponent(pBMgr);
		}

		if (pBMgr != NULL)
		{
			pBMgr->Clear();
		}
		else
		{
			return false;
        }
        TableManager::instance()->add_player_table(wChairID, GuID, 0, this);

	}
	return true;

}

//�û�����
bool  CTableFrameSink::OnActionUserStandUp(WORD wChairID, int GuID, bool bLookonUser)
{
	if (!bLookonUser)
	{
		if (wChairID >= GAME_PLAYER)
		{
			return false;
		}

		user_revenues_[wChairID] = 0;
		user_win_scores_[wChairID] = 0;
		// �����ǳԵ��ķ����˻����û����´ο��Գ��԰ѳԵ��ķ����˻����û�������
		if (user_score_pools_[wChairID] > 0) {
			user_win_scores_[wChairID] -= user_score_pools_[wChairID];
			user_score_pools_[wChairID] = 0;
		}
		// �����û���Ϣ�����ݿ�
		ReturnBulletScore(wChairID);

		WORD playerCount = 0;
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			if (m_player[i].get_guid() != 0)
				++playerCount;
		}

		if (playerCount == 0)
		{
			ResetTable();
		}
        m_player[wChairID].ClearSet(wChairID);
        TableManager::instance()->remove_player_table(GuID);
	}
	return true;
}


//��Ϸ״̬
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}

//��Ϸ��ʼ
bool  CTableFrameSink::OnEventGameStart()
{
	if (m_bRun)
		return false;

	ResetTable();

	m_dwLastTick = timeGetTime();

	m_nCurScene = CGameConfig::GetInstance()->SceneSets.begin()->first;
	m_fSceneTime = 0.0f;
	m_fPauseTime = 0.0f;
	m_bAllowFire = false;

	ResetSceneDistrub();

	//��ʼ���������
	RandSeed(timeGetTime());
	srand(timeGetTime());

	m_bRun = true;

    m_Timer.expires_from_now(boost::posix_time::millisec(1000 / 30));
    m_Timer.async_wait(boost::bind(&CTableFrameSink::OnGameUpdate, this));
	return true;
}
//���ó���
void CTableFrameSink::ResetSceneDistrub()
{
	//���ø�����Ⱥˢ��ʱ��
	int sn = CGameConfig::GetInstance()->SceneSets[m_nCurScene].DistrubList.size();
	m_vDistrubFishTime.resize(sn);
	for (int i = 0; i < sn; ++i)
	{
		m_vDistrubFishTime[i] = 0;
	}

	//������Ⱥ
	//��ȡ����ˢ����ʱ������
	sn = CGameConfig::GetInstance()->SceneSets[m_nCurScene].TroopList.size();
	m_vDistrubTroop.resize(sn);//����ˢ������Ϣ��С
	//��ʼ��ˢ����Ϣ
	for (int i = 0; i < sn; ++i)
	{
		m_vDistrubTroop[i].bSendDes = false;
		m_vDistrubTroop[i].bSendTroop = false;
		m_vDistrubTroop[i].fBeginTime = 0.0f;
	}
}

//����ԭ��
#define GER_NORMAL					0x00								//�������
#define GER_DISMISS					0x01								//��Ϸ��ɢ
#define GER_USER_LEAVE				0x02								//�û��뿪
#define GER_NETWORK_ERROR			0x03								//�������

//��Ϸ����
bool  CTableFrameSink::OnEventGameConclude(WORD wChairID, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:
	case GER_USER_LEAVE:
	case GER_NETWORK_ERROR:
	{
		//������ң������˳�
		//ASSERT(wChairID < m_pITableFrame->GetChairCount());
		ReturnBulletScore(wChairID);
		m_player[wChairID].ClearSet(wChairID);

		m_player[wChairID].set_guid_gateid(0, 0);

		return true;
	}
	case GER_DISMISS:
	{   //��������˳� ���������Ϣ
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			ReturnBulletScore(i);
			m_player[i].ClearSet(i);

			m_player[i].set_guid_gateid(0, 0);
		}
        m_bRun = false;
		return true;
	}
	}
	return false;
}

//���ͳ���
bool  CTableFrameSink::OnEventSendGameScene(WORD wChairID, BYTE cbGameStatus, bool bSendSecret)
{
    int GuID = GetPlayerGuID(wChairID);
    if (GuID == 0)
    {
        return false;
    }
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:
	case GAME_STATUS_PLAY:
	{
		SendGameConfig(wChairID);
		SendAllowFire(wChairID);
		SendPlayerInfo(wChairID);

		/*TCHAR szInfo[256];
		_sntprintf_s(szInfo, _TRUNCATE, TEXT("��ǰ�������Ϸ������ҵĶһ�����Ϊ%d��Ϸ�Ҷһ�%d���"),
		CGameConfig::GetInstance()->nChangeRatioUserScore, CGameConfig::GetInstance()->nChangeRatioFishScore);
		m_pITableFrame->SendGameMessage(pIServerUserItem, szInfo, SMT_CHAT);*/

		// ���� 1���ַ���תΪ��ֵ  2�ϱ�SendGameConfig �Ѿ�������nChangeRatioUserScore nChangeRatioFishScore Ϊ��Ҫ�ظ����� ��Ҫ�޸�Э��
		char szInfo[256];
		std::wstring str = TEXT("��ǰ�������Ϸ������ҵĶһ�����Ϊ%d��Ϸ�Ҷһ�%d���");
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		std::string narrowStr = conv.to_bytes(str);
		sprintf_s(szInfo, narrowStr.c_str(), CGameConfig::GetInstance()->nChangeRatioUserScore, CGameConfig::GetInstance()->nChangeRatioFishScore);

        CAutoLock cl(&g_LuaLock);
        CreatLuaPackage(g_LuaL, "on_Send2_pb", GuID, "SC_SystemMessage");
        PushNumToTable(g_LuaL, "wtype", SMT_CHAT);
        PushStrToTable(g_LuaL, "szstring", szInfo);
        CallLuaFun(g_LuaL);
		return true;
	}
	}
	return false;
}
//������Ϸϵͳ����
void CTableFrameSink::SendGameConfig(WORD wChairID)
{
    int GuID = GetPlayerGuID(wChairID);
    if (GuID == 0)
    {
        return;
    }
    {
        CAutoLock cl(&g_LuaLock);
        CreatLuaPackage(g_LuaL, "on_Send2_pb", GuID, "SC_GameConfig");
        PushNumToTable(g_LuaL, "server_id", 1);
        PushNumToTable(g_LuaL, "change_ratio_fish_score", CGameConfig::GetInstance()->nChangeRatioFishScore);
        PushNumToTable(g_LuaL, "change_ratio_user_score", CGameConfig::GetInstance()->nChangeRatioUserScore);
        PushNumToTable(g_LuaL, "exchange_once", CGameConfig::GetInstance()->nExchangeOnce);
        PushNumToTable(g_LuaL, "fire_interval", CGameConfig::GetInstance()->nFireInterval);
        PushNumToTable(g_LuaL, "max_interval", CGameConfig::GetInstance()->nMaxInterval);
        PushNumToTable(g_LuaL, "min_interval", CGameConfig::GetInstance()->nMinInterval);
        PushNumToTable(g_LuaL, "show_gold_min_mul", CGameConfig::GetInstance()->nShowGoldMinMul);
        PushNumToTable(g_LuaL, "max_bullet_count", CGameConfig::GetInstance()->nMaxBullet);
        PushNumToTable(g_LuaL, "max_cannon", CGameConfig::GetInstance()->m_MaxCannon);
        CallLuaFun(g_LuaL);
    }
	// ���� �ӵ����ã� �������ݲ���Ҫ����
	int nb = CGameConfig::GetInstance()->BulletVector.size();
	for (int i = 0; i < nb; ++i)
	{
        CAutoLock cl(&g_LuaLock);
        CreatLuaPackage(g_LuaL, "on_Send2_pb", GuID, "SC_BulletSet");
        PushNumToTable(g_LuaL, "first", i == 0 ? 1 : 0);
        PushNumToTable(g_LuaL, "bullet_size", CGameConfig::GetInstance()->BulletVector[i].nBulletSize);
        PushNumToTable(g_LuaL, "cannon_type", CGameConfig::GetInstance()->BulletVector[i].nCannonType);
        PushNumToTable(g_LuaL, "catch_radio", CGameConfig::GetInstance()->BulletVector[i].nCatchRadio);
        PushNumToTable(g_LuaL, "max_catch", CGameConfig::GetInstance()->BulletVector[i].nMaxCatch);
        PushNumToTable(g_LuaL, "mulriple", CGameConfig::GetInstance()->BulletVector[i].nMulriple);
        PushNumToTable(g_LuaL, "speed", CGameConfig::GetInstance()->BulletVector[i].nSpeed);
        CallLuaFun(g_LuaL);

		//send2client_pb(wChairID, &cbs);
	}
}
//���������Ϣ
void CTableFrameSink::SendPlayerInfo(WORD wChairID)
{
	for (WORD i = 0; i < GAME_PLAYER; ++i)
	{
		if (m_player[i].get_guid() != 0)
		{

            CAutoLock cl(&g_LuaLock);
            CreatLuaPackage(g_LuaL, "on_Send2_pb", m_player[i].get_guid(), "SC_UserInfo");
            PushNumToTable(g_LuaL, "chair_id", i);
            PushNumToTable(g_LuaL, "score", m_player[i].GetScore());
            PushNumToTable(g_LuaL, "cannon_mul", m_player[i].GetMultiply());
            PushNumToTable(g_LuaL, "cannon_type", m_player[i].GetCannonType());
            PushNumToTable(g_LuaL, "wastage", m_player[i].GetWastage());
            CallLuaFun(g_LuaL);

		}
	}
}

int   CTableFrameSink::GetPlayerGuID(WORD wChairID)
{
    if (INVALID_CHAIR == wChairID)
    {
        return 0;
    }

    if (wChairID >= m_player.size())
    {
        LOG_WARN("wChairID %d out of range[0,%d)", wChairID, m_player.size());
        return 0;
    }

    return m_player[wChairID].get_guid();
}
bool    CTableFrameSink::GetOnePlayerGuID(int &GuID)
{
    for (WORD i = 0; i < GAME_PLAYER; ++i)
    {
        if (m_player[i].get_guid() != 0)
        {
            GuID = m_player[i].get_guid();
        }
        return true;
    }
    return false;
}
//���ͳ�����Ϣ
void CTableFrameSink::SendSceneInfo(WORD wChairID)
{
    int GuID = GetPlayerGuID(wChairID);
    if (GuID == 0)
    {
        return;
    }
    {
        CAutoLock cl(&g_LuaLock);
        CreatLuaPackage(g_LuaL, "on_Send2_pb", GuID, "SC_SwitchScene");
        PushNumToTable(g_LuaL, "switching", 0);
        PushNumToTable(g_LuaL, "nst", m_nCurScene);

        CallLuaFun(g_LuaL);
    }

	//send2client_pb(wChairID, &css);


	m_BulletManager.Lock();
	obj_table_iter ibu = m_BulletManager.Begin();
	while (ibu != m_BulletManager.End())
	{
		CBullet* pBullet = (CBullet*)ibu->second;
		//�����ӵ�
		SendBullet(pBullet, wChairID);
		++ibu;
	}
	m_BulletManager.Unlock();

	m_FishManager.Lock();
	obj_table_iter ifs = m_FishManager.Begin();
	while (ifs != m_FishManager.End())
	{
		CFish* pFish = (CFish*)ifs->second;
		SendFish(pFish, wChairID);
		++ifs;
	}
	m_FishManager.Unlock();
}
//�����Ƿ�������
void CTableFrameSink::SendAllowFire(WORD wChairID)
{
    int GuID = GetPlayerGuID(wChairID);
    if (GuID == 0)
    {
        return;
    }
    CAutoLock cl(&g_LuaLock);
    CreatLuaPackage(g_LuaL, "on_Send2_pb", GuID, "SC_AllowFire");
    PushNumToTable(g_LuaL, "allow_fire", m_bAllowFire ? 1 : 0);
    CallLuaFun(g_LuaL);
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_GAMELOOP:
	{
		OnGameUpdate();
	}
	break;
	}
	return true;
}

stLuaMsg * CTableFrameSink::GetMsg()
{
    stLuaMsg * pTemp = NULL;
    CAutoLock cl(&m_LockLuaMsg);
    if (!m_lsLuaMsg.empty())
    {
        pTemp = m_lsLuaMsg.front();
        m_lsLuaMsg.pop_front();
    }
    return pTemp;
}
void    CTableFrameSink::PushMsg(stLuaMsg *Msg)
{
    CAutoLock cl(&m_LockLuaMsg);
    m_lsLuaMsg.push_back(Msg);
}
void CTableFrameSink::OnDealLuaMsg()
{
    stLuaMsg * pTemp = NULL;
    while (NULL != (pTemp = GetMsg()))
    {
        switch (pTemp->m_iMsgID)
        {
        case enMsgType_TreasureEnd:
        {
            if (pTemp->m_pMsg != NULL)
            {
                OnTreasureEND((CS_stTreasureEnd *)pTemp->m_pMsg);
            }
        }
        break;
        case enMsgType_ChangeCannonSet:
        {
            if (pTemp->m_pMsg != NULL)
            {
                OnChangeCannonSet((CS_stChangeCannonSet *)pTemp->m_pMsg);
            }
        }
        break;
        case enMsgType_Netcast:
        {
            if (pTemp->m_pMsg != NULL)
            {
                OnNetCast((CS_stNetcast *)pTemp->m_pMsg);
            }
        }
        break;
        case enMsgType_LockFish:
        {
            if (pTemp->m_pMsg != NULL)
            {
                OnLockFish((CS_stLockFish *)pTemp->m_pMsg);
            }
        }
        break;
        case enMsgType_Fire:
        {
            if (pTemp->m_pMsg != NULL)
            {
                OnFire((CS_stFire *)pTemp->m_pMsg);
            }
        }
        break;
        case enMsgType_ChangeCannon:
        {
            if (pTemp->m_pMsg != NULL)
            {
                OnChangeCannon((CS_stChangeCannon *)pTemp->m_pMsg);
            }
        }
        break;
        case enMsgType_TimeSync:
        {
            if (pTemp->m_pMsg != NULL)
            {
                OnTimeSync((CS_stTimeSync *)pTemp->m_pMsg);
            }
        }
        break;
        case enMsgType_RepositionSink:
        {
            RepositionSink();
        }
        break;
        case enMsgType_ActionUserSitDown:
        {
            if (pTemp->m_pMsg != NULL)
            {
                stLuaMsgType * pstTemp = (stLuaMsgType *)pTemp->m_pMsg;
                OnActionUserSitDown(pstTemp->cbByte, pstTemp->wValue, pstTemp->bRet);
            }
        }
        break;
        case enMsgType_ActionUserStandUp:
        {
            if (pTemp->m_pMsg != NULL)
            {
                stLuaMsgType * pstTemp = (stLuaMsgType *)pTemp->m_pMsg;
                OnActionUserStandUp(pstTemp->cbByte, pstTemp->wValue, pstTemp->bRet);
            }
        }
        break;
        case enMsgType_EventGameStart:
        {
            OnEventGameStart();
        }
        break;
        case enMsgType_EventGameConclude:
        {
            if (pTemp->m_pMsg != NULL)
            {
                stLuaMsgType * pstTemp = (stLuaMsgType *)pTemp->m_pMsg;
                OnEventGameConclude(pstTemp->wValue, pstTemp->cbByte);
            }
        }
        break;
        case enMsgType_EventSendGameScene:
        {
            if (pTemp->m_pMsg != NULL)
            {
                stLuaMsgType * pstTemp = (stLuaMsgType *)pTemp->m_pMsg;
                OnEventSendGameScene(pstTemp->wValue, pstTemp->cbByte, pstTemp->bRet);
            }
        }
        break;
        case enMsgType_SetNickNameAndMoney:
        {
            if (pTemp->m_pMsg != NULL)
            {
                stLuaMsgType * pstTemp = (stLuaMsgType *)pTemp->m_pMsg;
                set_nickname(pstTemp->wValue, pstTemp->strValue.c_str());
                set_money(pstTemp->wValue, pstTemp->lValue);
            }
        }
        break;
        case enMsgType_AddPlayerTable:
        {
        }
        break;
        case enMsgType_RemovePlayerTable:
        {
        }
        break;
        }
    }
}
//��Ϸ״̬����
void CTableFrameSink::OnGameUpdate()
{
	if (!m_bRun)
		return;

	DWORD NowTime = timeGetTime();
	int ndt = NowTime - m_dwLastTick;
	float fdt = ndt / 1000.0f;

	bool hasR = HasRealPlayer();

	for (WORD i = 0; i < GAME_PLAYER; ++i)
	{
		if (m_player[i].get_guid() == 0)
			continue;
		//��������¼�
		m_player[i].OnUpdate(ndt);
		//����Ҵ����������������
		if (m_player[i].bLocking())
		{
			//�����������ʱ�ж���ID���Ƿ����
			if (m_player[i].GetLockFishID() == 0)
			{
				//ID= 0 ��������
				LockFish(i);
				if (m_player[i].GetLockFishID() == 0)
					m_player[i].SetLocking(false);
			}
			else
			{
				CFish* pFish = (CFish*)m_FishManager.Find(m_player[i].GetLockFishID());
				if (pFish == NULL || !pFish->InSideScreen())
				{//���㲻���ڻ����Ѿ�����Ļ����������
					LockFish(i);
					if (m_player[i].GetLockFishID() == 0)
						m_player[i].SetLocking(false);
				}
			}
		}
	}
	//����������б�
	m_CanLockList.clear();
	//����������
	m_nFishCount = 0;

	//�Ƴ�����
	std::list<DWORD> rmList;
	//��������0
	m_nSpecialCount = 0;

	m_FishManager.Lock();
	obj_table_iter ifs = m_FishManager.Begin();
	while (ifs != m_FishManager.End())
	{
		CFish* pFish = (CFish*)ifs->second;
		//�������¼�
		pFish->OnUpdate(ndt);
		MoveCompent* pMove = (MoveCompent*)pFish->GetComponent(ECF_MOVE);
		if (pMove == NULL || pMove->IsEndPath())
		{//�ƶ����Ϊ�ջ� �Ѿ��ƶ�������
			if (pMove != NULL && pFish->InSideScreen())
			{//�ƶ���������ƶ���������������Ļ�� ��Ϊ��ָ�������ƶ�
				MoveCompent* pMove2 = (MoveCompent*)CreateComponent(EMCT_DIRECTION);
				if (pMove2 != NULL)
				{
					pMove2->SetSpeed(pMove->GetSpeed());
					pMove2->SetDirection(pMove->GetDirection());
					pMove2->SetPosition(pMove->GetPostion());
					pMove2->InitMove();
					//SetComponent��������������
					pFish->SetComponent(pMove2);
				}
			}
			else
			{//������ӵ��Ƴ��б�
				rmList.push_back(pFish->GetId());
			}
		}
		else if (pFish->GetFishType() != ESFT_NORMAL)
		{//Ǯ���Ͳ�������ͨ�� ������+1
			++m_nSpecialCount;
		}

		if (hasR && pFish->InSideScreen())
		{//������Ļ��
			if (pFish->GetLockLevel() > 0)
			{//�����ȼ�����0 ����������б�
				m_CanLockList.push_back(pFish->GetId());
			}
			//������+1
			++m_nFishCount;
		}
		++ifs;
	}
	m_FishManager.Unlock();
	//�����
	std::list<DWORD>::iterator it = rmList.begin();
	while (it != rmList.end())
	{
		m_FishManager.Remove(*it);
		++it;
	}
	rmList.clear();
	//�ӵ�
	m_BulletManager.Lock();
	obj_table_iter ibu = m_BulletManager.Begin();
	while (ibu != m_BulletManager.End())
	{
		CBullet* pBullet = (CBullet*)ibu->second;
		//�����ӵ��¼�
		pBullet->OnUpdate(ndt);
		//��ȡ�ƶ����
		MoveCompent* pMove = (MoveCompent*)pBullet->GetComponent(ECF_MOVE);
		if (pMove == NULL || pMove->IsEndPath())
		{//��û���ƶ�������Ѿ��ƶ����յ� ���뵽����б�
			rmList.push_back(pBullet->GetId());
		}
		//����Ҫֱ���жϣ�
		else if (CGameConfig::GetInstance()->bImitationRealPlayer && !hasR)
		{//�������ģ�� �� ����ң�
			{
				ifs = m_FishManager.Begin();
                int GuID = 0;
                GetOnePlayerGuID(GuID);
				while (ifs != m_FishManager.End())
				{
					CFish* pFish = (CFish*)ifs->second;
					//ֻҪ��û�� �ж� �Ƿ������
					if (pFish->GetState() < EOS_DEAD && pBullet->HitTest(pFish))
					{
						//��������ӵ�
                        if (GuID != 0)
                        {
                            CAutoLock cl(&g_LuaLock);
                            CreatLuaPackage(g_LuaL, "on_broadcast2client_pb", GuID, "SC_KillBullet");
                            PushNumToTable(g_LuaL, "chair_id", pBullet->GetChairID());
                            PushNumToTable(g_LuaL, "bullet_id", pBullet->GetId());
                            CallLuaFun(g_LuaL);
                        }
						//ץ����   //ץס�� Remove �����ƻ�ifs��
						CatchFish(pBullet, pFish, 1, 0);
						//�ӵ���������б�
						rmList.push_back(pBullet->GetId());
						break;
					}
					++ifs;
				}
			}
		}

		++ibu;
	}

	m_BulletManager.Unlock();

	it = rmList.begin();
	while (it != rmList.end())
	{
		m_BulletManager.Remove(*it);
		++it;
	}
	rmList.clear();

	DWORD tEvent = timeGetTime();
	CEventMgr::GetInstance()->Update(ndt);
	tEvent = timeGetTime() - tEvent;

	//�����������ˢ����
	DistrubFish(fdt);

	m_dwLastTick = NowTime;

    m_Timer.expires_from_now(boost::posix_time::millisec(1000/30));
    m_Timer.async_wait(boost::bind(&CTableFrameSink::OnGameUpdate, this));
}
//�ж��Ƿ��������
bool CTableFrameSink::HasRealPlayer()
{
	for (WORD i = 0; i < GAME_PLAYER; ++i)
	{
		if (m_player[i].get_guid() != 0)
			return true;
	}

	return false;
}
//ץ����
void CTableFrameSink::CatchFish(CBullet* pBullet, CFish* pFish, int nCatch, int* nCatched)
{
	//��ȡ�ӵ� �������͵ĸ���ֵ
	float pbb = pBullet->GetProbilitySet(pFish->GetTypeID()) / MAX_PROBABILITY;
	//��ȡ�㱻ץ������ֵ
	float pbf = pFish->GetProbability() / nCatch;
	//���ñ���
	float fPB = 1.0f;

	//��ȡ��׿����ֵ
	fPB = CGameConfig::GetInstance()->fAndroidProbMul;

	std::list<MyObject*> list;      //��ű���׽�� ����������������

	bool bCatch = false;        //�Ƿ�ץ��
	SCORE lScore = 0;           //��ֵ����
	auto chair_id = pBullet->GetChairID();  //��ȡ�ӵ��������
//	ASSERT(chair_id < MAX_TABLE_CHAIR);


	//�ж��Ƿ�ץ�����ӵ�ץ������ĸ���*�����㱻ץ�ĸ���*���ʣ�
	bCatch = RandFloat(0, MAX_PROBABILITY) < pbb * pbf * fPB;
	if (bCatch)
	{
		//ץ����ִ���㱻ץЧ��
		lScore = CommonLogic::GetFishEffect(pBullet, pFish, list, false);
	}

	auto score_pool = user_score_pools_[chair_id];
	if (!bCatch && score_pool > 0)
	{// �����ǰû�гɹ����� ��������б��Ե��ӵ�
		bCatch = score_pool > lScore;// ����Ե���ҵķ���������ķ��������ȸ�����˻��Ե��ķ���
		if (bCatch)
		{// �Եķ������˻���
			user_score_pools_[chair_id] -= lScore;
		}
	}


	// del lee for test
	//pFish->SetState(EOS_HIT, pBullet);

	if (bCatch)
	{
		//{
		//	// ���ϱ�ִ����һ�Σ� ��ִ��һ�Σ� ���ţ�
		//	std::list<MyObject*> ll;
		//	LONGLONG lst = CommonLogic::GetFishEffect(pBullet, pFish, ll, false);
		//	ll.clear();
		//}


		m_player[pBullet->GetChairID()].AddScore(lScore);

		user_win_scores_[chair_id] += lScore;

		//������ �����ֵ/�ڵ�ֵ ���� �����ڻ��� �� ���ֵ С���������� Ϊ��һ�ȡ˫����BUFF
		if (lScore / pBullet->GetScore() > CGameConfig::GetInstance()->nIonMultiply && RandInt(0, MAX_PROBABILITY) < CGameConfig::GetInstance()->nIonProbability)
		{
			BufferMgr* pBMgr = (BufferMgr*)m_player[pBullet->GetChairID()].GetComponent(ECF_BUFFERMGR);
			if (pBMgr != NULL && !pBMgr->HasBuffer(EBT_DOUBLE_CANNON))
			{
				pBMgr->Add(EBT_DOUBLE_CANNON, 0, CGameConfig::GetInstance()->fDoubleTime);
				//���ţ�ֱ���Ƿ��� SendCannonSet
				//RaiseEvent("CannonSetChanaged", &(m_player[pBullet->GetChairID()]));
				SendCannonSet(pBullet->GetChairID());
			}
		}

		//����ץ��
		SendCatchFish(pBullet, pFish, lScore);

		//��������������
		std::list<MyObject*>::iterator im = list.begin();
		while (im != list.end())
		{
			CFish* pf = (CFish*)*im;
			for (WORD i = 0; i < GAME_PLAYER; ++i)
			{
				if (m_player[i].GetLockFishID() == pf->GetId())
				{
					m_player[i].SetLockFishID(0);
				}
			}
			if (pf != pFish)
			{
				m_FishManager.Remove(pf);
			}
			++im;
		}
		//�Ƴ���
		m_FishManager.Remove(pFish);

		//�ô����� ����ȫΪ�� ����
		if (nCatched != NULL)
			*nCatched = *nCatched + 1;
	}
}
//�����㱻ץ
void CTableFrameSink::SendCatchFish(CBullet* pBullet, CFish*pFish, LONGLONG score)
{
    int GuID = 0;
    if (!GetOnePlayerGuID(GuID))
    {
        return;
    }
	if (pBullet != NULL && pFish != NULL)
	{
        CAutoLock cl(&g_LuaLock);
        CreatLuaPackage(g_LuaL, "on_broadcast2client_pb", GuID, "SC_KillFish");
        PushNumToTable(g_LuaL, "chair_id", pBullet->GetChairID());
        PushNumToTable(g_LuaL, "fish_id", pFish->GetId());
        PushNumToTable(g_LuaL, "score", score);
        PushNumToTable(g_LuaL, "bscoe", pBullet->GetScore());
        CallLuaFun(g_LuaL);
	}
}
//�����������BUFF
void CTableFrameSink::AddBuffer(int btp, float parm, float ft)
{
    int GuID = 0;
    if(GetOnePlayerGuID(GuID))
    {
        CAutoLock cl(&g_LuaLock);
        CreatLuaPackage(g_LuaL, "on_broadcast2client_pb", GuID, "SC_AddBuffer");
        PushNumToTable(g_LuaL, "buffer_type", btp);
        PushNumToTable(g_LuaL, "buffer_param", parm);
        PushNumToTable(g_LuaL, "buffer_time", ft);
        CallLuaFun(g_LuaL);
    }

	m_FishManager.Lock();
	obj_table_iter ifs = m_FishManager.Begin();
	while (ifs != m_FishManager.End())
	{
		MyObject* pObj = ifs->second;
		BufferMgr* pBM = (BufferMgr*)pObj->GetComponent(ECF_BUFFERMGR);
		if (pBM != NULL)
		{
			pBM->Add(btp, parm, ft);
		}
		++ifs;
	}
	m_FishManager.Unlock();
}
//�������� ������������ ��ˢ��
void CTableFrameSink::DistrubFish(float fdt)
{
	if (m_fPauseTime > 0.0f)
	{
		m_fPauseTime -= fdt;
		return;
	}
	//����ʱ������
	m_fSceneTime += fdt;
	//ʱ����ڳ���׼��ʱ�䣬�Ҳ��ɿ��� INVALID_CHAIRȺ���ɿ������� ���ţ��Ƿ�Ӧ�ó����ڴ˴���Ϊʱ��ص�
	if (m_fSceneTime > SWITCH_SCENE_END && !m_bAllowFire)
	{
		m_bAllowFire = true;
		SendAllowFire(INVALID_CHAIR);
	}
	//�жϵ�ǰ�����Ƿ����
	if (CGameConfig::GetInstance()->SceneSets.find(m_nCurScene) == CGameConfig::GetInstance()->SceneSets.end())
	{
		return;
	}
	//����ʱ���Ƿ�С�ڳ�������ʱ��
	if (m_fSceneTime < CGameConfig::GetInstance()->SceneSets[m_nCurScene].fSceneTime)
	{
		int npos = 0;
		//��ȡ��ǰ������ˢ��ʱ���б�
		std::list<TroopSet>::iterator is = CGameConfig::GetInstance()->SceneSets[m_nCurScene].TroopList.begin();
		while (is != CGameConfig::GetInstance()->SceneSets[m_nCurScene].TroopList.end())
		{
			TroopSet &ts = *is;
			//�Ƿ�����Ҵ���
			if (!HasRealPlayer())
			{
				//������ʱ�䡡�Ƿ�Ϊˢ��ʱ�䡡
				if ((m_fSceneTime >= ts.fBeginTime) && (m_fSceneTime <= ts.fEndTime))
				{
					//������Ϊˢ�����ʱ��
					m_fSceneTime = ts.fEndTime + fdt;
				}
			}
			//������ʱ�䡡�Ƿ�Ϊˢ��ʱ�䡡
			if ((m_fSceneTime >= ts.fBeginTime) && (m_fSceneTime <= ts.fEndTime))
			{
				//��ѭ��С��ˢ������Ϣ����
				if (npos < m_vDistrubTroop.size())
				{
					int tid = ts.nTroopID;
					//�Ƿ������� ���� �������跢�Ͱ�
					if (!m_vDistrubTroop[npos].bSendDes)
					{
						//����������ٶ�BUFF
						AddBuffer(EBT_CHANGESPEED, 5, 60);
						//��ȡˢ����Ⱥ������Ϣ
						Troop* ptp = PathManager::GetInstance()->GetTroop(tid);
						if (ptp != NULL)
						{
							//��ȡ����������
							size_t nCount = ptp->Describe.size();
							//����4����ֻ����4��
							if (nCount > 4) nCount = 4;
							//����ˢ��ʱ�俪ʼʱ�� Ϊ 2��
							m_vDistrubTroop[npos].fBeginTime = nCount * 2.0f;//ÿ�����ַ���2�����ʾʱ��
                            //��������  ���� ��Ϊ����ID
                            int Guid;
                            if (GetOnePlayerGuID(Guid))
                            {
                                CAutoLock cl(&g_LuaLock);
                                CreatLuaPackage(g_LuaL, "on_broadcast2client_pb", Guid, "SC_SendDes");
                                for (int i = 0; i < nCount; ++i)
                                {
                                    PushStrToTable(g_LuaL, "des", (char *)ptp->Describe[i].c_str());
                                }
                                CallLuaFun(g_LuaL);
                            }
						}
						//����Ϊ�ѷ���
						m_vDistrubTroop[npos].bSendDes = true;
					}
					else if (!m_vDistrubTroop[npos].bSendTroop && m_fSceneTime > (m_vDistrubTroop[npos].fBeginTime + ts.fBeginTime))
					{//���û�з��͹���Ⱥ�� ����ʱ�� ���� ˢ��ʱ�����������ʱ��
						m_vDistrubTroop[npos].bSendTroop = true;
						//��ȡˢ����Ⱥ������Ϣ
						Troop* ptp = PathManager::GetInstance()->GetTroop(tid);
						if (ptp == NULL)
						{
							//���Ϊ�գ�����һ����
							m_fSceneTime += CGameConfig::GetInstance()->SceneSets[m_nCurScene].fSceneTime;
						}
						else
						{
							int n = 0;
							int ns = ptp->nStep.size();    //��ȡ���� ���岻��
							for (int i = 0; i < ns; ++i)
							{
								//ˢ���ID
								int Fid = -1;
								//��ȡ�ܲ���
								int ncount = ptp->nStep[i];
								for (int j = 0; j < ncount; ++j)
								{
									//n���� ����״��ʱ �˳�ѭ��
									if (n >= ptp->Shape.size()) break;
									//��ȡ��״��
									ShapePoint& tp = ptp->Shape[n++];
									//��Ȩ��
									int WeightCount = 0;
									//��ȡ�������б��Ȩ���б���Сֵ
									int nsz = min(tp.m_lTypeList.size(), tp.m_lWeight.size());
									//���Ϊ0����������
									if (nsz == 0) continue;
									//��ȡ��Ȩ��
									for (int iw = 0; iw < nsz; ++iw)
										WeightCount += tp.m_lWeight[iw];

									for (int ni = 0; ni < tp.m_nCount; ++ni)
									{
										if (Fid == -1 || !tp.m_bSame)
										{
											//�ڼ�����Ŀ��
											int wpos = 0;
											//���Ȩ��
											int nf = RandInt(0, WeightCount);
											//����ƥ���Ȩ��
											while (nf > tp.m_lWeight[wpos])
											{
												//���ڻ����Ȩ�����ֵ������
												if (wpos >= tp.m_lWeight.size()) break;
												//���ֵ��ȥ��ǰȨ��
												nf -= tp.m_lWeight[wpos];
												//Ŀ���1
												++wpos;
												//��������������б� 
												if (wpos >= nsz)
													wpos = 0;
											}
											//���λ��С�����б� ��ȡ ��ID
											if (wpos < tp.m_lTypeList.size())
												Fid = tp.m_lTypeList[wpos];
										}
										//������
										std::map<int, Fish>::iterator ift = CGameConfig::GetInstance()->FishMap.find(Fid);
										if (ift != CGameConfig::GetInstance()->FishMap.end())
										{
											Fish &finf = ift->second;
											CFish* pFish = CommonLogic::CreateFish(finf, tp.x, tp.y, 0.0f, ni*tp.m_fInterval, tp.m_fSpeed, tp.m_nPathID, true);
											if (pFish != NULL)
											{
												m_FishManager.Add(pFish);
												SendFish(pFish);
											}
										}
									}
								}
							}
						}
					}
				}
				return;
			}

			++is;
			++npos;
		}
		//�������ʱ����� ������ʼѡ��ʱ��
		if (m_fSceneTime > SWITCH_SCENE_END)
		{
			int nfpos = 0;
			//��ȡ�������б�
			std::list<DistrubFishSet>::iterator it = CGameConfig::GetInstance()->SceneSets[m_nCurScene].DistrubList.begin();
			while (it != CGameConfig::GetInstance()->SceneSets[m_nCurScene].DistrubList.end())
			{
				//��ǰ���� ������Ⱥ��
				DistrubFishSet &dis = *it;

				if (nfpos >= m_vDistrubFishTime.size())
				{
					break;
				}
				m_vDistrubFishTime[nfpos] += fdt;
				//[nfpos]������ˢ��ʱ�� ���� ��ǰʱ������ʱ�� ����ˢ��ʱ��
				if (m_vDistrubFishTime[nfpos] > dis.ftime)
				{
					//���һ��ˢ��ʱ��
					m_vDistrubFishTime[nfpos] -= dis.ftime;
					//�Ƿ�ǰ�������
					if (HasRealPlayer())
					{
						//��ȡȨ�غ����б���Сֵ
						int nsz = min(dis.Weight.size(), dis.FishID.size());
						//��Ȩ��
						int WeightCount = 0;
						//ˢ��������    ���һ��ˢ����Сֵ�����ֵ
						int nct = RandInt(dis.nMinCount, dis.nMaxCount);
						//��ˢ������
						int nCount = nct;
						//�����ͣ�
						int SnakeType = 0;
						//�����Ƿ���ڴ��� ˢ��������2
						if (dis.nRefershType == ERT_SNAK)
						{
							nCount += 2;
							nct += 2;
						}

						//��ȡһ��ˢ��ID
						DWORD nRefershID = IDGenerator::GetInstance()->GetID64();

						//��ȡ��Ȩ��
						for (int wi = 0; wi < nsz; ++wi)
							WeightCount += dis.Weight[wi];

						//����Ȩ�ر������1
						if (nsz > 0)
						{
							//��ID
							int ftid = -1;
							//��ȡһ����ͨ·��ID
							int pid = PathManager::GetInstance()->GetRandNormalPathID();
							while (nct > 0)
							{
								//��ͨ��
								if (ftid == -1 || dis.nRefershType == ERT_NORMAL)
								{
									if (WeightCount == 0)
									{//Ȩ��Ϊ0 
										ftid = dis.FishID[0];
									}
									else
									{
										//Ȩ�����
										int wpos = 0, nw = RandInt(0, WeightCount);
										while (nw > dis.Weight[wpos])
										{
											if (wpos < 0 || wpos >= dis.Weight.size()) break;
											nw -= dis.Weight[wpos];
											++wpos;
											if (wpos >= nsz)
												wpos = 0;
										}
										if (wpos >= 0 || wpos < dis.FishID.size())
											ftid = dis.FishID[wpos];
									}

									SnakeType = ftid;
								}
								//�����ˢ���ߣ���ȡͷ��β
								if (dis.nRefershType == ERT_SNAK)
								{
									if (nct == nCount)
										ftid = CGameConfig::GetInstance()->nSnakeHeadType;
									else if (nct == 1)
										ftid = CGameConfig::GetInstance()->nSnakeTailType;
								}
								//������
								std::map<int, Fish>::iterator ift = CGameConfig::GetInstance()->FishMap.find(ftid);
								if (ift != CGameConfig::GetInstance()->FishMap.end())
								{
									Fish &finf = ift->second;
									//������ͨ
									int FishType = ESFT_NORMAL;
									//���ƫ��ֵ
									float xOffest = RandFloat(-dis.OffestX, dis.OffestX);
									float yOffest = RandFloat(-dis.OffestY, dis.OffestY);
									//�����ʱʱ��
									float fDelay = RandFloat(0.0f, dis.OffestTime);
									//������߻���� �����
									if (dis.nRefershType == ERT_LINE || dis.nRefershType == ERT_SNAK)
									{
										xOffest = dis.OffestX;
										yOffest = dis.OffestY;
										fDelay = dis.OffestTime * (nCount - nct);
									}
									else if (dis.nRefershType == ERT_NORMAL && m_nSpecialCount < CGameConfig::GetInstance()->nMaxSpecailCount)
									{
										std::map<int, SpecialSet>* pMap = NULL;
										//���������ıһ��������
										int nrand = rand() % 100;
										int fft = ESFT_NORMAL;

										if (nrand < CGameConfig::GetInstance()->nSpecialProb[ESFT_KING])
										{
											pMap = &(CGameConfig::GetInstance()->KingFishMap);
											fft = ESFT_KING;
										}
										else
										{
											nrand -= CGameConfig::GetInstance()->nSpecialProb[ESFT_KING];
										}

										if (nrand < CGameConfig::GetInstance()->nSpecialProb[ESFT_KINGANDQUAN])
										{
											pMap = &(CGameConfig::GetInstance()->KingFishMap);
											fft = ESFT_KINGANDQUAN;
										}
										else
										{
											nrand -= CGameConfig::GetInstance()->nSpecialProb[ESFT_KINGANDQUAN];
										}

										if (nrand < CGameConfig::GetInstance()->nSpecialProb[ESFT_SANYUAN])
										{
											pMap = &(CGameConfig::GetInstance()->SanYuanFishMap);
											fft = ESFT_SANYUAN;
										}
										else
										{
											nrand -= CGameConfig::GetInstance()->nSpecialProb[ESFT_SANYUAN];
										}

										if (nrand < CGameConfig::GetInstance()->nSpecialProb[ESFT_SIXI])
										{
											pMap = &(CGameConfig::GetInstance()->SiXiFishMap);
											fft = ESFT_SIXI;
										}
										//�ж��Ƿ������������
										if (pMap != NULL)
										{
											std::map<int, SpecialSet>::iterator ist = pMap->find(ftid);
											if (ist != pMap->end())
											{
												SpecialSet& kks = ist->second;
												//���������������ж��Ƿ�����
												if (RandFloat(0, MAX_PROBABILITY) < kks.fProbability)
													FishType = fft;
											}
										}
									}
									//������
									CFish* pFish = CommonLogic::CreateFish(finf, xOffest, yOffest, 0.0f, fDelay, finf.nSpeed, pid, false, FishType);
									if (pFish != NULL)
									{
										//������ID
										pFish->SetRefershID(nRefershID);
										m_FishManager.Add(pFish);
										SendFish(pFish);
									}
								}

								if (ftid == CGameConfig::GetInstance()->nSnakeHeadType)
									ftid = SnakeType;

								--nct;
							}
						}
					}
				}
				++it;
				++nfpos;
			}
		}
	}
	else
	{//������ʱ����ڳ�������ʱ�� �л�����
		//��ȡ��һ����ID ���ж��Ƿ����
		int nex = CGameConfig::GetInstance()->SceneSets[m_nCurScene].nNextID;
		if (CGameConfig::GetInstance()->SceneSets.find(nex) != CGameConfig::GetInstance()->SceneSets.end())
		{
			m_nCurScene = nex;
		}
		//���ó���
		ResetSceneDistrub();
		//������ ������ ������״̬ �ӵ�
        int GuID = 0;
		for (WORD wc = 0; wc < GAME_PLAYER; ++wc)
		{
			m_player[wc].SetLocking(false);
			m_player[wc].SetLockFishID(0);
			m_player[wc].ClearBulletCount();
            if (m_player[wc].get_guid() == 0)
            {
                continue;
            }
            GuID = m_player[wc].get_guid();
            //���� ������Ϣ
            CAutoLock cl(&g_LuaLock);
            CreatLuaPackage(g_LuaL, "on_broadcast2client_pb", m_player[wc].get_guid(), "SC_LockFish");
            PushNumToTable(g_LuaL, "chair_id", wc);
            CallLuaFun(g_LuaL);
		}

		//�趨���ɿ��� ������
		m_bAllowFire = false;
        SendAllowFire(INVALID_CHAIR);

        //���ͳ����滻
        if (GuID != 0)
        {
            CAutoLock cl(&g_LuaLock);
            CreatLuaPackage(g_LuaL, "on_broadcast2client_pb", GuID, "SC_SwitchScene");
            PushNumToTable(g_LuaL, "nst", m_nCurScene);
            PushNumToTable(g_LuaL, "switching", 1);
            CallLuaFun(g_LuaL);
        }

		//�����
		m_FishManager.Clear();
		//m_BulletManager.Clear();

		m_fSceneTime = 0.0f;
	}
}
//��ȡ������� ���ţ�ÿ��ѭ����ȡ��
int	CTableFrameSink::CountPlayer()
{
	int n = 0;

	for (WORD i = 0; i < GAME_PLAYER; ++i)
	{
		if (m_player[i].get_guid() != 0)
			++n;
	}

	return n;
}
//����������
void CTableFrameSink::SendFish(CFish* pFish, WORD wChairID)
{
    int GuID = GetPlayerGuID(wChairID);
    if (GuID == 0)
    {
        return;
    }
	std::map<int, Fish>::iterator ift = CGameConfig::GetInstance()->FishMap.find(pFish->GetTypeID());
	if (ift != CGameConfig::GetInstance()->FishMap.end())
	{
		Fish finf = ift->second;

        MoveCompent* pMove = (MoveCompent*)pFish->GetComponent(ECF_MOVE);
        BufferMgr* pBM = (BufferMgr*)pFish->GetComponent(ECF_BUFFERMGR);

        CAutoLock cl(&g_LuaLock);
        CreatLuaPackage(g_LuaL, "on_Send2_pb", GuID, "SC_SendFish");

        PushNumToTable(g_LuaL, "fish_id", pFish->GetId());
        PushNumToTable(g_LuaL, "type_id", pFish->GetTypeID());
        PushNumToTable(g_LuaL, "create_tick", pFish->GetCreateTick());
        PushNumToTable(g_LuaL, "fis_type", pFish->GetFishType());
        PushNumToTable(g_LuaL, "refersh_id", pFish->GetRefershID());

		if (pMove != NULL)
        {
            PushNumToTable(g_LuaL, "path_id", pMove->GetPathID());
			if (pMove->GetID() == EMCT_DIRECTION)
            {
                PushNumToTable(g_LuaL, "offest_x", pMove->GetPostion().x_);
                PushNumToTable(g_LuaL, "offest_y", pMove->GetPostion().y_);
			}
            else
            {
                PushNumToTable(g_LuaL, "offest_x", pMove->GetOffest().x_);
                PushNumToTable(g_LuaL, "offest_y", pMove->GetOffest().y_);
            }
            PushNumToTable(g_LuaL, "dir", pMove->GetDirection());
            PushNumToTable(g_LuaL, "delay", pMove->GetDelay());
            PushNumToTable(g_LuaL, "fish_speed", pMove->GetSpeed());
            PushNumToTable(g_LuaL, "troop", pMove->bTroop() ? 1 : 0);
		}

		if (pBM != NULL && pBM->HasBuffer(EBT_ADDMUL_BYHIT))
		{
			PostEvent("FishMulChange", pFish);
		}
        PushNumToTable(g_LuaL, "server_tick", timeGetTime());

        CallLuaFun(g_LuaL);
		// 		css.bSpecial = pFish->bSpecial();
	}
}

//��Ϸ��Ϣ����
/*bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID,  void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
switch(wSubCmdID)
{
case SUB_C_TIME_SYNC:
{
return OnTimeSync(pDataBuffer, wDataSize);
}
case SUB_C_CHANGE_SCORE:
{
return OnChangeScore(pDataBuffer, wDataSize, pIServerUserItem);
}
case SUB_C_CHANAGE_CANNON:
{
return OnChangeCannon(pDataBuffer, wDataSize);
}
case SUB_C_FIRE:
{
return OnFire(pDataBuffer, wDataSize, pIServerUserItem);
}
case SUB_C_ENDGAME:
{
//m_pITableFrame->PerformStandUpAction(pIServerUserItem);
// 			if (m_pITableFrame->PerformStandUpAction(pIServerUserItem) == false)
// 				CTraceService::TraceStringEx(TraceLevel_Debug, TEXT("վ��ʧ��221  %d %d=%s"), pIServerUserItem->GetTableID(), pIServerUserItem->GetChairID(), pIServerUserItem->GetNickName());
return true;
}
case SUB_C_LOCK_FISH:
{
return OnLockFish(pDataBuffer, wDataSize);
}
case SUB_C_BREADY:
{
SendSceneInfo(pIServerUserItem->GetChairID());
return true;
}
case SUB_C_NETCAST:
{
return OnNetCast(pDataBuffer, wDataSize);
}
case SUB_C_CHANGE_CANNONSET:
{
return OnChangeCannonSet(pDataBuffer, wDataSize);
}
case SUB_C_TREASURE_END:
{
return OnTreasureEND(pDataBuffer, wDataSize);
}
case SUB_C_RELOAD_CONFIG:
{
if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
{
LoadConfig();
}
return true;
}
case SUB_C_TURN_ON_CONTROL:
{
if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
{
ServerManager::InitControl();

if(ServerManager::m_pControl != NULL && m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
ServerManager::m_pControl->Initialization(m_pITableFrame, m_pGameServiceOption, (float)CGameConfig::GetInstance()->nChangeRatioUserScore / CGameConfig::GetInstance()->nChangeRatioFishScore);
}
return true;
}
case SUB_C_TURN_OFF_CONTROL:
{
if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
{
if(ServerManager::m_pControl != NULL && m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
ServerManager::FreeControl();
}
return true;
}
default:
{
if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
{
if(ServerManager::m_pControl != NULL && m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
{
return ServerManager::m_pControl->RecvControlReq(m_pITableFrame, wSubCmdID, pDataBuffer, wDataSize, pIServerUserItem->GetChairID());
}

return true;
}
}
}

return false;
}*/
//�ı���ڼ�
bool CTableFrameSink::OnChangeCannonSet(CS_stChangeCannonSet* msg)
{
	if (msg->chair_id >= GAME_PLAYER) return false;

	BufferMgr* pBMgr = (BufferMgr*)m_player[msg->chair_id].GetComponent(ECF_BUFFERMGR);
	if (pBMgr != NULL && (pBMgr->HasBuffer(EBT_DOUBLE_CANNON) || pBMgr->HasBuffer(EBT_ION_CANNON)))
	{
		return true;//�����ڻ�������ʱ��ֹ����
	}
	//��ȡ���ڼ�����
	int n = m_player[msg->chair_id].GetCannonSetType();

	do
	{
		if (msg->add)
		{
			if (n < CGameConfig::GetInstance()->CannonSetArray.size() - 1)
			{
				++n;
			}
			else
			{
				n = 0;
			}
		}
		else
		{
			if (n >= 1)
			{
				--n;
			}
			else
			{
				n = CGameConfig::GetInstance()->CannonSetArray.size() - 1;
			}
		}//����������ID ��˫��ID���˳�ѭ��
	} while (n == CGameConfig::GetInstance()->CannonSetArray[n].nIonID || n == CGameConfig::GetInstance()->CannonSetArray[n].nDoubleID);

	if (n < 0) n = 0;
	if (n >= CGameConfig::GetInstance()->CannonSetArray.size())
		n = CGameConfig::GetInstance()->CannonSetArray.size() - 1;

	//���ô��ڼ����� ��CacluteCannonPos ��ȡ���Ǵ������� m_nCannonType
	m_player[msg->chair_id].SetCannonSetType(n);
	//�����������
	m_player[msg->chair_id].CacluteCannonPos(msg->chair_id);
	//���ʹ�����Ϣ
	SendCannonSet(msg->chair_id);

	return true;
}
//����
bool CTableFrameSink::OnFire(CS_stFire* msg)
{
	if (msg->chair_id >= GAME_PLAYER) return false;
	auto chair_id = msg->chair_id;

	// lee test
	// 	m_player[pf->wChairID].SetLastFireTick(timeGetTime());
	// 	return true;
	// lee test end.
	//��ȡ�ӵ�����
	int mul = m_player[msg->chair_id].GetMultiply();
	if (mul < 0 || mul >= CGameConfig::GetInstance()->BulletVector.size()) return false;

	//��������ҿ��Կ���
	if (m_bAllowFire && (HasRealPlayer() || CGameConfig::GetInstance()->bImitationRealPlayer) && m_player[msg->chair_id].CanFire())
	{
		//��ȡ�ӵ�
		Bullet &binf = CGameConfig::GetInstance()->BulletVector[mul];
		//��ҽ�Ǯ�����ӵ�ֵ�� �� ������ӵ��� С������ӵ���
		if ((m_player[msg->chair_id].GetScore() >= binf.nMulriple) && (m_player[msg->chair_id].GetBulletCount() <= CGameConfig::GetInstance()->nMaxBullet))
		{
			m_player[msg->chair_id].AddScore(-binf.nMulriple);
			m_player[msg->chair_id].SetFired();

			LONGLONG lRevenue = 0;
			//if(ServerManager::m_pControl != NULL && ImitationRealPlayer(pIServerUserItem) && m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
			//	ServerManager::m_pControl->OnFire(m_pITableFrame, pIServerUserItem->GetChairID(), pIServerUserItem, binf.nMulriple, lRevenue);

			// ����˰�պ������Ӯ����
			user_revenues_[chair_id] += lRevenue;
			user_win_scores_[chair_id] -= binf.nMulriple;
			//�����ӵ�
			CBullet* pBullet = CommonLogic::CreateBullet(binf, m_player[msg->chair_id].GetCannonPos(), msg->direction,
				m_player[msg->chair_id].GetCannonType(), m_player[msg->chair_id].GetMultiply(), false);

			if (pBullet != NULL)
			{
				if (msg->client_id != 0)
					pBullet->SetId(msg->client_id);

				pBullet->SetChairID(msg->chair_id);       //��������
				pBullet->SetCreateTick(msg->fire_time);   //���ÿ���ʱ�� ��ʱ����ЧУ��

				//�������BUFF�Ƿ���˫����BUFF
				BufferMgr* pBMgr = (BufferMgr*)m_player[msg->chair_id].GetComponent(ECF_BUFFERMGR);
				if (pBMgr != NULL && pBMgr->HasBuffer(EBT_DOUBLE_CANNON))
					pBullet->setDouble(true);

				//�Ƿ���������
				if (m_player[msg->chair_id].GetLockFishID() != 0)
				{
					//��ȡ�ӵ��ƶ��ؼ�
					MoveCompent* pMove = (MoveCompent*)pBullet->GetComponent(ECF_MOVE);
					if (pMove != NULL)
					{
						pMove->SetTarget(&m_FishManager, m_player[msg->chair_id].GetLockFishID());
					}
				}

				DWORD now = timeGetTime();
				if (msg->fire_time > now)
				{
					//m_pITableFrame->SendTableData(pf->wChairID, SUB_S_FORCE_TIME_SYNC);
				}
				else
				{
					//����ӵ�����ʱ�����2��ִ�и����¼��������
					now = now - msg->fire_time;
					if (now > 2000) now = 2000;
					pBullet->OnUpdate(now);
				}
				//�����ӵ�
				m_player[msg->chair_id].ADDBulletCount(1);
				m_BulletManager.Add(pBullet);
				//�����ӵ�
				SendBullet(pBullet, INVALID_CHAIR, true);
			}
			//������󿪻�ʱ��
			m_player[msg->chair_id].SetLastFireTick(timeGetTime());
		}
	}

	return true;
}
//�����ӵ�
void CTableFrameSink::SendBullet(CBullet* pBullet, WORD wChairID, bool bNew)
{
	if (pBullet == NULL) return;


    CAutoLock cl(&g_LuaLock);
    CreatLuaPackage(g_LuaL, "on_Send2_pb", m_player[pBullet->GetChairID()].get_guid(), "SC_SendBullet");
    PushNumToTable(g_LuaL, "chair_id", pBullet->GetChairID());              //����ID
    PushNumToTable(g_LuaL, "id", pBullet->GetId());
    PushNumToTable(g_LuaL, "cannon_type", pBullet->GetCannonType());
    PushNumToTable(g_LuaL, "multiply", pBullet->GetTypeID());
    PushNumToTable(g_LuaL, "direction", pBullet->GetDirection());
    PushNumToTable(g_LuaL, "x_pos", pBullet->GetPosition().x_);
    PushNumToTable(g_LuaL, "y_pos", pBullet->GetPosition().y_);
    PushNumToTable(g_LuaL, "score", m_player[pBullet->GetChairID()].GetScore());
    PushNumToTable(g_LuaL, "is_new", bNew ? 1 : 0);
    PushNumToTable(g_LuaL, "is_double", pBullet->bDouble() ? 1 : 0);
    PushNumToTable(g_LuaL, "server_tick", timeGetTime());
    if (bNew)
    {
        PushNumToTable(g_LuaL, "create_tick", pBullet->GetCreateTick());
    }
    else
    {
        PushNumToTable(g_LuaL, "create_tick", timeGetTime());
    }
    CallLuaFun(g_LuaL);
}

//����ϵͳʱ��
bool CTableFrameSink::OnTimeSync(CS_stTimeSync* msg)
{
    if (GetPlayerGuID(msg->chair_id) != 0)
    {
        CAutoLock cl(&g_LuaLock);
        CreatLuaPackage(g_LuaL, "on_Send2_pb", m_player[msg->chair_id].get_guid(), "SC_TimeSync");
        PushNumToTable(g_LuaL, "chair_id", msg->chair_id);              //����ID
        PushNumToTable(g_LuaL, "client_tick", msg->client_tick);        //���ʱ��
        PushNumToTable(g_LuaL, "server_tick", timeGetTime());           //ϵͳʱ��
        CallLuaFun(g_LuaL);
        return true;
    }
    else
    {
        return false;
    }
}

/*bool CTableFrameSink::OnChangeScore(void* pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
if(sizeof(CMD_C_CHANGE_SCORE) != wDataSize) return false;
CMD_C_CHANGE_SCORE* pcs = (CMD_C_CHANGE_SCORE*)pData;

try
{
IServerUserItem *pUser = pIServerUserItem;
if(pUser == NULL) return false;
auto chair_id = pUser->GetChairID();

SCORE lUserScore = pUser->GetUserScore() - m_player[chair_id].GetWastage();
SCORE lFishScore = 0;
SCORE lBuyOnceScoe = CGameConfig::GetInstance()->nExchangeOnce * CGameConfig::GetInstance()->nChangeRatioUserScore / CGameConfig::GetInstance()->nChangeRatioFishScore;

if(pcs->bAdd)
{
if(pcs->bAddAll || lUserScore < lBuyOnceScoe)
{
lFishScore = lUserScore * CGameConfig::GetInstance()->nChangeRatioFishScore / CGameConfig::GetInstance()->nChangeRatioUserScore;
}
else
{
lFishScore = CGameConfig::GetInstance()->nExchangeOnce;
}
}
else
{
lFishScore = -m_player[chair_id].GetScore();
}

if(lFishScore != 0)
{
lUserScore = lFishScore * CGameConfig::GetInstance()->nChangeRatioUserScore / CGameConfig::GetInstance()->nChangeRatioFishScore;
m_player[chair_id].AddScore(lFishScore);
m_player[chair_id].AddWastage(lUserScore);

CMD_S_CHANGE_SCORE css;
css.wChairID = chair_id;
css.lWastageScore = m_player[chair_id].GetWastage();
css.lFishScore = lFishScore;

m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CHANGE_SCORE, &css, sizeof(CMD_S_CHANGE_SCORE));
m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CHANGE_SCORE, &css, sizeof(CMD_S_CHANGE_SCORE));

m_player[chair_id].SetLastFireTick(timeGetTime());

if(ServerManager::m_pControl != NULL && ImitationRealPlayer(pUser) && m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
ServerManager::m_pControl->OnChangeScore(m_pITableFrame, chair_id, pUser, -lUserScore, lFishScore);
}

return true;
}
catch (...)
{
CTraceService::TraceString(TEXT("OnChangeScore����"),TraceLevel_Exception);
DebugString(TEXT("[Fish]OnChangeScore����"));
return false;
}
}*/
//�任����
bool CTableFrameSink::OnChangeCannon(CS_stChangeCannon* msg)
{
	if (msg->chair_id >= GAME_PLAYER)
	{
		return false;
	}


	//��ȡBuff������
	BufferMgr* pBMgr = (BufferMgr*)m_player[msg->chair_id].GetComponent(ECF_BUFFERMGR);
	//�鿴��ǰ�����Ƿ�Ϊ˫����������
	if (pBMgr != NULL && (pBMgr->HasBuffer(EBT_DOUBLE_CANNON) || pBMgr->HasBuffer(EBT_ION_CANNON)))
	{
		return true;//�����ڻ�������ʱ��ֹ����
	}

	//��ȡ��ǰ�ӵ�����
	int mul = m_player[msg->chair_id].GetMultiply();

	if (msg->add)
	{
		++mul;
	}
	else
	{
		--mul;
	}
	//ѭ������
	if (mul < 0) mul = CGameConfig::GetInstance()->BulletVector.size() - 1;
	if (mul >= CGameConfig::GetInstance()->BulletVector.size()) mul = 0;
	//��������
	m_player[msg->chair_id].SetMultiply(mul);
	//��ȡ�ӵ���Ӧ��������
	int CannonType = CGameConfig::GetInstance()->BulletVector[mul].nCannonType;
	//������
	m_player[msg->chair_id].SetCannonType(CannonType);
	//����������
	SendCannonSet(msg->chair_id);
	//����
	m_player[msg->chair_id].SetLastFireTick(timeGetTime());

	return true;
}
//���ʹ�������
void CTableFrameSink::SendCannonSet(WORD wChairID)
{
    int GuID = GetPlayerGuID(wChairID);
    if (GuID == 0)
    {
        if (!GetOnePlayerGuID(GuID))
        {
            return;
        }
    }
    CAutoLock cl(&g_LuaLock);
    CreatLuaPackage(g_LuaL, "on_broadcast2client_pb", GuID, "SC_CannonSet");
    PushNumToTable(g_LuaL, "chair_id", wChairID);              //����ID
    PushNumToTable(g_LuaL, "cannon_mul", m_player[wChairID].GetMultiply());
    PushNumToTable(g_LuaL, "cannon_type", m_player[wChairID].GetCannonType());
    PushNumToTable(g_LuaL, "cannon_set", m_player[wChairID].GetCannonSetType());
    CallLuaFun(g_LuaL);
}
//�򿪱���
bool CTableFrameSink::OnTreasureEND(CS_stTreasureEnd* msg)
{
	if (msg->chair_id >= 0 && msg->chair_id < m_player.size() && m_player[msg->chair_id].get_guid() != 0)
	{

		char szInfo[512];
		std::wstring str = TEXT("��ϲ%s��%d������ҡ�%s�����б���,�������л��%I64d���!!!");
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		std::string narrowStr = conv.to_bytes(str);
		sprintf_s(szInfo, narrowStr.c_str(), "fishing",//GameServerConfigManager::instance()->get_config().game_name(),
			get_table_id(), m_player[msg->chair_id].get_nickname().c_str(), msg->score);
		//����CatchFishBroadCast �����¼� Bind_Event_Handler
		RaiseEvent("CatchFishBroadCast", szInfo, &m_player[msg->chair_id]);
	}

	return true;
}
//
void CTableFrameSink::ReturnBulletScore(WORD wChairID)
{
    {
        CAutoLock cl(&g_LuaLock);
        CreatLuaPackage(g_LuaL, "on_Send2luaback_pb", m_player[wChairID].get_guid(), "on_Send2luaback_pb");
        PushNumToTable(g_LuaL, "money", m_player[wChairID].GetScore());
        PushNumToTable(g_LuaL, "bout", true);
        CallLuaFun(g_LuaL);
    }

#if 0
	if (wChairID >= GAME_PLAYER)
	{
		DebugString(TEXT("[Fish]ReturnBulletScore Err: wTableID %d wChairID %d"), m_pITableFrame->GetTableID(), wChairID);
		return;
	}
	try
	{
		IServerUserItem* pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem != NULL)
		{
			// 			SCORE score = m_player[wChairID].GetScore();
			// 			if(score != 0)
			// 			{
			// 				LONGLONG ls = score * CGameConfig::GetInstance()->nChangeRatioUserScore / CGameConfig::GetInstance()->nChangeRatioFishScore;
			// 				m_player[wChairID].AddWastage(-ls);
			// 			}
			// 
			// 			tagScoreInfo ScoreInfo;
			// 			ZeroMemory(&ScoreInfo, sizeof(tagScoreInfo));
			// 			score = -m_player[wChairID].GetWastage();
			// 			LONGLONG lReve=0,cbRevenue=m_pGameServiceOption->wRevenueRatio;	
			// 			if (score > 0)
			// 			{	
			// 				float fRevenuePer = float(cbRevenue/1000);
			// 				lReve  = LONGLONG(score*fRevenuePer);
			// 				ScoreInfo.cbType = SCORE_TYPE_WIN;
			// 			}
			// 			else if (score < 0)
			// 				ScoreInfo.cbType = SCORE_TYPE_LOSE;
			// 			else
			// 				ScoreInfo.cbType = SCORE_TYPE_DRAW;
			// 			ScoreInfo.lScore = score;
			// 			ScoreInfo.lRevenue = lReve;
			// 
			// 			m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);

			if (user_win_scores_[wChairID] != 0 || user_revenues_[wChairID] != 0) {// �з��ڹ�
				tagScoreInfo ScoreInfo = { 0 };
				ScoreInfo.cbType = (user_win_scores_[wChairID] > 0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
				ScoreInfo.lRevenue = user_revenues_[wChairID];
				ScoreInfo.lScore = user_win_scores_[wChairID];
				user_revenues_[wChairID] = 0;
				user_win_scores_[wChairID] = 0;
				m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);
			}

			m_player[wChairID].ClearSet(wChairID);
		}
	}
	catch (...)
	{
		CTraceService::TraceString(TEXT("ReturnBulletScore����1"), TraceLevel_Exception);
		DebugString(TEXT("[Fish]ReturnBulletScore����1"));
	}

	std::list<DWORD> rmList;
	m_BulletManager.Lock();
	try
	{
		obj_table_iter ibu = m_BulletManager.Begin();
		while (ibu != m_BulletManager.End())
		{
			CBullet* pBullet = (CBullet*)ibu->second;
			if (pBullet->GetChairID() == wChairID)
				rmList.push_back(pBullet->GetId());

			++ibu;
		}
	}
	catch (...)
	{
		CTraceService::TraceString(TEXT("ReturnBulletScore����2"), TraceLevel_Exception);
		DebugString(TEXT("[Fish]ReturnBulletScore����2"));
	}
	m_BulletManager.Unlock();

	std::list<DWORD>::iterator it = rmList.begin();
	while (it != rmList.end())
	{
		m_BulletManager.Remove(*it);
		++it;
	}

	rmList.clear();
#endif
}
//�����¼�
void CTableFrameSink::OnAdwardEvent(CMyEvent* pEvent)
{
	//�ж��¼��Ƿ�Ϊ���¼�
	if (pEvent == NULL || pEvent->GetName() != "AdwardEvent") return;
	//�����¼�
	CEffectAward* pe = (CEffectAward*)pEvent->GetParam();
	//��
	CFish* pFish = (CFish*)pEvent->GetSource();
	//�ӵ�
	CBullet* pBullet = (CBullet*)pEvent->GetTarget();

	if (pe == NULL || pFish == NULL || pBullet == NULL) return;
	//������Ҳ��ɿ���
	m_player[pBullet->GetChairID()].SetCanFire(false);

	LONGLONG lScore = 0;
	//GetParam(1) ��������ʾʵ��Ч�� ���ӽ�ҡ������ӣ£գƣƣţ�
	if (pe->GetParam(1) == 0)
	{
		if (pe->GetParam(2) == 0)
			lScore = pe->GetParam(3);
		else
			lScore = pBullet->GetScore() * pe->GetParam(3);
	}
	else
	{
		//��ʹ�ӵ���BUFF
		BufferMgr* pBMgr = (BufferMgr*)m_player[pBullet->GetChairID()].GetComponent(ECF_BUFFERMGR);
		if (pBMgr != NULL && !pBMgr->HasBuffer(pe->GetParam(2)))
		{
			//GetParam(2)���� GetParam(3)����ʱ��
			pBMgr->Add(pe->GetParam(2), 0, pe->GetParam(3));
		}
	}
	//��Ҽ�Ǯ
	m_player[pBullet->GetChairID()].AddScore(lScore);
}
//������BUFF
void CTableFrameSink::OnAddBuffer(CMyEvent* pEvent)
{
	if (pEvent == NULL || pEvent->GetName() != "AddBuffer") return;
	CEffectAddBuffer* pe = (CEffectAddBuffer*)pEvent->GetParam();

	CFish* pFish = (CFish*)pEvent->GetSource();
	if (pFish == NULL) return;

	if (pFish->GetMgr() != &m_FishManager) return;

	//��Ŀ����ȫ����������Ϊ�ı��ٶ� �ı�ֵΪ0ʱ ���� ʱ��Ϊpe->GetParam(4)
	if (pe->GetParam(0) == 0 && pe->GetParam(2) == EBT_CHANGESPEED && pe->GetParam(3) == 0)//����
	{//��ֹֻͣ��ˢ��?
		m_fPauseTime = pe->GetParam(4);
	}
}
//ִ��������Ч��
void CTableFrameSink::OnMulChange(CMyEvent* pEvent)
{
    int GuID;
    if (!GetOnePlayerGuID(GuID))
    {
        return;
    }
	if (pEvent == NULL || pEvent->GetName() != "FishMulChange") return;

	CFish* pFish = (CFish*)pEvent->GetParam();
	if (pFish != NULL)
	{
		m_FishManager.Lock();
		obj_table_iter ifs = m_FishManager.Begin();
		while (ifs != m_FishManager.End())
		{

			CFish* pf = (CFish*)ifs->second;
			//�ҵ�һ��ͬ����㣬Ȼ��ִ������Ч��
			if (pf != NULL && pf->GetTypeID() == pFish->GetTypeID())
			{
				CBullet bt;
				bt.SetScore(1);
				std::list<MyObject*> llt;
				llt.clear();
				//����ҵ������������� 
				EffectMgr* pEM = (EffectMgr*)pf->GetComponent(ECF_EFFECTMGR);
                int multemp = 0;
				if (pEM != NULL)
				{//ִ������Ч��
                    multemp = pEM->Execute(&bt, llt, true);
				}

                CAutoLock cl(&g_LuaLock);
                CreatLuaPackage(g_LuaL, "on_broadcast2client_pb", GuID, "SC_FishMul");
                PushNumToTable(g_LuaL, "fish_id", pf->GetId());
                PushNumToTable(g_LuaL, "mul", multemp);
                CallLuaFun(g_LuaL);
			}

			++ifs;

		}
		m_FishManager.Unlock();
	}
}
//��һ�ο��� Ϊɶ��������� ��һ����������
void CTableFrameSink::OnFirstFire(CMyEvent* pEvent)
{
	if (pEvent == NULL || pEvent->GetName() != "FirstFire") return;

	CPlayer* pPlayer = (CPlayer*)pEvent->GetParam();

	for (WORD i = 0; i < GAME_PLAYER; ++i)
	{
		//������� ���� ������ 
		if (&m_player[i] == pPlayer)
		{
			//IServerUserItem* pUser = m_pITableFrame->GetTableUserItem(i);
			//if(pUser == NULL) break;

			int npos = 0;
			npos = CGameConfig::GetInstance()->FirstFireList.size() - 1;
			FirstFire& ff = CGameConfig::GetInstance()->FirstFireList[npos];
			//����������Ȩ����ȡ���ֵ
			int nsz = min(ff.FishTypeVector.size(), ff.WeightVector.size());

			if (nsz <= 0) continue;
			//��Ȩ��
			int WeightCount = 0;
			for (int iw = 0; iw < nsz; ++iw)
			{
				WeightCount += ff.WeightVector[iw];
			}
			//��ȡ����λ��
			MyPoint pt = m_player[i].GetCannonPos();
			//��ȡ���ڷ���
			float dir = CGameConfig::GetInstance()->CannonPos[i].m_Direction;
			//������
			for (int nc = 0; nc < ff.nCount; ++nc)
			{
				//�۸������
				for (int ni = 0; ni < ff.nPriceCount; ++ni)
				{
					//��ȡ һ����
					int Fid = ff.FishTypeVector[RandInt(0, nsz)];
					//���һ��Ȩ��
					int nf = RandInt(0, WeightCount);
					int wpos = 0;
					//ƥ��һ��Ȩ��
					for (; wpos < nsz; ++wpos)
					{
						if (nf > ff.WeightVector[wpos])
						{
							nf -= ff.WeightVector[wpos];
						}
						else
						{
							Fid = ff.FishTypeVector[wpos];
							break;;
						}
					}
					//���û��ƥ�䵽��ƥ���һ��
					if (wpos >= nsz)
					{
						Fid = ff.FishTypeVector[0];
					}

					//�������սǶȣ�
					dir = CGameConfig::GetInstance()->CannonPos[i].m_Direction - M_PI_2 + M_PI / ff.nPriceCount * ni;

					//����ƥ�䵽����
					std::map<int, Fish>::iterator ift = CGameConfig::GetInstance()->FishMap.find(Fid);
					if (ift != CGameConfig::GetInstance()->FishMap.end())
					{
						Fish& finf = ift->second;

						//������
						CFish* pFish = CommonLogic::CreateFish(finf, pt.x_, pt.y_, dir, RandFloat(0.0f, 1.0f) + nc, finf.nSpeed, -2);
						if (pFish != NULL)
						{
							m_FishManager.Add(pFish);
							SendFish(pFish);
						}
					}
				}
			}
			break;
		}
	}
}
//������
void CTableFrameSink::OnProduceFish(CMyEvent* pEvent)
{
	if (pEvent == NULL || pEvent->GetName() != "ProduceFish") return;

	CEffectProduce* pe = (CEffectProduce*)pEvent->GetParam();
	//SourceΪ��
	CFish* pFish = (CFish*)pEvent->GetSource();
	if (pFish == NULL) return;

	if (pFish->GetMgr() != &m_FishManager) return;
	//��ȡ����
	MyPoint& pt = pFish->GetPosition();
    list<SC_stSendFish> msg;
	//ͨ��ID������
	std::map<int, Fish>::iterator ift = CGameConfig::GetInstance()->FishMap.find(pe->GetParam(0));
	if (ift != CGameConfig::GetInstance()->FishMap.end())
	{
		Fish finf = ift->second;
		float fdt = M_PI * 2.0f / (float)pe->GetParam(2);
		//����Ϊ��ͨ
		int fishtype = ESFT_NORMAL;
		int ndif = -1;
		//����ѭ��
		for (int i = 0; i < pe->GetParam(1); ++i)
		{
			//�����һ�����������δ���2 ˢ����������10ֻʱ ���һ����ˢ��Ϊ����
			if ((i == pe->GetParam(1) - 1) && (pe->GetParam(1) > 2) && (pe->GetParam(2) > 10))
			{
				ndif = RandInt(0, pe->GetParam(2));
			}

			//ˢ������
			for (int j = 0; j < pe->GetParam(2); ++j)
			{
				if (j == ndif)
				{
					fishtype = ESFT_KING;
				}
				else
				{
					fishtype = ESFT_NORMAL;
				}
				//������
				CFish* pf = CommonLogic::CreateFish(finf, pt.x_, pt.y_, fdt*j, 1.0f + pe->GetParam(3)*i, finf.nSpeed, -2, false, fishtype);
				if (pf != NULL)
				{
					m_FishManager.Add(pf);
					//SendFish(pf);
					// ����ֻ��������
					//ASSERT(fishs->dwFishCount < 300);
					//if (fishs->dwFishCount >= 300) {
					//CTraceService::TraceString(_T("SUB_S_SEND_FISHS > 300"), TraceLevel_Warning);
					//	break;
					//}
					//if (msg.fishes_size() >= 300)
					//	break;
                    if (msg.size() >= 300)
                    {
                        break;
                    }
                    SC_stSendFish fish;
					fish.fish_id = pf->GetId();
                    fish.type_id = pf->GetTypeID();
                    fish.create_tick = pf->GetCreateTick();
                    fish.fis_type = pf->GetFishType();
                    fish.refersh_id = pf->GetRefershID();
					//����ƶ����
					MoveCompent* pMove = (MoveCompent*)pf->GetComponent(ECF_MOVE);
					if (pMove != NULL)
					{
						fish.path_id = pMove->GetPathID();
						fish.offest_x = pMove->GetOffest().x_;
						fish.offest_y = pMove->GetOffest().y_;
						if (pMove->GetID() == EMCT_DIRECTION)
						{
							fish.offest_x = pMove->GetPostion().x_;
							fish.offest_y = pMove->GetPostion().y_;
						}
						fish.dir = pMove->GetDirection();
						fish.delay = pMove->GetDelay();
						fish.fish_speed = pMove->GetSpeed();
						fish.troop = pMove->bTroop() ? 1 : 0;
					}

					BufferMgr* pBM = (BufferMgr*)pf->GetComponent(ECF_BUFFERMGR);
					if (pBM != NULL && pBM->HasBuffer(EBT_ADDMUL_BYHIT))
					{//�ҵ�BUFF������������BUFF ���� ���ӵ� ����¼�
						PostEvent("FishMulChange", pf);
					}

					fish.server_tick = timeGetTime();
                    msg.push_back(fish);
				}
			}
		}
	}
    int GuID;
    if (!GetOnePlayerGuID(GuID))
    {
        return;
    }
    list<SC_stSendFish>::iterator it = msg.begin();
    CAutoLock cl(&g_LuaLock);
    CreatLuaPackage(g_LuaL, "on_broadcast2client_pb", GuID, "SC_SendFishList");
    InitTableName();
    while (it != msg.end())
    {
        PushTabToTable_Begin(g_LuaL);
        SC_stSendFish &temp = *it++;
        PushNumToTable(g_LuaL, "fish_id",	   temp.fish_id); //��ID
        PushNumToTable(g_LuaL, "type_id",      temp.type_id);  //���ͣ�
        PushNumToTable(g_LuaL, "path_id",      temp.path_id);  //·��ID
        PushNumToTable(g_LuaL, "create_tick",  temp.create_tick);  //����ʱ��
        PushNumToTable(g_LuaL, "offest_x",     temp.offest_x);  //X����
        PushNumToTable(g_LuaL, "offest_y",     temp.offest_y);  //Y����
        PushNumToTable(g_LuaL, "dir",          temp.dir);  //����
        PushNumToTable(g_LuaL, "delay",        temp.delay);  //��ʱ
        PushNumToTable(g_LuaL, "server_tick",  temp.server_tick);  //ϵͳʱ��
        PushNumToTable(g_LuaL, "fish_speed",   temp.fish_speed);  //���ٶ�
        PushNumToTable(g_LuaL, "fis_type",     temp.fis_type);  //�����ͣ�
        PushNumToTable(g_LuaL, "troop",        temp.troop);      //�Ƿ���Ⱥ
        PushNumToTable(g_LuaL, "refersh_id",   temp.refersh_id);  //��ȡˢ��ID��
        PushTabToTable_End(g_LuaL);
    }
    CallLuaFun(g_LuaL);
    //����broadcast2client_pb(&msg);
}
//������
void CTableFrameSink::LockFish(WORD wChairID)
{
	DWORD dwFishID = 0;

	CFish* pf = NULL;
	//��ȡ��ǰ����ID
	dwFishID = m_player[wChairID].GetLockFishID();
	if (dwFishID != 0)
		pf = (CFish*)m_FishManager.Find(dwFishID);
	//�ж��Ƿ�������Ŀ��
	if (pf != NULL)
	{
		//�жϵ�ǰ������ �Ƿ��Ѿ�����������
		MoveCompent* pMove = (MoveCompent*)pf->GetComponent(ECF_MOVE);
		if (pf->GetState() >= EOS_DEAD || pMove == NULL || pMove->IsEndPath())
		{
			pf = NULL;
		}
	}

	dwFishID = 0;

	CFish* pLock = NULL;

	//��ѯ�������б�
	std::list<DWORD>::iterator iw = m_CanLockList.begin();
	while (iw != m_CanLockList.end())
	{
		//������
		CFish* pFish = (CFish*)m_FishManager.Find(*iw);
		//��ǰ����Ч �� û���� �� �����ȼ�����0 �� û���γ���Ļ
		if ((pFish != NULL) && (pFish->GetState() < EOS_DEAD) && (pFish->GetLockLevel()) > 0 && (pFish->InSideScreen()))
		{
			//��ȡ�����������ȼ�����
			if ((pf == NULL) || ((pf != pFish) && !m_player[wChairID].HasLocked(pFish->GetId())))
			{
				pf = pFish;

				if (pLock == NULL)
				{
					pLock = pf;
				}
				else if (pf->GetLockLevel() > pLock->GetLockLevel())
				{
					pLock = pf;
				}
			}
		}

		++iw;

	}

	if (pLock != NULL)
		dwFishID = pLock->GetId();
	//��������ID 
	m_player[wChairID].SetLockFishID(dwFishID);

	if (m_player[wChairID].GetLockFishID() != 0)
	{

        CAutoLock cl(&g_LuaLock);
        CreatLuaPackage(g_LuaL, "on_broadcast2client_pb", m_player[wChairID].get_guid(), "SC_LockFish");
        PushNumToTable(g_LuaL, "chair_id", wChairID);
        PushNumToTable(g_LuaL, "lock_id", dwFishID);
        CallLuaFun(g_LuaL);
	}
}
//������
bool CTableFrameSink::OnLockFish(CS_stLockFish* msg)
{
	//������λ���Ƿ����
	if (msg->chair_id >= GAME_PLAYER)
	{
		return false;
	}
	//���û������˳�
	if (!HasRealPlayer()) return true;

	if (msg->lock)
	{
		//�����������
		m_player[msg->chair_id].SetLocking(true);
		//������
		LockFish(msg->chair_id);
	}
	else
	{
		m_player[msg->chair_id].SetLocking(false);
		m_player[msg->chair_id].SetLockFishID(0);

        CAutoLock cl(&g_LuaLock);
        CreatLuaPackage(g_LuaL, "on_broadcast2client_pb", m_player[msg->chair_id].get_guid(), "SC_LockFish");
        PushNumToTable(g_LuaL, "chair_id", msg->chair_id);
        PushNumToTable(g_LuaL, "lock_id", 0);
        CallLuaFun(g_LuaL);
	}
	//�������һ�ο���ʱ��
	m_player[msg->chair_id].SetLastFireTick(timeGetTime());

	return true;

}
//���� ��Ҵ�������    �����Ǹı���ڣ�
void CTableFrameSink::OnCannonSetChange(CMyEvent* pEvent)
{
	if (pEvent == NULL || pEvent->GetName() != "CannonSetChanaged")
		return;

	CPlayer* pp = (CPlayer*)pEvent->GetParam();
	if (pp != NULL)
	{
		auto sz = m_player.size();
		for (size_t i = 0; i < sz; ++i)
		{
			if (&m_player[i] == pp)
			{
				SendCannonSet(i);
			}
		}
	}
}
//����
bool CTableFrameSink::OnNetCast(CS_stNetcast* msg)
{
	auto bullet_id = msg->bullet_id;      //�ӵ�ID
	auto fish_id = msg->fish_id;          //��ID

	m_BulletManager.Lock();
	//��ȡ�ӵ�
	CBullet* pBullet = (CBullet*)m_BulletManager.Find(bullet_id);
	if (pBullet != NULL)
	{
		//��ȡ�ӵ����������λ
		auto chair_id = pBullet->GetChairID();
		m_FishManager.Lock();
		CFish* pFish = (CFish*)m_FishManager.Find(fish_id);
		if (pFish != NULL)
		{
			CatchFish(pBullet, pFish, 1, 0);
		}
		else
		{
			// ������Ϊ�㲻���ڣ��������ǳԵ������һ���ӵ�������Ҽӵ���ҷ��������Ա�����˸����
			user_score_pools_[chair_id] += pBullet->GetScore();
		}
		m_FishManager.Unlock();

		//�����ӵ���ʧ
        {
            CAutoLock cl(&g_LuaLock);
            CreatLuaPackage(g_LuaL, "on_broadcast2client_pb", m_player[chair_id].get_guid(), "SC_KillBullet");
            PushNumToTable(g_LuaL, "chair_id", chair_id);              //����ID
            PushNumToTable(g_LuaL, "bullet_id", bullet_id);
            CallLuaFun(g_LuaL);
        }
		//����ӵ�-1
		m_player[chair_id].ADDBulletCount(-1);
		//�Ƴ��ӵ�
		m_BulletManager.Remove(bullet_id);
	}
	else
	{

		// TODO: ����ӵ������ڣ�Ҳ���ܵ���һЩ���⣺�����ײ�ӵ��İ����ӵ����ֵİ�����һ�������ˣ������������ 
	}
	m_BulletManager.Unlock();

	return true;
}
//�򿪱��� �޴���ֻ���ͣ� ����
void CTableFrameSink::OnCatchFishBroadCast(CMyEvent* pEvent)
{
	if (pEvent != NULL && pEvent->GetName() == "CatchFishBroadCast")
	{
		//IServerUserItem* pp = (IServerUserItem*)pEvent->GetSource();
		//��ȡ���
		CPlayer* pp = (CPlayer*)pEvent->GetSource();
		if (pp != NULL)
		{
			//for(WORD i = 0; i < GAME_PLAYER; ++i)
			{
				//if(m_pITableFrame->GetTableUserItem(i) == pp)
				{
					//m_pITableFrame->SendGameMessage(pp, (LPCTSTR)pEvent->GetParam(), SMT_TABLE_ROLL);

                    CAutoLock cl(&g_LuaLock);
                    CreatLuaPackage(g_LuaL, "on_Send2_pb", pp->get_guid(), "SC_SystemMessage");
                    PushNumToTable(g_LuaL, "wtype", SMT_TABLE_ROLL);
                    PushStrToTable(g_LuaL, "szstring", (char*)pEvent->GetParam());
                    CallLuaFun(g_LuaL);

					//break;
					//m_pITableFrame->SendRoomMessage((IServerUserItem*)pEvent->GetSource(), (LPCTSTR)pEvent->GetParam(), SMT_TABLE_ROLL);					
				}
			}
		}
	}
}
// �������� ���� ���ٹ������ز�
void CTableFrameSink::set_guid_gateid(int chair_id, int guid, int gate_id)
{
	if (chair_id >= 0 && chair_id < (int)m_player.size())
	{
		m_player[chair_id].set_guid_gateid(guid, gate_id);
		m_player[chair_id].set_chair_id(chair_id);
	}
	else
	{
		//LOG_WARN("chair_id %d error", chair_id);
	}
}
//�����ǳ�
void CTableFrameSink::set_nickname(int chair_id, const char* nickname)
{
	if (chair_id >= 0 && chair_id < (int)m_player.size())
	{
		m_player[chair_id].set_nickname(nickname);
	}
}

void CTableFrameSink::set_money(int chair_id, LONGLONG lvalue)
{
    if (chair_id >= 0 && chair_id < (int)m_player.size())
    {
        m_player[chair_id].SetScore(lvalue);
    }
}