/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
* and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#include "EClient.h"
#include "EWrapper.h"
#include "TwsSocketClientErrors.h"
#include "Contract.h"
#include "Order.h"
#include "Execution.h"
#include "ScannerSubscription.h"
#include "CommissionReport.h"
#include "econstants.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits>

#include <boost/asio.hpp>
using ::boost::asio::detail::socket_ops::network_to_host_short;
using ::boost::asio::detail::socket_ops::network_to_host_long;
using ::boost::asio::detail::socket_ops::host_to_network_short;
using ::boost::asio::detail::socket_ops::host_to_network_long;

#include <boost/assert.hpp>

using namespace ibapi::client_constants;

const int MIN_SERVER_VER_SUPPORTED    = 38; //all supported server versions are defined in EDecoder.h

// helper structures
namespace {

struct BarData {
    std::string date;
    double open;
    double high;
    double low;
    double close;
    int volume;
    double average;
    std::string hasGaps;
    int barCount;
};

struct ScanData {
    ContractDetails contract;
    int rank;
    std::string distance;
    std::string benchmark;
    std::string projection;
    std::string legsStr;
};

} // end of anonymous namespace

///////////////////////////////////////////////////////////
// utility funcs
static std::string errMsg(std::exception e) {
    // return the error associated with this exception
    return std::string(e.what());
}

///////////////////////////////////////////////////////////
// encoders


template<>
void EClient::EncodeField<bool>(std::ostream& os, bool boolValue)
{
    EncodeField<int>(os, boolValue ? 1 : 0);
}

template<>
void EClient::EncodeField<double>(std::ostream& os, double doubleValue)
{
    char str[128];

    snprintf(str, sizeof(str), "%.10g", doubleValue);

    EncodeField<const char*>(os, str);

    //os << std::setprecision(10) << doubleValue << std::ends;
}

///////////////////////////////////////////////////////////
// "max" encoders
void EClient::EncodeFieldMax(std::ostream& os, int intValue)
{
    if( intValue == INT_MAX) { //std::numeric_limits<int>::max()) {
        EncodeField(os, "");
        return;
    }
    EncodeField(os, intValue);
}

void EClient::EncodeFieldMax(std::ostream& os, double doubleValue)
{
    if( doubleValue == DBL_MAX ) { //std::numeric_limits<double>::max()) {
        EncodeField(os, "");
        return;
    }
    EncodeField(os, doubleValue);
}

bool EClient::CheckOffset(const char* ptr, const char* endPtr)
{
    assert(ptr && ptr <= endPtr);
    return (ptr && ptr < endPtr);
}

const char* EClient::FindFieldEnd(const char* ptr, const char* endPtr)
{
    return static_cast<const char*>(memchr(ptr, 0, endPtr - ptr));
}

bool EClient::DecodeField(bool& boolValue, const char*& ptr, const char* endPtr)
{
    int intValue;
    if( !DecodeField(intValue, ptr, endPtr))
        return false;
    boolValue = (intValue > 0);
    return true;
}

bool EClient::DecodeField(int& intValue, const char*& ptr, const char* endPtr)
{
    if( !CheckOffset(ptr, endPtr))
        return false;
    const char* fieldBeg = ptr;
    const char* fieldEnd = FindFieldEnd(fieldBeg, endPtr);
    if( !fieldEnd)
        return false;
    intValue = atoi(fieldBeg);
    ptr = ++fieldEnd;
    return true;
}

bool EClient::DecodeField(long& longValue, const char*& ptr, const char* endPtr)
{
    int intValue;
    if( !DecodeField(intValue, ptr, endPtr))
        return false;
    longValue = intValue;
    return true;
}

bool EClient::DecodeField(double& doubleValue, const char*& ptr, const char* endPtr)
{
    if( !CheckOffset(ptr, endPtr))
        return false;
    const char* fieldBeg = ptr;
    const char* fieldEnd = FindFieldEnd(fieldBeg, endPtr);
    if( !fieldEnd)
        return false;
    doubleValue = atof(fieldBeg);
    ptr = ++fieldEnd;
    return true;
}

bool EClient::DecodeField(std::string& stringValue,
                           const char*& ptr, const char* endPtr)
{
    if( !CheckOffset(ptr, endPtr))
        return false;
    const char* fieldBeg = ptr;
    const char* fieldEnd = FindFieldEnd(ptr, endPtr);
    if( !fieldEnd)
        return false;
    stringValue = fieldBeg; // better way?
    ptr = ++fieldEnd;
    return true;
}

bool EClient::DecodeFieldMax(int& intValue, const char*& ptr, const char* endPtr)
{
    std::string stringValue;
    if( !DecodeField(stringValue, ptr, endPtr))
        return false;
    intValue = stringValue.empty() ? UNSET_INTEGER : atoi(stringValue.c_str());
    return true;
}

bool EClient::DecodeFieldMax(long& longValue, const char*& ptr, const char* endPtr)
{
    int intValue;
    if( !DecodeFieldMax(intValue, ptr, endPtr))
        return false;
    longValue = intValue;
    return true;
}

bool EClient::DecodeFieldMax(double& doubleValue, const char*& ptr, const char* endPtr)
{
    std::string stringValue;
    if( !DecodeField(stringValue, ptr, endPtr))
        return false;
    doubleValue = stringValue.empty() ? UNSET_DOUBLE : atof(stringValue.c_str());
    return true;
}
///////////////////////////////////////////////////////////
// member funcs
EClient::EClient(EWrapper *ptr)
	: m_pEWrapper(ptr)
	, m_clientId(-1)
	, m_extraAuth(false)
	, m_serverVersion(0)
    , m_useV100Plus(true)
    , m_allowRedirect(false)
    , m_connState(CS_DISCONNECTED)
{
}

EClient::~EClient()
{
}

EClient::ConnState EClient::connState() const
{
	return m_connState;
}

void EClient::eConnectBase()
{
}

void EClient::eDisconnectBase()
{
	m_TwsTime.clear();
	m_serverVersion = 0;
	m_connState = CS_DISCONNECTED;
    //m_extraAuth = false;
    //m_clientId = -1;
}

void EClient::setConnectOptions(const std::string& connectOptions)
{
    if( isConnecting() || isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, ALREADY_CONNECTED.code(), ALREADY_CONNECTED.msg());
		return;
	}

	m_connectOptions = connectOptions;
}

void EClient::disableUseV100Plus()
{
    if( isConnecting() || isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, ALREADY_CONNECTED.code(), ALREADY_CONNECTED.msg());
		return;
	}

	m_useV100Plus = false;
	m_connectOptions = "";
}

void EClient::setAllowRedirect(bool allow)
{
    m_allowRedirect = allow;
}

void EClient::reqMktData(TickerId tickerId, const Contract& contract,
                         const std::string& genericTicks, bool snapshot, const TagValueListSPtr& mktDataOptions)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// not needed anymore validation
	//if( m_serverVersion < MIN_SERVER_VER_SNAPSHOT_MKT_DATA && snapshot) {
	//	m_pEWrapper->error( tickerId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support snapshot market data requests.");
	//	return;
	//}

	if( m_serverVersion < MIN_SERVER_VER_UNDER_COMP) {
		if( contract.underComp) {
			m_pEWrapper->error( tickerId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support delta-neutral orders.");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_REQ_MKT_DATA_CONID) {
		if( contract.conId > 0) {
			m_pEWrapper->error( tickerId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support conId parameter.");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_TRADING_CLASS) {
		if( !contract.tradingClass.empty() ) {
			m_pEWrapper->error( tickerId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support tradingClass parameter in reqMktData.");
			return;
		}
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 11;

	// send req mkt data msg
	ENCODE_FIELD( REQ_MKT_DATA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	// send contract fields
	if( m_serverVersion >= MIN_SERVER_VER_REQ_MKT_DATA_CONID) {
		ENCODE_FIELD( contract.conId);
	}
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.lastTradeDateOrContractMonth);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier); // srv v15 and above

	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.primaryExchange); // srv v14 and above
	ENCODE_FIELD( contract.currency);

	ENCODE_FIELD( contract.localSymbol); // srv v2 and above

	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.tradingClass);
	}

	// Send combo legs for BAG requests (srv v8 and above)
	if( contract.secType == "BAG")
	{
		const Contract::ComboLegList* const comboLegs = contract.comboLegs.get();
		const int comboLegsCount = comboLegs ? comboLegs->size() : 0;
		ENCODE_FIELD( comboLegsCount);
		if( comboLegsCount > 0) {
			for( int i = 0; i < comboLegsCount; ++i) {
				const ComboLeg* comboLeg = ((*comboLegs)[i]).get();
				assert( comboLeg);
				ENCODE_FIELD( comboLeg->conId);
				ENCODE_FIELD( comboLeg->ratio);
				ENCODE_FIELD( comboLeg->action);
				ENCODE_FIELD( comboLeg->exchange);
			}
		}
	}

	if( m_serverVersion >= MIN_SERVER_VER_UNDER_COMP) {
		if( contract.underComp) {
			const UnderComp& underComp = *contract.underComp;
			ENCODE_FIELD( true);
			ENCODE_FIELD( underComp.conId);
			ENCODE_FIELD( underComp.delta);
			ENCODE_FIELD( underComp.price);
		}
		else {
			ENCODE_FIELD( false);
		}
	}

	ENCODE_FIELD( genericTicks); // srv v31 and above
	ENCODE_FIELD( snapshot); // srv v35 and above
std::cout << " Options" << std::endl;
	// send mktDataOptions parameter
	if( m_serverVersion >= MIN_SERVER_VER_LINKING) {
		std::string mktDataOptionsStr("");
		const int mktDataOptionsCount = mktDataOptions.get() ? mktDataOptions->size() : 0;
		if( mktDataOptionsCount > 0) {
			for( int i = 0; i < mktDataOptionsCount; ++i) {
				const TagValue* tagValue = ((*mktDataOptions)[i]).get();
				mktDataOptionsStr += tagValue->tag;
				mktDataOptionsStr += "=";
				mktDataOptionsStr += tagValue->value;
				mktDataOptionsStr += ";";
			}
		}
		ENCODE_FIELD( mktDataOptionsStr);
	}

std::cout << " reqMktEnd" << std::endl;
	closeAndSend( msg.str());
}

void EClient::cancelMktData(TickerId tickerId)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 2;

	// send cancel mkt data msg
	ENCODE_FIELD( CANCEL_MKT_DATA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	closeAndSend( msg.str());
}

void EClient::reqMktDepth( TickerId tickerId, const Contract& contract, int numRows, const TagValueListSPtr& mktDepthOptions)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	// This feature is only available for versions of TWS >=6
	//if( m_serverVersion < 6) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}

	if (m_serverVersion < MIN_SERVER_VER_TRADING_CLASS) {
		if( !contract.tradingClass.empty() || (contract.conId > 0)) {
			m_pEWrapper->error( tickerId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support conId and tradingClass parameters in reqMktDepth.");
			return;
		}
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 5;

	// send req mkt data msg
	ENCODE_FIELD( REQ_MKT_DEPTH);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	// send contract fields
	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.conId);
	}
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.lastTradeDateOrContractMonth);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier); // srv v15 and above
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol);
	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.tradingClass);
	}

	ENCODE_FIELD( numRows); // srv v19 and above

	// send mktDepthOptions parameter
	if( m_serverVersion >= MIN_SERVER_VER_LINKING) {
		std::string mktDepthOptionsStr("");
		const int mktDepthOptionsCount = mktDepthOptions.get() ? mktDepthOptions->size() : 0;
		if( mktDepthOptionsCount > 0) {
			for( int i = 0; i < mktDepthOptionsCount; ++i) {
				const TagValue* tagValue = ((*mktDepthOptions)[i]).get();
				mktDepthOptionsStr += tagValue->tag;
				mktDepthOptionsStr += "=";
				mktDepthOptionsStr += tagValue->value;
				mktDepthOptionsStr += ";";
			}
		}
		ENCODE_FIELD( mktDepthOptionsStr);
	}

	closeAndSend( msg.str());
}


void EClient::cancelMktDepth( TickerId tickerId)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	// This feature is only available for versions of TWS >=6
	//if( m_serverVersion < 6) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	// send cancel mkt data msg
	ENCODE_FIELD( CANCEL_MKT_DEPTH);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	closeAndSend( msg.str());
}

void EClient::reqHistoricalData( TickerId tickerId, const Contract& contract,
										  const std::string& endDateTime, const std::string& durationStr,
										  const std::string&  barSizeSetting, const std::string& whatToShow,
										  int useRTH, int formatDate, const TagValueListSPtr& chartOptions)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 16) {
	//	m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}

	if (m_serverVersion < MIN_SERVER_VER_TRADING_CLASS) {
		if( !contract.tradingClass.empty() || (contract.conId > 0)) {
			m_pEWrapper->error( tickerId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support conId and tradingClass parameters in reqHistoricalData.");
			return;
		}
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 6;

	ENCODE_FIELD( REQ_HISTORICAL_DATA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	// send contract fields
	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.conId);
	}
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.lastTradeDateOrContractMonth);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier);
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.primaryExchange);
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol);
	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.tradingClass);
	}
	ENCODE_FIELD( contract.includeExpired); // srv v31 and above

	ENCODE_FIELD( endDateTime); // srv v20 and above
	ENCODE_FIELD( barSizeSetting); // srv v20 and above

	ENCODE_FIELD( durationStr);
	ENCODE_FIELD( useRTH);
	ENCODE_FIELD( whatToShow);
	ENCODE_FIELD( formatDate); // srv v16 and above

	// Send combo legs for BAG requests
	if( contract.secType == "BAG")
	{
		const Contract::ComboLegList* const comboLegs = contract.comboLegs.get();
		const int comboLegsCount = comboLegs ? comboLegs->size() : 0;
		ENCODE_FIELD( comboLegsCount);
		if( comboLegsCount > 0) {
			for( int i = 0; i < comboLegsCount; ++i) {
				const ComboLeg* comboLeg = ((*comboLegs)[i]).get();
				assert( comboLeg);
				ENCODE_FIELD( comboLeg->conId);
				ENCODE_FIELD( comboLeg->ratio);
				ENCODE_FIELD( comboLeg->action);
				ENCODE_FIELD( comboLeg->exchange);
			}
		}
	}

	// send chartOptions parameter
	if( m_serverVersion >= MIN_SERVER_VER_LINKING) {
		std::string chartOptionsStr("");
		const int chartOptionsCount = chartOptions.get() ? chartOptions->size() : 0;
		if( chartOptionsCount > 0) {
			for( int i = 0; i < chartOptionsCount; ++i) {
				const TagValue* tagValue = ((*chartOptions)[i]).get();
				chartOptionsStr += tagValue->tag;
				chartOptionsStr += "=";
				chartOptionsStr += tagValue->value;
				chartOptionsStr += ";";
			}
		}
		ENCODE_FIELD( chartOptionsStr);
	}

	closeAndSend( msg.str());
}

void EClient::cancelHistoricalData(TickerId tickerId)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 24) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support historical data query cancellation.");
	//	return;
	//}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( CANCEL_HISTORICAL_DATA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	closeAndSend( msg.str());
}

