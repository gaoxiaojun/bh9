/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is
 * subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial
 * License, as applicable. */

#pragma once
#ifndef ECLIENT_H
#define ECLIENT_H

#include "econfig.h"
#include "CommonDefs.h"
#include "TagValue.h"
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>
#include "Contract.h"
#include <functional>
#include <ostream>

struct Contract;
struct Order;
struct ExecutionFilter;
struct ScannerSubscription;
struct ETransport;

class EWrapper;
typedef std::vector<char> BytesVec;

/**
 * @class EClient
 * @brief TWS/Gateway client class
 * This client class contains all the available methods to communicate with IB. Up to thirty-two clients can be connected to a single instance of the TWS/Gateway simultaneously. From herein, the TWS/Gateway will be referred to as the Host.
 */
class TWS_API EClient
{
public:
  enum ConnState
  {
    CS_DISCONNECTED,
    CS_CONNECTING,
    CS_CONNECTED
  };

public:
  explicit EClient(EWrapper* ptr);
  virtual ~EClient();

protected:
    virtual void on_send(const std::string& msg) = 0;
    virtual void on_handshake_ack() = 0;
    virtual void on_redirect(const std::string& host, int port, int clientId, bool extraAuth) = 0;
    virtual void eDisconnect() = 0;

private:
  // callback when handshake ack
  void handShakeAck(int version, const std::string &time);
  virtual void redirect(const std::string& host, unsigned port);

public:
  int clientId() const { return m_clientId; }
  void setClientId(int clientId);

  bool extraAuth() const { return m_extraAuth; }
  void setExtraAuth(bool extraAuth);

  const std::string& host() const { return m_host; }
  void setHost(const std::string& host);

  unsigned port() const { return m_port; }
  void setPort(unsigned port);

  int serverVersion() const { return m_serverVersion; }

  const std::string& ServerTime() const;

  const std::string& optionalCapabilities() const { return m_optionalCapabilities; }
  void setOptionalCapabilities(const std::string& optCapts);

  const std::string& connectOptions() const { return m_connectOptions; }
  void setConnectOptions(const std::string& connectOptions);

  /**
   * @brief Allows to switch between different current (V100+) and previous connection mechanisms.
   */
  void disableUseV100Plus();
  bool usingV100Plus() const { return m_useV100Plus; }

  bool allowRedirect() const { return m_allowRedirect; }
  void setAllowRedirect(bool allow);

  // access to protected variables
  EWrapper* getWrapper() const { return m_pEWrapper; }
  void setEWrapper(EWrapper* wrapper) { m_pEWrapper = wrapper; }

  // connection state
  ConnState connState() const;
  void setConnState(ConnState state) { m_connState = state; }

  bool isDisconncted() const { return m_connState == CS_DISCONNECTED; }
  bool isConnected() const { return m_connState == CS_CONNECTED; }
  bool isConnecting() const { return m_connState == CS_CONNECTING; }

protected:
  void eConnectBase();
  void eDisconnectBase();

public:
  // send handshake message
  void sendConnectRequest();

  // process message
  int processMsg(const char*& ptr, const char* endPtr);

  // command api

  /**
   * @brief Requests real time market data.
   * This function will return the product's market data. It is important to notice that only real time data can be delivered via the API.
   * @param tickerId the request's identifier
   * @param contract the Contract for which the data is being requested
   * @param genericTickList comma separated ids of the available generic ticks:
   *      - 100 	Option Volume (currently for stocks)
   *      - 101 	Option Open Interest (currently for stocks)
   *      - 104 	Historical Volatility (currently for stocks)
   *      - 106 	Option Implied Volatility (currently for stocks)
   *      - 162 	Index Future Premium
   *      - 165 	Miscellaneous Stats
   *      - 221 	Mark Price (used in TWS P&L computations)
   *      - 225 	Auction values (volume, price and imbalance)
   *      - 233 	RTVolume - contains the last trade price, last trade size, last trade time, total volume, VWAP, and single trade flag.
   *      - 236 	Shortable
   *      - 256 	Inventory
   *      - 258 	Fundamental Ratios
   *      - 411 	Realtime Historical Volatility
   *      - 456 	IBDividends
   * @param snapshot when set to true, it will provide a single snapshot of the available data. Set to false if you want to receive continuous updates.
   * @sa cancelMktData, EWrapper::tickPrice, EWrapper::tickSize, EWrapper::tickString, EWrapper::tickEFP, EWrapper::tickGeneric, EWrapper::tickOptionComputation, EWrapper::tickSnapshotEnd
   */
  void reqMktData(TickerId id, const Contract& contract,
                  const std::string& genericTicks, bool snapshot,
                  const TagValueListSPtr& mktDataOptions);

