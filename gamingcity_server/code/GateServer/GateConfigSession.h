#pragma once

#include "NetworkConnectSession.h"
#include "NetworkDispatcher.h"
#include "common_msg_define.pb.h"
#include "msg_server.pb.h"

/**********************************************************************************************//**
 * \class	GateConfigSession
 *
 * \brief	game����config��session.
 **************************************************************************************************/

class GateConfigSession : public NetworkConnectSession
{
public:

	/**********************************************************************************************//**
	 * \brief	Constructor.
	 *
	 * \param [in,out]	ioservice	The ioservice.
	 **************************************************************************************************/

	GateConfigSession(boost::asio::io_service& ioservice);

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	virtual ~GateConfigSession();

	/**********************************************************************************************//**
	 * \brief	�����յ�����.
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
	* \brief	config��������
	**************************************************************************************************/

	void on_S_ReplyServerConfig(S_ReplyServerConfig* msg);

	void on_S_NotifyGameServerStart(S_NotifyGameServerStart* msg);

	void on_S_ReplyUpdateGameServerConfig(S_ReplyUpdateGameServerConfig* msg);


	void on_S_NotifyLoginServerStart(S_NotifyLoginServerStart* msg);

	void on_S_ReplyUpdateLoginServerConfigByGate(S_ReplyUpdateLoginServerConfigByGate* msg);

    void on_FG_GameServerCfg(FG_GameServerCfg * msg);

    void on_FS_ChangMoneyDeal(FS_ChangMoneyDeal * msg);

private:
	NetworkDispatcherManager*			dispatcher_manager_;
};