void EClient::reqRealTimeBars(TickerId tickerId, const Contract& contract,
										int barSize, const std::string& whatToShow, bool useRTH,
										const TagValueListSPtr& realTimeBarsOptions)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < MIN_SERVER_VER_REAL_TIME_BARS) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support real time bars.");
	//	return;
	//}

	if (m_serverVersion < MIN_SERVER_VER_TRADING_CLASS) {
		if( !contract.tradingClass.empty() || (contract.conId > 0)) {
			m_pEWrapper->error( tickerId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support conId and tradingClass parameters in reqRealTimeBars.");
			return;
		}
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 3;

	ENCODE_FIELD( REQ_REAL_TIME_BARS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	// send contract fields
	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.conId);
	}
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.lastTradeDateOrContractMonth);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier);
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.primaryExchange);
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol);
	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.tradingClass);
	}
	ENCODE_FIELD( barSize);
	ENCODE_FIELD( whatToShow);
	ENCODE_FIELD( useRTH);

	// send realTimeBarsOptions parameter
	if( m_serverVersion >= MIN_SERVER_VER_LINKING) {
		std::string realTimeBarsOptionsStr("");
		const int realTimeBarsOptionsCount = realTimeBarsOptions.get() ? realTimeBarsOptions->size() : 0;
		if( realTimeBarsOptionsCount > 0) {
			for( int i = 0; i < realTimeBarsOptionsCount; ++i) {
				const TagValue* tagValue = ((*realTimeBarsOptions)[i]).get();
				realTimeBarsOptionsStr += tagValue->tag;
				realTimeBarsOptionsStr += "=";
				realTimeBarsOptionsStr += tagValue->value;
				realTimeBarsOptionsStr += ";";
			}
		}
		ENCODE_FIELD( realTimeBarsOptionsStr);
	}

	closeAndSend( msg.str());
}


void EClient::cancelRealTimeBars(TickerId tickerId)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < MIN_SERVER_VER_REAL_TIME_BARS) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support realtime bar data query cancellation.");
	//	return;
	//}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( CANCEL_REAL_TIME_BARS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	closeAndSend( msg.str());
}


void EClient::reqScannerParameters()
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 24) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support API scanner subscription.");
	//	return;
	//}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( REQ_SCANNER_PARAMETERS);
	ENCODE_FIELD( VERSION);

	closeAndSend( msg.str());
}


void EClient::reqScannerSubscription(int tickerId,
											   const ScannerSubscription& subscription, const TagValueListSPtr& scannerSubscriptionOptions)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 24) {
	//	m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support API scanner subscription.");
	//	return;
	//}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 4;

	ENCODE_FIELD( REQ_SCANNER_SUBSCRIPTION);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);
	ENCODE_FIELD_MAX( subscription.numberOfRows);
	ENCODE_FIELD( subscription.instrument);
	ENCODE_FIELD( subscription.locationCode);
	ENCODE_FIELD( subscription.scanCode);
	ENCODE_FIELD_MAX( subscription.abovePrice);
	ENCODE_FIELD_MAX( subscription.belowPrice);
	ENCODE_FIELD_MAX( subscription.aboveVolume);
	ENCODE_FIELD_MAX( subscription.marketCapAbove);
	ENCODE_FIELD_MAX( subscription.marketCapBelow);
	ENCODE_FIELD( subscription.moodyRatingAbove);
	ENCODE_FIELD( subscription.moodyRatingBelow);
	ENCODE_FIELD( subscription.spRatingAbove);
	ENCODE_FIELD( subscription.spRatingBelow);
	ENCODE_FIELD( subscription.maturityDateAbove);
	ENCODE_FIELD( subscription.maturityDateBelow);
	ENCODE_FIELD_MAX( subscription.couponRateAbove);
	ENCODE_FIELD_MAX( subscription.couponRateBelow);
	ENCODE_FIELD_MAX( subscription.excludeConvertible);
	ENCODE_FIELD_MAX( subscription.averageOptionVolumeAbove); // srv v25 and above
	ENCODE_FIELD( subscription.scannerSettingPairs); // srv v25 and above
	ENCODE_FIELD( subscription.stockTypeFilter); // srv v27 and above

	// send scannerSubscriptionOptions parameter
	if( m_serverVersion >= MIN_SERVER_VER_LINKING) {
		std::string scannerSubscriptionOptionsStr("");
		const int scannerSubscriptionOptionsCount = scannerSubscriptionOptions.get() ? scannerSubscriptionOptions->size() : 0;
		if( scannerSubscriptionOptionsCount > 0) {
			for( int i = 0; i < scannerSubscriptionOptionsCount; ++i) {
				const TagValue* tagValue = ((*scannerSubscriptionOptions)[i]).get();
				scannerSubscriptionOptionsStr += tagValue->tag;
				scannerSubscriptionOptionsStr += "=";
				scannerSubscriptionOptionsStr += tagValue->value;
				scannerSubscriptionOptionsStr += ";";
			}
		}
		ENCODE_FIELD( scannerSubscriptionOptionsStr);
	}

	closeAndSend( msg.str());
}

void EClient::cancelScannerSubscription(int tickerId)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 24) {
	//	m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support API scanner subscription.");
	//	return;
	//}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( CANCEL_SCANNER_SUBSCRIPTION);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	closeAndSend( msg.str());
}

void EClient::reqFundamentalData(TickerId reqId, const Contract& contract, 
										   const std::string& reportType)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( reqId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_FUNDAMENTAL_DATA) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support fundamental data requests.");
		return;
	}

	if (m_serverVersion < MIN_SERVER_VER_TRADING_CLASS) {
		if( contract.conId > 0) {
			m_pEWrapper->error( reqId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support conId parameter in reqFundamentalData.");
			return;
		}
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 2;

	ENCODE_FIELD( REQ_FUNDAMENTAL_DATA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);

	// send contract fields
	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.conId);
	}
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.primaryExchange);
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol);

	ENCODE_FIELD( reportType);

	closeAndSend( msg.str());
}

void EClient::cancelFundamentalData( TickerId reqId)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( reqId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_FUNDAMENTAL_DATA) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support fundamental data requests.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( CANCEL_FUNDAMENTAL_DATA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);

	closeAndSend( msg.str());
}

void EClient::calculateImpliedVolatility(TickerId reqId, const Contract& contract, double optionPrice, double underPrice) {

	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if (m_serverVersion < MIN_SERVER_VER_REQ_CALC_IMPLIED_VOLAT) {
		m_pEWrapper->error( reqId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support calculate implied volatility requests.");
		return;
	}

	if (m_serverVersion < MIN_SERVER_VER_TRADING_CLASS) {
		if( !contract.tradingClass.empty()) {
			m_pEWrapper->error( reqId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support tradingClass parameter in calculateImpliedVolatility.");
			return;
		}
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 2;

	ENCODE_FIELD( REQ_CALC_IMPLIED_VOLAT);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);

	// send contract fields
	ENCODE_FIELD( contract.conId);
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.lastTradeDateOrContractMonth);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier);
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.primaryExchange);
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol);
	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.tradingClass);
	}

	ENCODE_FIELD( optionPrice);
	ENCODE_FIELD( underPrice);

	closeAndSend( msg.str());
}

void EClient::cancelCalculateImpliedVolatility(TickerId reqId) {

	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if (m_serverVersion < MIN_SERVER_VER_CANCEL_CALC_IMPLIED_VOLAT) {
		m_pEWrapper->error( reqId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support calculate implied volatility cancellation.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( CANCEL_CALC_IMPLIED_VOLAT);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);

	closeAndSend( msg.str());
}

void EClient::calculateOptionPrice(TickerId reqId, const Contract& contract, double volatility, double underPrice) {

	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if (m_serverVersion < MIN_SERVER_VER_REQ_CALC_OPTION_PRICE) {
		m_pEWrapper->error( reqId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support calculate option price requests.");
		return;
	}

	if (m_serverVersion < MIN_SERVER_VER_TRADING_CLASS) {
		if( !contract.tradingClass.empty()) {
			m_pEWrapper->error( reqId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support tradingClass parameter in calculateOptionPrice.");
			return;
		}
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 2;

	ENCODE_FIELD( REQ_CALC_OPTION_PRICE);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);

	// send contract fields
	ENCODE_FIELD( contract.conId);
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.lastTradeDateOrContractMonth);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier);
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.primaryExchange);
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol);
	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.tradingClass);
	}

	ENCODE_FIELD( volatility);
	ENCODE_FIELD( underPrice);

	closeAndSend( msg.str());
}

void EClient::cancelCalculateOptionPrice(TickerId reqId) {

	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if (m_serverVersion < MIN_SERVER_VER_CANCEL_CALC_OPTION_PRICE) {
		m_pEWrapper->error( reqId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support calculate option price cancellation.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( CANCEL_CALC_OPTION_PRICE);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);

	closeAndSend( msg.str());
}

void EClient::reqContractDetails( int reqId, const Contract& contract)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	// This feature is only available for versions of TWS >=4
	//if( m_serverVersion < 4) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}
	if (m_serverVersion < MIN_SERVER_VER_SEC_ID_TYPE) {
		if( !contract.secIdType.empty() || !contract.secId.empty()) {
			m_pEWrapper->error( reqId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support secIdType and secId parameters.");
			return;
		}
	}
	if (m_serverVersion < MIN_SERVER_VER_TRADING_CLASS) {
		if( !contract.tradingClass.empty()) {
			m_pEWrapper->error( reqId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support tradingClass parameter in reqContractDetails.");
			return;
		}
	}
	if (m_serverVersion < MIN_SERVER_VER_LINKING) {
		if (!contract.primaryExchange.empty()) {
			m_pEWrapper->error( reqId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support primaryExchange parameter in reqContractDetails.");
			return;
		}
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 8;

	// send req mkt data msg
	ENCODE_FIELD( REQ_CONTRACT_DATA);
	ENCODE_FIELD( VERSION);

	if( m_serverVersion >= MIN_SERVER_VER_CONTRACT_DATA_CHAIN) {
		ENCODE_FIELD( reqId);
	}

	// send contract fields
	ENCODE_FIELD( contract.conId); // srv v37 and above
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.lastTradeDateOrContractMonth);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier); // srv v15 and above

	if (m_serverVersion >= MIN_SERVER_VER_PRIMARYEXCH)
	{
		ENCODE_FIELD(contract.exchange);
		ENCODE_FIELD(contract.primaryExchange);
	}
	else if (m_serverVersion >= MIN_SERVER_VER_LINKING)
	{
		if (!contract.primaryExchange.empty() && (contract.exchange == "BEST" || contract.exchange == "SMART"))
		{
			ENCODE_FIELD( contract.exchange + ":" + contract.primaryExchange);
		}
		else
		{
			ENCODE_FIELD(contract.exchange);
		}
	}

	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol);
	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.tradingClass);
	}
	ENCODE_FIELD( contract.includeExpired); // srv v31 and above

	if( m_serverVersion >= MIN_SERVER_VER_SEC_ID_TYPE){
		ENCODE_FIELD( contract.secIdType);
		ENCODE_FIELD( contract.secId);
	}

	closeAndSend( msg.str());
}

void EClient::reqCurrentTime()
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	// This feature is only available for versions of TWS >= 33
	//if( m_serverVersion < 33) {
	//	m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support current time requests.");
	//	return;
	//}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	// send current time req
	ENCODE_FIELD( REQ_CURRENT_TIME);
	ENCODE_FIELD( VERSION);

	closeAndSend( msg.str());
}

