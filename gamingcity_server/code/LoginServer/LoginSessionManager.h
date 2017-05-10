#pragma once

#include "NetworkServer.h"
#include "NetworkDispatcher.h"
#include "Singleton.h"

class LoginSession;
class LoginDBSession;
class LOginSmsSession;

/**********************************************************************************************//**
 * \class	LoginSessionManager
 *
 * \brief	Manager for login sessions.
 **************************************************************************************************/

class LoginSessionManager : public NetworkAllocator, public TSingleton < LoginSessionManager >
{
public:

	/**********************************************************************************************//**
	 * \brief	Default constructor.
	 **************************************************************************************************/

	LoginSessionManager();

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	virtual ~LoginSessionManager();

	/**********************************************************************************************//**
	 * \brief	ע�������������Ϣ�Ĵ�����.
	 **************************************************************************************************/

	void register_connect_message();

	/**********************************************************************************************//**
	 * \brief	ע��GateServer���ӵ�LoginServer����Ϣ�Ĵ�����.
	 **************************************************************************************************/

	void register_gate2login_message();

	/**********************************************************************************************//**
	 * \brief	ע��GameServer���ӵ�LoginServer����Ϣ�Ĵ�����.
	 **************************************************************************************************/

	void register_game2login_message();

	/**********************************************************************************************//**
	 * \brief	ע��LoginServer���ӵ�DBServer����Ϣ�Ĵ�����.
	 **************************************************************************************************/

	void register_login2db_message();
	
	/**********************************************************************************************//**
	 * \brief	ע��GmServer���ӵ�LoginServer����Ϣ�Ĵ�����.
	 **************************************************************************************************/

	void register_web2login_message();

	/**********************************************************************************************//**
	 * \brief	ע��LoginServer���ӵ�SmsServer����Ϣ�Ĵ�����.
	 **************************************************************************************************/

	void register_login2sms_message();

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
	 * \brief	�õ�gate����login����Ϣ������.
	 *
	 * \return	null if it fails, else the dispatcher manager gate.
	 **************************************************************************************************/

	NetworkDispatcherManager* get_dispatcher_manager_gate() { return &dispatcher_manager_gate_; }

	/**********************************************************************************************//**
	 * \brief	�õ�game����login����Ϣ������.
	 *
	 * \return	null if it fails, else the dispatcher manager gate.
	 **************************************************************************************************/

	NetworkDispatcherManager* get_dispatcher_manager_game() { return &dispatcher_manager_game_; }

	/**********************************************************************************************//**
	 * \brief	�õ�login����db����Ϣ������.
	 *
	 * \return	null if it fails, else the dispatcher manager database.
	 **************************************************************************************************/

	NetworkDispatcherManager* get_dispatcher_manager_db() { return &dispatcher_manager_db_; }

	/**********************************************************************************************//**
	 * \brief	�õ�web����login����Ϣ������.
	 *
	 * \return	null if it fails, else the dispatcher manager web.
	 **************************************************************************************************/

	NetworkDispatcherManager* get_dispatcher_manager_web() { return &dispatcher_manager_web_; }

	/**********************************************************************************************//**
	 * \brief	�õ�gate����login��session.
	 *
	 * \param	server_id	Identifier for the server.
	 *
	 * \return	The gate session.
	 **************************************************************************************************/

	std::shared_ptr<NetworkSession> get_gate_session(int server_id);

	/**********************************************************************************************//**
	 * \brief	���gate����login��session.
	 *
	 * \param	session	The session.
	 **************************************************************************************************/

	void add_gate_session(std::shared_ptr<NetworkSession> session);

	/**********************************************************************************************//**
	 * \brief	ɾ��gate����login��session.
	 *
	 * \param	session	The session.
	 **************************************************************************************************/

	void del_gate_session(std::shared_ptr<NetworkSession> session);

	/**********************************************************************************************//**
	 * \brief	�õ�game����login��session.
	 *
	 * \param	server_id	Identifier for the server.
	 *
	 * \return	The game session.
	 **************************************************************************************************/

	std::shared_ptr<NetworkSession> get_game_session(int server_id);

	/**********************************************************************************************//**
	 * \brief	���game����login��session.
	 *
	 * \param	session	The session.
	 **************************************************************************************************/

	void add_game_session(std::shared_ptr<NetworkSession> session);

	/**********************************************************************************************//**
	 * \brief	ɾ��game����login��session.
	 *
	 * \param	session	The session.
	 **************************************************************************************************/

	void del_game_session(std::shared_ptr<NetworkSession> session);

	/**********************************************************************************************//**
	 * \brief	�õ�login����db��session.
	 *
	 * \return	The database session.
	 **************************************************************************************************/

	std::shared_ptr<NetworkSession> get_db_session();

	/**********************************************************************************************//**
	 * \brief	��db server������Ϣ.
	 *
	 * \tparam	T	Generic type parameter.
	 * \param [in,out]	pb	If non-null, the pb.
	 **************************************************************************************************/

