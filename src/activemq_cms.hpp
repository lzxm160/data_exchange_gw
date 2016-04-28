#ifndef ACTIVEMQ_CMS_HPP
#define	ACTIVEMQ_CMS_HPP

#include  "include.hpp"


#include <activemq/transport/DefaultTransportListener.h>
#include <decaf/lang/Integer.h>
#include <activemq/util/Config.h>
#include <decaf/util/Date.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Long.h>
#include <decaf/util/Date.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <activemq/library/ActiveMQCPP.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>

using namespace activemq;
using namespace activemq::core;
using namespace decaf;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;
//using namespace cms::Session;
// using namespace cms::Connection;
// using namespace cms::Destination;
// using namespace cms::MessageProducer;
////////////////////////////////////////////////////////////////////////////////
class activemq_cms_producer : public Runnable
{
private:
	friend class boost::signals2::deconstruct_access;  // give boost::signals2::deconstruct access to private constructor
    // private constructor forces use of boost::signals2::deconstruct to create objects.
    Connection* connection;
    cms::Session* session;
    Destination* destination;
   	MessageProducer* producer;
    bool useTopic;
    bool clientAck;
    unsigned int numMessages;
    std::string brokerURI;
    std::string destURI;
    std::string m_messages;
private:

    activemq_cms_producer( const activemq_cms_producer& );
    activemq_cms_producer& operator= ( const activemq_cms_producer& );

public:

/* This adl_predestruct friend function will be found by
    via argument-dependent lookup when using boost::signals2::deconstruct. */
    friend void adl_predestruct(activemq_cms_producer *)
    {
      std::cout << "Goodbye, ";
    }
    /* boost::signals2::deconstruct always requires an adl_postconstruct function
    which can be found via argument-dependent, so we define one which does nothing. */
    template<typename T> friend
      void adl_postconstruct(const boost::shared_ptr<T> &, activemq_cms_producer *)
    {}


    activemq_cms_producer(const std::string& message, const std::string& brokerURI, unsigned int numMessages,
                    const std::string& destURI, bool useTopic = false, bool clientAck = false ) :
        connection(NULL),
        session(NULL),
        destination(NULL),
        producer(NULL),
        useTopic(useTopic),
        clientAck(clientAck),
        numMessages(numMessages),
        brokerURI(brokerURI),
        destURI(destURI),m_messages(message) 
        {
        	// cout<<brokerURI<<endl;
        	// cout<<destURI<<endl;
        }

    virtual ~activemq_cms_producer()
    {
    	//cout<<"~activemq_cms_producer"<<endl;
        cleanup();
    }

    void close() 
    {
    	//cout<<"producer close"<<endl;
        this->cleanup();
    }

    virtual void run() 
    {
        try {

            // Create a ConnectionFactory
            boost::shared_ptr<ActiveMQConnectionFactory> connectionFactory(
                new ActiveMQConnectionFactory( brokerURI ) );

            // Create a Connection
            try
            {
                connection = connectionFactory->createConnection();
                connection->start();
            } 
            catch( CMSException& e ) 
            {
                e.printStackTrace();
                throw e;
            }

            // Create a Session
            if( clientAck ) 
            {
                session = connection->createSession( cms::Session::CLIENT_ACKNOWLEDGE );
            } 
            else 
            {
                session = connection->createSession( cms::Session::AUTO_ACKNOWLEDGE );
            }

            // Create the destination (Topic or Queue)
            if( useTopic ) 
            {
                destination = session->createTopic( destURI );
            } 
            else 
            {
                destination = session->createQueue( destURI );
            }

            // Create a MessageProducer from the Session to the Topic or Queue
            producer = session->createProducer( destination );
            producer->setDeliveryMode( DeliveryMode::PERSISTENT );
            //producer->setTimeToLive(10000);
            //producer->setPriority(5);
            // Create the Thread Id String
            string threadIdStr = Long::toString( Thread::currentThread()->getId() );

            // Create a messages
            //string text = (string)"Hello world! from thread " + threadIdStr;

            for( unsigned int ix=0; ix<numMessages; ++ix )
            {
                boost::shared_ptr<TextMessage> message(session->createTextMessage( m_messages ));

                // message->setIntProperty( "Integer", ix );
                // message->setStringProperty("testkey","testvalue");
                // Tell the producer to send the message
                printf( "Sent message #%d from thread %s\n", ix+1, threadIdStr.c_str() );
                BOOST_LOG_SEV(slg, severity_level::error) <<"(message send to activemq:)" << m_messages;
				boost_log->get_initsink()->flush();
                producer->send( message.get() );

                //delete message;
            }

        }
        // catch ( CMSException& e ) 
        // {
        //     e.printStackTrace();
        // } 
        catch(json_parser_error& e) 
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
				boost_log->get_initsink()->flush();
				cout<<e.what()<<endl;
		}
		catch (CMSException& e) 
        {
            BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
        }
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
		}
    }

