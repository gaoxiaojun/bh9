#include "EClientSocket.h"
#include "EWrapper.h"
#include <boost/asio.hpp>
#include <boost/current_function.hpp>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <memory>
static bool is_connected = false;

#define FDEBUG std::cout << BOOST_CURRENT_FUNCTION << std::endl;
void do_test(std::shared_ptr<EClientSocket> tws);
boost::asio::io_service ios;
std::shared_ptr<EClientSocket> tws = std::make_shared<EClientSocket>(ios, nullptr);

class Callback : public EWrapper
{
public:
   virtual ~Callback() = default;

    // connection
    void connectionClosed()  override
    {
        FDEBUG;
    }
    void connectAck()  override
    {
        FDEBUG;
        is_connected = true;
        do_test(tws);
    }
    void error(const int id, const int errorCode, const std::string errorString) override
    {
        std::cout << "id:" << id << " errorCode:" << errorCode << " msg:" << errorString << std::endl;
    }
    // market data
   void tickPrice( TickerId id, TickType field, double price, int canAutoExecute) override
   {
       std::cout << "id:" << id << " type:" << field << "price: " << price << "canAutoExecute" << canAutoExecute << std::endl;
   }
   void tickSize( TickerId id, TickType field, int size) override
   {
       std::cout << "id: " << id << " type:" << field << " size:" << size << std::endl;
   }

   void tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
       double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice)  override
   {
       FDEBUG;
   }
   void tickGeneric(TickerId tickerId, TickType tickType, double value)  override
   {
       FDEBUG;
   }
   void tickString(TickerId tickerId, TickType tickType, const std::string& value)  override
   {
       FDEBUG;
   }
   void tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const std::string& formattedBasisPoints,
       double totalDividends, int holdDays, const std::string& futureLastTradeDate, double dividendImpact, double dividendsToLastTradeDate)  override
   {
       FDEBUG;
   }
    void updateMktDepth(TickerId id, int position, int operation, int side,
       double price, int size)  override
    {
        FDEBUG;
    }
    void updateMktDepthL2(TickerId id, int position, std::string marketMaker, int operation,
       int side, double price, int size)  override
    {
        FDEBUG;
    }
    void updateNewsBulletin(int msgId, int msgType, const std::string& newsMessage, const std::string& originExch)  override
    {
        FDEBUG;
    }

    // order
    void orderStatus( OrderId orderId, const std::string& status, double filled,
       double remaining, double avgFillPrice, int permId, int parentId,
       double lastFillPrice, int clientId, const std::string& whyHeld)  override
    {
        FDEBUG;
    }
   void openOrder( OrderId orderId, const Contract&, const Order&, const OrderState&)  override
   {
       FDEBUG;
   }
   void openOrderEnd() override
   {
       FDEBUG;
   }

    // account
   void updateAccountValue(const std::string& key, const std::string& val,
   const std::string& currency, const std::string& accountName)  override
   {
       FDEBUG;
   }
   void updatePortfolio( const Contract& contract, double position,
      double marketPrice, double marketValue, double averageCost,
      double unrealizedPNL, double realizedPNL, const std::string& accountName)  override
   {
       FDEBUG;
   }
   void updateAccountTime(const std::string& timeStamp)  override
   {
       FDEBUG;
   }
   void accountDownloadEnd(const std::string& accountName)  override
   {
       FDEBUG;
   }
    void managedAccounts( const std::string& accountsList)  override
    {
        FDEBUG;
    }
   void receiveFA(faDataType pFaDataType, const std::string& cxml)  override
   {
       FDEBUG;
   }
    // id
   void nextValidId( OrderId orderId)  override
   {
       FDEBUG;
   }
    // contract
   void contractDetails( int reqId, const ContractDetails& contractDetails)  override
   {
       FDEBUG;
   }
   void bondContractDetails( int reqId, const ContractDetails& contractDetails)  override
   {
       FDEBUG;
   }
   void contractDetailsEnd( int reqId)  override
   {
       FDEBUG;
   }
    // execution
   void execDetails( int reqId, const Contract& contract, const Execution& execution) override
   {
       FDEBUG;
   }
   void execDetailsEnd( int reqId) override
   {
       FDEBUG;
   }

    // historical
   void historicalData(TickerId reqId, const std::string& date, double open, double high,
       double low, double close, int volume, int barCount, double WAP, int hasGaps)  override
   {
       FDEBUG;
   }


   void scannerParameters(const std::string& xml)  override
   {
       FDEBUG;
   }
   void scannerData(int reqId, int rank, const ContractDetails& contractDetails,
       const std::string& distance, const std::string& benchmark, const std::string& projection,
       const std::string& legsStr)  override
   {
       FDEBUG;
   }
   void scannerDataEnd(int reqId)  override
   {
       FDEBUG;
   }
   void realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
       long volume, double wap, int count)  override
   {
       FDEBUG;
   }
   void currentTime(long time)  override
   {
       FDEBUG;
   }
   void fundamentalData(TickerId reqId, const std::string& data)  override
   {
       FDEBUG;
   }
   void deltaNeutralValidation(int reqId, const UnderComp& underComp)  override
   {
       FDEBUG;
   }
   void tickSnapshotEnd( int reqId)  override
   {
       FDEBUG;
   }
   void marketDataType( TickerId reqId, int marketDataType)  override
   {
       FDEBUG;
   }
   void commissionReport( const CommissionReport& commissionReport)  override
   {
       FDEBUG;
   }
   void position( const std::string& account, const Contract& contract, double position, double avgCost)  override
   {
       FDEBUG;
   }
   void positionEnd()  override
   {
       FDEBUG;
   }
   void accountSummary( int reqId, const std::string& account, const std::string& tag, const std::string& value, const std::string& curency)  override
   {
       FDEBUG;
   }
   void accountSummaryEnd( int reqId)  override
   {
       FDEBUG;
   }
   void verifyMessageAPI( const std::string& apiData)  override
   {
       FDEBUG;
   }
   void verifyCompleted( bool isSuccessful, const std::string& errorText)  override
   {
       FDEBUG;
   }
   void displayGroupList( int reqId, const std::string& groups)  override
   {
       FDEBUG;
   }
   void displayGroupUpdated( int reqId, const std::string& contractInfo)  override
   {
       FDEBUG;
   }
   void verifyAndAuthMessageAPI( const std::string& apiData, const std::string& xyzChallange)  override
   {
       FDEBUG;
   }
   void verifyAndAuthCompleted( bool isSuccessful, const std::string& errorText)  override
   {
       FDEBUG;
   }

   void positionMulti( int reqId, const std::string& account,const std::string& modelCode, const Contract& contract, double pos, double avgCost)  override
   {
       FDEBUG;
   }
   void positionMultiEnd( int reqId)  override
   {
       FDEBUG;
   }
   void accountUpdateMulti( int reqId, const std::string& account, const std::string& modelCode, const std::string& key, const std::string& value, const std::string& currency)  override
   {
       FDEBUG;
   }
   void accountUpdateMultiEnd( int reqId)  override
   {
       FDEBUG;
   }
   void securityDefinitionOptionalParameter(int reqId, const std::string& exchange, int underlyingConId, const std::string& tradingClass, const std::string& multiplier, std::set<std::string> expirations, std::set<double> strikes)  override
   {
       FDEBUG;
   }
   void securityDefinitionOptionalParameterEnd(int reqId)  override
   {
       FDEBUG;
   }
   void softDollarTiers(int reqId, const std::vector<SoftDollarTier> &tiers)  override
   {
       FDEBUG;
   }
};