	template<typename T> void send2db_pb(T* pb)
	{
		auto session = get_db_session();
		if (session)
		{
			session->send_pb(pb);
		}
		else
		{
			LOG_WARN("db server disconnect");
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
    /**********************************************************************************************//**
    * \brief	��gate������Ϣ.
    *
    * \tparam	T	Generic type parameter.
    * \param [in,out]	pb	If non-null, the pb.
    **************************************************************************************************/
    template<typename T> bool sendgate_All( T* pb)
    {
        for (auto item : gate_session_)
        {
            if (item)
            {
                return item->send_pb(pb);
            }
            else
            {
                LOG_WARN("gate server[%d] disconnect", item->get_server_id());
            }
        }
        return false;
    }
	/**********************************************************************************************//**
	 * \brief	��gate������Ϣ.
	 *
	 * \tparam	T	Generic type parameter.
	 * \param	gate_id   	������ĸ�gate server.
	 * \param [in,out]	pb	If non-null, the pb.
	 **************************************************************************************************/

	template<typename T> bool send2gate_pb(int gate_id, T* pb)
	{
		auto session = get_gate_session(gate_id);
		if (session)
		{
			return session->send_pb(pb);
		}
		else
		{
			LOG_WARN("gate server[%d] disconnect", gate_id);
		}
		return false;
	}

	/**********************************************************************************************//**
	 * \brief	��gate�㲥��Ϣ.
	 *
	 * \tparam	T	Generic type parameter.
	 * \param [in,out]	pb	If non-null, the pb.
	 **************************************************************************************************/

	template<typename T> int broadcast2gate_pb(T* pb)
	{
		for (auto session : gate_session_)
		{
			session->send_pb(pb);
		}
		return (int)gate_session_.size();
	}
	
	/**********************************************************************************************//**
	 * \brief	��game������Ϣ.
	 *
	 * \tparam	T	Generic type parameter.
	 * \param	guid	  	Unique identifier.
	 * \param	game_id   	������ĸ�game server.
	 * \param [in,out]	pb	If non-null, the pb.
	 **************************************************************************************************/

	template<typename T> void send2game_pb(int game_id, T* pb)
	{
		auto session = get_game_session(game_id);
		if (session)
		{
			session->send_pb(pb);
		}
		else
		{
			LOG_WARN("game server[%d] disconnect", game_id);
		}
	}

	/**********************************************************************************************//**
	 * \brief	��game�㲥��Ϣ.
	 *
	 * \tparam	T	Generic type parameter.
	 * \param [in,out]	pb	If non-null, the pb.
	 **************************************************************************************************/

	template<typename T> int broadcast2game_pb(T* pb)
	{
		for (auto session : game_session_)
		{
			session->send_pb(pb);
		}
		return (int)game_session_.size();
	}

	/**********************************************************************************************//**
	 * \brief	��web������Ϣ.
	 *
	 * \tparam	T	Generic type parameter.
	 * \param [in,out]	pb	If non-null, the pb.
	 **************************************************************************************************/

	template<typename T> void send2web_pb(int id, T* pb)
	{
		auto session = find_by_id(id);
		if (session)
		{
			session->send_pb(pb);
		}
		else
		{
			LOG_WARN("web server[%d] disconnect", id);
		}
	}

	/**********************************************************************************************//**
	 * \brief	���һ������������Ϣ.
	 **************************************************************************************************/

	void add_game_server_info(int game_id, int first_game_type, int second_game_type, bool default_lobby, int player_limit);

	/**********************************************************************************************//**
	 * \brief	ɾ��һ������������Ϣ.
	 **************************************************************************************************/

	void remove_game_server_info(int game_id);

	/**********************************************************************************************//**
	 * \brief	�ж��Ƿ���һ������������Ϣ.
	 **************************************************************************************************/

	bool has_game_server_info(int game_id);

	/**********************************************************************************************//**
	 * \brief	���·����������������Ϣ.
	 **************************************************************************************************/

	void update_game_server_player_count(int game_id, int count);

	/**********************************************************************************************//**
	 * \brief	����һ��Ĭ�ϴ�����game_id.
	 **************************************************************************************************/

	int find_a_default_lobby();
	
	/**********************************************************************************************//**
	 * \brief	��ӡ��Ϸ����.
	 **************************************************************************************************/

	void print_game_server_info();

	/**********************************************************************************************//**
	 * \brief	����һ����ط����game_id.
	 **************************************************************************************************/

	int find_a_game_id(int first_game_type, int second_game_type);

	/**********************************************************************************************//**
	 * \brief	��gate server���Ϳ����б���Ϣ.
	 **************************************************************************************************/

	//void send_open_game_list(std::shared_ptr<NetworkSession> session);
	
	/**********************************************************************************************//**
	 * \brief	��gate server�㲥�����б���Ϣ.
	 **************************************************************************************************/

	//void broadcast_open_game_list();

	// �������ӵ�db
	void set_first_connect_db();

	// �Ƿ�������db
	bool is_first_connect_db();

	// ��һ��������db������
	virtual void on_first_connect_db();

	void Add_DB_Server_Session(const std::string& ip, int port);
protected:
	NetworkDispatcherManager			dispatcher_manager_;
	NetworkDispatcherManager			dispatcher_manager_gate_;
	NetworkDispatcherManager			dispatcher_manager_game_;
	NetworkDispatcherManager			dispatcher_manager_db_;
	NetworkDispatcherManager			dispatcher_manager_web_;

	std::vector<std::shared_ptr<NetworkSession>> gate_session_;
	std::vector<std::shared_ptr<NetworkSession>> game_session_;
	std::vector<std::shared_ptr<NetworkSession>> db_session_;

	size_t								cur_db_session_;

	// ��һ��������db
	int									first_connect_db_;

	struct RegGameServerInfo
	{
		int first_game_type;			// һ���˵�
		int second_game_type; 			// �����˵������������ļ�
		bool default_lobby; 			// �Ƿ���Ĭ�ϴ���
		int player_limit; 				// �����������
		int cur_player_count;			// ��ǰ�������
	};
	std::map<int, RegGameServerInfo>	reg_game_server_info_;
	std::recursive_mutex				mutex_reg_game_server_info_;
};
