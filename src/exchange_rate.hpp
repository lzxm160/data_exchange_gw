#ifndef EXCHANGE_RATE_HPP
#define	EXCHANGE_RATE_HPP

#include  "include.hpp"
string rand_string(int len);
class exchage_rate_data
{
public:
	string code;//CAD
	string to_usd_exchange_rate;//0.772558
	string from_usd_exchange_rate;//0.772558
	string currency_id;//J4YVQ3USQNO3U430EKE1
	string to_usd_exchange_rate_id;//TFTBLZNSNBNAZAZGC2RW
	string from_usd_exchange_rate_id;//TFTBLZNSNBNAZAZGC2RW
	void print()
	{	cout<<"{";
		cout<<code<<":";
		cout<<to_usd_exchange_rate<<":";//0.772558
		cout<<from_usd_exchange_rate<<":";//0.772558
		cout<<currency_id<<":";//J4YVQ3USQNO3U430EKE1
		cout<<to_usd_exchange_rate_id<<":";//TFTBLZNSNBNAZAZGC2RW
		cout<<from_usd_exchange_rate_id<<"}";//TFTBLZNSNBNAZAZGC2RW
		cout<<":"<<__FILE__<<":"<<__LINE__<<endl;
	}
};
class general_rate_data
{
public:
	string first_code;
	string second_code;
	string first_currency_id;
	string second_currency_id;
	string first_second_currency_exchange_rate_id;
	string second_first_currency_exchange_rate_id;
	string first_second_exchange_rate;
	void print()
	{
		cout<<"{"<<first_code<<":"<<second_code<<":"<<first_second_currency_exchange_rate_id<<":"<<second_first_currency_exchange_rate_id<<":"<<first_second_exchange_rate<<"}"<<endl;
	}
};
class mysql_database
{
public:
	string m_mysql_ip;
	unsigned short m_mysql_port;
	string m_mysql_username;
	string m_mysql_password;
	string m_mysql_database;

};
class update_svn_version_rest
{
public:
	update_svn_version_rest(mysql_database mysql_input)//:m_mysql_database(mysql_input)
	{
		cout<<__FILE__<<":"<<__LINE__<<endl;
		m_conn=boost::shared_ptr<MySql>(new MySql(mysql_input.m_mysql_ip.c_str(), mysql_input.m_mysql_username.c_str(), mysql_input.m_mysql_password.c_str(), mysql_input.m_mysql_database.c_str(), mysql_input.m_mysql_port));
	}
	update_svn_version_rest(boost::shared_ptr<MySql> conn):m_conn(conn)//,m_mysql_database(mysql_input)
	{}
	
	bool update_version_put(const string& version,const string& url)
	{
		return do_update(version,url);	
	}
private:
	bool do_update(const string& version,const string& url)
	{
		try
		{
			if(version.length()==0||url.length()==0)
			{
				return false;
			}
			ptime now = second_clock::local_time();
			string p4 = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());

			string insert_sql = "update t_website set deploy_software_version='"+version+"',deploy_time='"+p4+"',updateBy='exchange_gw_rest',updateAt='"+p4+"'"+" where access_url='"+url+"'";
			cout << insert_sql << endl;
			m_conn->runCommand(insert_sql.c_str());
			BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<insert_sql<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();
			return true;
		}
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;//m_conn=nullptr;
			return false;
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
			return false;
		}
	}
	
private:
	boost::shared_ptr<MySql> m_conn;
	//mysql_database m_mysql_database;
};
class exchange_rate_rest
{
public:
	exchange_rate_rest(mysql_database mysql_input)//:m_mysql_database(mysql_input)
	{
		cout<<__FILE__<<":"<<__LINE__<<endl;
		m_conn=boost::shared_ptr<MySql>(new MySql(mysql_input.m_mysql_ip.c_str(), mysql_input.m_mysql_username.c_str(), mysql_input.m_mysql_password.c_str(), mysql_input.m_mysql_database.c_str(), mysql_input.m_mysql_port));
	}
	exchange_rate_rest(boost::shared_ptr<MySql> conn):m_conn(conn)//,m_mysql_database(mysql_input)
	{}
	string get_rate(const string& source,const string& target,const string& which_day)
	{
		//cout<<__FILE__<<":"<<__LINE__<<endl;
		string id=get_exchange_rate_id(source,target);
		if(id.length()==0)
			return "0";
		//cout<<__FILE__<<":"<<__LINE__<<endl;
		return get_rate_from_myql(id,which_day);

	}
	void update_rate(const string& source,const string& target,const string& which_day,const string& ratio)
	{
		//cout<<__FILE__<<":"<<__LINE__<<endl;
		string id=get_exchange_rate_id(source,target);
		if(id.length()==0)
		{
			insert_exchange_rate_id(source,target);
			id=get_exchange_rate_id(source,target);
		}
		insert_exchange_rate(id,which_day,ratio);
		
	}
	void update_rate_put(const string& source,const string& target,const string& which_day,const string& ratio)
	{
		//cout<<__FILE__<<":"<<__LINE__<<endl;
		string id=get_exchange_rate_id(source,target);
		if(id.length()==0)
		{
			insert_exchange_rate_id(source,target);
			id=get_exchange_rate_id(source,target);
		}
		update_exchange_rate(id,which_day,ratio);
		
	}
private:
	void insert_exchange_rate_id(const string& source,const string& target)
	{
		cout<<__FILE__<<":"<<__LINE__<<endl;
		string source_currency_id=get_currency_id(source);
		string target_currency_id=get_currency_id(target);
		if(source_currency_id.length()==0||target_currency_id.length()==0)
		{
			return;
		}
		ptime now = second_clock::local_time();
		string p4 = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());

