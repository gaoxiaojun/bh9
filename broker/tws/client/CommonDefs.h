/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#pragma once
#ifndef common_defs_h_INCLUDED
#define common_defs_h_INCLUDED

#include <string>

typedef long TickerId;
typedef long OrderId;

enum faDataType { GROUPS=1, PROFILES, ALIASES } ;

inline const char* faDataTypeStr ( faDataType pFaDataType )
{
	switch (pFaDataType) {
		case GROUPS:
			return "GROUPS";
		case PROFILES:
			return "PROFILES";
		case ALIASES:
			return "ALIASES";
	}
	return 0 ;
}

enum MarketDataType { 
	REALTIME = 1, 
	FROZEN = 2,
	DELAYED = 3,
	DELAYED_FROZEN = 4
};

enum TickType { BID_SIZE, BID, ASK, ASK_SIZE, LAST, LAST_SIZE,
                HIGH, LOW, VOLUME, CLOSE,
                BID_OPTION_COMPUTATION,
                ASK_OPTION_COMPUTATION,
                LAST_OPTION_COMPUTATION,
                MODEL_OPTION,
                OPEN,
                LOW_13_WEEK,
                HIGH_13_WEEK,
                LOW_26_WEEK,
                HIGH_26_WEEK,
                LOW_52_WEEK,
                HIGH_52_WEEK,
                AVG_VOLUME,
                OPEN_INTEREST,
                OPTION_HISTORICAL_VOL,
                OPTION_IMPLIED_VOL,
                OPTION_BID_EXCH,
                OPTION_ASK_EXCH,
                OPTION_CALL_OPEN_INTEREST,
                OPTION_PUT_OPEN_INTEREST,
                OPTION_CALL_VOLUME,
                OPTION_PUT_VOLUME,
                INDEX_FUTURE_PREMIUM,
                BID_EXCH,
                ASK_EXCH,
                AUCTION_VOLUME,
                AUCTION_PRICE,
                AUCTION_IMBALANCE,
                MARK_PRICE,
                BID_EFP_COMPUTATION,
                ASK_EFP_COMPUTATION,
                LAST_EFP_COMPUTATION,
                OPEN_EFP_COMPUTATION,
                HIGH_EFP_COMPUTATION,
                LOW_EFP_COMPUTATION,
                CLOSE_EFP_COMPUTATION,
                LAST_TIMESTAMP,
                SHORTABLE,
                FUNDAMENTAL_RATIOS,
                RT_VOLUME,
                HALTED,
                BID_YIELD,
                ASK_YIELD,
                LAST_YIELD,
                CUST_OPTION_COMPUTATION,
                TRADE_COUNT,
                TRADE_RATE,
                VOLUME_RATE,
                LAST_RTH_TRADE,
                RT_HISTORICAL_VOL,
                IB_DIVIDENDS,
                BOND_FACTOR_MULTIPLIER,
                REGULATORY_IMBALANCE,
                NEWS_TICK,
                SHORT_TERM_VOLUME_3_MIN,
                SHORT_TERM_VOLUME_5_MIN,
                SHORT_TERM_VOLUME_10_MIN,
                DELAYED_BID,
                DELAYED_ASK,
                DELAYED_LAST,
                DELAYED_BID_SIZE,
                DELAYED_ASK_SIZE,
                DELAYED_LAST_SIZE,
                DELAYED_HIGH,
                DELAYED_LOW,
                DELAYED_VOLUME,
                DELAYED_CLOSE,
                DELAYED_OPEN,
                RT_TRD_VOLUME,
                CREDITMAN_MARK_PRICE,
                CREDITMAN_SLOW_MARK_PRICE,
                NOT_SET };

inline bool isPrice( TickType tickType) {
    return tickType == BID || tickType == ASK || tickType == LAST;
}

//AccountSummaryTags  
const std::string AccountType = "AccountType";
const std::string NetLiquidation = "NetLiquidation";
const std::string TotalCashValue = "TotalCashValue";
const std::string SettledCash = "SettledCash";
const std::string AccruedCash = "AccruedCash";
const std::string BuyingPower = "BuyingPower";
const std::string EquityWithLoanValue = "EquityWithLoanValue";
const std::string PreviousEquityWithLoanValue = "PreviousEquityWithLoanValue";
const std::string GrossPositionValue = "GrossPositionValue";
const std::string ReqTEquity = "ReqTEquity";
const std::string ReqTMargin = "ReqTMargin";
const std::string SMA = "SMA";
const std::string InitMarginReq = "InitMarginReq";
const std::string MaintMarginReq = "MaintMarginReq";
const std::string AvailableFunds = "AvailableFunds";
const std::string ExcessLiquidity = "ExcessLiquidity";
const std::string Cushion = "Cushion";
const std::string FullInitMarginReq = "FullInitMarginReq";
const std::string FullMaintMarginReq = "FullMaintMarginReq";
const std::string FullAvailableFunds = "FullAvailableFunds";
const std::string FullExcessLiquidity = "FullExcessLiquidity";
const std::string LookAheadNextChange = "LookAheadNextChange";
const std::string LookAheadInitMarginReq = "LookAheadInitMarginReq";
const std::string LookAheadMaintMarginReq = "LookAheadMaintMarginReq";
const std::string LookAheadAvailableFunds = "LookAheadAvailableFunds";
const std::string LookAheadExcessLiquidity = "LookAheadExcessLiquidity";
const std::string HighestSeverity = "HighestSeverity";
const std::string DayTradesRemaining = "DayTradesRemaining";
const std::string Leverage = "Leverage";


#endif /* common_defs_h_INCLUDED */