void EClient::placeOrder( OrderId id, const Contract& contract, const Order& order)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( id, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < MIN_SERVER_VER_SCALE_ORDERS) {
	//	if( order.scaleNumComponents != UNSET_INTEGER ||
	//		order.scaleComponentSize != UNSET_INTEGER ||
	//		order.scalePriceIncrement != UNSET_DOUBLE) {
	//		m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//			"  It does not support Scale orders.");
	//		return;
	//	}
	//}
	//
	//if( m_serverVersion < MIN_SERVER_VER_SSHORT_COMBO_LEGS) {
	//	if( contract.comboLegs && !contract.comboLegs->empty()) {
	//		typedef Contract::ComboLegList ComboLegList;
	//		const ComboLegList& comboLegs = *contract.comboLegs;
	//		ComboLegList::const_iterator iter = comboLegs.begin();
	//		const ComboLegList::const_iterator iterEnd = comboLegs.end();
	//		for( ; iter != iterEnd; ++iter) {
	//			const ComboLeg* comboLeg = *iter;
	//			assert( comboLeg);
	//			if( comboLeg->shortSaleSlot != 0 ||
	//				!comboLeg->designatedLocation.IsEmpty()) {
	//				m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//					"  It does not support SSHORT flag for combo legs.");
	//				return;
	//			}
	//		}
	//	}
	//}
	//
	//if( m_serverVersion < MIN_SERVER_VER_WHAT_IF_ORDERS) {
	//	if( order.whatIf) {
	//		m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//			"  It does not support what-if orders.");
	//		return;
	//	}
	//}

	if( m_serverVersion < MIN_SERVER_VER_UNDER_COMP) {
		if( contract.underComp) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support delta-neutral orders.");
			return;
		}
	}

	if( m_serverVersion < MIN_SERVER_VER_SCALE_ORDERS2) {
		if( order.scaleSubsLevelSize != UNSET_INTEGER) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support Subsequent Level Size for Scale orders.");
			return;
		}
	}

	if( m_serverVersion < MIN_SERVER_VER_ALGO_ORDERS) {

		if( !order.algoStrategy.empty()) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support algo orders.");
			return;
		}
	}

	if( m_serverVersion < MIN_SERVER_VER_NOT_HELD) {
		if (order.notHeld) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support notHeld parameter.");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_SEC_ID_TYPE) {
		if( !contract.secIdType.empty() || !contract.secId.empty()) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support secIdType and secId parameters.");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_PLACE_ORDER_CONID) {
		if( contract.conId > 0) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support conId parameter.");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_SSHORTX) {
		if( order.exemptCode != -1) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support exemptCode parameter.");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_SSHORTX) {
		const Contract::ComboLegList* const comboLegs = contract.comboLegs.get();
		const int comboLegsCount = comboLegs ? comboLegs->size() : 0;
		for( int i = 0; i < comboLegsCount; ++i) {
			const ComboLeg* comboLeg = ((*comboLegs)[i]).get();
			assert( comboLeg);
			if( comboLeg->exemptCode != -1 ){
				m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
					"  It does not support exemptCode parameter.");
				return;
			}
		}
	}

	if( m_serverVersion < MIN_SERVER_VER_HEDGE_ORDERS) {
		if( !order.hedgeType.empty()) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support hedge orders.");
			return;
		}
	}

	if( m_serverVersion < MIN_SERVER_VER_OPT_OUT_SMART_ROUTING) {
		if (order.optOutSmartRouting) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support optOutSmartRouting parameter.");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_DELTA_NEUTRAL_CONID) {
		if (order.deltaNeutralConId > 0 
			|| !order.deltaNeutralSettlingFirm.empty()
			|| !order.deltaNeutralClearingAccount.empty()
			|| !order.deltaNeutralClearingIntent.empty()
			) {
				m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
					"  It does not support deltaNeutral parameters: ConId, SettlingFirm, ClearingAccount, ClearingIntent.");
				return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_DELTA_NEUTRAL_OPEN_CLOSE) {
		if (!order.deltaNeutralOpenClose.empty()
			|| order.deltaNeutralShortSale
			|| order.deltaNeutralShortSaleSlot > 0 
			|| !order.deltaNeutralDesignatedLocation.empty()
			) {
				m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() + 
					"  It does not support deltaNeutral parameters: OpenClose, ShortSale, ShortSaleSlot, DesignatedLocation.");
				return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_SCALE_ORDERS3) {
		if (order.scalePriceIncrement > 0 && order.scalePriceIncrement != UNSET_DOUBLE) {
			if (order.scalePriceAdjustValue != UNSET_DOUBLE 
				|| order.scalePriceAdjustInterval != UNSET_INTEGER 
				|| order.scaleProfitOffset != UNSET_DOUBLE 
				|| order.scaleAutoReset 
				|| order.scaleInitPosition != UNSET_INTEGER 
				|| order.scaleInitFillQty != UNSET_INTEGER 
				|| order.scaleRandomPercent) {
					m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
						"  It does not support Scale order parameters: PriceAdjustValue, PriceAdjustInterval, " +
						"ProfitOffset, AutoReset, InitPosition, InitFillQty and RandomPercent");
					return;
			}
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_ORDER_COMBO_LEGS_PRICE && contract.secType == "BAG") {
		const Order::OrderComboLegList* const orderComboLegs = order.orderComboLegs.get();
		const int orderComboLegsCount = orderComboLegs ? orderComboLegs->size() : 0;
		for( int i = 0; i < orderComboLegsCount; ++i) {
			const OrderComboLeg* orderComboLeg = ((*orderComboLegs)[i]).get();
			assert( orderComboLeg);
			if( orderComboLeg->price != UNSET_DOUBLE) {
				m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
					"  It does not support per-leg prices for order combo legs.");
				return;
			}
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_TRAILING_PERCENT) {
		if (order.trailingPercent != UNSET_DOUBLE) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support trailing percent parameter");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_TRADING_CLASS) {
		if( !contract.tradingClass.empty()) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support tradingClass parameter in placeOrder.");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_SCALE_TABLE) {
		if( !order.scaleTable.empty() || !order.activeStartTime.empty() || !order.activeStopTime.empty()) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support scaleTable, activeStartTime and activeStopTime parameters");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_ALGO_ID) {
		if( !order.algoId.empty()) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support algoId parameter");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_ORDER_SOLICITED) {
		if (order.solicited) {
			m_pEWrapper->error(id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support order solicited parameter.");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_MODELS_SUPPORT) {
		if( !order.modelCode.empty()) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support model code parameter.");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_EXT_OPERATOR) {
		if( !order.extOperator.empty()) {
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support ext operator parameter");
			return;
		}
	}

	if (m_serverVersion < MIN_SERVER_VER_SOFT_DOLLAR_TIER) 
	{
		if (!order.softDollarTier.name().empty() || !order.softDollarTier.val().empty())
		{
			m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				" It does not support soft dollar tier");
			return;
		}
	}


	std::stringstream msg;
	prepareBuffer( msg);

	int VERSION = (m_serverVersion < MIN_SERVER_VER_NOT_HELD) ? 27 : 45;

	// send place order msg
	ENCODE_FIELD( PLACE_ORDER);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( id);

	// send contract fields
	if( m_serverVersion >= MIN_SERVER_VER_PLACE_ORDER_CONID) {
		ENCODE_FIELD( contract.conId);
	}
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.lastTradeDateOrContractMonth);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier); // srv v15 and above
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.primaryExchange); // srv v14 and above
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol); // srv v2 and above
	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.tradingClass);
	}

	if( m_serverVersion >= MIN_SERVER_VER_SEC_ID_TYPE){
		ENCODE_FIELD( contract.secIdType);
		ENCODE_FIELD( contract.secId);
	}

	// send main order fields
	ENCODE_FIELD( order.action);

	if (m_serverVersion >= MIN_SERVER_VER_FRACTIONAL_POSITIONS)
		ENCODE_FIELD(order.totalQuantity)
	else
		ENCODE_FIELD((long)order.totalQuantity)

	ENCODE_FIELD( order.orderType);
	if( m_serverVersion < MIN_SERVER_VER_ORDER_COMBO_LEGS_PRICE) {
		ENCODE_FIELD( order.lmtPrice == UNSET_DOUBLE ? 0 : order.lmtPrice);
	}
	else {
		ENCODE_FIELD_MAX( order.lmtPrice);
	}
	if( m_serverVersion < MIN_SERVER_VER_TRAILING_PERCENT) {
		ENCODE_FIELD( order.auxPrice == UNSET_DOUBLE ? 0 : order.auxPrice);
	}
	else {
		ENCODE_FIELD_MAX( order.auxPrice);
	}

	// send extended order fields
	ENCODE_FIELD( order.tif);
	ENCODE_FIELD( order.ocaGroup);
	ENCODE_FIELD( order.account);
	ENCODE_FIELD( order.openClose);
	ENCODE_FIELD( order.origin);
	ENCODE_FIELD( order.orderRef);
	ENCODE_FIELD( order.transmit);
	ENCODE_FIELD( order.parentId); // srv v4 and above

	ENCODE_FIELD( order.blockOrder); // srv v5 and above
	ENCODE_FIELD( order.sweepToFill); // srv v5 and above
	ENCODE_FIELD( order.displaySize); // srv v5 and above
	ENCODE_FIELD( order.triggerMethod); // srv v5 and above

	//if( m_serverVersion < 38) {
	// will never happen
	//	ENCODE_FIELD(/* order.ignoreRth */ false);
	//}
	//else {
	ENCODE_FIELD( order.outsideRth); // srv v5 and above
	//}

	ENCODE_FIELD( order.hidden); // srv v7 and above

	// Send combo legs for BAG requests (srv v8 and above)
	if( contract.secType == "BAG")
	{
		const Contract::ComboLegList* const comboLegs = contract.comboLegs.get();
		const int comboLegsCount = comboLegs ? comboLegs->size() : 0;
		ENCODE_FIELD( comboLegsCount);
		if( comboLegsCount > 0) {
			for( int i = 0; i < comboLegsCount; ++i) {
				const ComboLeg* comboLeg = ((*comboLegs)[i]).get();
				assert( comboLeg);
				ENCODE_FIELD( comboLeg->conId);
				ENCODE_FIELD( comboLeg->ratio);
				ENCODE_FIELD( comboLeg->action);
				ENCODE_FIELD( comboLeg->exchange);
				ENCODE_FIELD( comboLeg->openClose);

				ENCODE_FIELD( comboLeg->shortSaleSlot); // srv v35 and above
				ENCODE_FIELD( comboLeg->designatedLocation); // srv v35 and above
				if (m_serverVersion >= MIN_SERVER_VER_SSHORTX_OLD) { 
					ENCODE_FIELD( comboLeg->exemptCode);
				}
			}
		}
	}

	// Send order combo legs for BAG requests
	if( m_serverVersion >= MIN_SERVER_VER_ORDER_COMBO_LEGS_PRICE && contract.secType == "BAG")
	{
		const Order::OrderComboLegList* const orderComboLegs = order.orderComboLegs.get();
		const int orderComboLegsCount = orderComboLegs ? orderComboLegs->size() : 0;
		ENCODE_FIELD( orderComboLegsCount);
		if( orderComboLegsCount > 0) {
			for( int i = 0; i < orderComboLegsCount; ++i) {
				const OrderComboLeg* orderComboLeg = ((*orderComboLegs)[i]).get();
				assert( orderComboLeg);
				ENCODE_FIELD_MAX( orderComboLeg->price);
			}
		}
	}	

	if( m_serverVersion >= MIN_SERVER_VER_SMART_COMBO_ROUTING_PARAMS && contract.secType == "BAG") {
		const TagValueList* const smartComboRoutingParams = order.smartComboRoutingParams.get();
		const int smartComboRoutingParamsCount = smartComboRoutingParams ? smartComboRoutingParams->size() : 0;
		ENCODE_FIELD( smartComboRoutingParamsCount);
		if( smartComboRoutingParamsCount > 0) {
			for( int i = 0; i < smartComboRoutingParamsCount; ++i) {
				const TagValue* tagValue = ((*smartComboRoutingParams)[i]).get();
				ENCODE_FIELD( tagValue->tag);
				ENCODE_FIELD( tagValue->value);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// Send the shares allocation.
	//
	// This specifies the number of order shares allocated to each Financial
	// Advisor managed account. The format of the allocation string is as
	// follows:
	//			<account_code1>/<number_shares1>,<account_code2>/<number_shares2>,...N
	// E.g.
	//		To allocate 20 shares of a 100 share order to account 'U101' and the
	//      residual 80 to account 'U203' enter the following share allocation string:
	//          U101/20,U203/80
	/////////////////////////////////////////////////////////////////////////////
	{
		// send deprecated sharesAllocation field
		ENCODE_FIELD( ""); // srv v9 and above
	}

	ENCODE_FIELD( order.discretionaryAmt); // srv v10 and above
	ENCODE_FIELD( order.goodAfterTime); // srv v11 and above
	ENCODE_FIELD( order.goodTillDate); // srv v12 and above

	ENCODE_FIELD( order.faGroup); // srv v13 and above
	ENCODE_FIELD( order.faMethod); // srv v13 and above
	ENCODE_FIELD( order.faPercentage); // srv v13 and above
	ENCODE_FIELD( order.faProfile); // srv v13 and above

	if (m_serverVersion >= MIN_SERVER_VER_MODELS_SUPPORT) {
		ENCODE_FIELD( order.modelCode);
	}

	// institutional short saleslot data (srv v18 and above)
	ENCODE_FIELD( order.shortSaleSlot);      // 0 for retail, 1 or 2 for institutions
	ENCODE_FIELD( order.designatedLocation); // populate only when shortSaleSlot = 2.
	if (m_serverVersion >= MIN_SERVER_VER_SSHORTX_OLD) { 
		ENCODE_FIELD( order.exemptCode);
	}

	// not needed anymore
	//bool isVolOrder = (order.orderType.CompareNoCase("VOL") == 0);

	// srv v19 and above fields
	ENCODE_FIELD( order.ocaType);
	//if( m_serverVersion < 38) {
	// will never happen
	//	send( /* order.rthOnly */ false);
	//}
	ENCODE_FIELD( order.rule80A);
	ENCODE_FIELD( order.settlingFirm);
	ENCODE_FIELD( order.allOrNone);
	ENCODE_FIELD_MAX( order.minQty);
	ENCODE_FIELD_MAX( order.percentOffset);
	ENCODE_FIELD( order.eTradeOnly);
	ENCODE_FIELD( order.firmQuoteOnly);
	ENCODE_FIELD_MAX( order.nbboPriceCap);
	ENCODE_FIELD( order.auctionStrategy); // AUCTION_MATCH, AUCTION_IMPROVEMENT, AUCTION_TRANSPARENT
	ENCODE_FIELD_MAX( order.startingPrice);
	ENCODE_FIELD_MAX( order.stockRefPrice);
	ENCODE_FIELD_MAX( order.delta);
	// Volatility orders had specific watermark price attribs in server version 26
	//double lower = (m_serverVersion == 26 && isVolOrder) ? DBL_MAX : order.stockRangeLower;
	//double upper = (m_serverVersion == 26 && isVolOrder) ? DBL_MAX : order.stockRangeUpper;
	ENCODE_FIELD_MAX( order.stockRangeLower);
	ENCODE_FIELD_MAX( order.stockRangeUpper);

	ENCODE_FIELD( order.overridePercentageConstraints); // srv v22 and above

	// Volatility orders (srv v26 and above)
	ENCODE_FIELD_MAX( order.volatility);
	ENCODE_FIELD_MAX( order.volatilityType);
	// will never happen
	//if( m_serverVersion < 28) {
	//	send( order.deltaNeutralOrderType.CompareNoCase("MKT") == 0);
	//}
	//else {
	ENCODE_FIELD( order.deltaNeutralOrderType); // srv v28 and above
	ENCODE_FIELD_MAX( order.deltaNeutralAuxPrice); // srv v28 and above

	if (m_serverVersion >= MIN_SERVER_VER_DELTA_NEUTRAL_CONID && !order.deltaNeutralOrderType.empty()){
		ENCODE_FIELD( order.deltaNeutralConId);
		ENCODE_FIELD( order.deltaNeutralSettlingFirm);
		ENCODE_FIELD( order.deltaNeutralClearingAccount);
		ENCODE_FIELD( order.deltaNeutralClearingIntent);
	}

	if (m_serverVersion >= MIN_SERVER_VER_DELTA_NEUTRAL_OPEN_CLOSE && !order.deltaNeutralOrderType.empty()){
		ENCODE_FIELD( order.deltaNeutralOpenClose);
		ENCODE_FIELD( order.deltaNeutralShortSale);
		ENCODE_FIELD( order.deltaNeutralShortSaleSlot);
		ENCODE_FIELD( order.deltaNeutralDesignatedLocation);
	}

	//}
	ENCODE_FIELD( order.continuousUpdate);
	//if( m_serverVersion == 26) {
	//	// Volatility orders had specific watermark price attribs in server version 26
	//	double lower = (isVolOrder ? order.stockRangeLower : DBL_MAX);
	//	double upper = (isVolOrder ? order.stockRangeUpper : DBL_MAX);
	//	ENCODE_FIELD_MAX( lower);
	//	ENCODE_FIELD_MAX( upper);
	//}
	ENCODE_FIELD_MAX( order.referencePriceType);

	ENCODE_FIELD_MAX( order.trailStopPrice); // srv v30 and above

	if( m_serverVersion >= MIN_SERVER_VER_TRAILING_PERCENT) {
		ENCODE_FIELD_MAX( order.trailingPercent);
	}

	// SCALE orders
	if( m_serverVersion >= MIN_SERVER_VER_SCALE_ORDERS2) {
		ENCODE_FIELD_MAX( order.scaleInitLevelSize);
		ENCODE_FIELD_MAX( order.scaleSubsLevelSize);
	}
	else {
		// srv v35 and above)
		ENCODE_FIELD( ""); // for not supported scaleNumComponents
		ENCODE_FIELD_MAX( order.scaleInitLevelSize); // for scaleComponentSize
	}

	ENCODE_FIELD_MAX( order.scalePriceIncrement);

	if( m_serverVersion >= MIN_SERVER_VER_SCALE_ORDERS3 
		&& order.scalePriceIncrement > 0.0 && order.scalePriceIncrement != UNSET_DOUBLE) {
			ENCODE_FIELD_MAX( order.scalePriceAdjustValue);
			ENCODE_FIELD_MAX( order.scalePriceAdjustInterval);
			ENCODE_FIELD_MAX( order.scaleProfitOffset);
			ENCODE_FIELD( order.scaleAutoReset);
			ENCODE_FIELD_MAX( order.scaleInitPosition);
			ENCODE_FIELD_MAX( order.scaleInitFillQty);
			ENCODE_FIELD( order.scaleRandomPercent);
	}

	if( m_serverVersion >= MIN_SERVER_VER_SCALE_TABLE) {
		ENCODE_FIELD( order.scaleTable);
		ENCODE_FIELD( order.activeStartTime);
		ENCODE_FIELD( order.activeStopTime);
	}

	// HEDGE orders
	if( m_serverVersion >= MIN_SERVER_VER_HEDGE_ORDERS) {
		ENCODE_FIELD( order.hedgeType);
		if ( !order.hedgeType.empty()) {
			ENCODE_FIELD( order.hedgeParam);
		}
	}

	if( m_serverVersion >= MIN_SERVER_VER_OPT_OUT_SMART_ROUTING){
		ENCODE_FIELD( order.optOutSmartRouting);
	}

	if( m_serverVersion >= MIN_SERVER_VER_PTA_ORDERS) {
		ENCODE_FIELD( order.clearingAccount);
		ENCODE_FIELD( order.clearingIntent);
	}

	if( m_serverVersion >= MIN_SERVER_VER_NOT_HELD){
		ENCODE_FIELD( order.notHeld);
	}

	if( m_serverVersion >= MIN_SERVER_VER_UNDER_COMP) {
		if( contract.underComp) {
			const UnderComp& underComp = *contract.underComp;
			ENCODE_FIELD( true);
			ENCODE_FIELD( underComp.conId);
			ENCODE_FIELD( underComp.delta);
			ENCODE_FIELD( underComp.price);
		}
		else {
			ENCODE_FIELD( false);
		}
	}

	if( m_serverVersion >= MIN_SERVER_VER_ALGO_ORDERS) {
		ENCODE_FIELD( order.algoStrategy);

		if( !order.algoStrategy.empty()) {
			const TagValueList* const algoParams = order.algoParams.get();
			const int algoParamsCount = algoParams ? algoParams->size() : 0;
			ENCODE_FIELD( algoParamsCount);
			if( algoParamsCount > 0) {
				for( int i = 0; i < algoParamsCount; ++i) {
					const TagValue* tagValue = ((*algoParams)[i]).get();
					ENCODE_FIELD( tagValue->tag);
					ENCODE_FIELD( tagValue->value);
				}
			}
		}

	}

	if( m_serverVersion >= MIN_SERVER_VER_ALGO_ID) {
		ENCODE_FIELD( order.algoId);
	}

	ENCODE_FIELD( order.whatIf); // srv v36 and above

	// send miscOptions parameter
	if( m_serverVersion >= MIN_SERVER_VER_LINKING) {
		std::string miscOptionsStr("");
		const TagValueList* const orderMiscOptions = order.orderMiscOptions.get();
		const int orderMiscOptionsCount = orderMiscOptions ? orderMiscOptions->size() : 0;
		if( orderMiscOptionsCount > 0) {
			for( int i = 0; i < orderMiscOptionsCount; ++i) {
				const TagValue* tagValue = ((*orderMiscOptions)[i]).get();
				miscOptionsStr += tagValue->tag;
				miscOptionsStr += "=";
				miscOptionsStr += tagValue->value;
				miscOptionsStr += ";";
			}
		}
		ENCODE_FIELD( miscOptionsStr);
	}

	if (m_serverVersion >= MIN_SERVER_VER_ORDER_SOLICITED) {
		ENCODE_FIELD(order.solicited);
	}

    if (m_serverVersion >= MIN_SERVER_VER_RANDOMIZE_SIZE_AND_PRICE) {
        ENCODE_FIELD(order.randomizeSize);
        ENCODE_FIELD(order.randomizePrice);
    }

	if (m_serverVersion >= MIN_SERVER_VER_PEGGED_TO_BENCHMARK) {
		if (order.orderType == "PEG BENCH") {
			ENCODE_FIELD(order.referenceContractId);
			ENCODE_FIELD(order.isPeggedChangeAmountDecrease);
			ENCODE_FIELD(order.peggedChangeAmount);
			ENCODE_FIELD(order.referenceChangeAmount);
			ENCODE_FIELD(order.referenceExchangeId);
		}

		ENCODE_FIELD(order.conditions.size());

		if (order.conditions.size() > 0) {
            for (std::shared_ptr<OrderCondition> item : order.conditions) {
				ENCODE_FIELD(item->type());
				item->writeExternal(msg);
			}

			ENCODE_FIELD(order.conditionsIgnoreRth);
			ENCODE_FIELD(order.conditionsCancelOrder);
		}

		ENCODE_FIELD(order.adjustedOrderType);
		ENCODE_FIELD(order.triggerPrice);
		ENCODE_FIELD(order.lmtPriceOffset);
		ENCODE_FIELD(order.adjustedStopPrice);
		ENCODE_FIELD(order.adjustedStopLimitPrice);
		ENCODE_FIELD(order.adjustedTrailingAmount);
		ENCODE_FIELD(order.adjustableTrailingUnit);
	}

	if( m_serverVersion >= MIN_SERVER_VER_EXT_OPERATOR) {
		ENCODE_FIELD( order.extOperator);
	}

	if (m_serverVersion >= MIN_SERVER_VER_SOFT_DOLLAR_TIER) {
		ENCODE_FIELD(order.softDollarTier.name());
		ENCODE_FIELD(order.softDollarTier.val());
	}

	closeAndSend( msg.str());
}

void EClient::cancelOrder( OrderId id)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( id, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	const int VERSION = 1;

	// send cancel order msg
	std::stringstream msg;
	prepareBuffer( msg);

	ENCODE_FIELD( CANCEL_ORDER);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( id);

	closeAndSend( msg.str());
}

void EClient::reqAccountUpdates(bool subscribe, const std::string& acctCode)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 2;

	// send req acct msg
	ENCODE_FIELD( REQ_ACCT_DATA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( subscribe);  // TRUE = subscribe, FALSE = unsubscribe.

	// Send the account code. This will only be used for FA clients
	ENCODE_FIELD( acctCode); // srv v9 and above

	closeAndSend( msg.str());
}

void EClient::reqOpenOrders()
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	// send req open orders msg
	ENCODE_FIELD( REQ_OPEN_ORDERS);
	ENCODE_FIELD( VERSION);

	closeAndSend( msg.str());
}

void EClient::reqAutoOpenOrders(bool bAutoBind)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	// send req open orders msg
	ENCODE_FIELD( REQ_AUTO_OPEN_ORDERS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( bAutoBind);

	closeAndSend( msg.str());
}

void EClient::reqAllOpenOrders()
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	// send req open orders msg
	ENCODE_FIELD( REQ_ALL_OPEN_ORDERS);
	ENCODE_FIELD( VERSION);

	closeAndSend( msg.str());
}

