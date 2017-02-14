#ifndef ECONSTANTS_H
#define ECONSTANTS_H

namespace ibapi {
namespace client_constants {

/////////////////////////////////////////////////////////////////////////////////
// SOCKET CLIENT VERSION CHANGE LOG : Incremented when the format of incomming
//                                    server responses change
/////////////////////////////////////////////////////////////////////////////////
// constants
// 6 = Added parentId to orderStatus
// 7 = The new execDetails event returned for an order filled status and
// reqExecDetails
//     Also added market depth support.
// 8 = Added 'lastFillPrice' to orderStatus and 'permId' to execDetails
// 9 = Added 'avgCost', 'unrealizedPNL', and 'unrealizedPNL' to updatePortfolio
// event
// 10 = Added 'serverId' to the 'open order' & 'order status' events.
//      We send back all the API open orders upon connection.
//      Added new methods reqAllOpenOrders, reqAutoOpenOrders()
//      Added FA support - reqExecution has filter.
//                       - reqAccountUpdates takes acct code.
// 11 = Added permId to openOrder event.
// 12 = Added IgnoreRth, hidden, and discretionaryAmt
// 13 = Added GoodAfterTime
// 14 = always send size on bid/ask/last tick
// 15 = send allocation string with open order
// 16 = can receive account name in account and portfolio updates, and fa params
// in openOrder
// 17 = can receive liquidation field in exec reports, and notAutoAvailable
// field in mkt data
// 18 = can receive good till date field in open order messages, and send
// backfill requests
// 19 = can receive new extended order attributes in OPEN_ORDER
// 20 = expects TWS time string on connection after server version >= 20, and
// parentId in open order
// 21 = can receive bond contract details.
// 22 = can receive price magnifier in contract details
// 23 = support for scanner
// 24 = can receive volatility order parameters in open order messages
// 25 = can receive HMDS query start and end times
// 26 = can receive option vols in option market data messages
// 27 = can receive delta neutral order type and delta neutral aux price
// 28 = can receive option model computation ticks
// 29 = can receive trail stop limit price in open order and can place them: API
// 8.91
// 30 = can receive extended bond contract def, new ticks, and trade count in
// bars
// 31 = can receive EFP extensions to scanner and market data, and combo legs on
// open orders
//    ; can receive RT bars
// 32 = can receive TickType.LAST_TIMESTAMP
// 33 = can receive ScaleNumComponents and ScaleComponentSize is open order
// messages
// 34 = can receive whatIf orders / order state
// 35 = can receive contId field for Contract objects
// 36 = can receive outsideRth field for Order objects
// 37 = can receive clearingAccount and clearingIntent for Order objects
// 38 = can receive multipier and primaryExchange in portfolio updates
//    ; can receive cumQty and avgPrice in execution
//    ; can receive fundamental data
//    ; can receive underComp for Contract objects
//    ; can receive reqId and end marker in contractDetails/bondContractDetails
//    ; can receive ScaleInitComponentSize and ScaleSubsComponentSize for Order
//    objects
// 39 = can receive underConId in contractDetails
// 40 = can receive algoStrategy/algoParams in openOrder
// 41 = can receive end marker for openOrder
//    ; can receive end marker for account download
//    ; can receive end marker for executions download
// 42 = can receive deltaNeutralValidation
// 43 = can receive longName(companyName)
//    ; can receive listingExchange
//    ; can receive RTVolume tick
// 44 = can receive end market for ticker snapshot
// 45 = can receive notHeld field in openOrder
// 46 = can receive contractMonth, industry, category, subcategory fields in
// contractDetails
//    ; can receive timeZoneId, tradingHours, liquidHours fields in
//    contractDetails
// 47 = can receive gamma, vega, theta, undPrice fields in
// TICK_OPTION_COMPUTATION
// 48 = can receive exemptCode in openOrder
// 49 = can receive hedgeType and hedgeParam in openOrder
// 50 = can receive optOutSmartRouting field in openOrder
// 51 = can receive smartComboRoutingParams in openOrder
// 52 = can receive deltaNeutralConId, deltaNeutralSettlingFirm,
// deltaNeutralClearingAccount and deltaNeutralClearingIntent in openOrder
// 53 = can receive orderRef in execution
// 54 = can receive scale order fields (PriceAdjustValue, PriceAdjustInterval,
// ProfitOffset, AutoReset,
//      InitPosition, InitFillQty and RandomPercent) in openOrder
// 55 = can receive orderComboLegs (price) in openOrder
// 56 = can receive trailingPercent in openOrder
// 57 = can receive commissionReport message
// 58 = can receive CUSIP/ISIN/etc. in
// contractDescription/bondContractDescription
// 59 = can receive evRule, evMultiplier in
// contractDescription/bondContractDescription/executionDetails
//      can receive multiplier in executionDetails
// 60 = can receive deltaNeutralOpenClose, deltaNeutralShortSale,
// deltaNeutralShortSaleSlot
//      and deltaNeutralDesignatedLocation in openOrder
//      can receive position, positionEnd, accountSummary and accountSummaryEnd
// 61 = can receive multiplier in openOrder
//      can receive tradingClass in openOrder, updatePortfolio, execDetails and
//      position
// 62 = can receive avgCost in position message
// 63 = can receive verifyMessageAPI, verifyCompleted, displayGroupList and
// displayGroupUpdated messages
// 64 = can receive solicited attrib in openOrder message
// 65 = can receive verifyAndAuthMessageAPI and verifyAndAuthCompleted messages
// 66 = can receive randomize size and randomize price order fields

const int CLIENT_VERSION = 66;

// outgoing msg id's
const int REQ_MKT_DATA = 1;
const int CANCEL_MKT_DATA = 2;
const int PLACE_ORDER = 3;
const int CANCEL_ORDER = 4;
const int REQ_OPEN_ORDERS = 5;
const int REQ_ACCT_DATA = 6;
const int REQ_EXECUTIONS = 7;
const int REQ_IDS = 8;
const int REQ_CONTRACT_DATA = 9;
const int REQ_MKT_DEPTH = 10;
const int CANCEL_MKT_DEPTH = 11;
const int REQ_NEWS_BULLETINS = 12;
const int CANCEL_NEWS_BULLETINS = 13;
const int SET_SERVER_LOGLEVEL = 14;
const int REQ_AUTO_OPEN_ORDERS = 15;
const int REQ_ALL_OPEN_ORDERS = 16;
const int REQ_MANAGED_ACCTS = 17;
const int REQ_FA = 18;
const int REPLACE_FA = 19;
const int REQ_HISTORICAL_DATA = 20;
const int EXERCISE_OPTIONS = 21;
const int REQ_SCANNER_SUBSCRIPTION = 22;
const int CANCEL_SCANNER_SUBSCRIPTION = 23;
const int REQ_SCANNER_PARAMETERS = 24;
const int CANCEL_HISTORICAL_DATA = 25;
const int REQ_CURRENT_TIME = 49;
const int REQ_REAL_TIME_BARS = 50;
const int CANCEL_REAL_TIME_BARS = 51;
const int REQ_FUNDAMENTAL_DATA = 52;
const int CANCEL_FUNDAMENTAL_DATA = 53;
const int REQ_CALC_IMPLIED_VOLAT = 54;
const int REQ_CALC_OPTION_PRICE = 55;
const int CANCEL_CALC_IMPLIED_VOLAT = 56;
const int CANCEL_CALC_OPTION_PRICE = 57;
const int REQ_GLOBAL_CANCEL = 58;
const int REQ_MARKET_DATA_TYPE = 59;
const int REQ_POSITIONS = 61;
const int REQ_ACCOUNT_SUMMARY = 62;
const int CANCEL_ACCOUNT_SUMMARY = 63;
const int CANCEL_POSITIONS = 64;
const int VERIFY_REQUEST = 65;
const int VERIFY_MESSAGE = 66;
const int QUERY_DISPLAY_GROUPS = 67;
const int SUBSCRIBE_TO_GROUP_EVENTS = 68;
const int UPDATE_DISPLAY_GROUP = 69;
const int UNSUBSCRIBE_FROM_GROUP_EVENTS = 70;
const int START_API = 71;
const int VERIFY_AND_AUTH_REQUEST = 72;
const int VERIFY_AND_AUTH_MESSAGE = 73;
const int REQ_POSITIONS_MULTI = 74;
const int CANCEL_POSITIONS_MULTI = 75;
const int REQ_ACCOUNT_UPDATES_MULTI = 76;
const int CANCEL_ACCOUNT_UPDATES_MULTI = 77;
const int REQ_SEC_DEF_OPT_PARAMS = 78;
const int REQ_SOFT_DOLLAR_TIERS = 79;

// TWS New Bulletins constants
const int NEWS_MSG = 1; // standard IB news bulleting message
const int EXCHANGE_AVAIL_MSG =
  2; // control message specifing that an exchange is available for trading
const int EXCHANGE_UNAVAIL_MSG =
  3; // control message specifing that an exchange is unavailable for trading

//const int MIN_SERVER_VER_REAL_TIME_BARS       = 34;
//const int MIN_SERVER_VER_SCALE_ORDERS         = 35;
//const int MIN_SERVER_VER_SNAPSHOT_MKT_DATA    = 35;
//const int MIN_SERVER_VER_SSHORT_COMBO_LEGS    = 35;
//const int MIN_SERVER_VER_WHAT_IF_ORDERS       = 36;
//const int MIN_SERVER_VER_CONTRACT_CONID       = 37;
const int MIN_SERVER_VER_PTA_ORDERS             = 39;
const int MIN_SERVER_VER_FUNDAMENTAL_DATA       = 40;
const int MIN_SERVER_VER_UNDER_COMP             = 40;
const int MIN_SERVER_VER_CONTRACT_DATA_CHAIN    = 40;
const int MIN_SERVER_VER_SCALE_ORDERS2          = 40;
const int MIN_SERVER_VER_ALGO_ORDERS            = 41;
const int MIN_SERVER_VER_EXECUTION_DATA_CHAIN   = 42;
const int MIN_SERVER_VER_NOT_HELD               = 44;
const int MIN_SERVER_VER_SEC_ID_TYPE            = 45;
const int MIN_SERVER_VER_PLACE_ORDER_CONID      = 46;
const int MIN_SERVER_VER_REQ_MKT_DATA_CONID     = 47;
const int MIN_SERVER_VER_REQ_CALC_IMPLIED_VOLAT = 49;
const int MIN_SERVER_VER_REQ_CALC_OPTION_PRICE  = 50;
const int MIN_SERVER_VER_CANCEL_CALC_IMPLIED_VOLAT = 50;
const int MIN_SERVER_VER_CANCEL_CALC_OPTION_PRICE  = 50;
const int MIN_SERVER_VER_SSHORTX_OLD            = 51;
const int MIN_SERVER_VER_SSHORTX                = 52;
const int MIN_SERVER_VER_REQ_GLOBAL_CANCEL      = 53;
const int MIN_SERVER_VER_HEDGE_ORDERS			= 54;
const int MIN_SERVER_VER_REQ_MARKET_DATA_TYPE	= 55;
const int MIN_SERVER_VER_OPT_OUT_SMART_ROUTING  = 56;
const int MIN_SERVER_VER_SMART_COMBO_ROUTING_PARAMS = 57;
const int MIN_SERVER_VER_DELTA_NEUTRAL_CONID    = 58;
const int MIN_SERVER_VER_SCALE_ORDERS3          = 60;
const int MIN_SERVER_VER_ORDER_COMBO_LEGS_PRICE = 61;
const int MIN_SERVER_VER_TRAILING_PERCENT       = 62;
const int MIN_SERVER_VER_DELTA_NEUTRAL_OPEN_CLOSE = 66;
const int MIN_SERVER_VER_POSITIONS              = 67;
const int MIN_SERVER_VER_ACCOUNT_SUMMARY        = 67;
const int MIN_SERVER_VER_TRADING_CLASS          = 68;
const int MIN_SERVER_VER_SCALE_TABLE            = 69;
const int MIN_SERVER_VER_LINKING                = 70;
const int MIN_SERVER_VER_ALGO_ID                = 71;
const int MIN_SERVER_VER_OPTIONAL_CAPABILITIES  = 72;
const int MIN_SERVER_VER_ORDER_SOLICITED        = 73;
const int MIN_SERVER_VER_LINKING_AUTH           = 74;
const int MIN_SERVER_VER_PRIMARYEXCH            = 75;
const int MIN_SERVER_VER_RANDOMIZE_SIZE_AND_PRICE = 76;
const int MIN_SERVER_VER_FRACTIONAL_POSITIONS = 101;
const int MIN_SERVER_VER_PEGGED_TO_BENCHMARK = 102;
const int MIN_SERVER_VER_MODELS_SUPPORT         = 103;
const int MIN_SERVER_VER_SEC_DEF_OPT_PARAMS_REQ = 104;
const int MIN_SERVER_VER_EXT_OPERATOR	        = 105;
const int MIN_SERVER_VER_SOFT_DOLLAR_TIER		= 106;

/* 100+ messaging */
// 100 = enhanced handshake, msg length prefixes

const int MIN_CLIENT_VER = 100;
const int MAX_CLIENT_VER = MIN_SERVER_VER_SOFT_DOLLAR_TIER;


// incoming msg id's
const int TICK_PRICE                = 1;
const int TICK_SIZE                 = 2;
const int ORDER_STATUS              = 3;
const int ERR_MSG                   = 4;
const int OPEN_ORDER                = 5;
const int ACCT_VALUE                = 6;
const int PORTFOLIO_VALUE           = 7;
const int ACCT_UPDATE_TIME          = 8;
const int NEXT_VALID_ID             = 9;
const int CONTRACT_DATA             = 10;
const int EXECUTION_DATA            = 11;
const int MARKET_DEPTH              = 12;
const int MARKET_DEPTH_L2           = 13;
const int NEWS_BULLETINS            = 14;
const int MANAGED_ACCTS             = 15;
const int RECEIVE_FA                = 16;
const int HISTORICAL_DATA           = 17;
const int BOND_CONTRACT_DATA        = 18;
const int SCANNER_PARAMETERS        = 19;
const int SCANNER_DATA              = 20;
const int TICK_OPTION_COMPUTATION   = 21;
const int TICK_GENERIC              = 45;
const int TICK_STRING               = 46;
const int TICK_EFP                  = 47;
const int CURRENT_TIME              = 49;
const int REAL_TIME_BARS            = 50;
const int FUNDAMENTAL_DATA          = 51;
const int CONTRACT_DATA_END         = 52;
const int OPEN_ORDER_END            = 53;
const int ACCT_DOWNLOAD_END         = 54;
const int EXECUTION_DATA_END        = 55;
const int DELTA_NEUTRAL_VALIDATION  = 56;
const int TICK_SNAPSHOT_END         = 57;
const int MARKET_DATA_TYPE          = 58;
const int COMMISSION_REPORT         = 59;
const int POSITION_DATA             = 61;
const int POSITION_END              = 62;
const int ACCOUNT_SUMMARY           = 63;
const int ACCOUNT_SUMMARY_END       = 64;
const int VERIFY_MESSAGE_API        = 65;
const int VERIFY_COMPLETED          = 66;
const int DISPLAY_GROUP_LIST        = 67;
const int DISPLAY_GROUP_UPDATED     = 68;
const int VERIFY_AND_AUTH_MESSAGE_API = 69;
const int VERIFY_AND_AUTH_COMPLETED   = 70;
const int POSITION_MULTI            = 71;
const int POSITION_MULTI_END        = 72;
const int ACCOUNT_UPDATE_MULTI      = 73;
const int ACCOUNT_UPDATE_MULTI_END  = 74;
const int SECURITY_DEFINITION_OPTION_PARAMETER = 75;
const int SECURITY_DEFINITION_OPTION_PARAMETER_END = 76;
const int SOFT_DOLLAR_TIERS = 77;

const int HEADER_LEN = 4; // 4 bytes for msg length
const int MAX_MSG_LEN = 0xFFFFFF; // 16Mb - 1byte
const char API_SIGN[4] = { 'A', 'P', 'I', '\0' }; // "API"
} // namespace client_constants
} // namespace ibapi

#endif // ECONSTANTS_H