		string insert_sql = "insert into t_currency_exchange_rate(currency_exchange_rate_id,source_currency_id,target_currency_id,calculation_method,decimal_digits,match_method,createAt,createBy,updateAt,updateBy,dr,data_version)values('"+rand_string(20)+"','"+source_currency_id+"','"+target_currency_id+"',0,7,0,'"+p4+"','','"+p4+"','',0,1)";
		cout << insert_sql << endl;
		m_conn->runCommand(insert_sql.c_str());
		BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<insert_sql<<":"<<__FILE__<<":"<<__LINE__;
		boost_log->get_initsink()->flush();
	}
	string get_currency_id(const string& code)
	{
		try
		{
			if(code.length()==0) return "";
			cout<<__FILE__<<":"<<__LINE__<<endl;
			typedef tuple<unique_ptr<string>> t_currency_tuple;
		//select code,currency_id from t_currency
			//typedef tuple<string,double> credit_tuple;
			std::vector<t_currency_tuple> t_currency_tuple_vector;
			string query_sql = "select currency_id from t_currency where code='"+code+"'";
			cout << query_sql << endl;
			m_conn->runQuery(&t_currency_tuple_vector, query_sql.c_str());

			BOOST_LOG_SEV(slg, boost_log->get_log_level()) << query_sql;
			boost_log->get_initsink()->flush();
			
			if(t_currency_tuple_vector.empty())
			{
				BOOST_LOG_SEV(slg, boost_log->get_log_level()) << "nothing select from t_currency";
				boost_log->get_initsink()->flush();
				cout<<"nothing select from t_currency"<<endl;
				return "";
			}
			for (const auto& item : t_currency_tuple_vector)
			{
				return *(std::get<0>(item));						
			}
		}
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;//m_conn=nullptr;
			return "";
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
			return "";
		}
	}
	string get_exchange_rate_id(const string& source,const string& target)
	{
		try
		{
			cout<<__FILE__<<":"<<__LINE__<<endl;
			string source_currency_id=get_currency_id(source);
			string target_currency_id=get_currency_id(target);
			if(source_currency_id.length()==0||target_currency_id.length()==0)
			{
				return "";
			}
			string get_exchange_rate_ids = "select currency_exchange_rate_id from t_currency_exchange_rate where source_currency_id=\'"+source_currency_id+"\' and target_currency_id=\'"+target_currency_id+"\'";
		
			typedef tuple<unique_ptr<string>> c;

			vector<c> exchange_rate_ids;
			m_conn->runQuery(&exchange_rate_ids,get_exchange_rate_ids.c_str());
			if(exchange_rate_ids.empty())
				return "";
			for(const auto& i : exchange_rate_ids)
				return *(std::get<0>(i));
				
		}
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;m_conn=nullptr;
			return "";
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
			return "";
		}
	}
	void insert_exchange_rate(const string& exchange_rate_id,const string& which_day,const string& ratio)
	{
		if(exchange_rate_id.length()==0||ratio.length()==0)
		{
			return;
		}
		
		cout<<__FILE__<<":"<<__LINE__<<endl;
		std::vector<std::string> hms;
		boost::split(hms,which_day , boost::is_any_of("-"));
  		string year=hms[0];
  		string month=hms[1];
		string day=hms[2];

		string insert_sql = "insert into t_currency_daily_exchange_rate values('"+rand_string(20)+"',\'"+exchange_rate_id+"\',\'"+year+"\',\'"+month+"\',\'"+day+"\',"+ratio+",\'"+which_day+"\',\'"+which_day+" 00:00:05"+"\',\'exchange_gw_rest\',\'"+which_day+" 00:00:05"+"\',\'exchange_gw_rest\',\'\',\'\',0,1)";
		
		cout << insert_sql << endl;
		m_conn->runCommand(insert_sql.c_str());
		BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<insert_sql<<":"<<__FILE__<<":"<<__LINE__;
		boost_log->get_initsink()->flush();
			
	}
	void update_exchange_rate(const string& exchange_rate_id,const string& which_day,const string& ratio)
	{
		if(exchange_rate_id.length()==0||ratio.length()==0)
		{
			return;
		}
		
		cout<<__FILE__<<":"<<__LINE__<<endl;
		std::vector<std::string> hms;
		boost::split(hms,which_day , boost::is_any_of("-"));
  		string year=hms[0];
  		string month=hms[1];
		string day=hms[2];

		string update_sql="update t_currency_daily_exchange_rate set exchange_ratio="+ratio+" where exchange_date='"+year+"-"+month+"-"+ day+"' and (updateBy='exchange_gw' or updateBy='exchange_gw_rest') and exchange_rate_id='"+exchange_rate_id+"'";

		cout << update_sql << endl;
		m_conn->runCommand(update_sql.c_str());
		BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<update_sql<<":"<<__FILE__<<":"<<__LINE__;
		boost_log->get_initsink()->flush();
			
	}
	string get_rate_from_myql(const string& exchange_rate_id,string which_day)
	{
		try
		{
			cout<<__FILE__<<":"<<__LINE__<<endl;
			if(exchange_rate_id.length()==0)
				return "0";
			if(which_day.length()==0)
			{
				ptime now = second_clock::local_time();
 				which_day=to_iso_extended_string(now.date());
			}
		typedef tuple<unique_ptr<float>> t_exchange_rate_tuple;
		//select code,currency_id from t_currency
			//typedef tuple<string,double> credit_tuple;
			std::vector<t_exchange_rate_tuple> t_exchange_rate_tuple_vector;
			//select * from apollo_eu.t_currency_daily_exchange_rate where exchange_rate_id='BVVFOOI1LDHQSY3DL0AK' and createBy='exchange_gw'
			string query_sql = "select exchange_ratio from t_currency_daily_exchange_rate where exchange_rate_id=\'"+exchange_rate_id+"\' and (createBy='exchange_gw' or createBy='exchange_gw_rest') and exchange_date='"+which_day+"'";
			cout << query_sql << endl;
			m_conn->runQuery(&t_exchange_rate_tuple_vector, query_sql.c_str());

			BOOST_LOG_SEV(slg, boost_log->get_log_level()) << query_sql;
			boost_log->get_initsink()->flush();
			/********************************/
			cout.setf(ios::showpoint); cout.setf(ios::fixed); cout.precision(8);
			cout<<":"<<__FILE__<<":"<<__LINE__<<endl;
			/********************************/
			if(t_exchange_rate_tuple_vector.empty())
			{
				BOOST_LOG_SEV(slg, boost_log->get_log_level()) << "nothing select from t_currency";
				boost_log->get_initsink()->flush();
				cout<<"nothing select from t_currency"<<endl;
				return "0";
			}
			for (const auto& item : t_exchange_rate_tuple_vector)
			{
				return boost::lexical_cast<string>(*(std::get<0>(item)));	
			}
			
		}
		
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;//m_conn=nullptr;
			return "0";
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;return "0";
		}
	}
private:
	boost::shared_ptr<MySql> m_conn;
	//mysql_database m_mysql_database;
};
//#define DEBUG
//https://www.exchangerate-api.com/USD/GBP?k=d2408a508ec4f2bec554f465
class exchange_rate
{
public:
	exchange_rate(const std::string& url) : m_url(url),m_request_status(0),m_data(nullptr)
	{
		//register callback
		//register_callback();
		curl_global_init(CURL_GLOBAL_ALL);
		m_curl = curl_easy_init();
		curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);
		
#ifdef DEBUG
		curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1);
#endif
		//curl(m_download_url, "GET", filename, true);

		if (!share_handle)
		{
			share_handle = curl_share_init();
			curl_share_setopt(share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
		}
		curl_easy_setopt(m_curl, CURLOPT_SHARE, share_handle);
		curl_easy_setopt(m_curl, CURLOPT_DNS_CACHE_TIMEOUT, 600 * 5);
		curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 1000 );
		curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT,50);
	}
	virtual ~exchange_rate()
	{
		curl_easy_cleanup(m_curl);
		curl_global_cleanup();
	}
	void request(const std::string& method, const std::string& path, const std::string& param, const std::string& content)
	{
		/////////////////////////////////////
		BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<"request:"<<method<<":"<<path<<":"<<param<<":"<<content;
		boost_log->get_initsink()->flush();
		/////////////////////////////////////////////////////
		//t_currency_exchange_rate 
		curl(path, method, param, content);
	}
	boost::shared_ptr<string> get_data()
	{

		return m_data;
	}
	long int get_status()
	{
		return m_request_status;
	}
	long int get_length()
	{
		return (*m_data).length();
	}