void EClient::reqExecutions(int reqId, const ExecutionFilter& filter)
{
	//NOTE: Time format must be 'yyyymmdd-hh:mm:ss' E.g. '20030702-14:55'

	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 3;

	// send req open orders msg
	ENCODE_FIELD( REQ_EXECUTIONS);
	ENCODE_FIELD( VERSION);

	if( m_serverVersion >= MIN_SERVER_VER_EXECUTION_DATA_CHAIN) {
		ENCODE_FIELD( reqId);
	}

	// Send the execution rpt filter data (srv v9 and above)
	ENCODE_FIELD( filter.m_clientId);
	ENCODE_FIELD( filter.m_acctCode);
	ENCODE_FIELD( filter.m_time);
	ENCODE_FIELD( filter.m_symbol);
	ENCODE_FIELD( filter.m_secType);
	ENCODE_FIELD( filter.m_exchange);
	ENCODE_FIELD( filter.m_side);

	closeAndSend( msg.str());
}

void EClient::reqIds( int numIds)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( numIds, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	// send req open orders msg
	ENCODE_FIELD( REQ_IDS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( numIds);

	closeAndSend( msg.str());
}

void EClient::reqNewsBulletins(bool allMsgs)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	// send req news bulletins msg
	ENCODE_FIELD( REQ_NEWS_BULLETINS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( allMsgs);

	closeAndSend( msg.str());
}

void EClient::cancelNewsBulletins()
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	// send req news bulletins msg
	ENCODE_FIELD( CANCEL_NEWS_BULLETINS);
	ENCODE_FIELD( VERSION);

	closeAndSend( msg.str());
}

void EClient::setServerLogLevel(int logLevel)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	// send the set server logging level message
	ENCODE_FIELD( SET_SERVER_LOGLEVEL);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( logLevel);

	closeAndSend( msg.str());
}

void EClient::reqManagedAccts()
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	// send req FA managed accounts msg
	ENCODE_FIELD( REQ_MANAGED_ACCTS);
	ENCODE_FIELD( VERSION);

	closeAndSend( msg.str());
}


void EClient::requestFA(faDataType pFaDataType)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 13) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( REQ_FA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( (int)pFaDataType);

	closeAndSend( msg.str());
}

void EClient::replaceFA(faDataType pFaDataType, const std::string& cxml)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 13) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( REPLACE_FA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( (int)pFaDataType);
	ENCODE_FIELD( cxml);

	closeAndSend( msg.str());
}



void EClient::exerciseOptions( TickerId tickerId, const Contract& contract,
										int exerciseAction, int exerciseQuantity,
										const std::string& account, int override)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 21) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}

	if (m_serverVersion < MIN_SERVER_VER_TRADING_CLASS) {
		if( !contract.tradingClass.empty() || (contract.conId > 0)) {
			m_pEWrapper->error( tickerId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
				"  It does not support conId, multiplier and tradingClass parameters in exerciseOptions.");
			return;
		}
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 2;

	ENCODE_FIELD( EXERCISE_OPTIONS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	// send contract fields
	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.conId);
	}
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.lastTradeDateOrContractMonth);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier);
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol);
	if( m_serverVersion >= MIN_SERVER_VER_TRADING_CLASS) {
		ENCODE_FIELD( contract.tradingClass);
	}
	ENCODE_FIELD( exerciseAction);
	ENCODE_FIELD( exerciseQuantity);
	ENCODE_FIELD( account);
	ENCODE_FIELD( override);

	closeAndSend( msg.str());
}

void EClient::reqGlobalCancel()
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if (m_serverVersion < MIN_SERVER_VER_REQ_GLOBAL_CANCEL) {
		m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support globalCancel requests.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	// send current time req
	ENCODE_FIELD( REQ_GLOBAL_CANCEL);
	ENCODE_FIELD( VERSION);

	closeAndSend( msg.str());
}

void EClient::reqMarketDataType( int marketDataType)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_REQ_MARKET_DATA_TYPE) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support market data type requests.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( REQ_MARKET_DATA_TYPE);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( marketDataType);

	closeAndSend( msg.str());
}

void EClient::reqPositions()
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_POSITIONS) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support positions request.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( REQ_POSITIONS);
	ENCODE_FIELD( VERSION);

	closeAndSend( msg.str());
}

void EClient::cancelPositions()
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_POSITIONS) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support positions cancellation.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( CANCEL_POSITIONS);
	ENCODE_FIELD( VERSION);

	closeAndSend( msg.str());
}

void EClient::reqAccountSummary( int reqId, const std::string& groupName, const std::string& tags)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_ACCOUNT_SUMMARY) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support account summary request.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( REQ_ACCOUNT_SUMMARY);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);
	ENCODE_FIELD( groupName);
	ENCODE_FIELD( tags);

	closeAndSend( msg.str());
}

void EClient::cancelAccountSummary( int reqId)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_ACCOUNT_SUMMARY) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support account summary cancellation.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( CANCEL_ACCOUNT_SUMMARY);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);

	closeAndSend( msg.str());
}

void EClient::verifyRequest(const std::string& apiName, const std::string& apiVersion)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_LINKING) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support verification request.");
		return;
	}

	if( !m_extraAuth) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  Intent to authenticate needs to be expressed during initial connect request.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( VERIFY_REQUEST);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( apiName);
	ENCODE_FIELD( apiVersion);

	closeAndSend( msg.str());
}

void EClient::verifyMessage(const std::string& apiData)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_LINKING) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support verification message sending.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( VERIFY_MESSAGE);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( apiData);

	closeAndSend( msg.str());
}

void EClient::verifyAndAuthRequest(const std::string& apiName, const std::string& apiVersion, const std::string& opaqueIsvKey)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_LINKING_AUTH) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support verification request.");
		return;
	}

	if( !m_extraAuth) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  Intent to authenticate needs to be expressed during initial connect request.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( VERIFY_AND_AUTH_REQUEST);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( apiName);
	ENCODE_FIELD( apiVersion);
	ENCODE_FIELD( opaqueIsvKey);

	closeAndSend( msg.str());
}

void EClient::verifyAndAuthMessage(const std::string& apiData, const std::string& xyzResponse)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_LINKING_AUTH) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support verification message sending.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( VERIFY_AND_AUTH_MESSAGE);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( apiData);
	ENCODE_FIELD( xyzResponse);

	closeAndSend( msg.str());
}

void EClient::queryDisplayGroups( int reqId)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_LINKING) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support queryDisplayGroups request.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( QUERY_DISPLAY_GROUPS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);

	closeAndSend( msg.str());
}

void EClient::subscribeToGroupEvents( int reqId, int groupId)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_LINKING) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support subscribeToGroupEvents request.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( SUBSCRIBE_TO_GROUP_EVENTS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);
	ENCODE_FIELD( groupId);

	closeAndSend( msg.str());
}

void EClient::updateDisplayGroup( int reqId, const std::string& contractInfo)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_LINKING) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support updateDisplayGroup request.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( UPDATE_DISPLAY_GROUP);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);
	ENCODE_FIELD( contractInfo);

	closeAndSend( msg.str());
}

void EClient::startApi()
{
    // not connected?
    if( !isConnected()) {
        m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
        return;
    }

    if( m_serverVersion >= 3) {
        if( m_serverVersion < MIN_SERVER_VER_LINKING) {
            std::stringstream msg;
            ENCODE_FIELD( m_clientId);
            on_send( msg.str());
        }
        else
        {
            std::stringstream msg;
            prepareBuffer( msg);

            const int VERSION = 2;

            ENCODE_FIELD( START_API);
            ENCODE_FIELD( VERSION);
            ENCODE_FIELD( m_clientId);

            if (m_serverVersion >= MIN_SERVER_VER_OPTIONAL_CAPABILITIES)
                ENCODE_FIELD(m_optionalCapabilities);

            closeAndSend( msg.str());
        }
    }
}

void EClient::unsubscribeFromGroupEvents( int reqId)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_LINKING) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support unsubscribeFromGroupEvents request.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( UNSUBSCRIBE_FROM_GROUP_EVENTS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);

	closeAndSend( msg.str());
}

void EClient::reqPositionsMulti( int reqId, const std::string& account, const std::string& modelCode)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_MODELS_SUPPORT) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support positions multi request.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( REQ_POSITIONS_MULTI);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);
	ENCODE_FIELD( account);
	ENCODE_FIELD( modelCode);

	closeAndSend( msg.str());
}

void EClient::cancelPositionsMulti( int reqId)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_MODELS_SUPPORT) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support positions multi cancellation.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( CANCEL_POSITIONS_MULTI);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);

	closeAndSend( msg.str());
}

void EClient::reqAccountUpdatesMulti( int reqId, const std::string& account, const std::string& modelCode, bool ledgerAndNLV)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_MODELS_SUPPORT) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support account updates multi request.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( REQ_ACCOUNT_UPDATES_MULTI);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);
	ENCODE_FIELD( account);
	ENCODE_FIELD( modelCode);
	ENCODE_FIELD( ledgerAndNLV);

	closeAndSend( msg.str());
}

void EClient::cancelAccountUpdatesMulti( int reqId)
{
	// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_MODELS_SUPPORT) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support account updates multi cancellation.");
		return;
	}

	std::stringstream msg;
	prepareBuffer( msg);

	const int VERSION = 1;

	ENCODE_FIELD( CANCEL_ACCOUNT_UPDATES_MULTI);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( reqId);

	closeAndSend( msg.str());
}

void EClient::reqSecDefOptParams(int reqId, const std::string& underlyingSymbol, const std::string& futFopExchange, const std::string& underlyingSecType, int underlyingConId)
{
		// not connected?
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	if( m_serverVersion < MIN_SERVER_VER_SEC_DEF_OPT_PARAMS_REQ) {
		m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
			"  It does not support security definiton option requests.");
		return;
	}

	std::stringstream msg;
	prepareBuffer(msg);


	ENCODE_FIELD(REQ_SEC_DEF_OPT_PARAMS);
    ENCODE_FIELD(reqId);
    ENCODE_FIELD(underlyingSymbol); 
    ENCODE_FIELD(futFopExchange);
    ENCODE_FIELD(underlyingSecType);
    ENCODE_FIELD(underlyingConId);

	closeAndSend(msg.str());
}

void EClient::reqSoftDollarTiers(int reqId)
{
	if( !isConnected()) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::stringstream msg;
	prepareBuffer(msg);


	ENCODE_FIELD(REQ_SOFT_DOLLAR_TIERS);
    ENCODE_FIELD(reqId);

	closeAndSend(msg.str());
}

int EClient::processMsg(const char*& beginPtr, const char* endPtr)
{
    if( !m_useV100Plus) {
        return parseAndProcessMsg( beginPtr, endPtr);
    }
    return decodePrefixMsg( beginPtr, endPtr);
}