  /**
   * @brief Cancels a RT Market Data request
   * @param tickerId request's identifier
   * @sa reqMktData
   */
  void cancelMktData(TickerId id);

  /**
   * @brief Places an order
   * @param id the order's unique identifier. Use a sequential id starting with the id received at the nextValidId method. If a new order is placed with an order ID less than or equal to the order ID of a previous order an error will occur.
   * @param contract the order's contract
   * @param order the order
   * @sa EWrapper::nextValidId, reqAllOpenOrders, reqAutoOpenOrders, reqOpenOrders, cancelOrder, reqGlobalCancel, EWrapper::openOrder, EWrapper::orderStatus, Order, Contract
   */
  void placeOrder(OrderId id, const Contract& contract, const Order& order);

  /**
   * @brief Cancels an active order
   * @param orderId the order's client id
   * @sa placeOrder, reqGlobalCancel
   */
  void cancelOrder(OrderId id);

  /**
   * @brief Requests all open orders places by this specific API client (identified by the API client id)
   * @sa reqAllOpenOrders, reqAutoOpenOrders, placeOrder, cancelOrder, reqGlobalCancel, EWrapper::openOrder, EWrapper::orderStatus, EWrapper::openOrderEnd
   */
  void reqOpenOrders();
  /**
   * @brief Subscribes to an specific account's information and portfolio
   * Through this method, a single account's subscription can be started/stopped. As a result from the subscription, the account's information, portfolio and last update time will be received at EWrapper::updateAccountValue, EWrapper::updateAccountPortfolio, EWrapper::updateAccountTime respectively.
   * Only one account can be subscribed at a time. A second subscription request for another account when the previous one is still active will cause the first one to be canceled in favour of the second one. Consider user reqPositions if you want to retrieve all your accounts' portfolios directly.
   * @param subscribe set to true to start the subscription and to false to stop it.
   * @param acctCode the account id (i.e. U123456) for which the information is requested.
   * @sa reqPositions, EWrapper::updateAccountValue, EWrapper::updatePortfolio, EWrapper::updateAccountTime
   */
  void reqAccountUpdates(bool subscribe, const std::string& acctCode);

  /**
   * @brief Requests all the day's executions matching the filter.
   * Only the current day's executions can be retrieved. Along with the executions, the CommissionReport will also be returned. The execution details will arrive at EWrapper:execDetails
   * @param reqId the request's unique identifier.
   * @param filter the filter criteria used to determine which execution reports are returned.
   * @sa EWrapper::execDetails, EWrapper::commissionReport, ExecutionFilter
   */
  void reqExecutions(int reqId, const ExecutionFilter& filter);

