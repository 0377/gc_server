#include "DBServerConfigManager.h"
#include "GameLog.h"
#include <google/protobuf/text_format.h>

DBServerConfigManager::DBServerConfigManager()
{
	cfg_file_name_ = "../config/DBServerConfig.pb";
}

DBServerConfigManager::~DBServerConfigManager()
{

}

bool DBServerConfigManager::load_file(const char* file, std::string& buf)
{
	std::ifstream ifs(file, std::ifstream::in);
	if (!ifs.is_open())
	{
		LOG_ERR("load %s failed", file);
		return false;
	}

	buf = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
	if (ifs.bad())
	{
		LOG_ERR("load %s failed", file);
		return false;
	}

	return true;
}

bool DBServerConfigManager::load_config()
{
	std::string buf;
	if (!load_file(cfg_file_name_.c_str(), buf))
		return false;

	if (!google::protobuf::TextFormat::ParseFromString(buf, &config_))
	{
		LOG_ERR("parse %s failed", cfg_file_name_.c_str());
		return false;
	}

	LOG_INFO("load_config ok......");
	return true;
}

std::string DBServerConfigManager::get_title()
{
	auto pos1 = cfg_file_name_.find_last_of('/');
	if (pos1 != std::string::npos)
		pos1 += 1;
	else
		pos1 = 0;

	auto pos2 = cfg_file_name_.find_last_of('.');
	if (pos2 != std::string::npos)
		pos2 -= pos1;
	else
		pos2 = cfg_file_name_.size() - pos1;

	return cfg_file_name_.substr(pos1, pos2);
}
