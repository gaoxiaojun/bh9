#define ERR_DEF(_) \
_(100,	"Max rate of messages per second has been exceeded.") \
_(101,	"Max number of tickers has been reached.") \
_(102,	"Duplicate ticker ID.") \
_(103,	"Duplicate order ID.") \
_(104,	"Can't modify a filled order.") \
_(105,	"Order being modified does not match original order.") \
_(106,	"Can't transmit order ID:") \
_(107,	"Cannot transmit incomplete order.") \
_(109,	"Price is out of the range defined by the Percentage setting at order defaults frame. The order will not be transmitted.") \
_(110,	"The price does not conform to the minimum price variation for this contract.") \
_(111,	"The TIF (Tif type) and the order type are incompatible.") \
_(113,	"The Tif option should be set to DAY for MOC and LOC orders.") \
_(114,	"Relative orders are valid for stocks only.") \
_(115,	"Relative orders for US stocks can only be submitted to SMART, SMART_ECN, INSTINET, or PRIMEX.") \
_(116,	"The order cannot be transmitted to a dead exchange.") \
_(117,	"The block order size must be at least 50.") \
_(118,	"VWAP orders must be routed through the VWAP exchange.") \
_(119,	"Only VWAP orders may be placed on the VWAP exchange.") \
_(120,	"It is too late to place a VWAP order for today.") \
_(121,	"Invalid BD flag for the order. Check 'Destination' and 'BD' flag.") \
_(122,	"No request tag has been found for order:") \
_(123,	"No record is available for conid:") \
_(124,	"No market rule is available for conid:") \
_(125,	"Buy price must be the same as the best asking price.") \
_(126,	"Sell price must be the same as the best bidding price.") \
_(129,	"VWAP orders must be submitted at least three minutes before the start time.") \
_(131,	"The sweep-to-fill flag and display size are only valid for US stocks routed through SMART, and will be ignored.") \
_(132,	"This order cannot be transmitted without a clearing account.") \
_(133,	"Submit new order failed.") \
_(134,	"Modify order failed.") \
_(135,	"Can't find order with ID =") \
_(136,	"This order cannot be cancelled.") \
_(137,	"VWAP orders can only be cancelled up to three minutes before the start time.") \
_(138,	"Could not parse ticker request:") \
_(139,	"Parsing error:") \
_(140,	"The size value should be an integer:") \
_(141,	"The price value should be a double:") \
_(142,	"Institutional customer account does not have account info") \
_(143,	"Requested ID is not an integer number.") \
_(144,	"Order size does not match total share allocation. To adjust the share allocation, right-click on the order and select 'Modify > Share Allocation.'") \
_(145,	"Error in validating entry fields -") \
_(146,	"Invalid trigger method.") \
_(147,	"The conditional contract info is incomplete.") \
_(148,	"A conditional order can only be submitted when the order type is set to limit or market.") \
_(151,	"This order cannot be transmitted without a user name.") \
_(152,	"The 'hidden' order attribute may not be specified for this order.") \
_(153,	"EFPs can only be limit orders.") \
_(154,	"Orders cannot be transmitted for a halted security.") \
_(155,	"A sizeOp order must have a username and account.") \
_(156,	"A SizeOp order must go to IBSX") \
_(157,	"An order can be EITHER Iceberg or Discretionary. Please remove either the Discretionary amount or the Display size.") \
_(158,	"You must specify an offset amount or a percent offset value.") \
_(159,	"The percent offset value must be between 0% and 100%.") \
_(160,	"The size value cannot be zero.") \
_(161,	"Cancel attempted when order is not in a cancellable state. Order permId =") \
_(162,	"Historical market data Service error message.") \
_(163,	"The price specified would violate the percentage constraint specified in the default order settings.") \
_(164,	"There is no market data to check price percent violations.") \
_(165,	"Historical market Data Service query message.") \
_(166,	"HMDS Expired Contract Violation.") \
_(167,	"VWAP order time must be in the future.") \
_(168,	"Discretionary amount does not conform to the minimum price variation for this contract.") \
_(200,	"No security definition has been found for the request.") \
_(201,	"Order rejected - Reason:") \
_(202,	"Order cancelled - Reason:") \
_(203,	"The security <security> is not available or allowed for this account.") \
_(300,	"Can't find EId with ticker Id:") \
_(301,	"Invalid ticker action:") \
_(302,	"Error parsing stop ticker string:") \
_(303,	"Invalid action:") \
_(304,	"Invalid account value action:") \
_(305,	"Request parsing error, the request has been ignored.") \
_(306,	"Error processing DDE request:") \
_(307,	"Invalid request topic:") \
_(308,	"Unable to create the 'API' page in TWS as the maximum number of pages already exists.") \
_(309,	" Max number (3) of market depth requests has been reached. Note:  TWS currently limits users to a maximum of 3 distinct market depth requests. This same restriction applies to API clients, however API clients may make multiple market depth requests for the same security.") \
_(310,	"Can't find the subscribed market depth with tickerId:") \
_(311,	"The origin is invalid.") \
_(312,	"The combo details are invalid.") \
_(313,	"The combo details for leg '<leg number>' are invalid.") \
_(314,	"Security type 'BAG' requires combo leg details.") \
_(315,	"Stock combo legs are restricted to SMART order routing.") \
_(316,	"Market depth data has been HALTED. Please re-subscribe.") \
_(317,	"Market depth data has been RESET. Please empty deep book contents before applying any new entries.") \
_(319,	"Invalid log level <log level>") \
_(320,	"Server error when reading an API client request.") \
_(321,	"Server error when validating an API client request.") \
_(322,	"Server error when processing an API client request.") \
_(323,	"Server error: cause - %s") \
_(324,	"Server error when reading a DDE client request (missing information).") \
_(325,	"Discretionary orders are not supported for this combination of exchange and order type.") \
_(326,	"Unable to connect as the client id is already in use. Retry with a unique client id.") \
_(327,	"Only API connections with clientId set to 0 can set the auto bind TWS orders property.") \
_(328,	"Trailing stop orders can be attached to limit or stop-limit orders only.") \
_(329,	"Order modify failed. Cannot change to the new order type.") \
_(330,	"Only FA or STL customers can request managed accounts list.") \
_(331,	"Internal error. FA or STL does not have any managed accounts.") \
_(332,	"The account codes for the order profile are invalid.") \
_(333,	"Invalid share allocation syntax.") \
_(334,	"Invalid Good Till Date order") \
_(335,	"Invalid delta: The delta must be between 0 and 100.") \
_(336,	"The time or time zone is invalid. The correct format is hh:mm:ss xxx where xxx is an optionally specified time-zone. E.g.: 15:59:00 EST Note that there is a space between the time and the time zone. If no time zone is specified, local time is assumed.") \
_(337,	"The date, time, or time-zone entered is invalid. The correct format is yyyymmdd hh:mm:ss xxx where yyyymmdd and xxx are optional.") \
_(342,	"Pegged order is not supported on this exchange.") \
_(343,	"The date, time, or time-zone entered is invalid. The correct format is yyyymmdd hh:mm:ss xxx where yyyymmdd and xxx are optional.") \
_(344,	"The account logged into is not a financial advisor account.") \
_(345,	"Generic combo is not supported for FA advisor account.") \
_(346,	"Not an institutional account or an away clearing account.") \
_(347,	"Short sale slot value must be 1 (broker holds shares) or 2 (delivered from elsewhere).") \
_(348,	"Order not a short sale -- type must be SSHORT to specify short sale slot.") \
_(349,	"Generic combo does not support 'Good After' attribute.") \
_(350,	"Minimum quantity is not supported for best combo order.") \
_(351,	"The 'Regular Trading Hours only' flag is not valid for this order.") \
_(352,	"Short sale slot value of 2 (delivered from elsewhere) requires location.") \
_(353,	"Short sale slot value of 1 requires no location be specified.") \
_(354,	"Not subscribed to requested market data.") \
_(355,	"Order size does not conform to market rule.") \
_(356,	"Smart-combo order does not support OCA group.") \
_(357,	"Your client version is out of date.") \
_(358,	"Smart combo child order not supported.") \
_(359,	"Combo order only supports reduce on fill without block(OCA).") \
_(360,	"No whatif check support for smart combo order.") \
_(361,	"Invalid trigger price.") \
_(362,	"Invalid adjusted stop price.") \
_(363,	"Invalid adjusted stop limit price.") \
_(364,	"Invalid adjusted trailing amount.") \
_(365,	"No scanner subscription found for ticker id:") \
_(366,	"No historical data query found for ticker id:") \
_(367,	"Volatility type if set must be 1 or 2 for VOL orders. Do not set it for other order types.") \
_(368,	"Reference Price Type must be 1 or 2 for dynamic volatility management. Do not set it for non-VOL orders.") \
_(369,	"Volatility orders are only valid for US options.") \
_(370,	"Dynamic Volatility orders must be SMART routed, or trade on a Price Improvement Exchange.") \
_(371,	"VOL order requires positive floating point value for volatility. Do not set it for other order types.") \
_(372,	"Cannot set dynamic VOL attribute on non-VOL order.") \
_(373,	"Can only set stock range attribute on VOL or RELATIVE TO STOCK order.") \
_(374,	"If both are set, the lower stock range attribute must be less than the upper stock range attribute.") \
_(375,	"Stock range attributes cannot be negative.") \
_(376,	"The order is not eligible for continuous update. The option must trade on a cheap-to-reroute exchange.") \
_(377,	"Must specify valid delta hedge order aux. price.") \
_(378,	"Delta hedge order type requires delta hedge aux. price to be specified.") \
_(379,	"Delta hedge order type requires that no delta hedge aux. price be specified.") \
_(380,	"This order type is not allowed for delta hedge orders.") \
_(381,	"Your DDE.dll needs to be upgraded.") \
_(382,	"The price specified violates the number of ticks constraint specified in the default order settings.") \
_(383,	"The size specified violates the size constraint specified in the default order settings.") \
_(384,	"Invalid DDE array request.") \
_(385,	"Duplicate ticker ID for API scanner subscription.") \
_(386,	"Duplicate ticker ID for API historical data query.") \
_(387,	"Unsupported order type for this exchange and security type.") \
_(388,	"Order size is smaller than the minimum requirement.") \
_(389,	"Supplied routed order ID is not unique.") \
_(390,	"Supplied routed order ID is invalid.") \
_(391,	" The time or time-zone entered is invalid. The correct format is hh:mm:ss xxx where xxx is an optionally specified time-zone. If no time zone is specified, local time is assumed.") \
_(392,	"Invalid order: contract expired.") \
_(393,	"Short sale slot may be specified for delta hedge orders only.") \
_(394,	"Invalid Process Time: must be integer number of milliseconds between 100 and 2000.  Found:") \
_(395,	"Due to system problems, orders with OCA groups are currently not being accepted.") \
_(396,	"Due to system problems, application is currently accepting only Market and Limit orders for this contract.") \
_(397,	"Due to system problems, application is currently accepting only Market and Limit orders for this contract.") \
_(398,	"< > cannot be used as a condition trigger.") \
_(399,	"Order message error") \
_(400,	"Algo order error.") \
_(401,	"Length restriction.") \
_(402,	"Conditions are not allowed for this contract.") \
_(403,	"Invalid stop price.") \
_(404,	"Shares for this order are not immediately available for short sale. The order will be held while we attempt to locate the shares.") \
_(405,	"The child order quantity should be equivalent to the parent order size.") \
_(406,	"The currency < > is not allowed.") \
_(407,	"The symbol should contain valid non-unicode characters only.") \
_(408,	"Invalid scale order increment.") \
_(409,	"Invalid scale order. You must specify order component size.") \
_(410,	"Invalid subsequent component size for scale order.") \
_(411,	"The 'Outside Regular Trading Hours' flag is not valid for this order.") \
_(412,	"The contract is not available for trading.") \
_(413,	"What-if order should have the transmit flag set to true.") \
_(414,	"Snapshot market data subscription is not applicable to generic ticks.") \
_(415,	"Wait until previous RFQ finishes and try again.") \
_(416,	"RFQ is not applicable for the contract. Order ID:") \
_(417,	"Invalid initial component size for scale order.") \
_(418,	"Invalid scale order profit offset.") \
_(419,	"Missing initial component size for scale order.") \
_(420,	"Invalid real-time query.") \
_(421,	"Invalid route.") \
_(422,	"The account and clearing attributes on this order may not be changed.") \
_(423,	"Cross order RFQ has been expired. THI committed size is no longer available. Please open order dialog and verify liquidity allocation.") \
_(424,	"FA Order requires allocation to be specified.") \
_(425,	"FA Order requires per-account manual allocations because there is no common clearing instruction. Please use order dialog Adviser tab to enter the allocation.") \
_(426,	"None of the accounts have enough shares.") \
_(427,	"Mutual Fund order requires monetary value to be specified.") \
_(428,	"Mutual Fund Sell order requires shares to be specified.") \
_(429,	"Delta neutral orders are only supported for combos (BAG security type).") \
_(430,	"We are sorry, but fundamentals data for the security specified is not available.") \
_(431,	"What to show field is missing or incorrect.") \
_(432,	"Commission must not be negative.") \
_(433,	"Invalid 'Restore size after taking profit' for multiple account allocation scale order.") \
_(434,	"The order size cannot be zero.") \
_(435,	"You must specify an account.") \
_(436,	"You must specify an allocation (either a single account, group, or profile).") \
_(437,	"Order can have only one flag Outside RTH or Allow PreOpen.") \
_(438,	"The application is now locked.") \
_(439,	"Order processing failed. Algorithm definition not found.") \
_(440,	"Order modify failed. Algorithm cannot be modified.") \
_(441,	"Algo attributes validation failed:") \
_(442,	"Specified algorithm is not allowed for this order.") \
_(443,	"Order processing failed. Unknown algo attribute.") \
_(444,	"Volatility Combo order is not yet acknowledged. Cannot submit changes at this time.") \
_(445,	"The RFQ for this order is no longer valid.") \
_(446,	"Missing scale order profit offset.") \
_(447,	"Missing scale price adjustment amount or interval.") \
_(448,	"Invalid scale price adjustment interval.") \
_(449,	"Unexpected scale price adjustment amount or interval.") \
_(450,	"Dividend schedule query failed.") \
_(501,	"Already connected.") \
_(502,	"Couldn't connect to TWS. Confirm that API is enabled in TWS via the Configure>API menu command.") \
_(503,	"Your version of TWS is out of date and must be upgraded.") \
_(504,	"Not connected.") \
_(505,	"Fatal error: Unknown message id.") \
_(506,	"Unsupported version. For Java clients only.") \
_(507,	"Bad message length. For Java clients only.") \
_(508,	"Bad message. For Java clients only.") \
_(510,	"Request market data - sending error:") \
_(511,	"Cancel market data - sending error:") \
_(512,	"Order - sending error:") \
_(513,	"Account update request - sending error:") \
_(514,	"Request for executions  - sending error:") \
_(515,	"Cancel order - sending error:") \
_(516,	"Request open order - sending error:") \
_(517,	"Unknown contract. Verify the contract details supplied.") \
_(518,	"Request contract data - sending error:") \
_(519,	"Request market depth - sending error:") \
_(520,	"Cancel market depth - sending error:") \
_(521,	"Set server log level - sending error:") \
_(522,	"FA Information Request - sending error:") \
_(523,	"FA Information Replace - sending error:") \
_(524,	"Request Scanner subscription - sending error:") \
_(525,	"Cancel Scanner subscription - sending error:") \
_(526,	"Request Scanner parameter - sending error:") \
_(527,	"Request Historical data - sending error:") \
_(528,	"Cancel Historical data - sending error:") \
_(529,	"Request real-time bar data - sending error:") \
_(530,	"Cancel real-time bar data - sending error:") \
_(531,	"Request Current Time - Sending error:") \
_(532,  "Request Fundamental Data Sending Error - ") \
_(533,  "Cancel Fundamental Data Sending Error -") \
_(534,  "Request Calculate Implied Volatility Sending Error - ") \
_(535,  "Request Calculate Option Price Sending Error - ") \
_(536,  "Cancel Calculate Implied Volatility Sending Error - ") \
_(537,  "Cancel Calculate Option Price Sending Error - ") \
_(538,  "Request Global Cancel Sending Error - ") \
_(539,  "Request Market Data Type Sending Error -") \
_(540,  "Request Positions Sending Error -") \
_(541,  "Cancel Positions Sending Error - ") \
_(542,  "Request Account Data Sending Error -") \
_(543,  "Cancel Account Data Sending Error - ") \
_(544,  "Verify Request Sending Error - ") \
_(545,  "Verify Message Sending Error -") \
_(546,  "Query Display Groups Sending Error -") \
_(547,  "Subscribe To Group Events Sending Error -") \
_(548,  "Update Display Group Sending Error - ") \
_(549,  "Unsubscribe From Group Events Sending Error - ") \
_(551,  "Verify And Auth Request Sending Error - ") \
_(552,  "Verify And Auth Message Sending Error - ") \
_(553,  "Request Positions Multi Sending Error - ") \
_(554,  "Cancel Positions Multi Sending Error - ") \
_(555,  "Request Account Updates Multi Sending Error - ") \
_(556,  "Cancel Account Updates Multi Sending Error - ") \
_(557,  "Request Security Definition Option Parameters Sending Error -") \
_(558,  "Request Soft Dollar Tiers Sending Error -") \
_(559,  "Request Family Codes Sending Error -") \
_(560,  "Request Matching Symbols Sending Error -") \
_(561,  "Request Market Depth Exchanges Sending Error - ") \
_(562,  "Request Smart Components Sending Error - ") \
_(563,  "Request News Providers Sending Error -") \
_(564,  "Request News Article Sending Error - ") \
_(565,  "Request Historical News Sending Error - ") \
_(566,  "Request Head Time Stamp Sending Error -") \
_(10000,	"Cross currency combo error.") \
_(10001,	"Cross currency vol error.") \
_(10002,	"Invalid non-guaranteed legs.") \
_(10003,	"IBSX not allowed.") \
_(10005,	"Read-only models.") \
_(10006,	"Missing parent order.") \
_(10007,	"Invalid hedge type.") \
_(10008,	"Invalid beta value.") \
_(10009,	"Invalid hedge ratio.") \
_(10010,	"Invalid delta hedge order.") \
_(10011,	"Currency is not supported for Smart combo.") \
_(10012,	"Invalid allocation percentage") \
_(10013,	"Smart routing API error (Smart routing opt-out required).") \
_(10014,	"PctChange limits.") \
_(10015,	"Trading is not allowed in the API.") \
_(10016,	"Contract is not visible.") \
_(10017,	"Contracts are not visible.") \
_(10018,	"Orders use EV warning.") \
_(10019,	"Trades use EV warning.") \
_(10020,	"Display size should be smaller than order size.") \
_(10021,	"Invalid leg2 to Mkt Offset API.") \
_(10022,	"Invalid Leg Prio API.") \
_(10023,	"Invalid combo display size API.") \
_(10024,	"Invalid don’t start next legin API.") \
_(10025,	"Invalid leg2 to Mkt time1 API.") \
_(10026,	"Invalid leg2 to Mkt time2 API.") \
_(10027,	"Invalid combo routing tag API.") \
\
/* system message code */ \
_(1100, "Connectivity between IB and TWS has been lost.") \
_(1101, "Connectivity between IB and TWS has been restored- data lost.*") \
_(1102, "Connectivity between IB and TWS has been restored- data maintained.") \
_(1300, "TWS socket port has been reset and this connection is being dropped.Please reconnect on the new port - <port_num>*Market and account data subscription requests must be resubmitted") \
\
/* warning message code */ \
_(2100, "New account data requested from TWS.  API client has been unsubscribed from account data.") \
_(2101, "Unable to subscribe to account as the following clients are subscribed to a different account.") \
_(2102, "Unable to modify this order as it is still being processed.") \
_(2103, "A market data farm is disconnected.") \
_(2104, "A market data farm is connected.") \
_(2105, "A historical data farm is disconnected.") \
_(2106, "A historical data farm is connected.") \
_(2107, "A historical data farm connection has become inactive but should be available upon demand.") \
_(2108, "A market data farm connection has become inactive but should be available upon demand.") \
_(2109, "Order Event Warning: Attribute “Outside Regular Trading Hours” is ignored based on the order type and destination. PlaceOrder is now processed.") \
_(2110, "Connectivity between TWS and server is broken. It will be restored automatically.")

