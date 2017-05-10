#pragma once

#include "perinclude.h"
#include "Singleton.h"
#include "config_define.pb.h"
#include "common_msg_define.pb.h"
#include "msg_server.pb.h"

/**********************************************************************************************//**
 * \class	GateServerConfigManager
 *
 * \brief	Manager for gate server configurations.
 **************************************************************************************************/
#if 0
class GateServerConfigManager : public TSingleton<GateServerConfigManager>
{
public:

	/**********************************************************************************************//**
	 * \brief	Default constructor.
	 **************************************************************************************************/

	GateServerConfigManager();

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	~GateServerConfigManager();

	/**********************************************************************************************//**
	 * \brief	�õ������ļ�.
	 *
	 * \return	The configuration.
	 **************************************************************************************************/

	GateServerConfig& get_config() { return config_; }

	/**********************************************************************************************//**
	 * \brief	���������ļ�.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	bool load_config();

	
	/**********************************************************************************************//**
	 * \brief	�õ�game server�����ļ�.
	 *
	 * \return	The configuration.
	 **************************************************************************************************/

	GameServerCfg& get_gameserver_config() { return gameserver_cfg_; }

	/**********************************************************************************************//**
	 * \brief	����game server�����ļ�.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	bool load_gameserver_config();

	void load_gameserver_config_db(const std::vector<std::vector<std::string>>& data);

    void load_gameserver_config_pb(DL_ServerConfig & cfg);

    void load_gameserver_config_pb(LG_DBGameConfigMgr & cfg);
	/**********************************************************************************************//**
	 * \brief	���������ļ���.
	 *
	 * \return	null if it fails, else a pointer to a const char.
	 **************************************************************************************************/

	void set_cfg_file_name(const std::string& filename) { cfg_file_name_ = filename; }

	/**********************************************************************************************//**
	 * \brief	�õ������ļ������ڿ���̨����.
	 *
	 * \return	The title.
	 **************************************************************************************************/

	std::string get_title();

	/**********************************************************************************************//**
	 * \brief	��������GamerConfig.
	 *
	 * \return	The title.
	 **************************************************************************************************/
    void db_cfg_to_gamserver();
private:
	bool load_file(const char* file, std::string& buf);

private:
	std::string											cfg_file_name_;
	GateServerConfig									config_;
	GameServerCfg										gameserver_cfg_;
    DBGameConfigMgr                                     dbgamer_config;
};
#endif