protected:

	static size_t request_callback(char *buffer, size_t size, size_t nmemb, void* thisPtr)
	{
		if (thisPtr)
		{
			//cout << __LINE__ << endl;
			return ((exchange_rate*)thisPtr)->request_write_data(buffer, size, nmemb);
		}

		else
		{
			//cout << __LINE__ << endl;
			return 0;
		}

	}
	size_t request_write_data(const char *buffer, size_t size, size_t nmemb)
	{
		int result = 0;
		if (buffer != 0)
		{
			//cout << __LINE__ << endl;
			//m_data.clear();
			(*m_data).append(buffer, size * nmemb);
			// response->content.read(&buffer[0], length);
            // content.write(&buffer[0], length);
            //m_content.read(buffer,size * nmemb)
			// cout<<"m_data:"<<m_data.size()<<endl;
			// cout<<"max_size:"<<m_data.max_size() <<endl;
			// cout<<"capacity:"<<m_data.capacity()<<endl;
			result = size * nmemb;
			// boost::asio::streambuf write_buffer;

		}
		return result;
	}

	void curl(const std::string& uri, const std::string& method = "GET", const std::string& param = "", const std::string& content = "")
	{
		if(param.length()!=0)
			set_url(m_url + uri + "?" + param);
		else
			set_url(m_url + uri);
		

		//cout << __LINE__ << ":" << uri << endl;

#ifdef DEBUG
		curl_easy_setopt(m_curl, CURLOPT_HEADER, 1);
#endif
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, request_callback);
		curl_easy_setopt(m_curl, CURLOPT_MAXREDIRS, 50L);
		curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPALIVE, 1L);
		curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, method.c_str());

		//curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS,0L);
		curl_easy_setopt(m_curl, CURLOPT_CLOSESOCKETFUNCTION, close_socket_callback);

		curl_easy_setopt(m_curl, CURLOPT_CLOSESOCKETDATA, this);
		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, content.c_str());

  		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)content.length());
		if(on_request())
		{
			if(CURLE_OK==curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE,&m_request_status))
			{
				cout<<"get status success"<<endl;
			}
		}

	}
	void set_url(const std::string& url) const
	{
		curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
		cout<<url<<":"<<__FILE__<<":"<<__LINE__<<endl;
	}
	bool on_request()
	{
		if(m_data)
		{
			(*m_data).clear();
		}
		else
		{
			m_data=boost::shared_ptr<string>(new string());
		}
		return 0 == curl_easy_perform(m_curl);
	}
	static int close_socket_callback(void *clientp, curl_socket_t item)
	{
		if (clientp)
		{
			//cout << __LINE__ << endl;
			((exchange_rate*)clientp)->process_content();
		}

	}
	void process_content()
	{

		//cout<<*m_data<<":"<<__FILE__<<":"<<__LINE__<<endl;		
	}
	
	
public:
	boost::shared_ptr<std::string> m_data;
protected:	
	CURL* m_curl;
	std::string m_url;
	static CURLSH* share_handle;	
	long int m_request_status;
	// boost::asio::streambuf m_content_buffer;
	// std::istream m_content;
};
CURLSH* exchange_rate::share_handle = NULL;


