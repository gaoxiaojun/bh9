/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
* and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#pragma once
#ifndef eposixclientsocket_def
#define eposixclientsocket_def

#include "EClient.h"
#include "econfig.h"
#include <boost/asio.hpp>
#include <deque>
#include <string>

class EWrapper;
class EReaderSignal;

class TWS_API EClientSocket : public EClient
{
public:

    explicit EClientSocket(boost::asio::io_service &ios, EWrapper *ptr);
	~EClientSocket();

    // override virtual from EClient
    bool eConnect( const std::string& host, unsigned int port, int clientId = 0, bool extraAuth = false);
    void eDisconnect() override;

public:
    int reconnectSeconds() const { return m_reconnect_secs; }
    void setReconnectSeconds(int seconds);

    boost::asio::io_service& get_io_service() { return m_ios; }

protected:
    // override virtual from EClient
    void on_send(const std::string& msg) override;
    void on_handshake_ack() override;
    void on_redirect(const std::string& host, int port, int clientId, bool extraAuth) override;

protected:
    void handle_write(const boost::system::error_code &error, size_t);
    void handle_read(const boost::system::error_code &error, size_t);
    void handle_error(const boost::system::error_code &ec);
    void handle_resolve(const boost::system::error_code &err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
    void handle_connect(const boost::system::error_code &err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
    void StartAsyncRead();
    void handle_timer(const boost::system::error_code &ec);
    void StartTimer();
private:

	void onClose();

private:

    boost::asio::io_service& m_ios;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ip::tcp::resolver m_resolver;

    boost::asio::streambuf m_in_buffer;
    //boost::asio::streambuf m_out_buffer;
    std::deque<std::string> wqueue;

    boost::asio::deadline_timer m_reconnect_timer;

    int m_reconnect_secs;
};

#endif
