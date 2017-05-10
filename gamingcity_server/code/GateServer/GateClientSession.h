#pragma once

#include "NetworkSession.h"
#include "NetworkDispatcher.h"
#include "common_msg_define.pb.h"
#include "GateSessionManager.h"
#include "GateServerConfigManager.h"
#include "GameLog.h"

/**********************************************************************************************//**
 * \class	GateClientSession
 *
 * \brief	client����gate��session.
 **************************************************************************************************/

class GateClientSession : public NetworkSession
{
public:

	/**********************************************************************************************//**
	 * \brief	Constructor.
	 *
	 * \param [in,out]	sock	The sock.
	 **************************************************************************************************/

	GateClientSession(boost::asio::ip::tcp::socket& sock);

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	virtual ~GateClientSession();

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
	 * \brief	�õ�guid.
	 *
	 * \return	The unique identifier.
	 **************************************************************************************************/

	int get_guid() { return guid_; }

	/**********************************************************************************************//**
	 * \brief	����guid.
	 *
	 * \param	guid	Unique identifier.
	 **************************************************************************************************/

	void set_guid(int guid) {
        LOG_WARN("set guid old[%d] new[%d]", guid_, guid);
        guid_ = guid; 
    }

	/**********************************************************************************************//**
	 * \brief	�õ���ǰ������ĸ���Ϸ������.
	 *
	 * \return	The game server identifier.
	 **************************************************************************************************/

	int get_game_server_id() { return game_server_id_; }

	/**********************************************************************************************//**
	 * \brief	���õ�ǰ������ĸ���Ϸ������.
	 *
	 * \param	server_id	Identifier for the server.
	 **************************************************************************************************/

	void set_game_server_id(int server_id) { game_server_id_ = server_id; }

	virtual bool tick();

	// �����˺�
	void set_account(const std::string& account) { account_ = account; }

	std::string get_account() { return account_; }

	void set_user_data(int user_data) { user_data_ = user_data; }
	int get_user_data() { return user_data_; }

	void set_login(bool iflag) {
		LOG_WARN("set m_login [%d] guid[%d]", iflag, guid_);
		LOG_WARN("ip[%s] port[%d]", ip_.c_str(), port_);
        LOG_WARN("this address [%d]", this);
		m_islogin = iflag;
	}
	bool get_login() { return m_islogin; }

	// ���ö�����֤��
	void set_sms(const std::string& tel, const std::string& sms_no);
	void clear_sms();

	void reset_is_send_login() { is_send_login_ = false; }
public:
	void do_get_sms_http(const std::string& phone);
private:
	bool on_C_RequestPublicKey(MsgHeader* header);
	bool on_CL_RegAccount(MsgHeader* header);
	bool on_CL_Login(MsgHeader* header);
	bool on_CL_LoginBySms(MsgHeader* header);
	bool on_CS_RequestSms(MsgHeader* header);
	bool on_CG_GameServerCfg(MsgHeader* header);
	bool on_CS_ResetAccount(MsgHeader* header);
	bool on_CS_SetNickname(MsgHeader* header);
	bool on_CS_SetPassword(MsgHeader* header);
	bool on_CS_SetPasswordBySms(MsgHeader* header);
	bool on_CS_BankSetPassword(MsgHeader* header);
	bool on_CS_BankChangePassword(MsgHeader* header);
	bool on_CS_BankLogin(MsgHeader* header);
	bool on_CL_GetInviterInfo(MsgHeader* header);
	

	bool check_string(const std::string& str);

private:
	std::string							ip_;
	unsigned short						port_;

	int									guid_;

	int									game_server_id_;
	int									user_data_; // �ظ�login

	std::string							private_key_;
	time_t								timeout_limit_;
	//time_t								last_msg_time_;

	std::string							account_; // �˺�����
	bool                                m_islogin;

	std::string							tel_; // �ֻ���
	std::string							sms_no_; // �ֻ���֤��
	time_t								last_sms_time_; // �ϴ�����ʱ��
	time_t								sms_time_limit_;

	bool								is_send_login_;				// �Ƿ��͵�½��Ϣ
};
