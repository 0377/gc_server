#pragma once

#include "NetworkSession.h"
#include <atomic>

/**********************************************************************************************//**
 * \class	NetworkConnectSession
 *
 * \brief	A network connect session.
 **************************************************************************************************/

class NetworkConnectSession : public NetworkSession
{
public:

	/**********************************************************************************************//**
	 * \brief	Constructor.
	 *
	 * \param [in,out]	ioservice	The ioservice.
	 **************************************************************************************************/

	NetworkConnectSession(boost::asio::io_service& ioservice);

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	virtual ~NetworkConnectSession();

	/**********************************************************************************************//**
	 * \brief	����.
	 *
	 * \param	ip  	The IP.
	 * \param	port	The port.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	virtual bool connect(const char* ip, unsigned short port);

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
	 * \brief	����ر�socketǰ�ص�.
	 **************************************************************************************************/

	virtual void on_closed();

	/**********************************************************************************************//**
	 * \brief	ÿһ֡����.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	virtual bool tick();

	/**********************************************************************************************//**
	 * \brief	Sets IP port.
	 *
	 * \param	ip  	The IP.
	 * \param	port	The port.
	 **************************************************************************************************/

	void set_ip_port(const std::string& ip, unsigned short port);

	bool is_connected() { return connect_state_ == CONNECT_STATE_CONNECTED; }

	int get_connect_state() { return connect_state_; }
protected:
	void connect_impl(boost::asio::ip::tcp::resolver::iterator it);

protected:
	boost::asio::ip::tcp::resolver		resolver_;
	enum CONNECT_STATE
	{
		CONNECT_STATE_INVALID,
		CONNECT_STATE_DISCONNECT,
		CONNECT_STATE_CONNECTING,
		CONNECT_STATE_CONNECTED,
	};
	std::atomic<int32_t>				connect_state_;
	long long							wait_tick_;

	std::string							ip_;
	unsigned short						port_;

	// ������ʱ
	time_t								last_heartbeat_;
};
