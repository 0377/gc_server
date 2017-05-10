#pragma once

#include "NetworkSession.h"
#include "NetworkDispatcher.h"
#include "common_msg_define.pb.h"
#include "msg_server.pb.h"
//#include "PbClientSocket.h"
//#include "rapidjson/document.h"
//#include "rapidjson/writer.h"
//#include "rapidjson/stringbuffer.h"

/**********************************************************************************************//**
 * \class	GameSession
 *
 * \brief	gate����game��session.
 **************************************************************************************************/

class GameSession : public NetworkSession
{
public:

	/**********************************************************************************************//**
	 * \brief	Constructor.
	 *
	 * \param [in,out]	sock	The sock.
	 **************************************************************************************************/

	GameSession(boost::asio::ip::tcp::socket& sock);

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	virtual ~GameSession();

	/**********************************************************************************************//**
	 * \brief	�����յ�����Ϣ.
	 *
	 * \param [in,out]	header	If non-null, the header.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	virtual bool on_dispatch(MsgHeader* header);

	/**********************************************************************************************//**
	 * \brief	������ܻص�.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	virtual bool on_accept();

	/**********************************************************************************************//**
	 * \brief	�ر�socketǰ�ص�.
	 **************************************************************************************************/

	virtual void on_closed();

	/**********************************************************************************************//**
	 * \brief	�õ�������id.
	 *
	 * \return	The server identifier.
	 **************************************************************************************************/

	virtual int get_server_id() { return server_id_; }

	/**********************************************************************************************//**
	 * \brief	���÷�����id.
	 *
	 * \param	server_id	Identifier for the server.
	 **************************************************************************************************/

	void set_server_id(int server_id) { server_id_ = server_id; }

public:

	/**********************************************************************************************//**
	 * \brief	�յ�һ�������������ӡ���Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_s_connect(S_Connect* msg);

	/**********************************************************************************************//**
	 * \brief	�յ�һ��������˳�����Ϣ�Ĵ�����.
	 *
	 * \param [in,out]	msg	If non-null, the message.
	 **************************************************************************************************/

	void on_s_logout(S_Logout* msg);


    ///************************************************************************/
    ///* web ������غ���                                                                     */
    ///************************************************************************/
    //typedef MsgBuffer<MSG_SEND_BUFFER_SIZE> MsgSendBuffer;

    //void sendWebSocket(std::string ip,int port,std::string host,std::string url,std::string uData,const std::function<void(int retCode,std::string retData)>& func);
    //void handleWebSend(boost::shared_ptr<boost::asio::ip::tcp::socket> sockT, std::string Data, const std::function<void(int retCode, std::string retData)>& func,
    //    const boost::system::error_code& err, size_t bytes_transferred);
    //void handleWebRead(boost::shared_ptr<boost::asio::ip::tcp::socket> sockT, boost::shared_ptr<MsgSendBuffer> ubuffer, const std::function<void(int retCode, std::string retData)>& func,
    //    const boost::system::error_code& err, size_t bytes_transferred);
    //std::string createWebRequestData(std::string host, std::string url, std::string uData);
private:
	NetworkDispatcherManager*			dispatcher_manager_;

	std::string							ip_;
	unsigned short						port_;

	int									type_;
	int									server_id_;
};