#pragma once

#include "perinclude.h"

/**********************************************************************************************//**
 * \class	NetworkIoServicePool
 *
 * \brief	�ο�libs\asio\example\cpp03\http\server2\io_service_pool.hppʵ�ֶ��߳�.
 **************************************************************************************************/

class NetworkIoServicePool
	: public boost::noncopyable
{
public:

	/**********************************************************************************************//**
	 * \brief	Constructor.
	 *
	 * \param	pool_size	Size of the pool.
	 **************************************************************************************************/

	explicit NetworkIoServicePool(size_t pool_size);

	/**********************************************************************************************//**
	 * \brief	��ʼ�����߳�.
	 **************************************************************************************************/

	void start();

	/**********************************************************************************************//**
	 * \brief	�ȴ�����̹߳ر�.
	 **************************************************************************************************/

	void join();

	/**********************************************************************************************//**
	 * \brief	����.
	 **************************************************************************************************/

	void stop();

	/**********************************************************************************************//**
	 * \brief	Gets io_service.
	 *
	 * \return	The io_service.
	 **************************************************************************************************/

	boost::asio::io_service& get_io_service();

private:
	typedef std::shared_ptr<boost::asio::io_service> io_service_sptr;
	typedef std::shared_ptr<boost::asio::io_service::work> work_sptr;
	typedef std::shared_ptr<std::thread> thread_sptr;

	void run(io_service_sptr ioservice);
	void c_run(boost::asio::io_service* ioservice);
	void seh_run(boost::asio::io_service* ioservice);

	std::mutex								mutex_;

	std::vector<io_service_sptr>			io_services_;
	std::vector<work_sptr>					work_;
	std::vector<thread_sptr>				threads_;
	size_t									next_io_service_;

	volatile bool							brun_;
};
