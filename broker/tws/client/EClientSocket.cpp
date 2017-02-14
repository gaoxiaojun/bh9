/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
* and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#include "EClientSocket.h"

#include "TwsSocketClientErrors.h"
#include "EWrapper.h"
#include <boost/bind.hpp>
#include <string.h>
#include <assert.h>
#include <ostream>
#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>
#include <boost/numeric/conversion/cast.hpp>

using boost::asio::ip::tcp;

///////////////////////////////////////////////////////////
// member funcs
EClientSocket::EClientSocket(boost::asio::io_service &ios, EWrapper *ptr)
    : EClient( ptr)
    , m_ios(ios)
    , m_socket(ios)
    , m_resolver(ios)
    , m_reconnect_timer(ios)
{

}

EClientSocket::~EClientSocket()
{

}

bool EClientSocket::eConnect( const std::string& host, unsigned int port, int clientId, bool extraAuth)
{
    if (isConnecting())
        return true;
    // already connected?
    if( isConnected()) {
		getWrapper()->error( NO_VALID_ID, ALREADY_CONNECTED.code(), ALREADY_CONNECTED.msg());
		return false;
	}

    setConnState(CS_CONNECTING);

    // initialize host and port
    setHost(host);
    setPort(port);
    setClientId(clientId);
    setExtraAuth(extraAuth);
	
    tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));
    m_resolver.async_resolve(query,
                             boost::bind(&EClientSocket::handle_resolve,
                               this, boost::asio::placeholders::error,
                               boost::asio::placeholders::iterator));

    return true;
}

void EClientSocket::eDisconnect()
{
    boost::system::error_code ignore_ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignore_ec);
    m_socket.close();

    eDisconnectBase();
}

void EClientSocket::handle_timer(const boost::system::error_code &ec)
{
    if (!ec && isDisconncted()) {
        m_ios.post(boost::bind(&EClientSocket::eConnect, this, m_host, m_port, m_clientId, m_extraAuth));
        StartTimer();
    }
}

void EClientSocket::StartTimer()
{
    m_reconnect_timer.cancel();
    if (m_reconnect_secs > 0) {
        m_reconnect_timer.expires_from_now(boost::posix_time::seconds(m_reconnect_secs));
        m_reconnect_timer.async_wait(boost::bind(&EClientSocket::handle_timer, this,
                                                 boost::asio::placeholders::error));
    }
}

void EClientSocket::setReconnectSeconds(int seconds)
{
    if (m_reconnect_secs != seconds) {
        m_reconnect_secs = seconds;
        StartTimer();
    }
}

void EClientSocket::on_redirect(const std::string &host, int port, int clientId, bool extraAuth)
{
    //std::cout << "on_redirect" << std::endl;
    m_ios.post(boost::bind(&EClientSocket::eConnect, this, host, port, clientId, extraAuth));
}

void EClientSocket::on_handshake_ack()
{
    //std::cout << "on_handshake_ack" << std::endl;
}

void EClientSocket::on_send(const std::string& msg)
{
    //std::cout << "send: " << msg << std::endl;
    bool write_in_progress = !wqueue.empty();
    wqueue.push_back(msg);

    if (!write_in_progress) {
    boost::asio::async_write(m_socket, boost::asio::buffer(msg),
                             boost::bind(&EClientSocket::handle_write, this, boost::asio::placeholders::error,
                             boost::asio::placeholders::bytes_transferred)
                             );
    }
}

void EClientSocket::handle_resolve(const boost::system::error_code& err,
    tcp::resolver::iterator endpoint_iterator)
{
  if (!err)
  {
    // Attempt a connection to the first endpoint in the list. Each endpoint
    // will be tried until we successfully establish a connection.
    tcp::endpoint endpoint = *endpoint_iterator;
    m_socket.async_connect(endpoint,
        boost::bind(&EClientSocket::handle_connect, this,
          boost::asio::placeholders::error, ++endpoint_iterator));
  }
  else
  {
    handle_error(err);
  }
}

const int MAX_BUF_LEN = 1024*1024; // 1M bytes
const int MIN_MSG_LEN = 4;

void EClientSocket::handle_connect(const boost::system::error_code& err,
    tcp::resolver::iterator endpoint_iterator)
{
    //std::cout << "handle_connect" << " error:" << err << std::endl;
  if (!err)
  {
      boost::asio::ip::tcp::no_delay option ( true );
      m_socket.set_option ( option );
      sendConnectRequest();
      StartAsyncRead();
  }
  else if (endpoint_iterator != tcp::resolver::iterator())
  {
    // The connection failed. Try the next endpoint in the list.
    m_socket.close();
    tcp::endpoint endpoint = *endpoint_iterator;
    m_socket.async_connect(endpoint,
        boost::bind(&EClientSocket::handle_connect, this,
          boost::asio::placeholders::error, ++endpoint_iterator));
  }
  else
  {
    handle_error(err);
  }
}

void EClientSocket::StartAsyncRead()
{
    //std::cout << "StartAyncRead" << std::endl;
    boost::asio::streambuf::mutable_buffers_type mutableBuffer =
          m_in_buffer.prepare(MAX_BUF_LEN);
    m_socket.async_read_some(boost::asio::buffer(mutableBuffer),
                             //boost::asio::transfer_at_least(MIN_MSG_LEN),
                             boost::bind(&EClientSocket::handle_read, this,
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred));
}

void EClientSocket::handle_read(const boost::system::error_code& error, size_t len)
{
    //std::cout << "read:" << len << " error:" << error << std::endl;
    if(!error) {
        m_in_buffer.commit(len);
        boost::asio::streambuf::const_buffers_type buf = m_in_buffer.data();
        std::size_t size = boost::asio::buffer_size(buf);
        const char* begin = boost::asio::buffer_cast<const char*>(buf);
        const char* end = begin + size;
        //std::cout << "buf: " <<  " buf size:" << m_in_buffer.size() << std::endl;
        //std::cout << "begin:" << begin << " end:" << end << " size:" << size << std::endl;
        int processed = 0;
        do {
            processed = processMsg(begin, end);
            m_in_buffer.consume(boost::numeric_cast<std::size_t>(processed));
        }while(processed > 0);

        StartAsyncRead();
    }
    else {
        handle_error(error);
    }
}

void EClientSocket::handle_write(const boost::system::error_code& error, size_t)
{
    if (!error)
    {
      wqueue.pop_front();
      if (!wqueue.empty())
      {
        boost::asio::async_write(m_socket,
            boost::asio::buffer(wqueue.front().data(),
              wqueue.front().length()),
            boost::bind(&EClientSocket::handle_write, this,
              boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
      }
    }
    else
    {
      handle_error(error);
    }
}

void EClientSocket::handle_error(const boost::system::error_code& ec)
{
    getWrapper()->error( NO_VALID_ID, SOCKET_EXCEPTION.code(),
            SOCKET_EXCEPTION.msg() + ec.message());
    eDisconnect();
    getWrapper()->connectionClosed();
}
