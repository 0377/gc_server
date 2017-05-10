#include "TestPerfManager.h"

TestPerfManager::TestPerfManager()
	: is_run_(true)
{
	work_.reset(new boost::asio::io_service::work(ioservice_));
}

TestPerfManager::~TestPerfManager()
{
	release();
}

BOOL WINAPI CtrlHandler2(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		if (TestPerfManager::instance())
			TestPerfManager::instance()->stop();
		return TRUE;
	}
	return FALSE;
}

void TestPerfManager::startup()
{
	if (!init())
	{
		return;
	}

	SetConsoleCtrlHandler(CtrlHandler2, TRUE);

	thread_ = std::thread([this]() {
		run();
		release();
	});

	while (is_run_)
	{
#ifdef DEBUG
		windows_console_.read_console_input();
#endif // DEBUG

		Sleep(1);
	}

	thread_.join();
}

bool TestPerfManager::init()
{
	game_time_ = std::move(std::unique_ptr<GameTimeManager>(new GameTimeManager));
	game_log_ = std::move(std::unique_ptr<GameLog>(new GameLog));

	srand((unsigned int)GameTimeManager::instance()->now().get_second_time());

	GameLog::instance()->init("../log/%d-%d-%d.log");

	lua_manager_ = std::move(std::unique_ptr<TestPerfLuaScriptManager>(new TestPerfLuaScriptManager));
	lua_manager_->init();
	lua_manager_->dofile("../script/main.lua");


	std::string ip = lua_tinker::get<const char*>(lua_manager_->get_lua_state(), "ip"); // "127.0.0.1"
	unsigned short port = lua_tinker::get<unsigned short>(lua_manager_->get_lua_state(), "port"); // 8000

	int client_count = lua_tinker::get<unsigned short>(lua_manager_->get_lua_state(), "client_count");

	for (int i = 0; i < client_count; i++)
	{
		session_.push_back(create_client_session(i, ip, port));
	}

	return true;
}

void TestPerfManager::run()
{
	// ���������߳�
	thread_net_ = std::thread([this]() {
		try
		{
			ioservice_.run();
		}
		catch (const std::exception& e)
		{
			LOG_ERR(e.what());
		}
	});

	while (is_run_)
	{
		game_time_->tick();

		for (auto item : session_)
			item->tick();

		lua_tinker::call<void>(lua_manager_->get_lua_state(), "on_tick");

		Sleep(1);
	}

	for (auto item : session_)
	{
		item->on_closed();
	}
	session_.clear();

	ioservice_.stop();
	thread_net_.join();
}

void TestPerfManager::stop()
{
	if (is_run_)
	{
		for (auto item : session_)
			item->close();
	}

	work_.reset();
	is_run_ = false;
}

void TestPerfManager::release()
{
	lua_manager_.reset();

	game_time_.reset();
	game_log_.reset();
}

std::shared_ptr<NetworkSession> TestPerfManager::create_client_session(int client_id, const std::string& ip, unsigned short port)
{
	auto session = std::make_shared<ClientSession>(ioservice_);
	session->set_server_id(client_id);
	session->set_ip_port(ip, port);
	time_t deta = client_id / 100 * 3;
	session->set_deta(deta);
	return std::static_pointer_cast<NetworkSession>(session);
}

std::shared_ptr<NetworkSession> TestPerfManager::get_session(int client_id)
{ 
	if ((size_t)client_id < session_.size())
		return session_[client_id];
	return std::shared_ptr<ClientSession>();
}