class exchange_rate_on_time:public boost::enable_shared_from_this<exchange_rate_on_time>
{
	mysql_database m_mysql_database;
public:
	exchange_rate_on_time(mysql_database mysql_input):m_d_t(m_io_s),m_product_all(nullptr),m_mysql_database(mysql_input),m_conn(nullptr)
	{
		//m_conn=boost::shared_ptr<MySql>(new MySql(m_mysql_database.m_mysql_ip.c_str(), m_mysql_database.m_mysql_username.c_str(), m_mysql_database.m_mysql_password.c_str(), m_mysql_database.m_mysql_database.c_str(), m_mysql_database.m_mysql_port));
		
		//m_today_string=to_iso_extended_string(boost::gregorian::day_clock::local_day());
	}
	~exchange_rate_on_time()
	{
		
	}
	string get_rate_from_myql(const string& exchange_rate_id,string which_day)
	{
		try
		{

			if(which_day.length()==0)
			{
				ptime now = second_clock::local_time();
 				which_day=to_iso_extended_string(now.date());
			}
		typedef tuple<unique_ptr<float>> t_exchange_rate_tuple;
		//select code,currency_id from t_currency
			//typedef tuple<string,double> credit_tuple;
			std::vector<t_exchange_rate_tuple> t_exchange_rate_tuple_vector;
			//select * from apollo_eu.t_currency_daily_exchange_rate where exchange_rate_id='BVVFOOI1LDHQSY3DL0AK' and createBy='exchange_gw'
			string query_sql = "select exchange_ratio from "+m_mysql_database.m_mysql_database + ".t_currency_daily_exchange_rate where exchange_rate_id=\'"+exchange_rate_id+"\' and (createBy='exchange_gw' or createBy='exchange_gw_rest') and exchange_date='"+which_day+"'";
			cout << query_sql << endl;
			m_conn->runQuery(&t_exchange_rate_tuple_vector, query_sql.c_str());

			BOOST_LOG_SEV(slg, boost_log->get_log_level()) << query_sql;
			boost_log->get_initsink()->flush();
			/********************************/
			cout.setf(ios::showpoint); cout.setf(ios::fixed); cout.precision(8);
			cout<<":"<<__FILE__<<":"<<__LINE__<<endl;
			/********************************/
			if(t_exchange_rate_tuple_vector.empty())
			{
				BOOST_LOG_SEV(slg, boost_log->get_log_level()) << "nothing select from t_currency";
				boost_log->get_initsink()->flush();
				cout<<"nothing select from t_currency"<<endl;
				return "0";
			}
			for (const auto& item : t_exchange_rate_tuple_vector)
			{
				return boost::lexical_cast<string>(*(std::get<0>(item)));	
			}
			
		}
		
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;m_conn=nullptr;
			start_timer();
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
			start_timer();
		}
	}
	void get_info_from_myql()
	{
		try
		{
		typedef tuple<unique_ptr<string>, unique_ptr<string>> t_currency_tuple;
		//select code,currency_id from t_currency
			//typedef tuple<string,double> credit_tuple;
			std::vector<t_currency_tuple> t_currency_tuple_vector;
			string query_sql = "select code,currency_id from "+m_mysql_database.m_mysql_database + ".t_currency";
			cout << query_sql << endl;
			m_conn->runQuery(&t_currency_tuple_vector, query_sql.c_str());

			BOOST_LOG_SEV(slg, boost_log->get_log_level()) << query_sql;
			boost_log->get_initsink()->flush();
			/********************************/
			cout.setf(ios::showpoint); cout.setf(ios::fixed); cout.precision(8);
			cout<<":"<<__FILE__<<":"<<__LINE__<<endl;
			/********************************/
			if(t_currency_tuple_vector.empty())
			{
				BOOST_LOG_SEV(slg, boost_log->get_log_level()) << "nothing select from t_currency";
				boost_log->get_initsink()->flush();
				cout<<"nothing select from t_currency"<<endl;
				return;
			}
			for (const auto& item : t_currency_tuple_vector)
			{
				//cout<<item<<":"<<__FILE__<<":"<<__LINE__<<endl;
				//cout<<*(std::get<0>(item))<<":"<<__FILE__<<":"<<__LINE__<<endl;
				if(*(std::get<0>(item))=="USD")
				{
					m_usd_info.code="USD";
					m_usd_info.currency_id=*(std::get<1>(item));
					cout<<m_usd_info.code<<":"<<m_usd_info.currency_id<<":"<<__FILE__<<":"<<__LINE__<<endl;
				}
				
			}
			for (const auto& item : t_currency_tuple_vector)
			{
				// string code;//CAD
				// double to_usd_exchange_rate;//0.772558
				// double from_usd_exchange_rate;//0.772558
				// string currency_id;//J4YVQ3USQNO3U430EKE1
				// string to_usd_exchange_rate_id;//TFTBLZNSNBNAZAZGC2RW
				// string from_usd_exchange_rate_id;//TFTBLZNSNBNAZAZGC2RW
				exchage_rate_data temp;
				temp.code=*(std::get<0>(item));
				temp.currency_id=*(std::get<1>(item));
				if(temp.code=="USD")
					continue;
				//cout << *(std::get<0>(item))<<":"<<*(std::get<1>(item))<<":"<<__FILE__<<":"<<__LINE__<<endl;

				string get_exchange_rate_id1 = "select currency_exchange_rate_id from t_currency_exchange_rate where source_currency_id=\'"+temp.currency_id+"\' and target_currency_id=\'"+m_usd_info.currency_id+"\'";
				//cout << get_exchange_rate_id << endl;
				string get_exchange_rate_id2 = "select currency_exchange_rate_id from t_currency_exchange_rate where source_currency_id=\'"+m_usd_info.currency_id+"\' and target_currency_id=\'"+temp.currency_id+"\'";
				//cout << get_exchange_rate_id2 << endl;
				try
				{
					{
						typedef tuple<unique_ptr<string>> c;
			
						vector<c> exchange_rate_ids;
						m_conn->runQuery(&exchange_rate_ids,get_exchange_rate_id1.c_str());
						if(exchange_rate_ids.empty())
						{
							insert_exchange_rate_id(temp.code,m_usd_info.code);
							temp.to_usd_exchange_rate_id=get_exchange_rate_id(temp.code,m_usd_info.code);
						}
						for(const auto& i : exchange_rate_ids)
						temp.to_usd_exchange_rate_id=*(std::get<0>(i));
						exchange_rate_ids.clear();
					}
					{
						typedef tuple<unique_ptr<string>> c;
			
						vector<c> exchange_rate_ids;
						m_conn->runQuery(&exchange_rate_ids,get_exchange_rate_id2.c_str());
						if(exchange_rate_ids.empty())
						{
							insert_exchange_rate_id(m_usd_info.code,temp.code);
							temp.from_usd_exchange_rate_id=get_exchange_rate_id(m_usd_info.code,temp.code);
						}
						for(const auto& i : exchange_rate_ids)
						temp.from_usd_exchange_rate_id=*(std::get<0>(i));
						exchange_rate_ids.clear();
					}

				}
				catch (const MySqlException& e)
				{
					BOOST_LOG_SEV(slg, severity_level::error) << "(1)" << get_exchange_rate_id1 << "(2)" << get_exchange_rate_id2 << "(exception:)" << e.what();
					boost_log->get_initsink()->flush();start_timer();
				}
				m_exchage_rate_data_array.push_back(temp);
			}	
			t_currency_tuple_vector.clear();
		}
		
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
			start_timer();
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
			start_timer();
		}
	}
	void update_to_usd_exchange_rate(const exchage_rate_data& item)
	{
		try
		{
 			ptime now = second_clock::local_time();
 			std::vector<std::string> ymd;
 			string today=to_iso_extended_string(now.date());
			boost::split(ymd,today , boost::is_any_of("-"));
 			//string [] ymd=to_iso_extended_string(now.date()).split('-');
 			string year=ymd[0];
 			string month=ymd[1];
 			string day=ymd[2];
			string p4 = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			//if(t_currency_daily_exchange_rate_tuple_vector.empty())
			if(item.to_usd_exchange_rate_id.length()==0||item.to_usd_exchange_rate.length()==0)
			{
				return;
			}
			
			string insert_sql = "insert into t_currency_daily_exchange_rate values('"+rand_string(20)+"',\'"+item.to_usd_exchange_rate_id+"\',\'"+year+"\',\'"+month+"\',\'"+day+"\',"+item.to_usd_exchange_rate+",\'"+to_iso_extended_string(now.date())+"\',\'"+p4+"\',\'exchange_gw\',\'"+p4+"\',\'exchange_gw\',\'\',\'\',0,1)";

			cout << insert_sql << endl;
			m_conn->runCommand(insert_sql.c_str());
			
			BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<insert_sql<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();
		
		}
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
	}
	string get_exchange_rate_id(const string& source,const string& target)
	{
		try
		{
			cout<<__FILE__<<":"<<__LINE__<<endl;
			string source_currency_id=get_currency_id(source);
			string target_currency_id=get_currency_id(target);
			if(source_currency_id.length()==0||target_currency_id.length()==0)
			{
				return "";
			}
			string get_exchange_rate_ids = "select currency_exchange_rate_id from t_currency_exchange_rate where source_currency_id=\'"+source_currency_id+"\' and target_currency_id=\'"+target_currency_id+"\'";
		
			typedef tuple<unique_ptr<string>> c;

			vector<c> exchange_rate_ids;
			m_conn->runQuery(&exchange_rate_ids,get_exchange_rate_ids.c_str());
			if(exchange_rate_ids.empty())
				return "";
			for(const auto& i : exchange_rate_ids)
				return *(std::get<0>(i));
				
		}
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;m_conn=nullptr;
			return "";
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
			return "";
		}
	}
	void get_exchange_rate_id(general_rate_data& EUR_GBP_class,general_rate_data&EUR_CNY_class)
	{
		EUR_GBP_class.first_second_currency_exchange_rate_id=get_exchange_rate_id
		(EUR_GBP_class.first_currency_id,EUR_GBP_class.second_currency_id);
		EUR_GBP_class.second_first_currency_exchange_rate_id=get_exchange_rate_id
		(EUR_GBP_class.second_currency_id,EUR_GBP_class.first_currency_id);

		EUR_CNY_class.first_second_currency_exchange_rate_id=get_exchange_rate_id
		(EUR_CNY_class.first_currency_id,EUR_CNY_class.second_currency_id);
		EUR_CNY_class.second_first_currency_exchange_rate_id=get_exchange_rate_id
		(EUR_CNY_class.second_currency_id,EUR_CNY_class.first_currency_id);
		
	}
	void insert_exchange_rate(const string& exchange_rate_id,const string& rate)
	{
		if(exchange_rate_id.length()==0||rate.length()==0) return;
		ptime now = second_clock::local_time();
 			std::vector<std::string> ymd;
 			string today=to_iso_extended_string(now.date());
			boost::split(ymd,today , boost::is_any_of("-"));
 			//string [] ymd=to_iso_extended_string(now.date()).split('-');
 			string year=ymd[0];
 			string month=ymd[1];
 			string day=ymd[2];
			string p4 = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			
			string insert_sql = "insert into t_currency_daily_exchange_rate values('"+rand_string(20)+"',\'"+exchange_rate_id+"\',\'"+year+"\',\'"+month+"\',\'"+day+"\',"+rate+",\'"+to_iso_extended_string(now.date())+"\',\'"+p4+"\',\'exchange_gw\',\'"+p4+"\',\'exchange_gw\',\'\',\'\',0,1)";
			
			cout << insert_sql << endl;
			m_conn->runCommand(insert_sql.c_str());
			BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<insert_sql<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();
			
	}
	void update_rate_from_mysql(const string& from_usd_exchange_rate_id,const string& which_day,const string& ratio)
	{
		if(from_usd_exchange_rate_id.length()==0||ratio.length()==0) return;
 			std::vector<std::string> ymd;
 			
			boost::split(ymd,which_day , boost::is_any_of("-"));
 			//string [] ymd=to_iso_extended_string(now.date()).split('-');
 			string year=ymd[0];
 			string month=ymd[1];
 			string day=ymd[2];
			string p4 = which_day + " 00:00:05";
			
			string insert_sql = "insert into t_currency_daily_exchange_rate values('"+rand_string(20)+"',\'"+from_usd_exchange_rate_id+"\',\'"+year+"\',\'"+month+"\',\'"+day+"\',"+ratio+",\'"+which_day+"\',\'"+p4+"\',\'exchange_gw_rest\',\'"+p4+"\',\'exchange_gw_rest\',\'\',\'\',0,1)";
			
			cout << insert_sql << endl;
			m_conn->runCommand(insert_sql.c_str());
			BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<insert_sql<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();
			
	}
	void insert_exchange_rate(general_rate_data& EUR_GBP_class,general_rate_data&EUR_CNY_class)
	{
		insert_exchange_rate(EUR_GBP_class.first_second_currency_exchange_rate_id,EUR_GBP_class.first_second_exchange_rate);
		string temp=boost::lexical_cast<string>(1/boost::lexical_cast<float>(EUR_GBP_class.first_second_exchange_rate));
		insert_exchange_rate(EUR_GBP_class.second_first_currency_exchange_rate_id,temp);
		insert_exchange_rate(EUR_CNY_class.first_second_currency_exchange_rate_id,EUR_CNY_class.first_second_exchange_rate);
		temp=boost::lexical_cast<string>(1/boost::lexical_cast<float>(EUR_CNY_class.first_second_exchange_rate));
		insert_exchange_rate(EUR_CNY_class.second_first_currency_exchange_rate_id,temp);
	}
	void general_update()
	{
		try
		{
			// string code;//CAD
			// string to_usd_exchange_rate;//0.772558
			// string from_usd_exchange_rate;//0.772558
			// string currency_id;//J4YVQ3USQNO3U430EKE1
			// string to_usd_exchange_rate_id;//TFTBLZNSNBNAZAZGC2RW
			// string from_usd_exchange_rate_id;//TFTBLZNSNBNAZAZGC2RW
			float EUR_GBP=0,EUR_CNY=0; 
			float EUR_USD=0,USD_GBP=0;
			float USD_CNY=0;
// class general_rate_data
// {
// public:
// 	string first_code;
// 	string second_code;
// 	string first_currency_id;
// 	string second_currency_id;
// 	string first_second_currency_exchange_rate_id;
// 	string second_first_currency_exchange_rate_id;
// 	float first_second_exchange_rate;
// };
			general_rate_data EUR_GBP_class;
			general_rate_data EUR_CNY_class;
			for(auto& item :m_exchage_rate_data_array)
			{
				
				if(item.code=="EUR")
				{
					EUR_USD=boost::lexical_cast<float>(item.to_usd_exchange_rate);
					EUR_GBP_class.first_code=item.code;
					EUR_GBP_class.first_currency_id=item.currency_id;
					EUR_CNY_class.first_code=item.code;
					EUR_CNY_class.first_currency_id=item.currency_id;
				}
				else if(item.code=="GBP")
				{
					USD_GBP=boost::lexical_cast<float>(item.from_usd_exchange_rate);
					EUR_GBP_class.second_code=item.code;
					EUR_GBP_class.second_currency_id=item.currency_id;
				}
				else if(item.code=="CNY")
				{
					USD_CNY=boost::lexical_cast<float>(item.from_usd_exchange_rate);
					EUR_CNY_class.second_code=item.code;
					EUR_CNY_class.second_currency_id=item.currency_id;
				}
				//update_exchange_rate_to_mysql(item);
			}
			EUR_GBP=EUR_USD*USD_GBP;
			EUR_CNY=EUR_USD*USD_CNY;
			EUR_GBP_class.first_second_exchange_rate=boost::lexical_cast<string>(EUR_GBP);
			EUR_CNY_class.first_second_exchange_rate=boost::lexical_cast<string>(EUR_CNY);
			get_exchange_rate_id(EUR_GBP_class,EUR_CNY_class);
			EUR_GBP_class.print();
			EUR_CNY_class.print();
			insert_exchange_rate(EUR_GBP_class,EUR_CNY_class);
			
		}
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
	}
	void update_cny()
	{
		try
		{
			ptime now = second_clock::local_time();
 			
 			string today=to_iso_extended_string(now.date());
			
			// string code;//CAD
			// string to_usd_exchange_rate;//0.772558
			// string from_usd_exchange_rate;//0.772558
			// string currency_id;//J4YVQ3USQNO3U430EKE1
			// string to_usd_exchange_rate_id;//TFTBLZNSNBNAZAZGC2RW
			// string from_usd_exchange_rate_id;//TFTBLZNSNBNAZAZGC2RW
			
			float USD_CNY=0;
			float xxx_cny=0,cny_xxx=0;
			float xxx_usd;
			string source,target;
			//EUR_CNY=EUR_USD*USD_CNY;
			for(auto& item :m_exchage_rate_data_array)
			{	
				if(item.code=="CNY")
					USD_CNY=boost::lexical_cast<float>(item.from_usd_exchange_rate);
			}
			for(auto& item :m_exchage_rate_data_array)
			{	
				xxx_usd=boost::lexical_cast<float>(item.to_usd_exchange_rate);
				xxx_cny=xxx_usd*USD_CNY;
				cny_xxx=1/xxx_cny;
				// vector<string> v{"os","js","eu","as"};
				// boost::shared_ptr<exchange_rate_rest_client> t(new exchange_rate_rest_client(item.code,"CNY", boost::lexical_cast<string>(xxx_cny),today,"js"));
				// boost::shared_ptr<exchange_rate_rest_client> t2(new exchange_rate_rest_client("CNY",item.code, boost::lexical_cast<string>(cny_xxx),today,"js"));
				boost::shared_ptr<exchange_rate_rest> p(new exchange_rate_rest(m_conn));
				p->update_rate(item.code,"CNY",today,boost::lexical_cast<string>(xxx_cny));
				p->update_rate("CNY",item.code,today,boost::lexical_cast<string>(cny_xxx));
			}			
		}
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
	}
	void update_jpy()
	{
		try
		{
			if(get_config->m_exchange_rate_usd_jpy!="0")
			{
				ptime now = second_clock::local_time();
 			
 				string today=to_iso_extended_string(now.date());
				boost::shared_ptr<exchange_rate_rest> p(new exchange_rate_rest(m_conn));
				p->update_rate_put("USD","JPY",today,boost::lexical_cast<string>(get_config->m_exchange_rate_usd_jpy));
				p->update_rate_put("JPY","USD",today,boost::lexical_cast<string>(1/boost::lexical_cast<float>(get_config->m_exchange_rate_usd_jpy)));	
			}
				
		}
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
	}
	void update_from_usd_exchange_rate(const exchage_rate_data& item)
	{
		try
		{
			if(item.from_usd_exchange_rate_id.length()==0) return;
			
 			ptime now = second_clock::local_time();
 			std::vector<std::string> ymd;
 			string today=to_iso_extended_string(now.date());
			boost::split(ymd,today , boost::is_any_of("-"));
 			//string [] ymd=to_iso_extended_string(now.date()).split('-');
 			string year=ymd[0];
 			string month=ymd[1];
 			string day=ymd[2];
			string p4 = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
		
			if(item.from_usd_exchange_rate_id.length()==0||item.from_usd_exchange_rate.length()==0)
			{
				return;
			}
			string insert_sql = "insert into t_currency_daily_exchange_rate values('"+rand_string(20)+"',\'"+item.from_usd_exchange_rate_id+"\',\'"+year+"\',\'"+month+"\',\'"+day+"\',"+item.from_usd_exchange_rate+",\'"+to_iso_extended_string(now.date())+"\',\'"+p4+"\',\'exchange_gw\',\'"+p4+"\',\'exchange_gw\',\'\',\'\',0,1)";
			
			cout << insert_sql << endl;
			m_conn->runCommand(insert_sql.c_str());
			BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<insert_sql<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();
			
		}
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
		}
	}
	void update_exchange_rate_to_mysql(const exchage_rate_data& item)
	{
		try
		{
		
			if (item.to_usd_exchange_rate_id!="")
			{
				update_to_usd_exchange_rate(item);
			}
			
		
			if (item.from_usd_exchange_rate_id!="")
			{
				update_from_usd_exchange_rate(item);
			}

			
		}
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
	}
	void update_exchange_rate_to_mysql()
	{
		try
		{
			cout<<m_exchage_rate_data_array.size()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			for(auto& item :m_exchage_rate_data_array)
			{
				item.print();
				
				update_exchange_rate_to_mysql(item);
			}
		}	
		
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
			start_timer();
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
			start_timer();
		}
	}
	
	string get_exchange_rate_api_data()
	{
		//http://www.apilayer.net/api/live?access_key=beed451506493436d5a5ec0966b5e72a
		boost::shared_ptr<exchange_rate> rate = boost::shared_ptr<exchange_rate>(new exchange_rate("http://www.apilayer.net"));
				rate->request("GET", "/api/live", "access_key="+get_config->m_exchange_rate_key, "");
				//cout<<*(rate->m_data)<<":"<<__FILE__<<":"<<__LINE__<<endl;
				return *(rate->m_data);
	}
	string get_exchange_rate_from_yahoo()
	{
		//http://www.apilayer.net/api/live?access_key=beed451506493436d5a5ec0966b5e72a
		boost::shared_ptr<exchange_rate> rate = boost::shared_ptr<exchange_rate>(new exchange_rate(get_config->m_exchange_rate_url));
				rate->request("GET", "", "", "");
				cout<<*(rate->m_data)<<":"<<__FILE__<<":"<<__LINE__<<endl;

				return convert_json(*(rate->m_data));
	}
	string convert_json(const string& source)
	{
				//{
	//   "quotes":{
	//     "USDCNY":6.683198,
	//     "USDEUR":0.8993
	//   }
	// }
			//{"query":{"count":27,"created":"2016-07-12T09:16:05Z","lang":"zh-CN","results":{"rate":[{"id":"USDEUR","Rate":"0.9049"},{"id":"USDJPY","Rate":"103.4770"},{"id":"USDBGN","Rate":"1.7701"},{"id":"USDGBP","Rate":"0.7645"},{"id":"USDPLN","Rate":"4.0007"},{"id":"USDRON","Rate":"4.0594"},{"id":"USDSEK","Rate":"8.5780"},{"id":"USDHRK","Rate":"6.7240"},{"id":"USDRUB","Rate":"64.0240"},{"id":"USDTRY","Rate":"2.9010"},{"id":"USDAUD","Rate":"1.3181"},{"id":"USDBRL","Rate":"3.3098"},{"id":"USDCAD","Rate":"1.3097"},{"id":"USDCNY","Rate":"6.6917"},{"id":"USDHKD","Rate":"7.7575"},{"id":"USDIDR","Rate":"13080.0000"},{"id":"USDINR","Rate":"67.1380"},{"id":"USDKRW","Rate":"1148.3000"},{"id":"USDMXN","Rate":"18.4196"},{"id":"USDNZD","Rate":"1.3812"},{"id":"USDPHP","Rate":"47.2500"},{"id":"USDSGD","Rate":"1.3520"},{"id":"USDTHB","Rate":"35.1400"},{"id":"USDZAR","Rate":"14.4255"},{"id":"USDTWD","Rate":"32.1880"},{"id":"USDCNY","Rate":"6.6917"},{"id":"USDPHP","Rate":"47.2500"}]}}}
			
		try
		{
				const auto& j = nlohmann_map::json::parse(source);
			const auto& query = j["query"];//results":{"rate
			const auto& results =query["results"];
			if (results.find("rate") == results.end()) 
			{
				//LOG_ERROR<<"get json error from yahoo.finance.xchange:";
				return "";
			}
			const auto& rate=results["rate"];
			string temp="{\"quotes\":{";
			for(const auto& r:rate)
			{
				string id=r["id"];
				string Rate=r["Rate"];
				//cout<<id<<":"<<Rate<<endl;
				temp+="\""+id+"\":"+Rate+",";
			}
			string a=temp.substr(0,temp.length()-1);
			return a+"}}";
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)"<<":" << e.what()<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			return "";
		}
		catch(...)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)"<< "unknown error"<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();//cout<<item.code<<"unknown error"<<":"<<__FILE__<<":"<<__LINE__<<endl;
			return "";
		}
	}
	void get_exchange_rate()
	{
		try
		{
			////http://query.yahooapis.com/v1/public/yql?q=select id,Rate from yahoo.finance.xchange where pair in ("USDEUR", "USDJPY", "USDBGN", "USDGBP", "USDPLN", "USDRON", "USDSEK", "USDHRK", "USDRUB", "USDTRY", "USDAUD", "USDBRL", "USDCAD", "USDCNY", "USDHKD", "USDIDR", "USDINR", "USDKRW", "USDMXN", "USDNZD", "USDPHP", "USDSGD", "USDTHB", "USDZAR","USDTWD","USDCNY","USDPHP")&format=json&env=store://datatables.org/alltableswithkeys

//KRW TRY USD HKD SKW INR SGD GBP TWD JPY BGN CNY EUR SEK TRL ZAR THB MXP HRK ROL CAD RUR PHP IDR BRL AUD PLZ MXN NZD

			// RUR 对应雅虎的RUB
			// TRL 对应雅虎的TRY
			// MXP 对应雅虎的MXN
			// ROL 对应雅虎的RON
			// PLZ 对应雅虎的PLN

			if(m_exchage_rate_data_array.size()<5)
			{
				get_info_from_myql();
			}
			
			//SKW TRL RUR PLZ
			for(std::vector<exchage_rate_data>::iterator item=m_exchage_rate_data_array.begin();item!=m_exchage_rate_data_array.end();++item)
			{
				if(item->code=="SKW"||item->code=="TRL"||item->code=="RUR"||item->code=="PLZ"||item->code=="MXP"||item->code=="ROL"||item->code=="11")
				{
					m_exchage_rate_data_array.erase(item);
				}
			}


			//{
//   "success":true,
//   "terms":"https:\/\/currencylayer.com\/terms",
//   "privacy":"https:\/\/currencylayer.com\/privacy",
//   "timestamp":1468312932,
//   "source":"USD",
//   "quotes":{
//     "USDCNY":6.683198,
//     "USDEUR":0.8993
//   }
// }
			// 
			//http://www.apilayer.net/api/live?access_key=beed451506493436d5a5ec0966b5e72a
			//string exchange_rate=get_exchange_rate_api_data();
			string exchange_rate=get_exchange_rate_from_yahoo();
			//boost::this_thread::sleep(boost::posix_time::millisec(5000));
			cout<<exchange_rate<<":"<<__FILE__<<":"<<__LINE__<<endl;
			//BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<exchange_rate<<":"<<__FILE__<<":"<<__LINE__;
			BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<"get rate from curl"<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();
			//if(exchange_rate.length()==0)
			//	return;
			const auto& j = nlohmann_map::json::parse(exchange_rate);
			
			if (j.find("quotes") == j.end()) 
			{
				cout<<"get json error from apilayer:"<<__FILE__<<":"<<__LINE__<<endl;
				return;
			}
			const auto& quotes = j["quotes"];
			//cout<<quotes<<":"<<__FILE__<<":"<<__LINE__<<endl;
			//BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<quotes<<":"<<__FILE__<<":"<<__LINE__;
			BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<"get rate from curl"<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();

			for(auto& item :m_exchage_rate_data_array)
			{
				string usditem="USD"+item.code;
				//cout<<usditem<<":"<<__FILE__<<":"<<__LINE__<<endl;
				if (quotes.find(usditem) == quotes.end()) 
				{
  					cout<<item.code<<":"<<__FILE__<<":"<<__LINE__<<endl;
  					continue;
				}
				try
				{
	    			const auto& from_usd_exchange_rate=quotes[usditem];
	    			float temp_from_usd_exchange_rate=from_usd_exchange_rate;
	    			float temp_to_usd_exchange_rate=1/temp_from_usd_exchange_rate;
	    			item.from_usd_exchange_rate=boost::lexical_cast<string>(temp_from_usd_exchange_rate);
					item.to_usd_exchange_rate=boost::lexical_cast<string>(temp_to_usd_exchange_rate);
				}
				catch(std::exception& e)
				{
					BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)"<<item.code<<":" << e.what()<<":"<<__FILE__<<":"<<__LINE__;
					boost_log->get_initsink()->flush();cout<<item.code<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
					start_timer();
				}
				catch(...)
				{
					BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)"<<item.code<<":" << "unknown error"<<":"<<__FILE__<<":"<<__LINE__;
					boost_log->get_initsink()->flush();cout<<item.code<<"unknown error"<<":"<<__FILE__<<":"<<__LINE__<<endl;
					start_timer();
				}
				
    			
			}
			cout<<":"<<__FILE__<<":"<<__LINE__<<endl;
			update_exchange_rate_to_mysql();
		}
		
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;m_conn=nullptr;
			start_timer();
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
	}
	void start_timer()
	{
		m_d_t.expires_from_now(boost::posix_time::seconds(get_config->m_exchange_rate_request_interval));
		  
		m_d_t.async_wait(boost::bind(&exchange_rate_on_time::handle_wait, shared_from_this(), boost::asio::placeholders::error));
	}
	void start()
	{
		try
		{
		    m_d_t.expires_from_now(boost::posix_time::seconds(get_config->m_exchange_rate_request_interval));
		  
		    m_d_t.async_wait(boost::bind(&exchange_rate_on_time::handle_wait, shared_from_this(), boost::asio::placeholders::error));  
			m_io_s.run();
		}
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;
			boost_log->get_initsink()->flush();cout<<e.what()<<":"<<__FILE__<<":"<<__LINE__<<endl;
			start_timer();
		}
	}
	void handle_wait_method()
	{
		cout<<__FILE__<<":"<<__LINE__<<endl;
    	if(m_conn==nullptr)
    	{
    		cout<<__FILE__<<":"<<__LINE__<<endl;
    		m_conn=boost::shared_ptr<MySql>(new MySql(m_mysql_database.m_mysql_ip.c_str(), m_mysql_database.m_mysql_username.c_str(), m_mysql_database.m_mysql_password.c_str(), m_mysql_database.m_mysql_database.c_str(), m_mysql_database.m_mysql_port));
    		cout<<__FILE__<<":"<<__LINE__<<endl;
    	}
		start_update();
		general_update();
		update_cny();
		update_jpy();
		m_conn->close();
		m_conn=nullptr;
		boost::this_thread::sleep(boost::posix_time::millisec(30000));
	}
	void handle_wait(const boost::system::error_code& error)  
    {  
        if(!error)  
        {  
        	//BOOST_LOG_SEV(slg, severity_level::error) <<"(error:)" <<":"<<__FILE__<<":"<<__LINE__;
			//boost_log->get_initsink()->flush();

        	ptime now = second_clock::local_time();			
			string hour_minute_second = to_simple_string(now.time_of_day());
			std::vector<std::string> hms;
 			
			boost::split(hms,hour_minute_second , boost::is_any_of(":"));
 			//string [] ymd=to_iso_extended_string(now.date()).split('-');
 			string hour=hms[0];
 			string minute=hms[1];
 			
        	string hour_minute=hour+":"+minute;
        	if(hour_minute==get_config->m_exchange_rate_insert_time)
        	{   
        		if(m_mysql_database.m_mysql_database=="apollo_os")	
        		{
        			handle_wait_method();
        		}	
        		else if(m_mysql_database.m_mysql_database=="apollo_js")	
        		{
        			boost::this_thread::sleep(boost::posix_time::millisec(70000));
        			handle_wait_method();
        		}	
        		else if(m_mysql_database.m_mysql_database=="apollo_eu")	
        		{
        			boost::this_thread::sleep(boost::posix_time::millisec(120000));
        			handle_wait_method();
        		}
        		else if(m_mysql_database.m_mysql_database=="apollo_as")	
        		{
        			boost::this_thread::sleep(boost::posix_time::millisec(180000));
        			handle_wait_method();
        		}

        	}
        	
        	//cout<<"handle wait"<<endl;
            m_d_t.expires_from_now(boost::posix_time::seconds(get_config->m_exchange_rate_request_interval));  
            m_d_t.async_wait(boost::bind(&exchange_rate_on_time::handle_wait,shared_from_this(), boost::asio::placeholders::error));                 
    	}
   //  	else
   //  	{
   //  		boost::this_thread::sleep(boost::posix_time::millisec(10000));
   //  		start_timer();
   //  		BOOST_LOG_SEV(slg, severity_level::error) <<"(error:)" <<":"<<__FILE__<<":"<<__LINE__;
			// boost_log->get_initsink()->flush();
   //  	}   
	}  
	void start_update()
	{
		try
		{
			get_exchange_rate();
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what()<<":"<<__FILE__<<":"<<__LINE__;;
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
			start_timer();
		}
	}
	
	void insert_exchange_rate_id(const string& source,const string& target)
	{
		cout<<__FILE__<<":"<<__LINE__<<endl;
		string source_currency_id=get_currency_id(source);
		string target_currency_id=get_currency_id(target);
		if(source_currency_id.length()==0||target_currency_id.length()==0)
		{
			return;
		}
		ptime now = second_clock::local_time();
		string p4 = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());

		string insert_sql = "insert into t_currency_exchange_rate(currency_exchange_rate_id,source_currency_id,target_currency_id,calculation_method,decimal_digits,match_method,createAt,createBy,updateAt,updateBy,dr,data_version)values('"+rand_string(20)+"','"+source_currency_id+"','"+target_currency_id+"',0,7,0,'"+p4+"','','"+p4+"','',0,1)";
		cout << insert_sql << endl;
		m_conn->runCommand(insert_sql.c_str());
		BOOST_LOG_SEV(slg, boost_log->get_log_level()) <<insert_sql<<":"<<__FILE__<<":"<<__LINE__;
		boost_log->get_initsink()->flush();
	}
	string get_currency_id(const string& code)
	{
		try
		{
			if(code.length()==0) return "";
			cout<<__FILE__<<":"<<__LINE__<<endl;
			typedef tuple<unique_ptr<string>> t_currency_tuple;
		//select code,currency_id from t_currency
			//typedef tuple<string,double> credit_tuple;
			std::vector<t_currency_tuple> t_currency_tuple_vector;

			string query_sql = "select currency_id from t_currency where code='"+code+"'";
			cout << query_sql << endl;
			m_conn->runQuery(&t_currency_tuple_vector, query_sql.c_str());

			BOOST_LOG_SEV(slg, boost_log->get_log_level()) << query_sql;
			boost_log->get_initsink()->flush();
			
			if(t_currency_tuple_vector.empty())
			{
				BOOST_LOG_SEV(slg, boost_log->get_log_level()) << "nothing select from t_currency";
				boost_log->get_initsink()->flush();
				cout<<"nothing select from t_currency"<<endl;
				return "";
			}
			for (const auto& item : t_currency_tuple_vector)
			{
				return *(std::get<0>(item));						
			}
		}
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;//m_conn=nullptr;
			return "";
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
			return "";
		}
	}

