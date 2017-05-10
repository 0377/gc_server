#pragma once

#include "perinclude.h"
#include <boost/thread/tss.hpp>
#include <google/protobuf/text_format.h>
#include "DBConnection.h"
#include "DBQueryResult.h"

/**********************************************************************************************//**
 * \class	DBConnectionPool
 *
 * \brief	A database connection pool.
 **************************************************************************************************/

class DBConnectionPool
{
	DBConnectionPool(const DBConnectionPool&) = delete;
	DBConnectionPool& operator =(const DBConnectionPool&) = delete;
public:

	/**********************************************************************************************//**
	 * \brief	Default constructor.
	 **************************************************************************************************/

	DBConnectionPool();

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	virtual ~DBConnectionPool();

	/**********************************************************************************************//**
	 * \brief	����.
	 *
	 * \param	thread_count	Number of threads.
	 **************************************************************************************************/

	void run(size_t thread_count);

	/**********************************************************************************************//**
	 * \brief	�ȴ��߳̽���.
	 **************************************************************************************************/

	void join();

	/**********************************************************************************************//**
	 * \brief	����ر�.
	 **************************************************************************************************/

	void stop();

	/**********************************************************************************************//**
	 * \brief	����ʱ��ÿһ֡����.
	 **************************************************************************************************/

	virtual bool tick();

	/**********************************************************************************************//**
	 * \brief	ִ��һ��sql���.
	 *
	 * \param	fmt	��ʽ��sql���.
	 * \param	...	Variable arguments providing additional information.
	 **************************************************************************************************/

	void execute(const char* fmt, ...);

	/**********************************************************************************************//**
	 * \brief	ִ��һ��sql���.
	 *
	 * \param	message	protobuf��Ϣ.
	 * \param	fmt	   	��ʽ��sql���.
	 * \param	...	   	Variable arguments providing additional information.
	 **************************************************************************************************/

	void execute(const google::protobuf::Message& message, const char* fmt, ...);

	/**********************************************************************************************//**
	 * \brief	ִ��һ��sql���.
	 *
	 * \param	func	�߼��̴߳�������.
	 * \param	fmt		��ʽ��sql���.
	 * \param	...		Variable arguments providing additional information.
	 **************************************************************************************************/

	void execute_update(const std::function<void(int)>& func, const char* fmt, ...);

	/**********************************************************************************************//**
	 * \brief	ִ��һ��sql���.
	 *
	 * \param	func	�߼��̴߳�������.
	 * \param	message	protobuf��Ϣ.
	 * \param	fmt	   	��ʽ��sql���.
	 * \param	...	   	Variable arguments providing additional information.
	 **************************************************************************************************/

	void execute_update(const std::function<void(int)>& func, const google::protobuf::Message& message, const char* fmt, ...);

	/**********************************************************************************************//**
	 * \brief	ִ��һ��sql��䣬����mysql����.
	 *
	 * \param	func	�߼��̴߳�������.
	 * \param	fmt 	��ʽ��sql���.
	 * \param	... 	Variable arguments providing additional information.
	 **************************************************************************************************/

	void execute_try(const std::function<void(int)>& func, const char* fmt, ...);

	/**********************************************************************************************//**
	 * \brief	ִ��һ��sql��䣬����mysql����.
	 *
	 * \param	func	�߼��̴߳�������.
	 * \param	fmt 	��ʽ��sql���.
	 * \param	... 	Variable arguments providing additional information.
	 **************************************************************************************************/

	void execute_update_try(const std::function<void(int, int)>& func, const char* fmt, ...);

	/**********************************************************************************************//**
	 * \brief	ִ��һ��sql��ѯ��䣬����string.
	 *
	 * \param	func	�߼��̴߳�������.
	 * \param	fmt 	��ʽ��sql���.
	 * \param	... 	Variable arguments providing additional information.
	 **************************************************************************************************/

	void execute_query_string(const std::function<void(std::vector<std::string>*)>& func, const char* fmt, ...);
	
	/**********************************************************************************************//**
	 * \brief	ִ��һ��sql��ѯ��䣬����string.
	 *
	 * \param	func	�߼��̴߳�������.
	 * \param	fmt 	��ʽ��sql���.
	 * \param	... 	Variable arguments providing additional information.
	 **************************************************************************************************/

	void execute_query_vstring(const std::function<void(std::vector<std::vector<std::string>>*)>& func, const char* fmt, ...);

