#include "Condition.h"
#include "EClient.h"
#include "Order.h"
#include <sstream>

///////////////////OrderCondition//////////////////////////////
const char* OrderCondition::readExternal(const char* ptr, const char* endPtr) {
	std::string connector;

	DECODE_FIELD(connector)

	conjunctionConnection(connector == "a");

	return ptr;
}

void OrderCondition::writeExternal(std::ostream & msg) const {
	ENCODE_FIELD(conjunctionConnection() ? "a" : "o")
}

std::string OrderCondition::toString() {
	return conjunctionConnection() ? "<AND>" : "<OR>";
}

bool OrderCondition::conjunctionConnection() const {
	return m_isConjunctionConnection;
}

void OrderCondition::conjunctionConnection(bool isConjunctionConnection) {
	m_isConjunctionConnection = isConjunctionConnection;
}

OrderCondition::OrderConditionType OrderCondition::type() { return m_type; }

OrderCondition *OrderCondition::create(OrderConditionType type) {
	OrderCondition *rval = 0;

	switch (type) {
	case Execution:
		rval = new ExecutionCondition();
		break;

	case Margin:
		rval = new MarginCondition();
		break;

	case PercentChange:
		rval = new PercentChangeCondition();
		break;

	case Price:
		rval = new PriceCondition();
		break;

	case Time:
		rval = new TimeCondition();
		break;

	case Volume:
		rval = new VolumeCondition();
		break;
	}

	if (rval != 0)
		rval->m_type = type;

	return rval;
}

const char* OperatorCondition::readExternal(const char* ptr, const char* endPtr) {
    if (!(ptr = OrderCondition::readExternal(ptr, endPtr)))
        return 0;

    DECODE_FIELD(m_isMore);

    std::string str;

    DECODE_FIELD(str);

    valueFromString(str);

    return ptr;
}
////////////////////// OperatorCondition /////////////////////
std::string OperatorCondition::toString() {
    return " is " + std::string(isMore() ? ">= " : "<= ") + valueToString();
}

void OperatorCondition::writeExternal(std::ostream & msg) const {
    OrderCondition::writeExternal(msg);

    ENCODE_FIELD(m_isMore);
    ENCODE_FIELD(valueToString());
}

bool OperatorCondition::isMore() {
    return m_isMore;
}

void OperatorCondition::isMore(bool isMore) {
    m_isMore = isMore;
}


///////////////////////ContractCondition//////////////////////////////////
std::string ContractCondition::toString() {
    std::string strContract = conId() + "";

    return type() + " of " + strContract + OperatorCondition::toString();
}

const char* ContractCondition::readExternal(const char* ptr, const char* endPtr) {
    if (!(ptr = OperatorCondition::readExternal(ptr, endPtr)))
        return 0;

    DECODE_FIELD(m_conId);
    DECODE_FIELD(m_exchange);

    return ptr;
}

void ContractCondition::writeExternal(std::ostream & msg) const {
    OperatorCondition::writeExternal(msg);

    ENCODE_FIELD(m_conId);
    ENCODE_FIELD(m_exchange);
}

int ContractCondition::conId() {
    return m_conId;
}

void ContractCondition::conId(int conId) {
    m_conId = conId;
}

std::string ContractCondition::exchange() {
    return m_exchange;
}

void ContractCondition::exchange(const std::string & exchange) {
    m_exchange = exchange;
}

///////////////////////MarginCondition/////////////////////////////////
std::string MarginCondition::valueToString() const {
    std::stringstream tmp;

    tmp << m_percent;

    return tmp.str();
}

void MarginCondition::valueFromString(const std::string & v) {
    std::stringstream tmp;

    tmp << v;
    tmp >> m_percent;
}

std::string MarginCondition::toString() {
    return "the margin cushion percent" + OperatorCondition::toString();
}

int MarginCondition::percent() {
    return m_percent;
}

void MarginCondition::percent(int percent) {
    m_percent = percent;
}
////////////////PercentChangeCondition/////////////////