  /**
   * @brief Requests the next valid order id.
   * @param numIds deprecated- this parameter will not affect the value returned to nextValidId
   * @sa EWrapper::nextValidId
   */
  void reqIds(int numIds);
  /**
   * @brief Requests contract information.
   * This method will provide all the contracts matching the contract provided. It can also be used to retrieve complete options and futures chains. This information will be returned at EWrapper:contractDetails. Though it is now (in API version > 9.72.12) advised to use reqSecDefOptParams for that purpose.
   * @param reqId the unique request identifier.
   * @param contract the contract used as sample to query the available contracts. Typically, it will contain the Contract::Symbol, Contract::Currency, Contract::SecType, Contract::Exchange
   * @sa EWrapper::contractDetails, EWrapper::contractDetailsEnd
   */
  void reqContractDetails(int reqId, const Contract& contract);
  /**
   * @brief Requests the contract's market depth (order book). Note this request must be direct-routed to an exchange and not smart-routed. The number of simultaneous market depth requests allowed in an account is calculated based on a formula that looks at an accounts equity, commissions, and quote booster packs.
   * @param tickerId the request's identifier
   * @param contract the Contract for which the depth is being requested
   * @param numRows the number of rows on each side of the order book
   * @sa cancelMktDepth, EWrapper::updateMktDepth, EWrapper::updateMktDepthL2
   */
  void reqMktDepth(TickerId tickerId, const Contract& contract, int numRows,
                   const TagValueListSPtr& mktDepthOptions);
  /**
   * @brief Cancel's market depth's request.
   * @param tickerId request's identifier.
   * @sa reqMarketDepth
   */
  void cancelMktDepth(TickerId tickerId);
  /**
   * @brief Subscribes to IB's News Bulletins
   * @param allMessages if set to true, will return all the existing bulletins for the current day, set to false to receive only the new bulletins.
   * @sa cancelNewsBulletin, EWrapper::updateNewsBulletin
   */
  void reqNewsBulletins(bool allMsgs);
  /**
   * @brief Cancels IB's news bulletin subscription
   * @sa reqNewsBulletins
   */
  void cancelNewsBulletins();
  /**
   * @brief Changes the TWS/GW log level.
   * Valid values are:\n
   * 1 = SYSTEM\n
   * 2 = ERROR\n
   * 3 = WARNING\n
   * 4 = INFORMATION\n
   * 5 = DETAIL\n
   */
  void setServerLogLevel(int level);
  /**
   * @brief Requests all order placed on the TWS directly.
   * Only the orders created after this request has been made will be returned.
   * @param autoBind if set to true, the newly created orders will be implicitely associated with this client.
   * @sa reqAllOpenOrders, reqOpenOrders, cancelOrder, reqGlobalCancel, EWrapper::openOrder, EWrapper::orderStatus
   */
  void reqAutoOpenOrders(bool bAutoBind);
  /**
   * @brief Requests all open orders submitted by any API client as well as those directly placed in the TWS. The existing orders will be received via the openOrder and orderStatus events.
   * @sa reqAutoOpenOrders, reqOpenOrders, EWrapper::openOrder, EWrapper::orderStatus, EWrapper::openOrderEnd
   */
  void reqAllOpenOrders();
  /**
   * @brief Requests the accounts to which the logged user has access to.
   * @sa EWrapper::managedAccounts
   */
  void reqManagedAccts();
  /**
   * @brief Requests the FA configuration
   * A Financial Advisor can define three different configurations:
   *     1. Groups: offer traders a way to create a group of accounts and apply a single allocation method to all accounts in the group.
   *     2. Profiles: let you allocate shares on an account-by-account basis using a predefined calculation value.
   *     3. Account Aliases: let you easily identify the accounts by meaningful names rather than account numbers.
   * More information at https://www.interactivebrokers.com/en/?f=%2Fen%2Fsoftware%2Fpdfhighlights%2FPDF-AdvisorAllocations.php%3Fib_entity%3Dllc
   * @param faDataType the configuration to change. Set to 1, 2 or 3 as defined above.
   * @sa replaceFA
   */
  void requestFA(faDataType pFaDataType);
  /**
   * @brief Replaces Financial Advisor's settings
   * A Financial Advisor can define three different configurations:
   *   1. Groups: offer traders a way to create a group of accounts and apply a single allocation method to all accounts in the group.
   *   2. Profiles: let you allocate shares on an account-by-account basis using a predefined calculation value.
   *   3. Account Aliases: let you easily identify the accounts by meaningful names rather than account numbers.
   * More information at https://www.interactivebrokers.com/en/?f=%2Fen%2Fsoftware%2Fpdfhighlights%2FPDF-AdvisorAllocations.php%3Fib_entity%3Dllc
   * @param faDataType the configuration to change. Set to 1, 2 or 3 as defined above.
   * @param xml the xml-formatted configuration string
   * @sa requestFA
   */
  void replaceFA(faDataType pFaDataType, const std::string& cxml);
  /**
   * @brief Requests contracts' historical data.
   * When requesting historical data, a finishing time and date is required along with a duration string. For example, having:
   *     - endDateTime: 20130701 23:59:59 GMT
   *     - durationStr: 3 D
   * will return three days of data counting backwards from July 1st 2013 at 23:59:59 GMT resulting in all the available bars of the last three days until the date and time specified. It is possible to specify a timezone optionally. The resulting bars will be returned in EWrapper::historicalData
   * @param tickerId the request's unique identifier.
   * @param contract the contract for which we want to retrieve the data.
   * @param endDateTime request's ending time with format yyyyMMdd HH:mm:ss {TMZ}
   * @param durationString the amount of time for which the data needs to be retrieved:
   *     - " S (seconds)
   *     - " D (days)
   *     - " W (weeks)
   *     - " M (months)
   *     - " Y (years)
   * @param barSizeSetting the size of the bar:
   *     - 1 sec
   *     - 5 secs
   *     - 15 secs
   *     - 30 secs
   *     - 1 min
   *     - 2 mins
   *     - 3 mins
   *     - 5 mins
   *     - 15 mins
   *     - 30 mins
   *     - 1 hour
   *     - 1 day
   * @param whatToShow the kind of information being retrieved:
   *     - TRADES
   *     - MIDPOINT
   *     - BID
   *     - ASK
   *     - BID_ASK
   *     - HISTORICAL_VOLATILITY
   *     - OPTION_IMPLIED_VOLATILITY
           *	    - FREE_RATE
           *	    - REBATE_RATE
   * @param useRTH set to 0 to obtain the data which was also generated outside of the Regular Trading Hours, set to 1 to obtain only the RTH data
   * @param formatDate set to 1 to obtain the bars' time as yyyyMMdd HH:mm:ss, set to 2 to obtain it like system time format in seconds
   * @sa EWrapper::historicalData
   */
  void reqHistoricalData(TickerId id, const Contract& contract,
                         const std::string& endDateTime,
                         const std::string& durationStr,
                         const std::string& barSizeSetting,
                         const std::string& whatToShow, int useRTH,
                         int formatDate, const TagValueListSPtr& chartOptions);
  /**
   * @brief Exercises your options
   * @param tickerId exercise request's identifier
   * @param contract the option Contract to be exercised.
   * @param exerciseAction set to 1 to exercise the option, set to 2 to let the option lapse.
   * @param exerciseQuantity number of contracts to be exercised
   * @param account destination account
   * @param ovrd Specifies whether your setting will override the system's natural action. For example, if your action is "exercise" and the option is not in-the-money, by natural action the option would not exercise. If you have override set to "yes" the natural action would be overridden and the out-of-the money option would be exercised. Set to 1 to override, set to 0 not to.
   */
  void exerciseOptions(TickerId tickerId, const Contract& contract,
                       int exerciseAction, int exerciseQuantity,
                       const std::string& account, int override);

