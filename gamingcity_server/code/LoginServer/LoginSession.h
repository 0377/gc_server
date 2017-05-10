#pragma once

#include "NetworkSession.h"
#include "NetworkDispatcher.h"
#include "common_msg_define.pb.h"
#include "msg_server.pb.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

/**********************************************************************************************//**
 * \class	LoginSession
 *
 * \brief	gate��game����login��session.
 **************************************************************************************************/

#include "stdarg.h" 
#define endStr "JudgeParamEnd"
#define checkJsonMember(ABC,...)  LoginSession::checkJsonMemberT(ABC,1,__VA_ARGS__,endStr)

struct stCostBankMoeny
{
	std::string m_data;
	std::function<void(int  retCode, int oldmoeny, int newmoney, std::string)> func;
};
struct stDoSql
{
	std::string m_data;
	std::function<void(int  retCode, std::string retData, std::string stData)> func;
};
class LoginSession : public NetworkSession
{
public:

	/**********************************************************************************************//**
	 * \brief	Constructor.
	 *
	 * \param [in,out]	sock	The sock.
	 **************************************************************************************************/

	LoginSession(boost::asio::ip::tcp::socket& sock);

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	virtual ~LoginSession();

	/**********************************************************************************************//**
	 * \brief	�����յ�����Ϣ.
	 *
	 * \param [in,out]	header	If non-null, the header.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	virtual bool on_dispatch(MsgHeader* header);

	/**********************************************************************************************//**
	 * \brief	������ܻص�.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	virtual bool on_accept();

	/**********************************************************************************************//**
	 * \brief	�ر�socketǰ�ص�.
	 **************************************************************************************************/

	virtual void on_closed();

	/**********************************************************************************************//**
	 * \brief	�õ�������id.
	 *
	 * \return	The server identifier.
	 **************************************************************************************************/

	virtual int get_server_id() { return server_id_; }

	/**********************************************************************************************//**
	 * \brief	���÷�����id.
	 *
	 * \param	server_id	Identifier for the server.
	 **************************************************************************************************/

	void set_server_id(int server_id) { server_id_ = server_id; }

public:

	/**********************************************************************************************//**
	 * \brief	�յ�һ�������������ӡ���Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_s_connect(S_Connect* msg);

	/**********************************************************************************************//**
	 * \brief	�յ�һ����������Ϸ��������������Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_S_UpdateGamePlayerCount(S_UpdateGamePlayerCount* msg);

	/**********************************************************************************************//**
	 * \brief	�յ�һ��������˳�����Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_s_logout(S_Logout* msg);

	/**********************************************************************************************//**
	 * \brief	�յ�һ�����ͻ��˵�¼����Ϣ�Ĵ�����.
	 *
	 * \param	session_id 	Identifier for the session.
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_cl_login(int session_id, CL_Login* msg);

	/**********************************************************************************************//**
	 * \brief	�յ�һ����ע���˺š���Ϣ�Ĵ�����.
	 *
	 * \param	session_id 	Identifier for the session.
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_cl_reg_account(int session_id, CL_RegAccount* msg);
	
	/**********************************************************************************************//**
	 * \brief	�յ�һ�����ö�����֤���½����Ϣ�Ĵ�����.
	 *
	 * \param	session_id 	Identifier for the session.
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_cl_login_by_sms(int session_id, CL_LoginBySms* msg);
	
	/**********************************************************************************************//**
	 * \brief	�յ�һ�����ظ�login���ˡ���Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_L_KickClient(L_KickClient* msg);

	/**********************************************************************************************//**
	 * \brief	�յ�һ�����л���Ϸ����������Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_ss_change_game(SS_ChangeGame* msg);
	void on_SL_ChangeGameResult(SL_ChangeGameResult* msg);

	/**********************************************************************************************//**
	 * \brief	�յ�һ�������������֤����Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_cs_request_sms(CS_RequestSms* msg);

	/**********************************************************************************************//**
	 * \brief	�յ�һ����ת�ˡ���Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_sd_bank_transfer(SD_BankTransfer* msg);
	
	/**********************************************************************************************//**
	 * \brief	�յ�һ����ͨ��guidת�ˡ���Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_sd_bank_transfer_by_guid(S_BankTransferByGuid* msg);

	/**********************************************************************************************//**
	 * \brief	�յ�һ����ת�ˡ���Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_cs_chat_world(int session_id, CS_ChatWorld* msg);

	/**********************************************************************************************//**
	 * \brief	�յ�һ����˽�ġ���Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_sc_chat_private(SC_ChatPrivate* msg);

	/**********************************************************************************************//**
	 * \brief	�յ�һ����web:�����������Ϣ����Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_wl_request_game_server_info(WL_RequestGameServerInfo* msg);

	/**********************************************************************************************//**
	 * \brief	�յ�һ����web:���ط�������Ϣ����Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_sl_web_game_server_info(SL_WebGameServerInfo* msg);

    /**********************************************************************************************//**
    * \brief	�յ�һ��GmCommand��Ϣ�Ĵ�����.
    *
    * \param [in,out]	msg	If non-null, the message.
    **************************************************************************************************/
    void on_wl_request_GMMessage(WL_GMMessage * msg);