int EClient::decodePrefixMsg(const char*& beginPtr, const char* endPtr)
{
	if( beginPtr + HEADER_LEN >= endPtr)
		return 0;

    static_assert(sizeof(unsigned) == HEADER_LEN, "size(unsigned) != 4");

	unsigned netLen = 0;
	memcpy( &netLen, beginPtr, HEADER_LEN);

    const unsigned msgLen = network_to_host_long(netLen);

	// shold never happen, but still....
	if( !msgLen) {
		beginPtr += HEADER_LEN;
		return HEADER_LEN;
	}

	// enforce max msg len limit
	if( msgLen > MAX_MSG_LEN) {
		m_pEWrapper->error( NO_VALID_ID, BAD_LENGTH.code(), BAD_LENGTH.msg());
		eDisconnect();
		m_pEWrapper->connectionClosed();
		return 0;
	}

	const char* msgStart = beginPtr + HEADER_LEN;
	const char* msgEnd = msgStart + msgLen;

	// handle incomplete messages
	if( msgEnd > endPtr) {
		return 0;
	}

    int decoded = parseAndProcessMsg( msgStart, msgEnd);
	if( decoded <= 0) {
		// this would mean something went real wrong
		// and message was incomplete from decoder POV
		m_pEWrapper->error( NO_VALID_ID, BAD_MESSAGE.code(), BAD_MESSAGE.msg());
		eDisconnect();
		m_pEWrapper->connectionClosed();
		return 0;
	}

    int consumed = static_cast<int>(msgEnd - beginPtr);
	beginPtr = msgEnd;
	return consumed;
}

void EClient::setClientId( int clientId)
{
	m_clientId = clientId;
}

void EClient::setExtraAuth( bool extraAuth)
{
	m_extraAuth = extraAuth;
}

void EClient::setHost( const std::string& host)
{
	m_host = host;
}

void EClient::setPort( unsigned port)
{
    m_port = port;
}

void EClient::setOptionalCapabilities(const std::string& optCapts)
{
    m_optionalCapabilities = optCapts;
}


///////////////////////////////////////////////////////////
// callbacks from socket
void EClient::sendConnectRequest()
{
	m_connState = CS_CONNECTING;

	// send client version
	std::stringstream msg;
	if( m_useV100Plus) {
		msg.write( API_SIGN, sizeof(API_SIGN));
		prepareBufferImpl( msg);
		if( MIN_CLIENT_VER < MAX_CLIENT_VER) {
			msg << 'v' << MIN_CLIENT_VER << ".." << MAX_CLIENT_VER;
		}
		else {
			msg << 'v' << MIN_CLIENT_VER;
		}
		if( !m_connectOptions.empty()) {
			msg << ' ' << m_connectOptions;
		}

        closeAndSend( msg.str(), sizeof(API_SIGN));
	}
    else {
        ENCODE_FIELD( CLIENT_VERSION);

        on_send( msg.str());
    }
}

const char* EClient::processTickPriceMsg(const char* ptr, const char* endPtr) {
    int version;
    int tickerId;
    int tickTypeInt;
    double price;

    int size;
    int canAutoExecute;

    DECODE_FIELD( version);
    DECODE_FIELD( tickerId);
    DECODE_FIELD( tickTypeInt);
    DECODE_FIELD( price);

    DECODE_FIELD( size); // ver 2 field
    DECODE_FIELD( canAutoExecute); // ver 3 field

    m_pEWrapper->tickPrice( tickerId, (TickType)tickTypeInt, price, canAutoExecute);

    // process ver 2 fields
    {
        TickType sizeTickType = NOT_SET;
        switch( (TickType)tickTypeInt) {
        case BID:
            sizeTickType = BID_SIZE;
            break;
        case ASK:
            sizeTickType = ASK_SIZE;
            break;
        case LAST:
            sizeTickType = LAST_SIZE;
            break;
        default:
            break;
        }
        if( sizeTickType != NOT_SET)
            m_pEWrapper->tickSize( tickerId, sizeTickType, size);
    }

    return ptr;
}

const char* EClient::processTickSizeMsg(const char* ptr, const char* endPtr) {
    int version;
    int tickerId;
    int tickTypeInt;
    int size;

    DECODE_FIELD( version);
    DECODE_FIELD( tickerId);
    DECODE_FIELD( tickTypeInt);
    DECODE_FIELD( size);

    m_pEWrapper->tickSize( tickerId, (TickType)tickTypeInt, size);

    return ptr;
}

const char* EClient::processTickOptionComputationMsg(const char* ptr, const char* endPtr) {
    int version;
    int tickerId;
    int tickTypeInt;
    double impliedVol;
    double delta;

    double optPrice = DBL_MAX;
    double pvDividend = DBL_MAX;

    double gamma = DBL_MAX;
    double vega = DBL_MAX;
    double theta = DBL_MAX;
    double undPrice = DBL_MAX;

    DECODE_FIELD( version);
    DECODE_FIELD( tickerId);
    DECODE_FIELD( tickTypeInt);

    DECODE_FIELD( impliedVol);
    DECODE_FIELD( delta);

    if( impliedVol < 0) { // -1 is the "not computed" indicator
        impliedVol = DBL_MAX;
    }
    if( delta > 1 || delta < -1) { // -2 is the "not computed" indicator
        delta = DBL_MAX;
    }

    if( version >= 6 || tickTypeInt == MODEL_OPTION) { // introduced in version == 5

        DECODE_FIELD( optPrice);
        DECODE_FIELD( pvDividend);

        if( optPrice < 0) { // -1 is the "not computed" indicator
            optPrice = DBL_MAX;
        }
        if( pvDividend < 0) { // -1 is the "not computed" indicator
            pvDividend = DBL_MAX;
        }
    }
    if( version >= 6) {

        DECODE_FIELD( gamma);
        DECODE_FIELD( vega);
        DECODE_FIELD( theta);
        DECODE_FIELD( undPrice);

        if( gamma > 1 || gamma < -1) { // -2 is the "not yet computed" indicator
            gamma = DBL_MAX;
        }
        if( vega > 1 || vega < -1) { // -2 is the "not yet computed" indicator
            vega = DBL_MAX;
        }
        if( theta > 1 || theta < -1) { // -2 is the "not yet computed" indicator
            theta = DBL_MAX;
        }
        if( undPrice < 0) { // -1 is the "not computed" indicator
            undPrice = DBL_MAX;
        }
    }

    m_pEWrapper->tickOptionComputation( tickerId, (TickType)tickTypeInt,
        impliedVol, delta, optPrice, pvDividend, gamma, vega, theta, undPrice);

    return ptr;
}

const char* EClient::processTickGenericMsg(const char* ptr, const char* endPtr) {
    int version;
    int tickerId;
    int tickTypeInt;
    double value;

    DECODE_FIELD( version);
    DECODE_FIELD( tickerId);
    DECODE_FIELD( tickTypeInt);
    DECODE_FIELD( value);

    m_pEWrapper->tickGeneric( tickerId, (TickType)tickTypeInt, value);

    return ptr;
}

const char* EClient::processTickStringMsg(const char* ptr, const char* endPtr) {
    int version;
    int tickerId;
    int tickTypeInt;
    std::string value;

    DECODE_FIELD( version);
    DECODE_FIELD( tickerId);
    DECODE_FIELD( tickTypeInt);
    DECODE_FIELD( value);

    m_pEWrapper->tickString( tickerId, (TickType)tickTypeInt, value);

    return ptr;
}

const char* EClient::processTickEfpMsg(const char* ptr, const char* endPtr) {
    int version;
    int tickerId;
    int tickTypeInt;
    double basisPoints;
    std::string formattedBasisPoints;
    double impliedFuturesPrice;
    int holdDays;
    std::string futureLastTradeDate;
    double dividendImpact;
    double dividendsToLastTradeDate;

    DECODE_FIELD( version);
    DECODE_FIELD( tickerId);
    DECODE_FIELD( tickTypeInt);
    DECODE_FIELD( basisPoints);
    DECODE_FIELD( formattedBasisPoints);
    DECODE_FIELD( impliedFuturesPrice);
    DECODE_FIELD( holdDays);
    DECODE_FIELD( futureLastTradeDate);
    DECODE_FIELD( dividendImpact);
    DECODE_FIELD( dividendsToLastTradeDate);

    m_pEWrapper->tickEFP( tickerId, (TickType)tickTypeInt, basisPoints, formattedBasisPoints,
        impliedFuturesPrice, holdDays, futureLastTradeDate, dividendImpact, dividendsToLastTradeDate);

    return ptr;
}

const char* EClient::processOrderStatusMsg(const char* ptr, const char* endPtr) {
    int version;
    int orderId;
    std::string status;
    double filled;
    double remaining;
    double avgFillPrice;
    int permId;
    int parentId;
    double lastFillPrice;
    int clientId;
    std::string whyHeld;

    DECODE_FIELD( version);
    DECODE_FIELD( orderId);
    DECODE_FIELD( status);

    if (m_serverVersion >= MIN_SERVER_VER_FRACTIONAL_POSITIONS)
    {
        DECODE_FIELD( filled);
    }
    else
    {
        int iFilled;

        DECODE_FIELD(iFilled);

        filled = iFilled;
    }

    if (m_serverVersion >= MIN_SERVER_VER_FRACTIONAL_POSITIONS)
    {
        DECODE_FIELD( remaining);
    }
    else
    {
        int iRemaining;

        DECODE_FIELD(iRemaining);

        remaining = iRemaining;
    }

    DECODE_FIELD( avgFillPrice);

    DECODE_FIELD( permId); // ver 2 field
    DECODE_FIELD( parentId); // ver 3 field
    DECODE_FIELD( lastFillPrice); // ver 4 field
    DECODE_FIELD( clientId); // ver 5 field
    DECODE_FIELD( whyHeld); // ver 6 field

    m_pEWrapper->orderStatus( orderId, status, filled, remaining,
        avgFillPrice, permId, parentId, lastFillPrice, clientId, whyHeld);


    return ptr;
}

const char* EClient::processErrMsgMsg(const char* ptr, const char* endPtr) {
    int version;
    int id; // ver 2 field
    int errorCode; // ver 2 field
    std::string errorMsg;

    DECODE_FIELD( version);
    DECODE_FIELD( id);
    DECODE_FIELD( errorCode);
    DECODE_FIELD( errorMsg);

    m_pEWrapper->error( id, errorCode, errorMsg);

    return ptr;
}