  /**
         * @brief Cancels a historical data request.
         * @param reqId the request's identifier.
         * @sa reqHistoricalData
         */
  void cancelHistoricalData(TickerId tickerId);
  /**
   * @brief Requests real time bars
   * Currently, only 5 seconds bars are provided. This request is subject to the same pacing as any historical data request: no more than 60 API queries in more than 600 seconds. Real time bars subscriptions are also included in the calculation of the number of Level 1 market data subscriptions allowed in an account.
   * @param tickerId the request's unique identifier.
   * @param contract the Contract for which the depth is being requested
   * @param barSize currently being ignored
   * @param whatToShow the nature of the data being retrieved:
   *     - TRADES
   *     - MIDPOINT
   *     - BID
   *     - ASK
   * @param useRTH set to 0 to obtain the data which was also generated ourside of the Regular Trading Hours, set to 1 to obtain only the RTH data
   * @sa cancelRealTimeBars, EWrapper::realtimeBar
   */
  void reqRealTimeBars(TickerId id, const Contract& contract, int barSize,
                       const std::string& whatToShow, bool useRTH,
                       const TagValueListSPtr& realTimeBarsOptions);
  /**
   * @brief Cancels Real Time Bars' subscription
   * @param tickerId the request's identifier.
   * @sa reqRealTimeBars
   */
  void cancelRealTimeBars(TickerId tickerId);
  /**
   * @brief Cancels Scanner Subscription
   * @param tickerId the subscription's unique identifier.
   * @sa reqScannerSubscription, ScannerSubscription, reqScannerParameters
   */
  void cancelScannerSubscription(int tickerId);
  /**
   * @brief Requests all possible parameters which can be used for a scanner subscription
   * @sa reqScannerSubscription
   */
  void reqScannerParameters();
  /**
   * @brief Starts a subscription to market scan results based on the provided parameters.
   * @param reqId the request's identifier
   * @param subscription summary of the scanner subscription including its filters.
   * @sa reqScannerParameters, ScannerSubscription, EWrapper::scannerData
   */
  void reqScannerSubscription(
    int tickerId, const ScannerSubscription& subscription,
    const TagValueListSPtr& scannerSubscriptionOptions);
  /**
   * @brief Requests the server's current time.
   * @sa EWrapper::currentTime
   */
  void reqCurrentTime();
  /**
   * @brief Requests the contract's Reuters' global fundamental data.
   * Reuters funalmental data will be returned at EWrapper::fundamentalData
   * @param reqId the request's unique identifier.
   * @param contract the contract's description for which the data will be returned.
   * @param reportType there are three available report types:
   *     - ReportSnapshot: Company overview
   *     - ReportsFinSummary: Financial summary
                  - ReportRatios:	Financial ratios
                  - ReportsFinStatements:	Financial statements
                  - RESC: Analyst estimates
                  - CalendarReport: Company calendar
   * @sa EWrapper::fundamentalData
   */
  void reqFundamentalData(TickerId reqId, const Contract&,
                          const std::string& reportType);
  /**
   * @brief Cancels Fundamental data request
   * @param reqId the request's idenfier.
   * @sa reqFundamentalData
   */
  void cancelFundamentalData(TickerId reqId);
  /**
   * @brief Calculate the volatility for an option.
   * Request the calculation of the implied volatility based on hypothetical option and its underlying prices. The calculation will be return in EWrapper's tickOptionComputation callback.
   * @param reqId unique identifier of the request.
   * @param contract the option's contract for which the volatility wants to be calculated.
   * @param optionPrice hypothetical option price.
   * @param underPrice hypothetical option's underlying price.
   * @sa EWrapper::tickOptionComputation, cancelCalculateImpliedVolatility, Contract
   */
  void calculateImpliedVolatility(TickerId reqId, const Contract& contract,
                                  double optionPrice, double underPrice);
  /**
   * @brief Calculates an option's price.
   * Calculates an option's price based on the provided volatility and its underlying's price. The calculation will be return in EWrapper's tickOptionComputation callback.
   * @param reqId request's unique identifier.
   * @param contract the option's contract for which the price wants to be calculated.
   * @param volatility hypothetical volatility.
   * @param underPrice hypothetical underlying's price.
   * @sa EWrapper::tickOptionComputation, cancelCalculateOptionPrice, Contract
   */
  void calculateOptionPrice(TickerId reqId, const Contract& contract,
                            double volatility, double underPrice);
  /**
   * @brief Cancels an option's implied volatility calculation request
   * @param reqId the identifier of the implied volatility's calculation request.
   * @sa calculateImpliedVolatility
   */
  void cancelCalculateImpliedVolatility(TickerId reqId);
  /**
   * @brief Cancels an option's price calculation request
   * @param reqId the identifier of the option's price's calculation request.
   * @sa calculateOptionPrice
   */
  void cancelCalculateOptionPrice(TickerId reqId);
  /**
   * @brief Cancels all the active orders.
   * This method will cancel ALL open orders included those placed directly via the TWS.
   * @sa cancelOrder
   */
  void reqGlobalCancel();
  /**
   * @brief indicates the TWS to switch to "frozen", "delayed" or "delayed-frozen" market data.
   * The API can receive frozen market data from Trader Workstation. Frozen market data is the last data recorded in our system. During normal trading hours, the API receives real-time market data. If you use this function, you are telling TWS to automatically switch to frozen market data after the close. Then, before the opening of the next trading day, market data will automatically switch back to real-time market data.
   * @param marketDataType set to 1 for real time streaming, set to 2 for frozen market data, set to 3 for delayed market data, set to 4 for delayed-frozen market data. Note: At the present time, only data types 1 and 2 are supported.
   */
  void reqMarketDataType(int marketDataType);
  /**
   * @brief Requests all positions from all accounts
   * @sa cancelPositions, EWrapper::position, EWrapper::positionEnd
   */
  void reqPositions();
  /**
   * @brief Cancels all account's positions request
   * @sa reqPositions
   */
  void cancelPositions();
  /**
   * @brief Requests a specific account's summary.
   * This method will subscribe to the account summary as presented in the TWS' Account Summary tab. The data is returned at EWrapper::accountSummary
   * https://www.interactivebrokers.com/en/software/tws/accountwindowtop.htm
   * @param reqId the unique request identifier.
   * @param group set to "All" to return account summary data for all accounts, or set to a specific Advisor Account Group name that has already been created in TWS Global Configuration.
   * @param tags a comma separated list with the desired tags:
   *     - AccountType — Identifies the IB account structure
   *     - NetLiquidation — The basis for determining the price of the assets in your account. Total cash value + stock value + options value + bond value
   *     - TotalCashValue — Total cash balance recognized at the time of trade + futures PNL
   *     - SettledCash — Cash recognized at the time of settlement - purchases at the time of trade - commissions - taxes - fees
   *     - AccruedCash — Total accrued cash value of stock, commodities and securities
   *     - BuyingPower — Buying power serves as a measurement of the dollar value of securities that one may purchase in a securities account without depositing additional funds
   *     - EquityWithLoanValue — Forms the basis for determining whether a client has the necessary assets to either initiate or maintain security positions. Cash + stocks + bonds + mutual funds
   *     - PreviousEquityWithLoanValue — Marginable Equity with Loan value as of 16:00 ET the previous day
   *     - GrossPositionValue — The sum of the absolute value of all stock and equity option positions
   *     - RegTEquity — Regulation T equity for universal account
   *     - RegTMargin — Regulation T margin for universal account
   *     - SMA — Special Memorandum Account: Line of credit created when the market value of securities in a Regulation T account increase in value
   *     - InitMarginReq — Initial Margin requirement of whole portfolio
   *     - MaintMarginReq — Maintenance Margin requirement of whole portfolio
   *     - AvailableFunds — This value tells what you have available for trading
   *     - ExcessLiquidity — This value shows your margin cushion, before liquidation
   *     - Cushion — Excess liquidity as a percentage of net liquidation value
   *     - FullInitMarginReq — Initial Margin of whole portfolio with no discounts or intraday credits
   *     - FullMaintMarginReq — Maintenance Margin of whole portfolio with no discounts or intraday credits
   *     - FullAvailableFunds — Available funds of whole portfolio with no discounts or intraday credits
   *     - FullExcessLiquidity — Excess liquidity of whole portfolio with no discounts or intraday credits
   *     - LookAheadNextChange — Time when look-ahead values take effect
   *     - LookAheadInitMarginReq — Initial Margin requirement of whole portfolio as of next period's margin change
   *     - LookAheadMaintMarginReq — Maintenance Margin requirement of whole portfolio as of next period's margin change
   *     - LookAheadAvailableFunds — This value reflects your available funds at the next margin change
   *     - LookAheadExcessLiquidity — This value reflects your excess liquidity at the next margin change
   *     - HighestSeverity — A measure of how close the account is to liquidation
   *     - DayTradesRemaining — The Number of Open/Close trades a user could put on before Pattern Day Trading is detected. A value of "-1" means that the user can put on unlimited day trades.
   *     - Leverage — GrossPositionValue / NetLiquidation
   *     - $LEDGER — Single flag to relay all cash balance tags*, only in base currency.
   *     - $LEDGER:CURRENCY — Single flag to relay all cash balance tags*, only in the specified currency.
   *     - $LEDGER:ALL — Single flag to relay all cash balance tags* in all currencies.
   * @sa cancelAccountSummary, EWrapper::accountSummary, EWrapper::accountSummaryEnd
   */
  void reqAccountSummary(int reqId, const std::string& groupName,
                         const std::string& tags);
  /**
   * @brief Cancels the account's summary request.
   * After requesting an account's summary, invoke this function to cancel it.
   * @param reqId the identifier of the previously performed account request
   * @sa reqAccountSummary
   */
  void cancelAccountSummary(int reqId);
  /**
   * @brief For IB's internal purpose. Allows to provide means of verification between the TWS and third party programs.
   */
  void verifyRequest(const std::string& apiName, const std::string& apiVersion);
  /**
   * @brief For IB's internal purpose. Allows to provide means of verification between the TWS and third party programs.
   */
  void verifyMessage(const std::string& apiData);
  /**
   * @brief For IB's internal purpose. Allows to provide means of verification between the TWS and third party programs.
   */
  void verifyAndAuthRequest(const std::string& apiName,
                            const std::string& apiVersion,
                            const std::string& opaqueIsvKey);
  /**
   * @brief For IB's internal purpose. Allows to provide means of verification between the TWS and third party programs.
   */
  void verifyAndAuthMessage(const std::string& apiData,
                            const std::string& xyzResponse);
  /**
   * @brief Requests all available Display Groups in TWS
   * @params requestId is the ID of this request
   */
  void queryDisplayGroups(int reqId);
  /**
   * @brief Integrates API client and TWS window grouping.
   * @params requestId is the Id chosen for this subscription request
   * @params groupId is the display group for integration
   */
  void subscribeToGroupEvents(int reqId, int groupId);
  /**
   * @brief Updates the contract displayed in a TWS Window Group
   * @params requestId is the ID chosen for this request
   * @params contractInfo is an encoded value designating a unique IB contract. Possible values include:
   * 1. none = empty selection
   * 2. contractID@exchange - any non-combination contract. Examples 8314@SMART for IBM SMART; 8314@ARCA for IBM ARCA
   * 3. combo= if any combo is selected
   * Note: This request from the API does not get a TWS response unless an error occurs.
   */
  void updateDisplayGroup(int reqId, const std::string& contractInfo);
  /**
   * @brief Cancels a TWS Window Group subscription
   */
  void unsubscribeFromGroupEvents(int reqId);
  /**
   * @brief Requests positions for account and/or model
   * @param requestId - Request's identifier
   * @param account - If an account Id is provided, only the account's positions belonging to the specified model will be delivered
   * @params modelCode - The code of the model's positions we are interested in.
   * @sa cancelPositionsMulti, EWrapper::positionMulti, EWrapper::positionMultiEnd
   */
  void reqPositionsMulti(int reqId, const std::string& account,
                         const std::string& modelCode);
  /**
   * @brief Cancels positions request for account and/or model
   * @param requestId - the identifier of the request to be canceled.
   * @sa reqPositionsMulti
   */
  void cancelPositionsMulti(int reqId);
  /**
   * @brief Requests account updates for account and/or model
   * @sa cancelAccountUpdatesMulti, EWrapper::accountUpdateMulti, EWrapper::accountUpdateMultiEnd
   */
  void reqAccountUpdatesMulti(int reqId, const std::string& account,
                               const std::string& modelCode, bool ledgerAndNLV);
  /**
   * @brief Cancels account updates request for account and/or model
   * @sa reqAccountUpdatesMulti
   */
  void cancelAccountUpdatesMulti(int reqId);
  /**
   * @brief Requests security definition option parameters for viewing a contract's option chain
   * @params reqId the ID chosen for the request
   * @params underlyingSymbol
   * @params futFopExchange The exchange on which the returned options are trading. Can be set to the empty string "" for all exchanges.
   * @params underlyingSecType The type of the underlying security, i.e. STK
   * @params underlyingConId the contract ID of the underlying security
   * @sa EWrapper::securityDefinitionOptionParameter
   */
  void reqSecDefOptParams(int reqId, const std::string& underlyingSymbol,
                          const std::string& futFopExchange,
                          const std::string& underlyingSecType,
                          int underlyingConId);
  void reqSoftDollarTiers(int reqId);

protected:
  // message pack
  void prepareBufferImpl(std::ostream&) const;
  void prepareBuffer(std::ostream&) const;
  void closeAndSend(std::string msg, unsigned offset = 0);

protected:
  // try to process single msg
  int decodePrefixMsg(const char*& ptr, const char* endPtr);

protected:
  void startApi();

protected:
  const char* processTickPriceMsg(const char* ptr, const char* endPtr);
  const char* processTickSizeMsg(const char* ptr, const char* endPtr);
  const char* processTickOptionComputationMsg(const char* ptr, const char* endPtr);
  const char* processTickGenericMsg(const char* ptr, const char* endPtr);
  const char* processTickStringMsg(const char* ptr, const char* endPtr);
  const char* processTickEfpMsg(const char* ptr, const char* endPtr);
  const char* processOrderStatusMsg(const char* ptr, const char* endPtr);
  const char* processErrMsgMsg(const char* ptr, const char* endPtr);
  const char* processOpenOrderMsg(const char* ptr, const char* endPtr);
  const char* processAcctValueMsg(const char* ptr, const char* endPtr);
  const char* processPortfolioValueMsg(const char* ptr, const char* endPtr);
  const char* processAcctUpdateTimeMsg(const char* ptr, const char* endPtr);
  const char* processNextValidIdMsg(const char* ptr, const char* endPtr);
  const char* processContractDataMsg(const char* ptr, const char* endPtr);
  const char* processBondContractDataMsg(const char* ptr, const char* endPtr);
  const char* processExecutionDataMsg(const char* ptr, const char* endPtr);
  const char* processMarketDepthMsg(const char* ptr, const char* endPtr);
  const char* processMarketDepthL2Msg(const char* ptr, const char* endPtr);
  const char* processNewsBulletinsMsg(const char* ptr, const char* endPtr);
  const char* processManagedAcctsMsg(const char* ptr, const char* endPtr);
  const char* processReceiveFaMsg(const char* ptr, const char* endPtr);
  const char* processHistoricalDataMsg(const char* ptr, const char* endPtr);
  const char* processScannerDataMsg(const char* ptr, const char* endPtr);
  const char* processScannerParametersMsg(const char* ptr, const char* endPtr);
  const char* processCurrentTimeMsg(const char* ptr, const char* endPtr);
  const char* processRealTimeBarsMsg(const char* ptr, const char* endPtr);
  const char* processFundamentalDataMsg(const char* ptr, const char* endPtr);
  const char* processContractDataEndMsg(const char* ptr, const char* endPtr);
  const char* processOpenOrderEndMsg(const char* ptr, const char* endPtr);
  const char* processAcctDownloadEndMsg(const char* ptr, const char* endPtr);
  const char* processExecutionDataEndMsg(const char* ptr, const char* endPtr);
  const char* processDeltaNeutralValidationMsg(const char* ptr, const char* endPtr);
  const char* processTickSnapshotEndMsg(const char* ptr, const char* endPtr);
  const char* processMarketDataTypeMsg(const char* ptr, const char* endPtr);
  const char* processCommissionReportMsg(const char* ptr, const char* endPtr);
  const char* processPositionDataMsg(const char* ptr, const char* endPtr);
  const char* processPositionEndMsg(const char* ptr, const char* endPtr);
  const char* processAccountSummaryMsg(const char* ptr, const char* endPtr);
  const char* processAccountSummaryEndMsg(const char* ptr, const char* endPtr);
  const char* processVerifyMessageApiMsg(const char* ptr, const char* endPtr);
  const char* processVerifyCompletedMsg(const char* ptr, const char* endPtr);
  const char* processDisplayGroupListMsg(const char* ptr, const char* endPtr);
  const char* processDisplayGroupUpdatedMsg(const char* ptr, const char* endPtr);
  const char* processVerifyAndAuthMessageApiMsg(const char* ptr, const char* endPtr);
  const char* processVerifyAndAuthCompletedMsg(const char* ptr, const char* endPtr);
  const char* processPositionMultiMsg(const char* ptr, const char* endPtr);
  const char* processPositionMultiEndMsg(const char* ptr, const char* endPtr);
  const char* processAccountUpdateMultiMsg(const char* ptr, const char* endPtr);
  const char* processAccountUpdateMultiEndMsg(const char* ptr, const char* endPtr);
  const char* processSecurityDefinitionOptionalParameterMsg(const char* ptr, const char* endPtr);
  const char* processSecurityDefinitionOptionalParameterEndMsg(const char* ptr, const char* endPtr);
  const char* processSoftDollarTiersMsg(const char* ptr, const char* endPtr);