    /**********************************************************************************************//**
    * \brief	�ж��û��Ƿ�����.
    *
    * \param [in,out]	msg	If non-null, the message.
    **************************************************************************************************/
    static void player_is_online(int guid, const std::function<void( int  gateid,  int sessionid, std::string)>& func);
    
    /**********************************************************************************************//**
    * \brief	gate������Ϣ��ɷ���.
    *
    * \param [in,out]	msg	If non-null, the message.
    **************************************************************************************************/
    void on_gl_NewNotice(GL_NewNotice * msg);

    /**********************************************************************************************//**
    * \brief	������Ϣ��ɷ���.
    *
    * \param [in,out]	msg	If non-null, the message.
    **************************************************************************************************/
    static void Ret_GMMessage(int retCode, int retID);

    /**********************************************************************************************//**
    * \brief	��������Ϣ.
    *
    * \param [in,out]	msg	If non-null, the message.
    **************************************************************************************************/
    void UpdateFeedBack(rapidjson::Document &document);
    static bool checkJsonMemberT(rapidjson::Document &document, int start, ...);
	
    /**********************************************************************************************//**
    * \brief	����ʧ�������ݿⷢ�Ͳ�ѯ����.
    *
    * \param [in,out]	msg	If non-null, the message.
    **************************************************************************************************/
 //   void on_wl_request_cash_false(WL_CashFalse* msg);
    
    /**********************************************************************************************//**
    * \brief	����ʧ�ܽ�����Ϸ������������.
    *
    * \param [in,out]	msg	If non-null, the message.
    **************************************************************************************************/
    void on_sl_cash_false_reply(SL_CashReply* msg);

    ///**********************************************************************************************//**
    //* \brief	��ֵ�����ݿⷢ�Ͳ�ѯ����.
    //*
    //* \param [in,out]	msg	If non-null, the message.
    //**************************************************************************************************/
    //void on_wl_request_recharge(WL_Recharge* msg);
    ///**********************************************************************************************//**
    //* \brief	��ֵ������Ϸ������������.
    //*
    //* \param [in,out]	msg	If non-null, the message.
    //**************************************************************************************************/
    //void on_sl_recharge_reply(SL_RechargeReply* msg);
    
    /**********************************************************************************************//**
    * \brief	�������˰�ʸı�.
    *
    * \param [in,out]	msg	If non-null, the message.
    **************************************************************************************************/
    void on_wl_request_change_tax(WL_ChangeTax* msg);
    
    /**********************************************************************************************//**
    * \brief	����˰�ʸı䴦����.
    *
    * \param [in,out]	msg	If non-null, the message.
    **************************************************************************************************/
    void on_sl_change_tax_reply(SL_ChangeTax* msg); 

	/**********************************************************************************************//**
	* \brief	����phpͨ��gm�����Ǯ����
	*
	* \param [in,out]	msg	If non-null, the message.
	**************************************************************************************************/
	void on_wl_request_gm_change_money(WL_ChangeMoney *msg);

	void on_WL_LuaCmdPlayerResult(WL_LuaCmdPlayerResult* msg);
	void on_SL_LuaCmdPlayerResult(SL_LuaCmdPlayerResult* msg);
    
	/**********************************************************************************************//**
	* \brief	����Game��Ǯʧ�ܺ� ���˴���
	*
	* \param [in,out]	msg	If non-null, the message.
	**************************************************************************************************/
    void on_SL_AddMoney(SL_AddMoney* msg);
	/**********************************************************************************************//**
	* \brief	Gate��������ȡ����
	*
	* \param [in,out]	msg	If non-null, the message.
	**************************************************************************************************/
    void on_gl_get_server_cfg(int session_id, GL_GetServerCfg* msg);
	//��ȡ��������Ϣ
	void on_cl_get_server_cfg(int session_id, CL_GetInviterInfo* msg);

	//loginServer֪ͨ���еķ������㲥�ͻ���
	void on_wl_broadcast_gameserver_cmd(WL_BroadcastClientUpdate *msg);

	/**********************************************************************************************//**
	* \brief	�ۼ����������Ľ�Ǯ.
	*
	* \param [in,out]	msg	If non-null, the message.
	**************************************************************************************************/
	bool cost_player_bank_money(std::string keyid, int guid, int money, std::string strData, std::function<void(int  retCode, int oldmoeny, int newmoney, std::string)> func);
	void create_do_Sql(std::string  keyid, std::string database, std::string strSql, std::string strData, std::function<void(int  retCode, std::string retData, std::string stData)>);
	void on_SL_AT_ChangeMoney(SL_CC_ChangeMoney* msg);
	void on_sl_FreezeAccount(SL_FreezeAccount * msg);
	void on_DB_Request(DL_CC_ChangeMoney * msg);
	void on_do_SqlReQuest(DL_DO_SQL * msg);
	void on_AT_PL_ChangeMoney(AgentsTransferData stData);
	//�޸�֧������Ϣ
	void EditAliPay(rapidjson::Document &document);
private:
	std::map<std::string, stCostBankMoeny > m_mapCostBankFunc;
	std::map<std::string, stDoSql > m_mapDoSql;
	NetworkDispatcherManager*			dispatcher_manager_;
	std::string							ip_;
	unsigned short						port_;

	int									type_;
	int									server_id_;
};