const char* EClient::processOpenOrderMsg(const char* ptr, const char* endPtr) {
    // read version
    int version;
    DECODE_FIELD( version);

    // read order id
    Order order;
    DECODE_FIELD( order.orderId);

    // read contract fields
    Contract contract;
    DECODE_FIELD( contract.conId); // ver 17 field
    DECODE_FIELD( contract.symbol);
    DECODE_FIELD( contract.secType);
    DECODE_FIELD( contract.lastTradeDateOrContractMonth);
    DECODE_FIELD( contract.strike);
    DECODE_FIELD( contract.right);
    if (version >= 32) {
        DECODE_FIELD( contract.multiplier);
    }
    DECODE_FIELD( contract.exchange);
    DECODE_FIELD( contract.currency);
    DECODE_FIELD( contract.localSymbol); // ver 2 field
    if (version >= 32) {
        DECODE_FIELD( contract.tradingClass);
    }

    // read order fields
    DECODE_FIELD( order.action);

    if (m_serverVersion >= MIN_SERVER_VER_FRACTIONAL_POSITIONS)
    {
        DECODE_FIELD( order.totalQuantity);
    }
    else
    {
        long lTotalQuantity;

        DECODE_FIELD(lTotalQuantity);

        order.totalQuantity = lTotalQuantity;
    }

    DECODE_FIELD( order.orderType);
    if (version < 29) {
        DECODE_FIELD( order.lmtPrice);
    }
    else {
        DECODE_FIELD_MAX( order.lmtPrice);
    }
    if (version < 30) {
        DECODE_FIELD( order.auxPrice);
    }
    else {
        DECODE_FIELD_MAX( order.auxPrice);
    }
    DECODE_FIELD( order.tif);
    DECODE_FIELD( order.ocaGroup);
    DECODE_FIELD( order.account);
    DECODE_FIELD( order.openClose);

    int orderOriginInt;
    DECODE_FIELD( orderOriginInt);
    order.origin = (Origin)orderOriginInt;

    DECODE_FIELD( order.orderRef);
    DECODE_FIELD( order.clientId); // ver 3 field
    DECODE_FIELD( order.permId); // ver 4 field

    //if( version < 18) {
    //	// will never happen
    //	/* order.ignoreRth = */ readBoolFromInt();
    //}

    DECODE_FIELD( order.outsideRth); // ver 18 field
    DECODE_FIELD( order.hidden); // ver 4 field
    DECODE_FIELD( order.discretionaryAmt); // ver 4 field
    DECODE_FIELD( order.goodAfterTime); // ver 5 field

    {
        std::string sharesAllocation;
        DECODE_FIELD( sharesAllocation); // deprecated ver 6 field
    }

    DECODE_FIELD( order.faGroup); // ver 7 field
    DECODE_FIELD( order.faMethod); // ver 7 field
    DECODE_FIELD( order.faPercentage); // ver 7 field
    DECODE_FIELD( order.faProfile); // ver 7 field

    if( m_serverVersion >= MIN_SERVER_VER_MODELS_SUPPORT ) {
        DECODE_FIELD( order.modelCode);
    }

    DECODE_FIELD( order.goodTillDate); // ver 8 field

    DECODE_FIELD( order.rule80A); // ver 9 field
    DECODE_FIELD_MAX( order.percentOffset); // ver 9 field
    DECODE_FIELD( order.settlingFirm); // ver 9 field
    DECODE_FIELD( order.shortSaleSlot); // ver 9 field
    DECODE_FIELD( order.designatedLocation); // ver 9 field
    if( m_serverVersion == MIN_SERVER_VER_SSHORTX_OLD){
        int exemptCode;
        DECODE_FIELD( exemptCode);
    }
    else if( version >= 23){
        DECODE_FIELD( order.exemptCode);
    }
    DECODE_FIELD( order.auctionStrategy); // ver 9 field
    DECODE_FIELD_MAX( order.startingPrice); // ver 9 field
    DECODE_FIELD_MAX( order.stockRefPrice); // ver 9 field
    DECODE_FIELD_MAX( order.delta); // ver 9 field
    DECODE_FIELD_MAX( order.stockRangeLower); // ver 9 field
    DECODE_FIELD_MAX( order.stockRangeUpper); // ver 9 field
    DECODE_FIELD( order.displaySize); // ver 9 field

    //if( version < 18) {
    //		// will never happen
    //		/* order.rthOnly = */ readBoolFromInt();
    //}

    DECODE_FIELD( order.blockOrder); // ver 9 field
    DECODE_FIELD( order.sweepToFill); // ver 9 field
    DECODE_FIELD( order.allOrNone); // ver 9 field
    DECODE_FIELD_MAX( order.minQty); // ver 9 field
    DECODE_FIELD( order.ocaType); // ver 9 field
    DECODE_FIELD( order.eTradeOnly); // ver 9 field
    DECODE_FIELD( order.firmQuoteOnly); // ver 9 field
    DECODE_FIELD_MAX( order.nbboPriceCap); // ver 9 field

    DECODE_FIELD( order.parentId); // ver 10 field
    DECODE_FIELD( order.triggerMethod); // ver 10 field

    DECODE_FIELD_MAX( order.volatility); // ver 11 field
    DECODE_FIELD( order.volatilityType); // ver 11 field
    DECODE_FIELD( order.deltaNeutralOrderType); // ver 11 field (had a hack for ver 11)
    DECODE_FIELD_MAX( order.deltaNeutralAuxPrice); // ver 12 field

    if (version >= 27 && !order.deltaNeutralOrderType.empty()) {
        DECODE_FIELD( order.deltaNeutralConId);
        DECODE_FIELD( order.deltaNeutralSettlingFirm);
        DECODE_FIELD( order.deltaNeutralClearingAccount);
        DECODE_FIELD( order.deltaNeutralClearingIntent);
    }

    if (version >= 31 && !order.deltaNeutralOrderType.empty()) {
        DECODE_FIELD( order.deltaNeutralOpenClose);
        DECODE_FIELD( order.deltaNeutralShortSale);
        DECODE_FIELD( order.deltaNeutralShortSaleSlot);
        DECODE_FIELD( order.deltaNeutralDesignatedLocation);
    }

    DECODE_FIELD( order.continuousUpdate); // ver 11 field

    // will never happen
    //if( m_serverVersion == 26) {
    //	order.stockRangeLower = readDouble();
    //	order.stockRangeUpper = readDouble();
    //}

    DECODE_FIELD( order.referencePriceType); // ver 11 field

    DECODE_FIELD_MAX( order.trailStopPrice); // ver 13 field

    if (version >= 30) {
        DECODE_FIELD_MAX( order.trailingPercent);
    }

    DECODE_FIELD_MAX( order.basisPoints); // ver 14 field
    DECODE_FIELD_MAX( order.basisPointsType); // ver 14 field
    DECODE_FIELD( contract.comboLegsDescrip); // ver 14 field

    if (version >= 29) {
        int comboLegsCount = 0;
        DECODE_FIELD( comboLegsCount);

        if (comboLegsCount > 0) {
            Contract::ComboLegListSPtr comboLegs( new Contract::ComboLegList);
            comboLegs->reserve( comboLegsCount);
            for (int i = 0; i < comboLegsCount; ++i) {
                ComboLegSPtr comboLeg( new ComboLeg());
                DECODE_FIELD( comboLeg->conId);
                DECODE_FIELD( comboLeg->ratio);
                DECODE_FIELD( comboLeg->action);
                DECODE_FIELD( comboLeg->exchange);
                DECODE_FIELD( comboLeg->openClose);
                DECODE_FIELD( comboLeg->shortSaleSlot);
                DECODE_FIELD( comboLeg->designatedLocation);
                DECODE_FIELD( comboLeg->exemptCode);

                comboLegs->push_back( comboLeg);
            }
            contract.comboLegs = comboLegs;
        }

        int orderComboLegsCount = 0;
        DECODE_FIELD( orderComboLegsCount);
        if (orderComboLegsCount > 0) {
            Order::OrderComboLegListSPtr orderComboLegs( new Order::OrderComboLegList);
            orderComboLegs->reserve( orderComboLegsCount);
            for (int i = 0; i < orderComboLegsCount; ++i) {
                OrderComboLegSPtr orderComboLeg( new OrderComboLeg());
                DECODE_FIELD_MAX( orderComboLeg->price);

                orderComboLegs->push_back( orderComboLeg);
            }
            order.orderComboLegs = orderComboLegs;
        }
    }

    if (version >= 26) {
        int smartComboRoutingParamsCount = 0;
        DECODE_FIELD( smartComboRoutingParamsCount);
        if( smartComboRoutingParamsCount > 0) {
            TagValueListSPtr smartComboRoutingParams( new TagValueList);
            smartComboRoutingParams->reserve( smartComboRoutingParamsCount);
            for( int i = 0; i < smartComboRoutingParamsCount; ++i) {
                TagValueSPtr tagValue( new TagValue());
                DECODE_FIELD( tagValue->tag);
                DECODE_FIELD( tagValue->value);
                smartComboRoutingParams->push_back( tagValue);
            }
            order.smartComboRoutingParams = smartComboRoutingParams;
        }
    }

    if( version >= 20) {
        DECODE_FIELD_MAX( order.scaleInitLevelSize);
        DECODE_FIELD_MAX( order.scaleSubsLevelSize);
    }
    else {
        // ver 15 fields
        int notSuppScaleNumComponents = 0;
        DECODE_FIELD_MAX( notSuppScaleNumComponents);
        DECODE_FIELD_MAX( order.scaleInitLevelSize); // scaleComponectSize
    }
    DECODE_FIELD_MAX( order.scalePriceIncrement); // ver 15 field

    if (version >= 28 && order.scalePriceIncrement > 0.0 && order.scalePriceIncrement != UNSET_DOUBLE) {
        DECODE_FIELD_MAX( order.scalePriceAdjustValue);
        DECODE_FIELD_MAX( order.scalePriceAdjustInterval);
        DECODE_FIELD_MAX( order.scaleProfitOffset);
        DECODE_FIELD( order.scaleAutoReset);
        DECODE_FIELD_MAX( order.scaleInitPosition);
        DECODE_FIELD_MAX( order.scaleInitFillQty);
        DECODE_FIELD( order.scaleRandomPercent);
    }

    if( version >= 24) {
        DECODE_FIELD( order.hedgeType);
        if( !order.hedgeType.empty()) {
            DECODE_FIELD( order.hedgeParam);
        }
    }

    if( version >= 25) {
        DECODE_FIELD( order.optOutSmartRouting);
    }

    DECODE_FIELD( order.clearingAccount); // ver 19 field
    DECODE_FIELD( order.clearingIntent); // ver 19 field

    if( version >= 22) {
        DECODE_FIELD( order.notHeld);
    }

    UnderComp underComp;
    if( version >= 20) {
        bool underCompPresent = false;
        DECODE_FIELD(underCompPresent);
        if( underCompPresent){
            DECODE_FIELD(underComp.conId);
            DECODE_FIELD(underComp.delta);
            DECODE_FIELD(underComp.price);
            contract.underComp = &underComp;
        }
    }


    if( version >= 21) {
        DECODE_FIELD( order.algoStrategy);
        if( !order.algoStrategy.empty()) {
            int algoParamsCount = 0;
            DECODE_FIELD( algoParamsCount);
            if( algoParamsCount > 0) {
                TagValueListSPtr algoParams( new TagValueList);
                algoParams->reserve( algoParamsCount);
                for( int i = 0; i < algoParamsCount; ++i) {
                    TagValueSPtr tagValue( new TagValue());
                    DECODE_FIELD( tagValue->tag);
                    DECODE_FIELD( tagValue->value);
                    algoParams->push_back( tagValue);
                }
                order.algoParams = algoParams;
            }
        }
    }

    if (version >= 33) {
        DECODE_FIELD(order.solicited);
    }

    OrderState orderState;

    DECODE_FIELD( order.whatIf); // ver 16 field

    DECODE_FIELD( orderState.status); // ver 16 field
    DECODE_FIELD( orderState.initMargin); // ver 16 field
    DECODE_FIELD( orderState.maintMargin); // ver 16 field
    DECODE_FIELD( orderState.equityWithLoan); // ver 16 field
    DECODE_FIELD_MAX( orderState.commission); // ver 16 field
    DECODE_FIELD_MAX( orderState.minCommission); // ver 16 field
    DECODE_FIELD_MAX( orderState.maxCommission); // ver 16 field
    DECODE_FIELD( orderState.commissionCurrency); // ver 16 field
    DECODE_FIELD( orderState.warningText); // ver 16 field

    if (version >= 34) {
        DECODE_FIELD(order.randomizeSize);
        DECODE_FIELD(order.randomizePrice);
    }

    if (m_serverVersion >= MIN_SERVER_VER_PEGGED_TO_BENCHMARK) {
        if (order.orderType == "PEG BENCH") {
            DECODE_FIELD(order.referenceContractId);
            DECODE_FIELD(order.isPeggedChangeAmountDecrease);
            DECODE_FIELD(order.peggedChangeAmount);
            DECODE_FIELD(order.referenceChangeAmount);
            DECODE_FIELD(order.referenceExchangeId);
        }

        int conditionsSize;

        DECODE_FIELD(conditionsSize);

        if (conditionsSize > 0) {
            for (; conditionsSize; conditionsSize--) {
                int conditionType;

                DECODE_FIELD(conditionType);

                std::shared_ptr<OrderCondition> item = std::shared_ptr<OrderCondition>(OrderCondition::create(static_cast<OrderCondition::OrderConditionType>(conditionType)));

                if (!(ptr = item->readExternal(ptr, endPtr)))
                    return 0;

                order.conditions.push_back(item);
            }

            DECODE_FIELD(order.conditionsIgnoreRth);
            DECODE_FIELD(order.conditionsCancelOrder);
        }

        DECODE_FIELD(order.adjustedOrderType);
        DECODE_FIELD(order.triggerPrice);
        DECODE_FIELD(order.trailStopPrice);
        DECODE_FIELD(order.lmtPriceOffset);
        DECODE_FIELD(order.adjustedStopPrice);
        DECODE_FIELD(order.adjustedStopLimitPrice);
        DECODE_FIELD(order.adjustedTrailingAmount);
        DECODE_FIELD(order.adjustableTrailingUnit);
    }

    if (m_serverVersion >= MIN_SERVER_VER_SOFT_DOLLAR_TIER) {
        std::string name, value, displayName;

        DECODE_FIELD(name);
        DECODE_FIELD(value);
        DECODE_FIELD(displayName);

        order.softDollarTier = SoftDollarTier(name, value, displayName);
    }

    m_pEWrapper->openOrder( (OrderId)order.orderId, contract, order, orderState);

    return ptr;
}

const char* EClient::processAcctValueMsg(const char* ptr, const char* endPtr) {
    int version;
    std::string key;
    std::string val;
    std::string cur;
    std::string accountName;

    DECODE_FIELD( version);
    DECODE_FIELD( key);
    DECODE_FIELD( val);
    DECODE_FIELD( cur);
    DECODE_FIELD( accountName); // ver 2 field

    m_pEWrapper->updateAccountValue( key, val, cur, accountName);
    return ptr;
}

const char* EClient::processPortfolioValueMsg(const char* ptr, const char* endPtr) {
    // decode version
    int version;
    DECODE_FIELD( version);

    // read contract fields
    Contract contract;
    DECODE_FIELD( contract.conId); // ver 6 field
    DECODE_FIELD( contract.symbol);
    DECODE_FIELD( contract.secType);
    DECODE_FIELD( contract.lastTradeDateOrContractMonth);
    DECODE_FIELD( contract.strike);
    DECODE_FIELD( contract.right);

    if( version >= 7) {
        DECODE_FIELD( contract.multiplier);
        DECODE_FIELD( contract.primaryExchange);
    }

    DECODE_FIELD( contract.currency);
    DECODE_FIELD( contract.localSymbol); // ver 2 field
    if (version >= 8) {
        DECODE_FIELD( contract.tradingClass);
    }

    double  position;
    double  marketPrice;
    double  marketValue;
    double  averageCost;
    double  unrealizedPNL;
    double  realizedPNL;

    if (m_serverVersion >= MIN_SERVER_VER_FRACTIONAL_POSITIONS)
    {
        DECODE_FIELD( position);
    }
    else
    {
        int iPosition;

        DECODE_FIELD(iPosition);

        position = iPosition;
    }

    DECODE_FIELD( marketPrice);
    DECODE_FIELD( marketValue);
    DECODE_FIELD( averageCost); // ver 3 field
    DECODE_FIELD( unrealizedPNL); // ver 3 field
    DECODE_FIELD( realizedPNL); // ver 3 field

    std::string accountName;
    DECODE_FIELD( accountName); // ver 4 field

    if( version == 6 && m_serverVersion == 39) {
        DECODE_FIELD( contract.primaryExchange);
    }

    m_pEWrapper->updatePortfolio( contract,
        position, marketPrice, marketValue, averageCost,
        unrealizedPNL, realizedPNL, accountName);

    return ptr;
}

const char* EClient::processAcctUpdateTimeMsg(const char* ptr, const char* endPtr) {
    int version;
    std::string accountTime;

    DECODE_FIELD( version);
    DECODE_FIELD( accountTime);

    m_pEWrapper->updateAccountTime( accountTime);

    return ptr;
}

const char* EClient::processNextValidIdMsg(const char* ptr, const char* endPtr) {
    int version;
    int orderId;

    DECODE_FIELD( version);
    DECODE_FIELD( orderId);

    m_pEWrapper->nextValidId(orderId);

    return ptr;
}

const char* EClient::processContractDataMsg(const char* ptr, const char* endPtr) {
    int version;
    DECODE_FIELD( version);

    int reqId = -1;
    if( version >= 3) {
        DECODE_FIELD( reqId);
    }

    ContractDetails contract;
    DECODE_FIELD( contract.summary.symbol);
    DECODE_FIELD( contract.summary.secType);
    DECODE_FIELD( contract.summary.lastTradeDateOrContractMonth);
    DECODE_FIELD( contract.summary.strike);
    DECODE_FIELD( contract.summary.right);
    DECODE_FIELD( contract.summary.exchange);
    DECODE_FIELD( contract.summary.currency);
    DECODE_FIELD( contract.summary.localSymbol);
    DECODE_FIELD( contract.marketName);
    DECODE_FIELD( contract.summary.tradingClass);
    DECODE_FIELD( contract.summary.conId);
    DECODE_FIELD( contract.minTick);
    DECODE_FIELD( contract.summary.multiplier);
    DECODE_FIELD( contract.orderTypes);
    DECODE_FIELD( contract.validExchanges);
    DECODE_FIELD( contract.priceMagnifier); // ver 2 field
    if( version >= 4) {
        DECODE_FIELD( contract.underConId);
    }
    if( version >= 5) {
        DECODE_FIELD( contract.longName);
        DECODE_FIELD( contract.summary.primaryExchange);
    }
    if( version >= 6) {
        DECODE_FIELD( contract.contractMonth);
        DECODE_FIELD( contract.industry);
        DECODE_FIELD( contract.category);
        DECODE_FIELD( contract.subcategory);
        DECODE_FIELD( contract.timeZoneId);
        DECODE_FIELD( contract.tradingHours);
        DECODE_FIELD( contract.liquidHours);
    }
    if( version >= 8) {
        DECODE_FIELD( contract.evRule);
        DECODE_FIELD( contract.evMultiplier);
    }
    if( version >= 7) {
        int secIdListCount = 0;
        DECODE_FIELD( secIdListCount);
        if( secIdListCount > 0) {
            TagValueListSPtr secIdList( new TagValueList);
            secIdList->reserve( secIdListCount);
            for( int i = 0; i < secIdListCount; ++i) {
                TagValueSPtr tagValue( new TagValue());
                DECODE_FIELD( tagValue->tag);
                DECODE_FIELD( tagValue->value);
                secIdList->push_back( tagValue);
            }
            contract.secIdList = secIdList;
        }
    }

    m_pEWrapper->contractDetails( reqId, contract);

    return ptr;
}