private:

    void cleanup()
    {

        // Destroy resources.
        try
        {
            if( destination != NULL )
            {
            	delete destination;
            	cout<<__FILE__<<":"<<__LINE__<<":activemq_cms_producer delete destination"<<endl;
            } 
        }
        catch ( CMSException& e ) 
        { 
        	e.printStackTrace(); 
        }
        destination = NULL;

        try
        {
            if( producer != NULL )
            {
            	delete producer;
            	cout<<__FILE__<<":"<<__LINE__<<":activemq_cms_producer delete producer"<<endl;
            } 
        }
        catch ( CMSException& e )
        {
          e.printStackTrace();
        }
        producer = NULL;

        // Close open resources.
        try
        {
            if( session != NULL ) session->close();
            if( connection != NULL ) connection->close();
        }
        catch ( CMSException& e ) 
        {
         	e.printStackTrace(); 
     	}

        try
        {
            if( session != NULL )
            {
            	delete session;
            	cout<<__FILE__<<":"<<__LINE__<<":activemq_cms_producer delete session"<<endl;
            } 
        }
        catch ( CMSException& e ) 
        { 
        	e.printStackTrace(); 
        }
        session = NULL;

        try
        {
            if( connection != NULL )
            {
            	delete connection;
            	cout<<__FILE__<<":"<<__LINE__<<":activemq_cms_producer delete connection"<<endl;
            } 
        }
        catch ( CMSException& e ) 
        { 
        	e.printStackTrace(); 
        }
        connection = NULL;
    }
};