void do_test(std::shared_ptr<EClientSocket> tws) 
{
    std::cout << "in DO_TEST" << std::endl;
    //timer->cancel();
    Contract c;
    c.symbol = "EUR";
    c.exchange = "IDEALPRO";
    c.secType = "CASH";
    c.currency = "USD";
    tws->reqCurrentTime();
    tws->reqMktData(1, c, "", false, nullptr);
     std::cout << "out DO_TEST" << std::endl;
}

/*void handle_timer(std::shared_ptr<EClientSocket> tws, std::shared_ptr<boost::asio::deadline_timer> timer, const boost::system::error_code &ec)
{
    FDEBUG;
    if(!ec && is_connected) {
        do_test(tws, timer);
    }else {
        timer->async_wait(boost::bind(handle_timer,tws, timer, boost::asio::placeholders::error));
    }
}*/

int main(int argc, char**argv)
{

    Callback wrapper;
    tws->setEWrapper(&wrapper);
    //tws->disableUseV100Plus();
    tws->setReconnectSeconds(20);
    tws->eConnect("127.0.0.1", 4001);
    
    
    //std::shared_ptr<boost::asio::deadline_timer> timer = std::make_shared<boost::asio::deadline_timer>(ios, boost::posix_time::seconds(5));
    //timer->async_wait(boost::bind(handle_timer, tws, timer, boost::asio::placeholders::error));
    ios.run();
}