const char* EClient::processBondContractDataMsg(const char* ptr, const char* endPtr) {
    int version;
    DECODE_FIELD( version);

    int reqId = -1;
    if( version >= 3) {
        DECODE_FIELD( reqId);
    }

    ContractDetails contract;
    DECODE_FIELD( contract.summary.symbol);
    DECODE_FIELD( contract.summary.secType);
    DECODE_FIELD( contract.cusip);
    DECODE_FIELD( contract.coupon);
    DECODE_FIELD( contract.maturity);
    DECODE_FIELD( contract.issueDate);
    DECODE_FIELD( contract.ratings);
    DECODE_FIELD( contract.bondType);
    DECODE_FIELD( contract.couponType);
    DECODE_FIELD( contract.convertible);
    DECODE_FIELD( contract.callable);
    DECODE_FIELD( contract.putable);
    DECODE_FIELD( contract.descAppend);
    DECODE_FIELD( contract.summary.exchange);
    DECODE_FIELD( contract.summary.currency);
    DECODE_FIELD( contract.marketName);
    DECODE_FIELD( contract.summary.tradingClass);
    DECODE_FIELD( contract.summary.conId);
    DECODE_FIELD( contract.minTick);
    DECODE_FIELD( contract.orderTypes);
    DECODE_FIELD( contract.validExchanges);
    DECODE_FIELD( contract.nextOptionDate); // ver 2 field
    DECODE_FIELD( contract.nextOptionType); // ver 2 field
    DECODE_FIELD( contract.nextOptionPartial); // ver 2 field
    DECODE_FIELD( contract.notes); // ver 2 field
    if( version >= 4) {
        DECODE_FIELD( contract.longName);
    }
    if( version >= 6) {
        DECODE_FIELD( contract.evRule);
        DECODE_FIELD( contract.evMultiplier);
    }
    if( version >= 5) {
        int secIdListCount = 0;
        DECODE_FIELD( secIdListCount);
        if( secIdListCount > 0) {
            TagValueListSPtr secIdList( new TagValueList);
            secIdList->reserve( secIdListCount);
            for( int i = 0; i < secIdListCount; ++i) {
                TagValueSPtr tagValue( new TagValue());
                DECODE_FIELD( tagValue->tag);
                DECODE_FIELD( tagValue->value);
                secIdList->push_back( tagValue);
            }
            contract.secIdList = secIdList;
        }
    }

    m_pEWrapper->bondContractDetails( reqId, contract);

    return ptr;
}

const char* EClient::processExecutionDataMsg(const char* ptr, const char* endPtr) {
    int version;
    DECODE_FIELD( version);

    int reqId = -1;
    if( version >= 7) {
        DECODE_FIELD(reqId);
    }

    int orderId;
    DECODE_FIELD( orderId);

    // decode contract fields
    Contract contract;
    DECODE_FIELD( contract.conId); // ver 5 field
    DECODE_FIELD( contract.symbol);
    DECODE_FIELD( contract.secType);
    DECODE_FIELD( contract.lastTradeDateOrContractMonth);
    DECODE_FIELD( contract.strike);
    DECODE_FIELD( contract.right);
    if( version >= 9) {
        DECODE_FIELD( contract.multiplier);
    }
    DECODE_FIELD( contract.exchange);
    DECODE_FIELD( contract.currency);
    DECODE_FIELD( contract.localSymbol);
    if (version >= 10) {
        DECODE_FIELD( contract.tradingClass);
    }

    // decode execution fields
    Execution exec;
    exec.orderId = orderId;
    DECODE_FIELD( exec.execId);
    DECODE_FIELD( exec.time);
    DECODE_FIELD( exec.acctNumber);
    DECODE_FIELD( exec.exchange);
    DECODE_FIELD( exec.side);

    if (m_serverVersion >= MIN_SERVER_VER_FRACTIONAL_POSITIONS) {
        DECODE_FIELD( exec.shares)
    }
    else {
        int iShares;

        DECODE_FIELD(iShares);

        exec.shares = iShares;
    }

    DECODE_FIELD( exec.price);
    DECODE_FIELD( exec.permId); // ver 2 field
    DECODE_FIELD( exec.clientId); // ver 3 field
    DECODE_FIELD( exec.liquidation); // ver 4 field

    if( version >= 6) {
        DECODE_FIELD( exec.cumQty);
        DECODE_FIELD( exec.avgPrice);
    }

    if( version >= 8) {
        DECODE_FIELD( exec.orderRef);
    }

    if( version >= 9) {
        DECODE_FIELD( exec.evRule);
        DECODE_FIELD( exec.evMultiplier);
    }
    if( m_serverVersion >= MIN_SERVER_VER_MODELS_SUPPORT) {
        DECODE_FIELD( exec.modelCode);
    }

    m_pEWrapper->execDetails( reqId, contract, exec);

    return ptr;
}

const char* EClient::processMarketDepthMsg(const char* ptr, const char* endPtr) {
    int version;
    int id;
    int position;
    int operation;
    int side;
    double price;
    int size;

    DECODE_FIELD( version);
    DECODE_FIELD( id);
    DECODE_FIELD( position);
    DECODE_FIELD( operation);
    DECODE_FIELD( side);
    DECODE_FIELD( price);
    DECODE_FIELD( size);

    m_pEWrapper->updateMktDepth( id, position, operation, side, price, size);

    return ptr;
}

const char* EClient::processMarketDepthL2Msg(const char* ptr, const char* endPtr) {
    int version;
    int id;
    int position;
    std::string marketMaker;
    int operation;
    int side;
    double price;
    int size;

    DECODE_FIELD( version);
    DECODE_FIELD( id);
    DECODE_FIELD( position);
    DECODE_FIELD( marketMaker);
    DECODE_FIELD( operation);
    DECODE_FIELD( side);
    DECODE_FIELD( price);
    DECODE_FIELD( size);

    m_pEWrapper->updateMktDepthL2( id, position, marketMaker, operation, side,
        price, size);

    return ptr;
}

const char* EClient::processNewsBulletinsMsg(const char* ptr, const char* endPtr) {
    int version;
    int msgId;
    int msgType;
    std::string newsMessage;
    std::string originatingExch;

    DECODE_FIELD( version);
    DECODE_FIELD( msgId);
    DECODE_FIELD( msgType);
    DECODE_FIELD( newsMessage);
    DECODE_FIELD( originatingExch);

    m_pEWrapper->updateNewsBulletin( msgId, msgType, newsMessage, originatingExch);

    return ptr;
}

const char* EClient::processManagedAcctsMsg(const char* ptr, const char* endPtr) {
    int version;
    std::string accountsList;

    DECODE_FIELD( version);
    DECODE_FIELD( accountsList);

    m_pEWrapper->managedAccounts( accountsList);

    return ptr;
}

const char* EClient::processReceiveFaMsg(const char* ptr, const char* endPtr) {
    int version;
    int faDataTypeInt;
    std::string cxml;

    DECODE_FIELD( version);
    DECODE_FIELD( faDataTypeInt);
    DECODE_FIELD( cxml);

    m_pEWrapper->receiveFA( (faDataType)faDataTypeInt, cxml);

    return ptr;
}

const char* EClient::processHistoricalDataMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;
    std::string startDateStr;
    std::string endDateStr;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);
    DECODE_FIELD( startDateStr); // ver 2 field
    DECODE_FIELD( endDateStr); // ver 2 field

    int itemCount;
    DECODE_FIELD( itemCount);

    typedef std::vector<BarData> BarDataList;
    BarDataList bars;

    bars.reserve( itemCount);

    for( int ctr = 0; ctr < itemCount; ++ctr) {

        BarData bar;
        DECODE_FIELD( bar.date);
        DECODE_FIELD( bar.open);
        DECODE_FIELD( bar.high);
        DECODE_FIELD( bar.low);
        DECODE_FIELD( bar.close);
        DECODE_FIELD( bar.volume);
        DECODE_FIELD( bar.average);
        DECODE_FIELD( bar.hasGaps);
        DECODE_FIELD( bar.barCount); // ver 3 field

        bars.push_back(bar);
    }

    assert( (int)bars.size() == itemCount);

    for( int ctr = 0; ctr < itemCount; ++ctr) {

        const BarData& bar = bars[ctr];
        m_pEWrapper->historicalData( reqId, bar.date, bar.open, bar.high, bar.low,
            bar.close, bar.volume, bar.barCount, bar.average,
            bar.hasGaps == "true");
    }

    // send end of dataset marker
    std::string finishedStr = std::string("finished-") + startDateStr + "-" + endDateStr;
    m_pEWrapper->historicalData( reqId, finishedStr, -1, -1, -1, -1, -1, -1, -1, 0);

    return ptr;
}

const char* EClient::processScannerDataMsg(const char* ptr, const char* endPtr) {
    int version;
    int tickerId;

    DECODE_FIELD( version);
    DECODE_FIELD( tickerId);

    int numberOfElements;
    DECODE_FIELD( numberOfElements);

    typedef std::vector<ScanData> ScanDataList;
    ScanDataList scannerDataList;

    scannerDataList.reserve( numberOfElements);

    for( int ctr=0; ctr < numberOfElements; ++ctr) {

        ScanData data;

        DECODE_FIELD( data.rank);
        DECODE_FIELD( data.contract.summary.conId); // ver 3 field
        DECODE_FIELD( data.contract.summary.symbol);
        DECODE_FIELD( data.contract.summary.secType);
        DECODE_FIELD( data.contract.summary.lastTradeDateOrContractMonth);
        DECODE_FIELD( data.contract.summary.strike);
        DECODE_FIELD( data.contract.summary.right);
        DECODE_FIELD( data.contract.summary.exchange);
        DECODE_FIELD( data.contract.summary.currency);
        DECODE_FIELD( data.contract.summary.localSymbol);
        DECODE_FIELD( data.contract.marketName);
        DECODE_FIELD( data.contract.summary.tradingClass);
        DECODE_FIELD( data.distance);
        DECODE_FIELD( data.benchmark);
        DECODE_FIELD( data.projection);
        DECODE_FIELD( data.legsStr);

        scannerDataList.push_back( data);
    }

    assert( (int)scannerDataList.size() == numberOfElements);

    for( int ctr=0; ctr < numberOfElements; ++ctr) {

        const ScanData& data = scannerDataList[ctr];
        m_pEWrapper->scannerData( tickerId, data.rank, data.contract,
            data.distance, data.benchmark, data.projection, data.legsStr);
    }

    m_pEWrapper->scannerDataEnd( tickerId);

    return ptr;
}

const char* EClient::processScannerParametersMsg(const char* ptr, const char* endPtr) {
    int version;
    std::string xml;

    DECODE_FIELD( version);
    DECODE_FIELD( xml);

    m_pEWrapper->scannerParameters( xml);

    return ptr;
}

const char* EClient::processCurrentTimeMsg(const char* ptr, const char* endPtr) {
    int version;
    int time;

    DECODE_FIELD(version);
    DECODE_FIELD(time);

    m_pEWrapper->currentTime( time);

    return ptr;
}

const char* EClient::processRealTimeBarsMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;
    int time;
    double open;
    double high;
    double low;
    double close;
    int volume;
    double average;
    int count;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);
    DECODE_FIELD( time);
    DECODE_FIELD( open);
    DECODE_FIELD( high);
    DECODE_FIELD( low);
    DECODE_FIELD( close);
    DECODE_FIELD( volume);
    DECODE_FIELD( average);
    DECODE_FIELD( count);

    m_pEWrapper->realtimeBar( reqId, time, open, high, low, close,
        volume, average, count);

    return ptr;
}

const char* EClient::processFundamentalDataMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;
    std::string data;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);
    DECODE_FIELD( data);

    m_pEWrapper->fundamentalData( reqId, data);

    return ptr;
}

const char* EClient::processContractDataEndMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);

    m_pEWrapper->contractDetailsEnd( reqId);

    return ptr;
}

const char* EClient::processOpenOrderEndMsg(const char* ptr, const char* endPtr) {
    int version;

    DECODE_FIELD( version);

    m_pEWrapper->openOrderEnd();

    return ptr;
}

const char* EClient::processAcctDownloadEndMsg(const char* ptr, const char* endPtr) {
    int version;
    std::string account;

    DECODE_FIELD( version);
    DECODE_FIELD( account);

    m_pEWrapper->accountDownloadEnd( account);

    return ptr;
}

const char* EClient::processExecutionDataEndMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);

    m_pEWrapper->execDetailsEnd( reqId);

    return ptr;
}

const char* EClient::processDeltaNeutralValidationMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);

    UnderComp underComp;

    DECODE_FIELD( underComp.conId);
    DECODE_FIELD( underComp.delta);
    DECODE_FIELD( underComp.price);

    m_pEWrapper->deltaNeutralValidation( reqId, underComp);

    return ptr;
}

const char* EClient::processTickSnapshotEndMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);

    m_pEWrapper->tickSnapshotEnd( reqId);

    return ptr;
}

const char* EClient::processMarketDataTypeMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;
    int marketDataType;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);
    DECODE_FIELD( marketDataType);

    m_pEWrapper->marketDataType( reqId, marketDataType);

    return ptr;
}

const char* EClient::processCommissionReportMsg(const char* ptr, const char* endPtr) {
    int version;
    DECODE_FIELD( version);

    CommissionReport commissionReport;
    DECODE_FIELD( commissionReport.execId);
    DECODE_FIELD( commissionReport.commission);
    DECODE_FIELD( commissionReport.currency);
    DECODE_FIELD( commissionReport.realizedPNL);
    DECODE_FIELD( commissionReport.yield);
    DECODE_FIELD( commissionReport.yieldRedemptionDate);

    m_pEWrapper->commissionReport( commissionReport);

    return ptr;
}

const char* EClient::processPositionDataMsg(const char* ptr, const char* endPtr) {
    int version;
    std::string account;
    double position;
    double avgCost = 0;

    DECODE_FIELD( version);
    DECODE_FIELD( account);

    // decode contract fields
    Contract contract;
    DECODE_FIELD( contract.conId);
    DECODE_FIELD( contract.symbol);
    DECODE_FIELD( contract.secType);
    DECODE_FIELD( contract.lastTradeDateOrContractMonth);
    DECODE_FIELD( contract.strike);
    DECODE_FIELD( contract.right);
    DECODE_FIELD( contract.multiplier);
    DECODE_FIELD( contract.exchange);
    DECODE_FIELD( contract.currency);
    DECODE_FIELD( contract.localSymbol);
    if (version >= 2) {
        DECODE_FIELD( contract.tradingClass);
    }

    if (m_serverVersion >= MIN_SERVER_VER_FRACTIONAL_POSITIONS)
    {
        DECODE_FIELD( position);
    }
    else
    {
        int iPosition;

        DECODE_FIELD(iPosition);

        position = iPosition;
    }

    if (version >= 3) {
        DECODE_FIELD( avgCost);
    }

    m_pEWrapper->position( account, contract, position, avgCost);

    return ptr;
}

const char* EClient::processPositionEndMsg(const char* ptr, const char* endPtr) {
    int version;

    DECODE_FIELD( version);

    m_pEWrapper->positionEnd();

    return ptr;
}

const char* EClient::processAccountSummaryMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;
    std::string account;
    std::string tag;
    std::string value;
    std::string curency;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);
    DECODE_FIELD( account);
    DECODE_FIELD( tag);
    DECODE_FIELD( value);
    DECODE_FIELD( curency);

    m_pEWrapper->accountSummary( reqId, account, tag, value, curency);

    return ptr;
}

const char* EClient::processAccountSummaryEndMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);

    m_pEWrapper->accountSummaryEnd( reqId);

    return ptr;
}

const char* EClient::processVerifyMessageApiMsg(const char* ptr, const char* endPtr) {
    int version;
    std::string apiData;

    DECODE_FIELD( version);
    DECODE_FIELD( apiData);

    m_pEWrapper->verifyMessageAPI( apiData);

    return ptr;
}

const char* EClient::processVerifyCompletedMsg(const char* ptr, const char* endPtr) {
    int version;
    std::string isSuccessful;
    std::string errorText;

    DECODE_FIELD( version);
    DECODE_FIELD( isSuccessful);
    DECODE_FIELD( errorText);

    bool bRes = isSuccessful == "true";

    m_pEWrapper->verifyCompleted( bRes, errorText);

    return ptr;
}

const char* EClient::processDisplayGroupListMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;
    std::string groups;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);
    DECODE_FIELD( groups);

    m_pEWrapper->displayGroupList( reqId, groups);

    return ptr;
}

const char* EClient::processDisplayGroupUpdatedMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;
    std::string contractInfo;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);
    DECODE_FIELD( contractInfo);

    m_pEWrapper->displayGroupUpdated( reqId, contractInfo);

    return ptr;
}

const char* EClient::processVerifyAndAuthMessageApiMsg(const char* ptr, const char* endPtr) {
    int version;
    std::string apiData;
    std::string xyzChallenge;

    DECODE_FIELD( version);
    DECODE_FIELD( apiData);
    DECODE_FIELD( xyzChallenge);

    m_pEWrapper->verifyAndAuthMessageAPI( apiData, xyzChallenge);

    return ptr;
}

const char* EClient::processVerifyAndAuthCompletedMsg(const char* ptr, const char* endPtr) {
    int version;
    std::string isSuccessful;
    std::string errorText;

    DECODE_FIELD( version);
    DECODE_FIELD( isSuccessful);
    DECODE_FIELD( errorText);

    bool bRes = isSuccessful == "true";

    m_pEWrapper->verifyAndAuthCompleted( bRes, errorText);

    return ptr;
}

const char* EClient::processPositionMultiMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;
    std::string account;
    std::string modelCode;
    double position;
    double avgCost = 0;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);
    DECODE_FIELD( account);

    // decode contract fields
    Contract contract;
    DECODE_FIELD( contract.conId);
    DECODE_FIELD( contract.symbol);
    DECODE_FIELD( contract.secType);
    DECODE_FIELD( contract.lastTradeDateOrContractMonth);
    DECODE_FIELD( contract.strike);
    DECODE_FIELD( contract.right);
    DECODE_FIELD( contract.multiplier);
    DECODE_FIELD( contract.exchange);
    DECODE_FIELD( contract.currency);
    DECODE_FIELD( contract.localSymbol);
    DECODE_FIELD( contract.tradingClass);
    DECODE_FIELD( position);
    DECODE_FIELD( avgCost);
    DECODE_FIELD( modelCode);

    m_pEWrapper->positionMulti( reqId, account, modelCode, contract, position, avgCost);

    return ptr;
}

const char* EClient::processPositionMultiEndMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);

    m_pEWrapper->positionMultiEnd( reqId);

    return ptr;
}

const char* EClient::processAccountUpdateMultiMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;
    std::string account;
    std::string modelCode;
    std::string key;
    std::string value;
    std::string currency;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);
    DECODE_FIELD( account);
    DECODE_FIELD( modelCode);
    DECODE_FIELD( key);
    DECODE_FIELD( value);
    DECODE_FIELD( currency);

    m_pEWrapper->accountUpdateMulti( reqId, account, modelCode, key, value, currency);

    return ptr;
}

const char* EClient::processAccountUpdateMultiEndMsg(const char* ptr, const char* endPtr) {
    int version;
    int reqId;

    DECODE_FIELD( version);
    DECODE_FIELD( reqId);

    m_pEWrapper->accountUpdateMultiEnd( reqId);

    return ptr;
}

const char* EClient::processSecurityDefinitionOptionalParameterMsg(const char* ptr, const char* endPtr) {
    int reqId;
    std::string exchange;
    int underlyingConId;
    std::string tradingClass;
    std::string multiplier;
    int expirationsSize, strikesSize;
    std::set<std::string> expirations;
    std::set<double> strikes;

    DECODE_FIELD(reqId);
    DECODE_FIELD(exchange);
    DECODE_FIELD(underlyingConId);
    DECODE_FIELD(tradingClass);
    DECODE_FIELD(multiplier);
    DECODE_FIELD(expirationsSize);

    for (int i = 0; i < expirationsSize; i++) {
        std::string expiration;

        DECODE_FIELD(expiration);

        expirations.insert(expiration);
    }

    DECODE_FIELD(strikesSize);

    for (int i = 0; i < strikesSize; i++) {
        double strike;

        DECODE_FIELD(strike);

        strikes.insert(strike);
    }

    m_pEWrapper->securityDefinitionOptionalParameter(reqId, exchange, underlyingConId, tradingClass, multiplier, expirations, strikes);

    return ptr;
}

const char* EClient::processSecurityDefinitionOptionalParameterEndMsg(const char* ptr, const char* endPtr) {
    int reqId;

    DECODE_FIELD(reqId);

    m_pEWrapper->securityDefinitionOptionalParameterEnd(reqId);

    return ptr;
}

const char* EClient::processSoftDollarTiersMsg(const char* ptr, const char* endPtr)
{
    int reqId;
    int nTiers;

    DECODE_FIELD(reqId);
    DECODE_FIELD(nTiers);

    std::vector<SoftDollarTier> tiers(nTiers);

    for (int i = 0; i < nTiers; i++) {
        std::string name, value, dislplayName;

        DECODE_FIELD(name);
        DECODE_FIELD(value);
        DECODE_FIELD(dislplayName);

        tiers[i] = SoftDollarTier(name, value, value);
    }

    m_pEWrapper->softDollarTiers(reqId, tiers);

    return ptr;
}


int EClient::processConnectAck(const char*& beginPtr, const char* endPtr)
{
    // process a connect Ack message from the buffer;
    // return number of bytes consumed
    assert( beginPtr && beginPtr < endPtr);

    try {

        const char* ptr = beginPtr;

        // check server version
        DECODE_FIELD( m_serverVersion);

        // handle redirects
        if( m_serverVersion < 0) {
            m_serverVersion = 0;

            std::string hostport, host;
            int port = -1;

            DECODE_FIELD( hostport);

            std::string::size_type sep = hostport.find( ':');
            if( sep != std::string::npos) {
                host = hostport.substr(0, sep);
                port = atoi( hostport.c_str() + ++sep);
            }
            else {
                host = hostport;
            }

            redirect(host, port);
        } else {
            std::string twsTime;

            if( m_serverVersion >= 20) {

                DECODE_FIELD(twsTime);
            }

            handShakeAck(m_serverVersion, twsTime);

            m_pEWrapper->connectAck();
        }

        int processed = ptr - beginPtr;
        beginPtr = ptr;
        return processed;
    }
    catch(  std::exception e) {
        m_pEWrapper->error( NO_VALID_ID, SOCKET_EXCEPTION.code(),
            SOCKET_EXCEPTION.msg() + errMsg( e) );
    }

    return 0;
}


int EClient::parseAndProcessMsg(const char*& beginPtr, const char* endPtr) {
    // process a single message from the buffer;
    // return number of bytes consumed

    assert( beginPtr && beginPtr < endPtr);

    if (m_serverVersion == 0)
        return processConnectAck(beginPtr, endPtr);

    try {

        const char* ptr = beginPtr;

        int msgId;
        DECODE_FIELD( msgId);

        switch( msgId) {
        case TICK_PRICE:
            ptr = processTickPriceMsg(ptr, endPtr);
            break;

        case TICK_SIZE:
            ptr = processTickSizeMsg(ptr, endPtr);
            break;

        case TICK_OPTION_COMPUTATION:
            ptr = processTickOptionComputationMsg(ptr, endPtr);
            break;

        case TICK_GENERIC:
            ptr = processTickGenericMsg(ptr, endPtr);
            break;

        case TICK_STRING:
            ptr = processTickStringMsg(ptr, endPtr);
            break;

        case TICK_EFP:
            ptr = processTickEfpMsg(ptr, endPtr);
            break;

        case ORDER_STATUS:
            ptr = processOrderStatusMsg(ptr, endPtr);
            break;

        case ERR_MSG:
            ptr = processErrMsgMsg(ptr, endPtr);
            break;

        case OPEN_ORDER:
            ptr = processOpenOrderMsg(ptr, endPtr);
            break;

        case ACCT_VALUE:
            ptr = processAcctValueMsg(ptr, endPtr);
            break;

        case PORTFOLIO_VALUE:
            ptr = processPortfolioValueMsg(ptr, endPtr);
            break;

        case ACCT_UPDATE_TIME:
            ptr = processAcctUpdateTimeMsg(ptr, endPtr);
            break;

        case NEXT_VALID_ID:
            ptr = processNextValidIdMsg(ptr, endPtr);
            break;

        case CONTRACT_DATA:
            ptr = processContractDataMsg(ptr, endPtr);
            break;

        case BOND_CONTRACT_DATA:
            ptr = processBondContractDataMsg(ptr, endPtr);
            break;

        case EXECUTION_DATA:
            ptr = processExecutionDataMsg(ptr, endPtr);
            break;

        case MARKET_DEPTH:
            ptr = processMarketDepthMsg(ptr, endPtr);
            break;

        case MARKET_DEPTH_L2:
            ptr = processMarketDepthL2Msg(ptr, endPtr);
            break;

        case NEWS_BULLETINS:
            ptr = processNewsBulletinsMsg(ptr, endPtr);
            break;

        case MANAGED_ACCTS:
            ptr = processManagedAcctsMsg(ptr, endPtr);
            break;

        case RECEIVE_FA:
            ptr = processReceiveFaMsg(ptr, endPtr);
            break;

        case HISTORICAL_DATA:
            ptr = processHistoricalDataMsg(ptr, endPtr);
            break;

        case SCANNER_DATA:
            ptr = processScannerDataMsg(ptr, endPtr);
            break;

        case SCANNER_PARAMETERS:
            ptr = processScannerParametersMsg(ptr, endPtr);
            break;

        case CURRENT_TIME:
            ptr = processCurrentTimeMsg(ptr, endPtr);
            break;

        case REAL_TIME_BARS:
            ptr = processRealTimeBarsMsg(ptr, endPtr);
            break;

        case FUNDAMENTAL_DATA:
            ptr = processFundamentalDataMsg(ptr, endPtr);
            break;

        case CONTRACT_DATA_END:
            ptr = processContractDataEndMsg(ptr, endPtr);
            break;

        case OPEN_ORDER_END:
            ptr = processOpenOrderEndMsg(ptr, endPtr);
            break;

        case ACCT_DOWNLOAD_END:
            ptr = processAcctDownloadEndMsg(ptr, endPtr);
            break;

        case EXECUTION_DATA_END:
            ptr = processExecutionDataEndMsg(ptr, endPtr);
            break;

        case DELTA_NEUTRAL_VALIDATION:
            ptr = processDeltaNeutralValidationMsg(ptr, endPtr);
            break;

        case TICK_SNAPSHOT_END:
            ptr = processTickSnapshotEndMsg(ptr, endPtr);
            break;

        case MARKET_DATA_TYPE:
            ptr = processMarketDataTypeMsg(ptr, endPtr);
            break;

        case COMMISSION_REPORT:
            ptr = processCommissionReportMsg(ptr, endPtr);
            break;

        case POSITION_DATA:
            ptr = processPositionDataMsg(ptr, endPtr);
            break;

        case POSITION_END:
            ptr = processPositionEndMsg(ptr, endPtr);
            break;

        case ACCOUNT_SUMMARY:
            ptr = processAccountSummaryMsg(ptr, endPtr);
            break;

        case ACCOUNT_SUMMARY_END:
            ptr = processAccountSummaryEndMsg(ptr, endPtr);
            break;

        case VERIFY_MESSAGE_API:
            ptr = processVerifyMessageApiMsg(ptr, endPtr);
            break;

        case VERIFY_COMPLETED:
            ptr = processVerifyCompletedMsg(ptr, endPtr);
            break;

        case DISPLAY_GROUP_LIST:
            ptr = processDisplayGroupListMsg(ptr, endPtr);
            break;

        case DISPLAY_GROUP_UPDATED:
            ptr = processDisplayGroupUpdatedMsg(ptr, endPtr);
            break;

        case VERIFY_AND_AUTH_MESSAGE_API:
            ptr = processVerifyAndAuthMessageApiMsg(ptr, endPtr);
            break;

        case VERIFY_AND_AUTH_COMPLETED:
            ptr = processVerifyAndAuthCompletedMsg(ptr, endPtr);
            break;

        case POSITION_MULTI:
            ptr = processPositionMultiMsg(ptr, endPtr);
            break;

        case POSITION_MULTI_END:
            ptr = processPositionMultiEndMsg(ptr, endPtr);
            break;

        case ACCOUNT_UPDATE_MULTI:
            ptr = processAccountUpdateMultiMsg(ptr, endPtr);
            break;

        case ACCOUNT_UPDATE_MULTI_END:
            ptr = processAccountUpdateMultiEndMsg(ptr, endPtr);
            break;

        case SECURITY_DEFINITION_OPTION_PARAMETER:
            ptr = processSecurityDefinitionOptionalParameterMsg(ptr, endPtr);
            break;

        case SECURITY_DEFINITION_OPTION_PARAMETER_END:
            ptr = processSecurityDefinitionOptionalParameterEndMsg(ptr, endPtr);
            break;

        case SOFT_DOLLAR_TIERS:
            ptr = processSoftDollarTiersMsg(ptr, endPtr);
            break;

        default:
            {
                m_pEWrapper->error( msgId, UNKNOWN_ID.code(), UNKNOWN_ID.msg());
                m_pEWrapper->connectionClosed();
                break;
            }
        }

        if (!ptr)
            return 0;

        int processed = ptr - beginPtr;
        beginPtr = ptr;
        return processed;
    }
    catch( std::exception e) {
        m_pEWrapper->error( NO_VALID_ID, SOCKET_EXCEPTION.code(),
            SOCKET_EXCEPTION.msg() + errMsg(e));
    }
    return 0;
}


void EClient::encodeMsgLen(std::string& msg, unsigned offset) const
{
    assert( !msg.empty());
    assert( m_useV100Plus);

    assert( sizeof(unsigned) == HEADER_LEN);
    assert( msg.size() > offset + HEADER_LEN);
    unsigned len = msg.size() - HEADER_LEN - offset;
    if( len > MAX_MSG_LEN) {
        m_pEWrapper->error( NO_VALID_ID, BAD_LENGTH.code(), BAD_LENGTH.msg());
        return;
    }

    unsigned netlen = host_to_network_long(len);
    memcpy( &msg[offset], &netlen, HEADER_LEN);
}

void EClient::closeAndSend(std::string msg, unsigned offset)
{
    assert( !msg.empty());
    if( m_useV100Plus) {
        encodeMsgLen( msg, offset);
    }

    on_send(msg);
}

void EClient::prepareBufferImpl(std::ostream& buf) const
{
    assert( m_useV100Plus);
    assert( sizeof(unsigned) == HEADER_LEN);

    char header[HEADER_LEN] = { 0 };
    buf.write( header, sizeof(header));
}

void EClient::prepareBuffer(std::ostream& buf) const
{
    if( !m_useV100Plus)
        return;

    prepareBufferImpl( buf);
}

void EClient::handShakeAck(int version, const std::string& time)
{
    m_connState = CS_CONNECTED;
    m_serverVersion = version;
    m_TwsTime = time;

    if( usingV100Plus() ? (m_serverVersion < MIN_CLIENT_VER || m_serverVersion > MAX_CLIENT_VER) : m_serverVersion < MIN_SERVER_VER_SUPPORTED ) {
        getWrapper()->error( NO_VALID_ID, UNSUPPORTED_VERSION.code(), UNSUPPORTED_VERSION.msg());
        eDisconnect();
    }
    startApi();

    on_handshake_ack();
}

void EClient::redirect(const std::string& host, unsigned port) {
    // handle redirect
    if( (host != this->host() || port != this->port())) {
        if (!m_allowRedirect) {
            getWrapper()->error(NO_VALID_ID, CONNECT_FAIL.code(), CONNECT_FAIL.msg());

            return;
        }

        int sClientId = clientId();
        bool sExtraAuth = extraAuth();

        eDisconnect();

        on_redirect(host, port, sClientId, sExtraAuth);
    }
}