std::string PercentChangeCondition::valueToString() const {
	std::stringstream tmp;

	tmp << m_changePercent;

	return tmp.str();
}

void PercentChangeCondition::valueFromString(const std::string & v) {
	std::stringstream tmp;
	
	tmp << v;
	tmp >> m_changePercent;
}

double PercentChangeCondition::changePercent() {
	return m_changePercent;
}

void PercentChangeCondition::changePercent(double changePercent) {
	m_changePercent = changePercent;
}

std::string PriceCondition::valueToString() const {
	std::stringstream tmp;

	tmp << m_price;

	return tmp.str();
}

void PriceCondition::valueFromString(const std::string & v) {
	std::stringstream tmp;

	tmp << v;
	tmp >> m_price;
}

double PriceCondition::price() {
	return m_price;
}

void PriceCondition::price(double price) {
	m_price = price;
}

std::string PriceCondition::toString() {
	return strTriggerMethod() + " " + ContractCondition::toString();
}

PriceCondition::Method PriceCondition::triggerMethod() {
	return (Method)m_triggerMethod;
}

std::string PriceCondition::strTriggerMethod() {
	static std::string mthdNames[] = { "default", "double bid/ask", "last", "double last", "bid/ask", "", "", "last of bid/ask", "mid-point" };

	return mthdNames[triggerMethod()];
}

void PriceCondition::triggerMethod(Method triggerMethod) {
	m_triggerMethod = triggerMethod;
}

const char* PriceCondition::readExternal(const char* ptr, const char* endPtr) {
	if (!(ptr = ContractCondition::readExternal(ptr, endPtr)))
		return 0;

	DECODE_FIELD(m_triggerMethod)

	return ptr;
}

void PriceCondition::writeExternal(std::ostream & msg) const {
	ContractCondition::writeExternal(msg);

	ENCODE_FIELD(m_triggerMethod);
}

///////////////////////TimeCondition//////////////////////////////////
std::string TimeCondition::valueToString() const {
	return m_time;
}

void TimeCondition::valueFromString(const std::string & v) {
	m_time = v;
}

std::string TimeCondition::toString() {
	return "time" + OperatorCondition::toString();
}

std::string TimeCondition::time() {
	return m_time;
}

void TimeCondition::time(const std::string & time) {
	m_time = time;
}

////////////////////VolumeCondition/////////////////////
std::string VolumeCondition::valueToString() const {
	std::stringstream tmp;

	tmp << m_volume;

	return tmp.str();
}

void VolumeCondition::valueFromString(const std::string & v) {
	std::stringstream tmp;

	tmp << v;
	tmp >> m_volume;
}

int VolumeCondition::volume() {
	return m_volume;
}

void VolumeCondition::volume(int volume) {
	m_volume = volume;
}

////////////////////ExecutionCondition////////////////////////////////
const char* ExecutionCondition::readExternal(const char* ptr, const char* endPtr) {
	if (!(ptr = OrderCondition::readExternal(ptr, endPtr)))
		return 0;

	DECODE_FIELD(m_secType)
	DECODE_FIELD(m_exchange);
	DECODE_FIELD(m_symbol);

	return ptr;
}

std::string ExecutionCondition::toString() {
	return "trade occurs for " + m_symbol + " symbol on " + m_exchange + " exchange for " + m_secType + " security type";
}

void ExecutionCondition::writeExternal(std::ostream & msg) const {
	OrderCondition::writeExternal(msg);

	ENCODE_FIELD(m_secType);
	ENCODE_FIELD(m_exchange);
	ENCODE_FIELD(m_symbol);
}

std::string ExecutionCondition::exchange() {
	return m_exchange;
}

void ExecutionCondition::exchange(const std::string &exchange) {
	m_exchange = exchange;
}

std::string ExecutionCondition::secType() {
	return m_secType;
}

void ExecutionCondition::secType(const std::string &secType) {
	m_secType = secType;
}

std::string ExecutionCondition::symbol() {
	return m_symbol;
}

void ExecutionCondition::symbol(const std::string &symbol) {
	m_symbol = symbol;
}
