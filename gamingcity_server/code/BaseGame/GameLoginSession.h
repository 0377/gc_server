#pragma once

#include "NetworkConnectSession.h"
#include "NetworkDispatcher.h"
#include "common_msg_define.pb.h"
#include "msg_server.pb.h"
#include "GameSessionManager.h"

/**********************************************************************************************//**
 * \class	GameLoginSession
 *
 * \brief	game����login��session.
 **************************************************************************************************/

class GameLoginSession : public NetworkConnectSession
{
public:

	/**********************************************************************************************//**
	 * \brief	Constructor.
	 *
	 * \param [in,out]	ioservice	The ioservice.
	 **************************************************************************************************/

	GameLoginSession(boost::asio::io_service& ioservice);

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	virtual ~GameLoginSession();

	/**********************************************************************************************//**
	 * \brief	�����յ�����Ϣ.
	 *
	 * \param [in,out]	header	If non-null, the header.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	virtual bool on_dispatch(MsgHeader* header);

	/**********************************************************************************************//**
	 * \brief	�������ӻص�.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	virtual bool on_connect();

	/**********************************************************************************************//**
	 * \brief	��������ʧ�ܻص�.
	 **************************************************************************************************/

	virtual void on_connect_failed();

	/**********************************************************************************************//**
	 * \brief	�ر�socketǰ�ص�.
	 **************************************************************************************************/

	virtual void on_closed();

public:
	
	/**********************************************************************************************//**
	 * \brief	�յ�һ����web:�����������Ϣ����Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_wl_request_game_server_info(WL_RequestGameServerInfo* msg);

	/**********************************************************************************************//**
	* \brief	�յ�һ����php����ͨ��gm�����Ǯ����Ϣ�Ĵ�����.
	*
	* \param [in,out]	msg	If non-null, the message.
	**************************************************************************************************/
	void on_wl_request_php_gm_cmd_change_money(LS_ChangeMoney * msg);
	void on_wl_broadcast_gameserver_gmcommand(WL_BroadcastClientUpdate * msg);

	void on_wl_request_LS_LuaCmdPlayerResult(LS_LuaCmdPlayerResult* msg);
private:
	NetworkDispatcherManager*			dispatcher_manager_;
};
