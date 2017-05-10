#pragma once

#include "perinclude.h"
#include "NetworkSession.h"
#include "GameLog.h"

/**********************************************************************************************//**
 * \class	NetworkDispatcher
 *
 * \brief	��Ϣ����������.
 **************************************************************************************************/

class NetworkDispatcher
{
public:
	NetworkDispatcher() {}

	virtual ~NetworkDispatcher() {}

	/**********************************************************************************************//**
	 * \brief	�õ���ϢID.
	 *
	 * \return	The message identifier.
	 **************************************************************************************************/

	virtual unsigned short get_msg_id() = 0;

	/**********************************************************************************************//**
	 * \brief	��MsgHeader��Ϣ����Ϊprotobuf.
	 *
	 * \param [in]		session	If non-null, the session.
	 * \param [in]		header 	If non-null, the header.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	virtual bool parse(NetworkSession* session, MsgHeader* header) = 0;
};

/**********************************************************************************************//**
 * \class	MsgDispatcher
 *
 * \brief	A message dispatcher.
 **************************************************************************************************/

template<typename T, typename Session>
class MsgDispatcher : public NetworkDispatcher
{
public:
	typedef void (Session::* DispatchFunction)(T*);

	/**********************************************************************************************//**
	 * \brief	Constructor.
	 *
	 * \param	func	��Ϣ����ص�����.
	 **************************************************************************************************/

	MsgDispatcher(DispatchFunction func)
		: func_(func)
	{
	}

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	virtual ~MsgDispatcher()
	{

	}

	/**********************************************************************************************//**
	 * \brief	�õ���ϢID.
	 *
	 * \return	The message identifier.
	 **************************************************************************************************/

	virtual unsigned short get_msg_id()
	{
		return T::ID;
	}

	/**********************************************************************************************//**
	 * \brief	��MsgHeader��Ϣ����Ϊprotobuf.
	 *
	 * \param [in]		session	If non-null, the session.
	 * \param [in]		header 	If non-null, the header.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	virtual bool parse(NetworkSession* session, MsgHeader* header)
	{
		try
		{
			T msg;
			if (!msg.ParseFromArray(header + 1, header->len - sizeof(MsgHeader)))
			{
				LOG_ERR("ParseFromArray failed, id=%d", header->id);
				return false;
			}

			(static_cast<Session*>(session)->*func_)(&msg);
		}
		catch (const std::exception& e)
		{
			LOG_ERR("pb error:%s", e.what());
			return false;
		}
		return true;
	}

private:
	DispatchFunction func_;
};

/**********************************************************************************************//**
 * \class	GateMsgDispatcher
 *
 * \brief	ͨ��GateServerת���ķ�������Ϣ������.
 **************************************************************************************************/

template<typename T, typename Session>
class GateMsgDispatcher : public NetworkDispatcher
{
public:
	typedef void (Session::* DispatchFunction)(int, T*);

	/**********************************************************************************************//**
	 * \brief	Constructor.
	 *
	 * \param	func	��Ϣ����ص�����.
	 **************************************************************************************************/

	GateMsgDispatcher(DispatchFunction func)
		: func_(func)
	{
	}

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	virtual ~GateMsgDispatcher()
	{

	}

	/**********************************************************************************************//**
	 * \brief	�õ���ϢID.
	 *
	 * \return	The message identifier.
	 **************************************************************************************************/

	virtual unsigned short get_msg_id()
	{
		return T::ID;
	}

	/**********************************************************************************************//**
	 * \brief	��MsgHeader��Ϣ����Ϊprotobuf.
	 *
	 * \param [in]		session	If non-null, the session.
	 * \param [in]		header 	If non-null, the header.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	virtual bool parse(NetworkSession* session, MsgHeader* header)
	{
		GateMsgHeader* h = reinterpret_cast<GateMsgHeader*>(header);
		
		try
		{
			T msg;
			if (!msg.ParseFromArray(h + 1, h->len - sizeof(GateMsgHeader)))
			{
				LOG_ERR("ParseFromArray failed, id=%d", header->id);
				return false;
			}

			(static_cast<Session*>(session)->*func_)(h->guid, &msg);
		}
		catch (const std::exception& e)
		{
			LOG_ERR("pb error:%s", e.what());
			return false;
		}
		return true;
	}

private:
	DispatchFunction func_;
};

/**********************************************************************************************//**
 * \class	NetworkDispatcherManager
 *
 * \brief	Manager for network dispatchers.
 **************************************************************************************************/

class NetworkDispatcherManager
{
public:

	/**********************************************************************************************//**
	 * \brief	Default constructor.
	 **************************************************************************************************/

	NetworkDispatcherManager();

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	~NetworkDispatcherManager();

	/**********************************************************************************************//**
	 * \brief	ע����Ϣ������.
	 *
	 * \param [in,out]	dispatcher	If non-null, the dispatcher.
	 * \param	show_log		  	true to show, false to hide the log.
	 **************************************************************************************************/

	void register_dispatcher(NetworkDispatcher* dispatcher, bool show_log = true);

	/**********************************************************************************************//**
	 * \brief	��ѯ��Ϣ������.
	 *
	 * \param	id	��ϢID.
	 *
	 * \return	null if it fails, else the dispatcher.
	 **************************************************************************************************/

	NetworkDispatcher* query_dispatcher(unsigned short id);

private:
	std::unordered_map<unsigned short, NetworkDispatcher*> dispatcher_;
};
