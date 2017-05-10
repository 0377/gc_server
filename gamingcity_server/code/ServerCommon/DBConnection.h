#pragma once

#include "perinclude.h"
#include <mysqld_error.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/metadata.h>
#include <cppconn/exception.h>

/**********************************************************************************************//**
 * \class	DBConnection
 *
 * \brief	A database connection.
 **************************************************************************************************/

class DBConnection
{
public:

	/**********************************************************************************************//**
	 * \brief	Default constructor.
	 **************************************************************************************************/

	DBConnection();

	/**********************************************************************************************//**
	 * \brief	Destructor.
	 **************************************************************************************************/

	~DBConnection();

	/**********************************************************************************************//**
	 * \brief	�������ݿ�.
	 *
	 * \param	host		���ݿ�ip�˿ڣ���ʽ���磺tcp://127.0.0.1:3306.
	 * \param	user		mysql�˺�.
	 * \param	password	mysql����.
	 * \param	database	mysql���ݿ�.
	 **************************************************************************************************/

	void connect(const std::string& host, const std::string& user, const std::string& password, const std::string& database);

	/**********************************************************************************************//**
	 * \brief	�ر����ݿ�����.
	 **************************************************************************************************/

	void close();

	/**********************************************************************************************//**
	 * \brief	ִ��һ��sql���.
	 *
	 * \param	sql	The SQL.
	 **************************************************************************************************/

	void execute(const std::string& sql);

	/**********************************************************************************************//**
	 * \brief	ִ��һ��sql���.
	 *
	 * \param	sql	The SQL.
	 **************************************************************************************************/

	int execute_update(const std::string& sql);

	/**********************************************************************************************//**
	 * \brief	ִ��һ��sql��䣬����mysql����.
	 *
	 * \param	sql	The SQL.
	 *
	 * \return	An int.
	 **************************************************************************************************/

	int execute_try(const std::string& sql);

	/**********************************************************************************************//**
	 * \brief	ִ��һ��sql��䣬����mysql����.
	 *
	 * \param	sql	The SQL.
	 * \param	ret	���½��.
	 *
	 * \return	An int.
	 **************************************************************************************************/
	int execute_update_try(const std::string& sql, int& ret);

	/**********************************************************************************************//**
	 * \brief	ִ��һ���н�������ص�sql��䣬��������ַ�������.
	 *
	 * \param [in,out]	output	�����.
	 * \param	sql			  	The SQL.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	bool execute_query_string(std::vector<std::string>& output, const std::string& sql);

	/**********************************************************************************************//**
	 * \brief	ִ��һ���н�������ص�sql��䣬��������ַ�������.
	 *
	 * \param [in,out]	output	�����.
	 * \param	sql			  	The SQL.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	bool execute_query_vstring(std::vector<std::vector<std::string>>& output, const std::string& sql);

	/**********************************************************************************************//**
	 * \brief	ִ��һ���н�������ص�sql��䣬�������protobuf.
	 *
	 * \param [out]	output		�����.
	 * \param	sql			  	The SQL.
	 * \param	name		  	��������protobuf������.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	bool execute_query(std::string& output, const std::string& sql, const std::string& name);

	/**********************************************************************************************//**
	 * \brief	ִ��һ���н�������ص�sql��䣬�������protobuf.
	 *
	 * \param [in,out]	output	�����.
	 * \param	sql			  	The SQL.
	 * \param	name		  	��������protobuf������.
	 * \param	filter_func   	���˽�����ֶα���string��ʽ����������.
	 *
	 * \return	true if it succeeds, false if it fails.
	 **************************************************************************************************/

	bool execute_query_filter(std::string& output, const std::string& sql, const std::string& name, 
		const std::function<bool(const std::string&)>& filter_func);

	bool execute_query_lua(std::string& output, bool b_more, const std::string& sql);

private:
	sql::Connection*							con_;
	sql::Statement*								stmt_;
};
