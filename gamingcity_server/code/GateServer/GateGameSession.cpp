#include "GateGameSession.h"
#include "GateClientSession.h"
#include "GateServerConfigManager.h"
#include "GameLog.h"
#include "common_enum_define.pb.h"
#include "GateServer.h"

GateGameSession::GateGameSession(boost::asio::io_service& ioservice)
	: NetworkConnectSession(ioservice)
    , server_id_(0)
{
}

GateGameSession::~GateGameSession()
{
}

bool GateGameSession::on_dispatch(MsgHeader* header)
{
	if (header->id == S_Heartbeat::ID)
	{
		return true;
	}

	if (header->id == S_Connect::ID)
	{
		try
		{
			S_Connect msg;
			if (!msg.ParseFromArray(header + 1, header->len - sizeof(MsgHeader)))
			{
				LOG_ERR("ParseFromArray failed, id=%d", header->id);
				return false;
			}

			server_id_ = msg.server_id();

			GateSessionManager::instance()->add_game_id(server_id_);

			GC_GameServerCfg notify;
			for (auto& item : static_cast<GateServer*>(BaseServer::instance())->get_gamecfg().pb_cfg())
			{
				if (GateSessionManager::instance()->in_open_game_list(item.game_id()))
				{
					notify.add_pb_cfg()->CopyFrom(item);
				}
			}

			GateSessionManager::instance()->broadcast_client(&notify);
		}
		catch (const std::exception& e)
		{
			LOG_ERR("pb error:%s", e.what());
			return false;
		}
	}
	else
	{
		GateMsgHeader* h = static_cast<GateMsgHeader*>(header);

		if (h->id == LC_Login::ID)
		{
			// ��¼�ɹ�֪ͨ��Ϣ
			try
			{
				LC_Login msg;
				if (!msg.ParseFromArray(h + 1, h->len - sizeof(GateMsgHeader)))
				{
					LOG_ERR("ParseFromArray failed, id=%d", header->id);
					return false;
				}

				auto s = GateSessionManager::instance()->find_by_id(h->guid);
				if (s)
				{
					auto session = static_cast<GateClientSession*>(s.get());

					session->set_guid(msg.guid());
					if (msg.game_id() != 0)
					{
						session->set_game_server_id(msg.game_id());
					}
					else
					{
						LOG_WARN("game_id=0");
					}
					if (msg.is_guest())
					{
						session->set_account(msg.account());
					}
					GateSessionManager::instance()->add_client_session(s);

					session->send_xc(h);
					session->set_login(true);

					LOG_INFO("login step gate->client CL_Login, guid:%d,account=%s session_id=%d ret=%d ok", msg.guid(),session->get_account().c_str(), session->get_id(), msg.result());


                    if (msg.result() == LOGIN_RESULT_SUCCESS)
                    {
                        GF_PlayerIn nmsg;
                        nmsg.set_guid(msg.guid());
                        GateConfigNetworkServer::instance()->send2cfg_pb(&nmsg);
                    }
				}
				else
				{
					LOG_ERR("login step gate->client CL_Login guid:%d", msg.guid());
				}
			}
			catch (const std::exception& e)
			{
				LOG_ERR("pb error:%s", e.what());
				return false;
			}
		}
        else
		{
			auto s = GateSessionManager::instance()->get_client_session(h->guid);
			if (!s)
			{
				LOG_WARN("msg[%d] guid[%d] not find", h->id, h->guid);
				return true;
			}

			auto session = static_cast<GateClientSession*>(s.get());

			if (header->id == SC_EnterRoomAndSitDown::ID)
			{
				try
				{
					SC_EnterRoomAndSitDown msg;
					if (!msg.ParseFromArray(h + 1, h->len - sizeof(GateMsgHeader)))
					{
						LOG_ERR("ParseFromArray failed, id=%d", header->id);
						return false;
					}

					if (msg.game_id() != 0)
					{
						session->set_game_server_id(msg.game_id());
					}
					else
					{
						LOG_WARN("game_id=0");
					}
				}
				catch (const std::exception& e)
				{
					LOG_ERR("pb error:%s", e.what());
					return false;
				}
			}
			else if (header->id == SC_PlayerReconnection::ID)
			{
				try
				{
					SC_PlayerReconnection msg;
					if (!msg.ParseFromArray(h + 1, h->len - sizeof(GateMsgHeader)))
					{
						LOG_ERR("ParseFromArray failed, id=%d", header->id);
						return false;
					}

					if (msg.game_id() != 0)
					{
						session->set_game_server_id(msg.game_id());
					}
					else
					{
						LOG_WARN("game_id=0");
					}
				}
				catch (const std::exception& e)
				{
					LOG_ERR("pb error:%s", e.what());
					return false;
				}
			}
			else if (header->id == SC_ChangeTable::ID)
			{
				try
				{
					SC_ChangeTable msg;
					if (!msg.ParseFromArray(h + 1, h->len - sizeof(GateMsgHeader)))
					{
						LOG_ERR("ParseFromArray failed, id=%d", header->id);
						return false;
					}

					if (msg.game_id() != 0)
					{
						session->set_game_server_id(msg.game_id());
					}
					else
					{
						LOG_WARN("game_id=0");
					}
				}
				catch (const std::exception& e)
				{
					LOG_ERR("pb error:%s", e.what());
					return false;
				}
			}

			session->send_xc(h);
		}
	}
	return true;
}

bool GateGameSession::on_connect()
{
	LOG_INFO("gate->game accept ... [%s:%d]", ip_.c_str(), port_);

	S_Connect msg;
	msg.set_type(ServerSessionFromGate);
	msg.set_server_id(static_cast<GateServer*>(BaseServer::instance())->get_gate_id());
	send_pb(&msg);

	return NetworkConnectSession::on_connect();
}

void GateGameSession::on_connect_failed()
{
	LOG_INFO("gate->game connect failed ... [%s:%d]", ip_.c_str(), port_);

	NetworkConnectSession::on_connect_failed();
}

void GateGameSession::on_closed()
{
	LOG_INFO("gate->game disconnect ... [%s:%d]", ip_.c_str(), port_);

	GateSessionManager::instance()->remove_game_id(server_id_);

    // ���ߴ���
	for (auto& item : static_cast<GateServer*>(BaseServer::instance())->get_gamecfg().pb_cfg())
    {
        if (item.game_id() == server_id_)
		{
			GC_GameServerCfg notify;
			for (auto& item : static_cast<GateServer*>(BaseServer::instance())->get_gamecfg().pb_cfg())
			{
				if (GateSessionManager::instance()->in_open_game_list(item.game_id()))
				{
					notify.add_pb_cfg()->CopyFrom(item);
				}
			}

			GateSessionManager::instance()->broadcast_client(&notify);

			break;
        }
    }
	NetworkConnectSession::on_closed();
}