private:
	boost::shared_ptr<MySql> m_conn;
	string m_today_string;
	boost::shared_ptr<string> m_product_all;
	std::stringstream m_ss;
	boost::asio::io_service m_io_s;  
	deadline_timer m_d_t;
	std::vector<exchage_rate_data> m_exchage_rate_data_array;
	exchage_rate_data m_usd_info;
	vector<general_rate_data> m_general_rate_data;

};

void start_exchange_rate_thread()
{
		mysql_database mysql_os;
		mysql_os.m_mysql_ip=get_config->m_mysql_ip;
		mysql_os.m_mysql_port=get_config->m_mysql_port;
		mysql_os.m_mysql_username=get_config->m_mysql_username;
		mysql_os.m_mysql_password=get_config->m_mysql_password;
		mysql_os.m_mysql_database=get_config->m_mysql_database;

		mysql_database mysql_js;
		mysql_js.m_mysql_ip=get_config->m_mysql_js_ip;
		mysql_js.m_mysql_port=get_config->m_mysql_js_port;
		mysql_js.m_mysql_username=get_config->m_mysql_js_username;
		mysql_js.m_mysql_password=get_config->m_mysql_js_password;
		mysql_js.m_mysql_database=get_config->m_mysql_js_database;

		mysql_database mysql_eu;
		mysql_eu.m_mysql_ip=get_config->m_mysql_eu_ip;
		mysql_eu.m_mysql_port=get_config->m_mysql_eu_port;
		mysql_eu.m_mysql_username=get_config->m_mysql_eu_username;
		mysql_eu.m_mysql_password=get_config->m_mysql_eu_password;
		mysql_eu.m_mysql_database=get_config->m_mysql_eu_database;

		mysql_database mysql_as;
		mysql_as.m_mysql_ip=get_config->m_mysql_as_ip;
		mysql_as.m_mysql_port=get_config->m_mysql_as_port;
		mysql_as.m_mysql_username=get_config->m_mysql_as_username;
		mysql_as.m_mysql_password=get_config->m_mysql_as_password;
		mysql_as.m_mysql_database=get_config->m_mysql_as_database;

		boost::shared_ptr<exchange_rate_on_time> producer_exchange_rate_on_time_os(new exchange_rate_on_time(mysql_os));
		boost::shared_ptr<exchange_rate_on_time> producer_exchange_rate_on_time_js(new exchange_rate_on_time(mysql_js));
		boost::shared_ptr<exchange_rate_on_time> producer_exchange_rate_on_time_eu(new exchange_rate_on_time(mysql_eu));
		boost::shared_ptr<exchange_rate_on_time> producer_exchange_rate_on_time_as(new exchange_rate_on_time(mysql_as));
		
		thread producer_exchange_rate_on_time_os_Thread([&producer_exchange_rate_on_time_os](){producer_exchange_rate_on_time_os->start();});
		thread producer_exchange_rate_on_time_js_Thread([&producer_exchange_rate_on_time_js](){producer_exchange_rate_on_time_js->start();});
		thread producer_exchange_rate_on_time_eu_Thread([&producer_exchange_rate_on_time_eu](){producer_exchange_rate_on_time_eu->start();});
		thread producer_exchange_rate_on_time_as_Thread([&producer_exchange_rate_on_time_as](){producer_exchange_rate_on_time_as->start();});
		producer_exchange_rate_on_time_os_Thread.join();
		producer_exchange_rate_on_time_js_Thread.join();
		producer_exchange_rate_on_time_eu_Thread.join();
		producer_exchange_rate_on_time_as_Thread.join();
}
#endif

