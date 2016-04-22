#ifndef BOOST_SIGNALSET_HPP
#define	BOOST_SIGNALSET_HPP
#include "include.hpp"

//boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
//signals.add(SIGUSR1); // 也可以直接用add函数添加信号
boost::asio::signal_set signal_set_all(SIGTERM);
signal_set_all.async_wait(boost::bind(handler, _1, _2));

void handler(
  const boost::system::error_code& error,
  int signal_number // 通过这个参数获取当前触发的信号值
)
{
	BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:signal)" <<signal_number<<",error:"<<error ;
	boost_log->get_initsink()->flush();
	cout<<"(exception:signal)" <<signal_number<<",error:"<<error<<endl;
}

#endif	/* BOOST_SIGNALSET_HPP */

