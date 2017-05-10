#pragma once

#include "perinclude.h"
#include "Singleton.h"
#include "config_define.pb.h"

/**********************************************************************************************//**
 * \class	ConfigServerConfigManager
 *
 * \brief	Manager for db server configurations.
 **************************************************************************************************/

class ConfigServerConfigManager : public TSingleton<ConfigServerConfigManager>
{
public:

	/**********************************************************************************************//**
	 * \brief	Default constructor.
	 **************************************************************************************************/

	ConfigServerConfigManager();

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	~ConfigServerConfigManager();

	/**********************************************************************************************//**
	 * \brief	�õ������ļ�.
	 *
	 * \return	The configuration.
	 **************************************************************************************************/

	ConfigServer_Config& get_config() { return config_; }

	/**********************************************************************************************//**
	 * \brief	���������ļ�.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	bool load_config();
	
	/**********************************************************************************************//**
	 * \brief	���������ļ���.
	 *
	 * \return	null if it fails, else a pointer to a const char.
	 **************************************************************************************************/

	void set_cfg_file_name(const std::string& filename) { cfg_file_name_ = filename; }

private:
	bool load_file(const char* file, std::string& buf);

private:
	std::string										cfg_file_name_;
	ConfigServer_Config								config_;
};
