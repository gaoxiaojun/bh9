/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#pragma once
#include "EWrapper.h"
#include "econfig.h"

class TWS_API DefaultEWrapper :
    public EWrapper
{
public:
    void tickPrice( TickerId , TickType , double , int ) { }
    void tickSize( TickerId , TickType , int ) { }
    void tickOptionComputation( TickerId , TickType , double , double ,
           double , double , double , double , double , double ) { }
    void tickGeneric(TickerId , TickType , double ) { }
    void tickString(TickerId , TickType , const std::string& ) { }
    void tickEFP(TickerId , TickType , double , const std::string& ,
           double , int , const std::string& , double , double ) { }
    void orderStatus( OrderId , const std::string& , double ,
           double , double , int , int ,
           double , int , const std::string& ) { }
    void openOrder( OrderId , const Contract&, const Order&, const OrderState&) { }
    void openOrderEnd() { }
    void connectionClosed() { }
    void updateAccountValue(const std::string& , const std::string& ,
       const std::string& , const std::string& ) { }
    void updatePortfolio( const Contract& , double ,
          double , double , double ,
          double , double , const std::string& ) { }
    void updateAccountTime(const std::string& ) { }
    void accountDownloadEnd(const std::string& ) { }
    void nextValidId( OrderId ) { }
    void contractDetails( int , const ContractDetails& ) { }
    void bondContractDetails( int , const ContractDetails& ) { }
    void contractDetailsEnd( int ) { }
    void execDetails( int , const Contract& , const Execution& ) { }
    void execDetailsEnd( int ) { }
    void error(const int , const int , const std::string ) { }
    void updateMktDepth(TickerId , int , int , int ,
          double , int ) { }
    void updateMktDepthL2(TickerId , int , std::string , int ,
          int , double , int ) { }
    void updateNewsBulletin(int , int , const std::string& , const std::string& ) { }
    void managedAccounts( const std::string& ) { }
    void receiveFA(faDataType , const std::string& ) { }
    void historicalData(TickerId , const std::string& , double , double ,
           double , double , int , int , double , int ) { }
    void scannerParameters(const std::string& ) { }
    void scannerData(int , int , const ContractDetails& ,
           const std::string& , const std::string& , const std::string& ,
           const std::string& ) { }
    void scannerDataEnd(int ) { }
    void realtimeBar(TickerId , long , double , double , double , double ,
           long , double , int ) { }
    void currentTime(long ) { }
    void fundamentalData(TickerId , const std::string& ) { }
    void deltaNeutralValidation(int , const UnderComp& ) { }
    void tickSnapshotEnd( int ) { }
    void marketDataType( TickerId , int ) { }
    void commissionReport( const CommissionReport& ) { }
    void position( const std::string& , const Contract& , double , double ) { }
    void positionEnd() { }
    void accountSummary( int , const std::string& , const std::string& , const std::string& , const std::string&) { }
    void accountSummaryEnd( int ) { }
    void verifyMessageAPI( const std::string& ) { }
    void verifyCompleted( bool , const std::string& ) { }
    void displayGroupList( int , const std::string& ) { }
    void displayGroupUpdated( int , const std::string& ) { }
    void verifyAndAuthMessageAPI( const std::string& , const std::string& ) { }
    void verifyAndAuthCompleted( bool , const std::string& ) { }
    void connectAck() { }
    void positionMulti( int , const std::string& ,const std::string& , const Contract& , double , double ) { }
    void positionMultiEnd( int ) { }
    void accountUpdateMulti( int , const std::string& , const std::string& , const std::string& , const std::string& , const std::string& ) { }
    void accountUpdateMultiEnd( int ) { }
    void securityDefinitionOptionalParameter(int , const std::string& , int , const std::string& , const std::string& , std::set<std::string> , std::set<double> ) { }
    void securityDefinitionOptionalParameterEnd(int ) { }
    void softDollarTiers(int , const std::vector<SoftDollarTier> &) { }
};

