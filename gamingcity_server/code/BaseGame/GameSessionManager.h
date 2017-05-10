#pragma once

#include "NetworkServer.h"
#include "NetworkDispatcher.h"
#include "Singleton.h"

class GameSession;
class GameDBSession;

/**********************************************************************************************//**
 * \class	GameSessionManager
 *
 * \brief	Manager for game sessions.
 **************************************************************************************************/

class GameSessionManager : public NetworkAllocator, public TSingleton < GameSessionManager >
{
public:

	/**********************************************************************************************//**
	 * \brief	Default constructor.
	 **************************************************************************************************/

	GameSessionManager();

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	virtual ~GameSessionManager();

	/**********************************************************************************************//**
	 * \brief	ע�������������Ϣ�Ĵ�����.
	 **************************************************************************************************/

	void register_connect_message();
	
	/**********************************************************************************************//**
	 * \brief	ע��GameServer���ӵ�LoginServer����Ϣ�Ĵ�����.
	 **************************************************************************************************/

	void register_game2login_message();

	/**********************************************************************************************//**
	 * \brief	ע��GateServer���ӵ�GameServer����Ϣ�Ĵ�����.
	 **************************************************************************************************/

	void register_gate2game_message();

	/**********************************************************************************************//**
	 * \brief	ע��GameServer���ӵ�DBServer����Ϣ�Ĵ�����.
	 **************************************************************************************************/

	void register_game2db_message();

	/**********************************************************************************************//**
	 * \brief	�ر�����session.
	 **************************************************************************************************/

	virtual void close_all_session();

	/**********************************************************************************************//**
	 * \brief	�ͷ�����session��Դ.
	 **************************************************************************************************/

	virtual void release_all_session();

	/**********************************************************************************************//**
	 * \brief	ÿһ֡����.
	 **************************************************************************************************/

	virtual bool tick();

	/**********************************************************************************************//**
	 * \brief	����sesssion.
	 *
	 * \param [in,out]	socket	The socket.
	 *
	 * \return	The new session.
	 **************************************************************************************************/

	virtual std::shared_ptr<NetworkSession> create_session(boost::asio::ip::tcp::socket& socket);
	
	/**********************************************************************************************//**
	 * \brief	����login server session.
	 *
	 * \param	ip  	The IP.
	 * \param	port	The port.
	 *
	 * \return	The new login session.
	 **************************************************************************************************/

	virtual std::shared_ptr<NetworkSession> create_login_session(const std::string& ip, unsigned short port);

	/**********************************************************************************************//**
	 * \brief	����database server session.
	 *
	 * \param	ip  	The IP.
	 * \param	port	The port.
	 *
	 * \return	The new database session.
	 **************************************************************************************************/

	virtual std::shared_ptr<NetworkSession> create_db_session(const std::string& ip, unsigned short port);

	/**********************************************************************************************//**
	 * \brief	�������������.
	 *
	 * \param [in,out]	network_server	If non-null, the network server.
	 **************************************************************************************************/

	virtual void set_network_server(NetworkServer* network_server);

	/**********************************************************************************************//**
	 * \brief	�õ�����S_Connect����Ϣ������.
	 *
	 * \return	null if it fails, else the dispatcher manager.
	 **************************************************************************************************/

	NetworkDispatcherManager* get_dispatcher_manager() { return &dispatcher_manager_; }
	
	/**********************************************************************************************//**
	 * \brief	�õ�game����login����Ϣ������.
	 *
	 * \return	null if it fails, else the dispatcher manager gate.
	 **************************************************************************************************/

	NetworkDispatcherManager* get_dispatcher_manager_login() { return &dispatcher_manager_login_; }

	/**********************************************************************************************//**
	 * \brief	�õ�gate����game����Ϣ������.
	 *
	 * \return	null if it fails, else the dispatcher manager gate.
	 **************************************************************************************************/

	NetworkDispatcherManager* get_dispatcher_manager_gate() { return &dispatcher_manager_gate_; }

	/**********************************************************************************************//**
	 * \brief	�õ�game����db����Ϣ������.
	 *
	 * \return	null if it fails, else the dispatcher manager database.
	 **************************************************************************************************/

	NetworkDispatcherManager* get_dispatcher_manager_db() { return &dispatcher_manager_db_; }