	/**********************************************************************************************//**
	 * \brief	ִ��һ��sql��ѯ��䣬����protobuf.
	 *
	 * \tparam	T	protobuf����.
	 * \param	func	�߼��̴߳�������.
	 * \param	name	��������protobuf������.
	 * \param	fmt 	��ʽ��sql���.
	 * \param	... 	Variable arguments providing additional information.
	 **************************************************************************************************/

	template<typename T> void execute_query(const std::function<void(T*)>& func, const char* name, const char* fmt, ...)
	{
		char str[4096] = { 0 };

		va_list arg;
		va_start(arg, fmt);
#ifdef PLATFORM_WINDOWS
		_vsnprintf_s(str, 4095, fmt, arg);
#else
		vsnprintf(str, 4095, fmt, arg);
#endif
		va_end(arg);

		std::string sql = str;

		std::string strname;
		if (name)
			strname = name;

		io_service_.post([=] {
			DBConnection* con = get_db_connection();
			
			std::string str;
			bool ret = con->execute_query(str, sql, strname);

			auto p = new DBQueryResult<T>(sql, func, ret, str);

			std::lock_guard<std::recursive_mutex> lock(mutex_query_result_);
			query_result_.push_back(p);
		});
	}

	/**********************************************************************************************//**
	 * \brief	ִ��һ���н�������ص�sql��䣬����protobuf.
	 *
	 * \tparam	T	Generic type parameter.
	 * \param	func	   	�߼��̴߳�������.
	 * \param	name	   	��������protobuf������.
	 * \param	filter_func	���˽�����ֶα���string��ʽ����������.
	 * \param	fmt		   	��ʽ��sql���.
	 * \param	...		   	Variable arguments providing additional information.
	 **************************************************************************************************/

	template<typename T> void execute_query_filter(const std::function<void(T*)>& func, const char* name, 
		const std::function<bool(const std::string&)>& filter_func, const char* fmt, ...)
	{
		char str[4096] = { 0 };

		va_list arg;
		va_start(arg, fmt);
#ifdef PLATFORM_WINDOWS
		_vsnprintf_s(str, 4095, fmt, arg);
#else
		vsnprintf(str, 4095, fmt, arg);
#endif
		va_end(arg);

		std::string sql = str;

		std::string strname;
		if (name)
			strname = name;

		io_service_.post([=] {
			DBConnection* con = get_db_connection();

			std::string str;
			bool ret = con->execute_query_filter(str, sql, strname, filter_func);

			auto p = new DBQueryResult<T>(func, ret, str);

			std::lock_guard<std::recursive_mutex> lock(mutex_query_result_);
			query_result_.push_back(p);
		});
	}

	/**********************************************************************************************//**
	 * \brief	�������ݿ�ip�˿�.
	 *
	 * \param	host	���ݿ�ip�˿ڣ���ʽ���磺tcp://127.0.0.1:3306.
	 **************************************************************************************************/

	void set_host(const std::string& host)
	{
		host_  = host; 
	}

	/**********************************************************************************************//**
	 * \brief	����mysql�˺�.
	 *
	 * \param	user	mysql�˺�.
	 **************************************************************************************************/

	void set_user(const std::string& user)
	{
		user_ = user;
	}

	/**********************************************************************************************//**
	 * \brief	����mysql����.
	 *
	 * \param	password	mysql����.
	 **************************************************************************************************/

	void set_password(const std::string& password)
	{
		password_ = password;
	}

	/**********************************************************************************************//**
	 * \brief	����mysql���ݿ�.
	 *
	 * \param	database	mysql���ݿ�.
	 **************************************************************************************************/

	void set_database(const std::string& database)
	{
		database_ = database;
	}

protected:

	/**********************************************************************************************//**
	 * \brief	Gets database connection.
	 *
	 * \return	null if it fails, else the database connection.
	 **************************************************************************************************/

	DBConnection* get_db_connection();

	/**********************************************************************************************//**
	 * \brief	����һ��db�߳�.
	 **************************************************************************************************/

	void run_thread();

protected:
	std::string										host_;
	std::string										user_;
	std::string										password_;
	std::string										database_;

	boost::asio::io_service							io_service_;
	std::shared_ptr<boost::asio::io_service::work>	work_;
	std::vector<std::shared_ptr<std::thread>>		thread_;
	std::mutex										mutex_;

	volatile bool									is_run_;

	boost::thread_specific_ptr<DBConnection>		con_ptr_;

	std::recursive_mutex							mutex_query_result_;
	std::deque<BaseDBQueryResult*>					query_result_;
};