class activemq_cms_consumer : public ExceptionListener,
                            public MessageListener,
                            public activemq::transport::DefaultTransportListener {
private:

    Connection* connection;
    cms::Session* session;
    Destination* destination;
    MessageConsumer* consumer;
    bool useTopic;
    std::string brokerURI;
    std::string destURI;
    bool clientAck;
    std::stringstream m_ss;
    std::stringstream m_ss_tomq;
private:

    activemq_cms_consumer( const activemq_cms_consumer& );
    activemq_cms_consumer& operator= ( const activemq_cms_consumer& );

public:

    activemq_cms_consumer( const std::string& brokerURI,
                         const std::string& destURI,
                         bool useTopic = false,
                         bool clientAck = false ) :
        connection(NULL),
        session(NULL),
        destination(NULL),
        consumer(NULL),
        useTopic(useTopic),
        brokerURI(brokerURI),
        destURI(destURI),
        clientAck(clientAck) {
    }

    virtual ~activemq_cms_consumer() {
        this->cleanup();
    }

    void close() {
        this->cleanup();
    }

    void runConsumer() {

        try {

            // Create a ConnectionFactory
            ActiveMQConnectionFactory* connectionFactory =
                new ActiveMQConnectionFactory( brokerURI );

            // Create a Connection
            connection = connectionFactory->createConnection();
            delete connectionFactory;

            ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>( connection );
            if( amqConnection != NULL ) {
                amqConnection->addTransportListener( this );
            }

            connection->start();

            connection->setExceptionListener(this);

            // Create a Session
            if( clientAck ) {
                session = connection->createSession( cms::Session::CLIENT_ACKNOWLEDGE );
            } else {
                session = connection->createSession( cms::Session::AUTO_ACKNOWLEDGE );
            }

            // Create the destination (Topic or Queue)
            if( useTopic ) {
                destination = session->createTopic( destURI );
            } else {
                destination = session->createQueue( destURI );
            }

            // Create a MessageConsumer from the Session to the Topic or Queue
            consumer = session->createConsumer( destination );
            consumer->setMessageListener( this );

        } catch (CMSException& e) {
            e.printStackTrace();
        }
    }

    // Called from the consumer since this class is a registered MessageListener.
    virtual void onMessage( const Message* message ) {

        static int count = 0;

        try
        {
            //count++;
            const TextMessage* textMessage =
                dynamic_cast< const TextMessage* >( message );
            string text = "";

            if( textMessage != NULL ) {
                text = textMessage->getText();
            } else {
                text = "NOT A TEXTMESSAGE!";
            }

            if( clientAck ) {
                message->acknowledge();
            }
            decode_request_orderbot(text);
            cout<<"Message Received:"<<text<<endl;
        } catch (CMSException& e) {
            e.printStackTrace();
        }
    }

    // If something bad happens you see it here as this class is also been
    // registered as an ExceptionListener with the connection.
    virtual void onException( const CMSException& ex AMQCPP_UNUSED ) {
        printf("CMS Exception occurred.  Shutting down client.\n");
        exit(1);
    }

    virtual void transportInterrupted() {
        std::cout << "The Connection's Transport has been Interrupted." << std::endl;
    }

    virtual void transportResumed() {
        std::cout << "The Connection's Transport has been Restored." << std::endl;
    }
	void parser_json_write_ss(const string& texts)
    {
        string text="{\"sales_order_id\" : \"\",\"so_no\" : \"\",\"po_no\" : \"\",\"status\" : 0,\"order_date\" : \"\",\"company_id\" : \"\",\"sales_id\" : \"\",\"currency_id\" : \"\",\"ss_currency_daily_exchange_rate\" : 6.45,\"tax_schedule_id\" : \"\",\"ss_tax_rate\" : 7.2,\"customer_master_id\" : \"\",\"customer_contact_id\" : \"\",\"customer_invoice_address_id\" : \"\",\"ship_to_customer_name\" : \"\",\"ship_to_address\" : \"\",\"ship_to_state\" : \"\",\"ship_to_city\" : \"\",\"ship_to_zip_code\" : \"\",\"ship_to_contact_name\" : \"\",\"ship_to_contact_phone_number\" : \"\",\"ship_to_contact_email\" : \"\",\"trade_term_id\" : \"\",\"ss_landed_cost_coefficient\" : 3.3,\"dispatch_warehouse_id\" : \"\",\"requested_delivery_date\" : \"\",\"promotion_code\" : \"\",\"company_bank_account_id\" : \"\",\"shipping_cost_total\" : 25.48,\"saving_total\" : 3.56,\"tax_total\" : 22.51,\"sub_total\" : 180.37,\"grand_total\" : 218.67,\"note\" : \"\",   \"detail\" : [{\"sales_order_detail_id\" : \"\",\"item_master_id\" : \"\",\"ss_guidance_price\" : 5.46,\"ss_promotion_price\" : 5.41,\"unit_price\" : 5.43,\"uom_id\" : \"\",\"quantity\" : 12,\"sub_total\" : 63.67,\"sub_tax\" : 4.33,\"sub_shipping_cost\" : 5.68,\"sub_discount\" : 0.0,\"note\" : \"\"}]}";
            ptree pt,ret_json_all;
            ptree return_json;
            std::istringstream is(text);
            read_json(is, pt);

            string sales_order_id=pt.get<string>("sales_order_id");

            string so_no=pt.get<string>("so_no");
            string po_no=pt.get<string>("po_no");
            size_t status=pt.get<size_t>("status");
            string order_date=pt.get<string>("order_date");
            string company_id=pt.get<string>("company_id");
            string sales_id=pt.get<string>("sales_id");
            string currency_id=pt.get<string>("currency_id");
            double ss_currency_daily_exchange_rate=pt.get<double>("ss_currency_daily_exchange_rate");
            string tax_schedule_id=pt.get<string>("tax_schedule_id");
            double ss_tax_rate=pt.get<double>("ss_tax_rate");
            string customer_master_id=pt.get<string>("customer_master_id");
            string customer_contact_id=pt.get<string>("customer_contact_id");
            string customer_invoice_address_id=pt.get<string>("customer_invoice_address_id");
            string ship_to_customer_name=pt.get<string>("ship_to_customer_name");
            string ship_to_address=pt.get<string>("ship_to_address");
            string ship_to_state=pt.get<string>("ship_to_state");
            string ship_to_city=pt.get<string>("ship_to_city");
            string ship_to_zip_code=pt.get<string>("ship_to_zip_code");
            string ship_to_contact_name=pt.get<string>("ship_to_contact_name");
            string ship_to_contact_phone_number=pt.get<string>("ship_to_contact_phone_number");
            string ship_to_contact_email=pt.get<string>("ship_to_contact_email");
            string trade_term_id=pt.get<string>("trade_term_id");
            double ss_landed_cost_coefficient=pt.get<double>("ss_landed_cost_coefficient");
            string dispatch_warehouse_id=pt.get<string>("dispatch_warehouse_id");
            string requested_delivery_date=pt.get<string>("requested_delivery_date");
            string promotion_code=pt.get<string>("promotion_code");
            string company_bank_account_id=pt.get<string>("company_bank_account_id");
            double shipping_cost_total=pt.get<double>("shipping_cost_total");
            double saving_total=pt.get<double>("saving_total");
            double tax_total=pt.get<double>("tax_total");
            double sub_total=pt.get<double>("sub_total");
            double grand_total=pt.get<double>("grand_total");
            string note=pt.get<string>("note");
            ptree detail_child = pt.get_child("detail");
///////////////////////////////////////////////////////////////////////////
            ret_json_all.put<std::string>("order_date",order_date);
            ret_json_all.put<std::string>("ship_date",requested_delivery_date);
            ret_json_all.put<std::string>("orderbot_customer_id","need get from orderbot");//need get from orderbot
            ret_json_all.put<std::string>("reference_customer_id",customer_master_id);
            ret_json_all.put<std::string>("reference_order_id",sales_order_id);
            ret_json_all.put<std::string>("customer_po", sales_order_id);
            
            //"0", "OrderByCustomer", "1", "OrderBysales","2","Canceled","3","UnConfirmed"); 
            if(status==0||status==1)
            {
               ret_json_all.put<std::string>("order_status", "unshipped");
            }
            else if(status==2)
            {
                ret_json_all.put<std::string>("order_status", "do_not_ship");
            }
            else
            {
                ret_json_all.put<std::string>("order_status", "unconfirmed");
            }  
            ret_json_all.put<std::string>("order_notes", note);
            ret_json_all.put<std::string>("internal_notes", "test internal");
            ret_json_all.put<std::string>("bill_third_party", false);
            ret_json_all.put<std::string>("distribution_center_id", "null");//need get from orderbot
            ret_json_all.put<std::string>("account_group_id", "null");//need get from orderbot
            ret_json_all.put<int>("order_guide_id", 0);//need get from orderbot
            ret_json_all.put<std::string>("insure_packages", false);//not sure
            ret_json_all.put<std::string>("shipping_code", "A1");//need get from orderbot
            ret_json_all.put<std::string>("email_confirmation_address", "test@orderbot.com");
            ret_json_all.put<std::string>("subtotal", sub_total);
            ret_json_all.put<std::string>("shipping", shipping_cost_total);
            ret_json_all.put<std::string>("order_discount", 0);
            ret_json_all.put<std::string>("order_total", grand_total);

            ptree shipping_tax;
            shipping_tax.put<std::string>("tax_name","TAX");
            shipping_tax.put<double>("tax_rate",0.05);
            shipping_tax.put<double>("amount",0.15);
              
            //ret_json_all.push_back(std::make_pair("shipping_tax", shipping_tax));


            ptree shipping_address;
            shipping_address.put<std::string>("store_name", "Test Store");
            shipping_address.put<std::string>("first_name", ship_to_contact_name);
            shipping_address.put<std::string>("last_name", "x");
            shipping_address.put<std::string>("address1", ship_to_address);
            shipping_address.put<std::string>("address2", "");
            shipping_address.put<std::string>("city", ship_to_city);
            shipping_address.put<std::string>("state", ship_to_state);
            shipping_address.put<std::string>("postal_code",ship_to_zip_code);
            shipping_address.put<std::string>("country", "US");
            shipping_address.put<std::string>("phone_number", ship_to_contact_phone_number);
            shipping_address.put<std::string>("email",ship_to_contact_email);
            ret_json_all.push_back(std::make_pair("shipping_address", shipping_address));

            ptree billing_address;
            billing_address.put<std::string>("account_name", "Test Store");
            billing_address.put<std::string>("first_name", ship_to_contact_name);
            billing_address.put<std::string>("last_name", "x");
            billing_address.put<std::string>("address1", ship_to_address);
            billing_address.put<std::string>("address2", "");
            billing_address.put<std::string>("city", ship_to_city);
            billing_address.put<std::string>("state", ship_to_state);
            billing_address.put<std::string>("postal_code", ship_to_zip_code);
            billing_address.put<std::string>("country", "US");
            billing_address.put<std::string>("phone_number", ship_to_contact_phone_number);
            billing_address.put<std::string>("email", shipping_address);
            ret_json_all.push_back(std::make_pair("billing_address", billing_address));


            for(auto& sub:detail_child)
            {
                string sales_order_detail_id=sub.get<string>("sales_order_detail_id");
                string item_master_id=sub.get<string>("item_master_id");
                double ss_guidance_price=sub.get<double>("ss_guidance_price");
                double ss_promotion_price=sub.get<double>("ss_promotion_price");
                double unit_price=sub.get<double>("unit_price");
                string uom_id=sub.get<string>("uom_id");
                size_t quantity=sub.get<size_t>("quantity");
                double sub_total=sub.get<double>("sub_total");
                double sub_tax=sub.get<double>("sub_tax");
                double sub_shipping_cost=sub.get<double>("sub_shipping_cost");
                double sub_discount=sub.get<double>("sub_discount");
                string note=sub.get<string>("note");

                ptree order_lines;

                try 
                {
                    order_lines.put<int>("line_number", boost::lexical_cast<int>(sales_order_detail_id));
                }
                catch(boost::bad_lexical_cast& e) 
                {
                   BOOST_LOG_SEV(slg, severity_level::error) <<__FILE__<<":"<<__LINE__<<":(exception:)" << e.what();
                    boost_log->get_initsink()->flush();
                    cout<<__FILE__<<":"<<__LINE__<<":"<<e.what()<<endl; 
                   order_lines.put<int>("line_number",0);
                }

                
                order_lines.put<std::string>("product_sku", "123");
                order_lines.put<std::string>("custom_description", note);
                order_lines.put<size_t>("quantity", quantity);
                order_lines.put<double>("price", unit_price);
                order_lines.put<double>("product_discount",sub_discount);
                ptree product_taxes;
                product_taxes.put<std::string>("tax_name", "TAX");
                product_taxes.put<double>("tax_rate",sub_tax/sub_total);
                product_taxes.put<double>("amount", quantity);

                order_lines.push_back(std::make_pair("product_taxes", product_taxes));

                ret_json_all.push_back(std::make_pair("order_lines", order_lines));

            }
                return_json.push_back(std::make_pair("product", ret_json_all));
                write_json(m_ss, return_json,false);
                cout<<m_ss.str()<<":"<<__FILE__<<":"<<__LINE__<<endl;
    }
    void decode_request_orderbot(const string& texts)
    {
    	try
    	{
            parser_json_write_ss(texts);

			boost::shared_ptr<orderbot> order = boost::shared_ptr<orderbot>(new orderbot(get_config->m_orderbot_username, get_config->m_orderbot_password, get_config->m_orderbot_url));
			order->request("POST", "/admin/orders.json/", "", m_ss.str());

			// return_json.put<std::string>("orders1",*(order->m_data));
			
			// write_json(m_ss,return_json);
            // 
	        m_ss_tomq<<*(order->m_data);
    		send_message_to_activemq();
			cout<<__FILE__<<":"<<__LINE__<<endl;   

    	}
    	catch(json_parser_error& e) 
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
				boost_log->get_initsink()->flush();
				cout<<e.what()<<endl;
		}
		catch (CMSException& e) 
        {
            BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
        }
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
		}
    }
    void send_message_to_activemq()
	{
		try
		{
			cout<<__FILE__<<":"<<__LINE__<<endl;
			string message(m_ss_tomq.str());
			message.erase(remove(message.begin(), message.end(), '\n'), message.end());
			//activemq::library::ActiveMQCPP::initializeLibrary();
			std::string brokerURI =
		        "failover://(tcp://"+get_config->m_activemq_url+
		       // "?wireFormat=openwire"
		       // "&connection.useAsyncSend=true"
		       // "&transport.commandTracingEnabled=true"
		       // "&transport.tcpTracingEnabled=true"
		       // "&wireFormat.tightEncodingEnabled=true"
		        ")";

		    bool useTopics = false;

		    boost::shared_ptr<activemq_cms_producer> producer(new activemq_cms_producer(message,brokerURI, 1, get_config->m_activemq_write_order_queue, useTopics,true ));

		    producer->run();

		    producer->close();
			cout<<__FILE__<<":"<<__LINE__<<endl;
		    //activemq::library::ActiveMQCPP::shutdownLibrary();
	    }
	    catch(json_parser_error& e) 
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
				boost_log->get_initsink()->flush();
				cout<<e.what()<<endl;
		}
		catch (CMSException& e) 
        {
            BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
        }
		catch (const MySqlException& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
		}
		catch(std::exception& e)
		{
			BOOST_LOG_SEV(slg, severity_level::error) <<"(exception:)" << e.what();
			boost_log->get_initsink()->flush();cout<<e.what()<<endl;
		}
	}
private:

    void cleanup(){

        //*************************************************
        // Always close destination, consumers and producers before
        // you destroy their sessions and connection.
        //*************************************************

        // Destroy resources.
        try{
            if( destination != NULL ) delete destination;
        }catch (CMSException& e) {}
        destination = NULL;

        try{
            if( consumer != NULL ) delete consumer;
        }catch (CMSException& e) {}
        consumer = NULL;

        // Close open resources.
        try{
            if( session != NULL ) session->close();
            if( connection != NULL ) connection->close();
        }catch (CMSException& e) {}

        // Now Destroy them
        try{
            if( session != NULL ) delete session;
        }catch (CMSException& e) {}
        session = NULL;

        try{
            if( connection != NULL ) delete connection;
        }catch (CMSException& e) {}
        connection = NULL;
    }
};
#endif