	/**********************************************************************************************//**
	 * \brief	�õ�game����db��session.
	 *
	 * \return	The database session.
	 **************************************************************************************************/

	std::shared_ptr<NetworkSession> get_db_session();

	/**********************************************************************************************//**
	 * \brief	�õ�game����login��session.
	 *
	 * \return	The login session.
	 **************************************************************************************************/

	std::shared_ptr<NetworkSession> get_login_session();

    std::shared_ptr<NetworkSession> get_login_session(int login_id);
	/**********************************************************************************************//**
	 * \brief	���game����login��session.
	 *
	 * \param	session	The session.
	 **************************************************************************************************/

	void add_login_session(std::shared_ptr<NetworkSession> session);

	/**********************************************************************************************//**
	 * \brief	ɾ��game����login��session.
	 *
	 * \param	session	The session.
	 **************************************************************************************************/

	void del_login_session(std::shared_ptr<NetworkSession> session);

	/**********************************************************************************************//**
	 * \brief	�õ�gate����game��session.
	 *
	 * \param	server_id	Identifier for the server.
	 *
	 * \return	The gate session.
	 **************************************************************************************************/

	std::shared_ptr<NetworkSession> get_gate_session(int server_id);

	/**********************************************************************************************//**
	 * \brief	���gate����game��session.
	 *
	 * \param	session	The session.
	 **************************************************************************************************/

	void add_gate_session(std::shared_ptr<NetworkSession> session);

	/**********************************************************************************************//**
	 * \brief	ɾ��gate����game��session.
	 *
	 * \param	session	The session.
	 **************************************************************************************************/

	void del_gate_session(std::shared_ptr<NetworkSession> session);

	/**********************************************************************************************//**
	 * \brief	��db server������Ϣ.
	 *
	 * \tparam	T	Generic type parameter.
	 * \param [in,out]	pb	If non-null, the pb.
	 **************************************************************************************************/

	/*template<typename T> void send2db_pb(T* pb)
	{
		if (db_session_)
		{
			db_session_->send_pb(pb);
		}
		else
		{
			LOG_WARN("db server disconnect");
		}
	}*/

	/**********************************************************************************************//**
	 * \brief	��login������Ϣ.
	 *
	 * \tparam	T	Generic type parameter.
	 * \param [in,out]	pb	If non-null, the pb.
	 **************************************************************************************************/

	template<typename T> void send2login_pb(T* pb)
	{
		auto session = get_login_session();
		if (session)
		{
			session->send_pb(pb);
		}
		else
		{
			LOG_WARN("login server disconnect");
		}
	}
	
	/**********************************************************************************************//**
	 * \brief	��client������Ϣ.
	 *
	 * \tparam	T	Generic type parameter.
	 * \param	guid	  	Unique identifier.
	 * \param	gate_id   	������ĸ�gate server.
	 * \param [in,out]	pb	If non-null, the pb.
	 **************************************************************************************************/

	template<typename T> void send2client_pb(int guid, int gate_id, T* pb)
	{
		auto session = get_gate_session(gate_id);
		if (session)
		{
			session->send_xc_pb(guid, pb);
		}
		else
		{
			LOG_WARN("gate server[%d] disconnect", gate_id);
		}
	}

	// ��login�㲥��ǰ�������
	void broadcast_player_count(int count);

	// �������ӵ�db
	void set_first_connect_db();

	// ��һ��������db������
	virtual void on_first_connect_db();

	void Add_Login_Server_Session(const std::string& ip, int port);
	void Add_DB_Server_Session(const std::string& ip, int port);
protected:
	NetworkDispatcherManager			dispatcher_manager_;
	NetworkDispatcherManager			dispatcher_manager_login_;
	NetworkDispatcherManager			dispatcher_manager_gate_;
	NetworkDispatcherManager			dispatcher_manager_db_;

	std::vector<std::shared_ptr<NetworkSession>> login_session_;
	std::vector<std::shared_ptr<NetworkSession>> db_session_;
	std::vector<std::shared_ptr<NetworkSession>> gate_session_;

	size_t								cur_login_session_;
	size_t								cur_db_session_;

	// ��һ��������db
	int									first_connect_db_;
};