  /////////////////////////////Codec//////////////////////////////
protected:
  static bool CheckOffset(const char* ptr, const char* endPtr);
  static const char* FindFieldEnd(const char* ptr, const char* endPtr);

public:
  // encoders
  template <class T>
  static void EncodeField(std::ostream& os, T value)
  {
      os << value << '\0'; //std::ends;
  }

  // "max" encoders
  static void EncodeFieldMax(std::ostream& os, int);
  static void EncodeFieldMax(std::ostream& os, double);


    // decoders
    static bool DecodeField(bool&, const char*& ptr, const char* endPtr);
    static bool DecodeField(int&, const char*& ptr, const char* endPtr);
    static bool DecodeField(long&, const char*& ptr, const char* endPtr);
    static bool DecodeField(double&, const char*& ptr, const char* endPtr);
    static bool DecodeField(std::string&, const char*& ptr, const char* endPtr);

    static bool DecodeFieldMax(int&, const char*& ptr, const char* endPtr);
    static bool DecodeFieldMax(long&, const char*& ptr, const char* endPtr);
    static bool DecodeFieldMax(double&, const char*& ptr, const char* endPtr);

  int processConnectAck(const char*& beginPtr, const char* endPtr);
  int parseAndProcessMsg(const char*& beginPtr, const char* endPtr);

protected:
    void encodeMsgLen(std::string& msg, unsigned offset) const;

protected:
  EWrapper* m_pEWrapper;

  std::string m_host;
  unsigned int m_port;
  int m_clientId;
  bool m_extraAuth;

  int m_serverVersion;
  std::string m_TwsTime;

  std::string m_optionalCapabilities;
  std::string m_connectOptions;

  bool m_useV100Plus;
  bool m_allowRedirect;

  ConnState m_connState;
};

template <>
void EClient::EncodeField<bool>(std::ostream& os, bool);
template <>
void EClient::EncodeField<double>(std::ostream& os, double);

#define ENCODE_FIELD(x) EClient::EncodeField(msg, x);
#define ENCODE_FIELD_MAX(x) EClient::EncodeFieldMax(msg, x);

#define DECODE_FIELD(x) if (!EClient::DecodeField(x, ptr, endPtr)) return 0;
#define DECODE_FIELD_MAX(x) if (!EClient::DecodeFieldMax(x, ptr, endPtr)) return 0;

#endif // ECLIENT_H
