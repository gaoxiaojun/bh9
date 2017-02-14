#pragma once
#include "econfig.h"
#include <string>
#include <float.h>
#include <limits.h>
#include <ios>
#define UNSET_DOUBLE DBL_MAX

struct IExternalizable
{
    virtual const char* readExternal(const char* ptr, const char* endPtr) = 0;
    virtual void writeExternal(std::ostream &out) const = 0;
};

class TWS_API OrderCondition : public IExternalizable {
public:
    enum OrderConditionType {
        Price = 1,
        Time = 3,
        Margin = 4,
        Execution = 5,
        Volume = 6,
        PercentChange = 7
    };

    virtual ~OrderCondition() = default;
private:
    OrderConditionType m_type;
    bool m_isConjunctionConnection;

public:
    virtual const char* readExternal(const char* ptr, const char* endPtr) override;
    virtual void writeExternal(std::ostream &out) const override;

    std::string toString();
    bool conjunctionConnection() const;
    void conjunctionConnection(bool isConjunctionConnection);
    OrderConditionType type();

    static OrderCondition *create(OrderConditionType type);
};

class TWS_API OperatorCondition : public OrderCondition {
    bool m_isMore;

protected:
    virtual std::string valueToString() const = 0;
    virtual void valueFromString(const std::string &v) = 0;

public:
    virtual const char* readExternal(const char* ptr, const char* endPtr) override;
    virtual std::string toString();
    virtual void writeExternal(std::ostream &out) const override;

    bool isMore();
    void isMore(bool isMore);
};

class TWS_API ContractCondition : public OperatorCondition {
	int m_conId;
	std::string m_exchange;

public:
    virtual std::string toString() override;
    virtual const char* readExternal(const char* ptr, const char* endPtr) override;
    virtual void writeExternal(std::ostream &out) const override;

	int conId();
	void conId(int conId);
	std::string exchange();
	void exchange(const std::string &exchange);
};


class TWS_API MarginCondition : public OperatorCondition {
	friend OrderCondition;

	int m_percent;

public:
	static const OrderConditionType conditionType = OrderConditionType::Margin;

protected:
	MarginCondition() { }

    virtual std::string valueToString() const override;
    virtual void valueFromString(const std::string &v) override;

public:
    virtual std::string toString() override;

	int percent();
	void percent(int percent);
};

class TWS_API TimeCondition : public OperatorCondition {
    friend OrderCondition;

    std::string m_time;

protected:
    TimeCondition() { }

    virtual std::string valueToString() const override;
    virtual void valueFromString(const std::string &v) override;

public:
    static const OrderConditionType conditionType = OrderConditionType::Time;

    virtual std::string toString() override;

    std::string time();
    void time(const std::string &time);
};

class TWS_API PercentChangeCondition : public ContractCondition {
	friend OrderCondition;

	double m_changePercent;

protected:
	PercentChangeCondition()
	: m_changePercent(UNSET_DOUBLE)
	{ }

	virtual std::string valueToString() const;
	virtual void valueFromString(const std::string &v);

public:
	static const OrderConditionType conditionType = OrderConditionType::PercentChange;

	double changePercent();
	void changePercent(double changePercent);
};

class TWS_API PriceCondition : public ContractCondition {
	friend OrderCondition;

	double m_price;
	int m_triggerMethod;

    virtual std::string valueToString() const override;
    virtual void valueFromString(const std::string &v) override;

protected:
    PriceCondition() { }

public:
	static const OrderConditionType conditionType = OrderConditionType::Price;
	enum Method {
		Default = 0,
		DoubleBidAsk = 1,
		Last = 2,
		DoubleLast = 3,
		BidAsk = 4,
		LastBidAsk = 7,
		MidPoint = 8
	};

	double price();
	void price(double price);

    virtual std::string toString() override;
    virtual const char* readExternal(const char* ptr, const char* endPtr) override;
    virtual void writeExternal(std::ostream & out) const override;

	Method triggerMethod();
	std::string strTriggerMethod();
	void triggerMethod(Method triggerMethod);
};

class TWS_API VolumeCondition : public ContractCondition {
	friend OrderCondition;

	int m_volume;

protected:
	VolumeCondition() { }

    virtual std::string valueToString() const override;
    virtual void valueFromString(const std::string &v) override;

public:
	static const OrderConditionType conditionType = OrderConditionType::Volume;

	int volume();
	void volume(int volume);
};

class TWS_API ExecutionCondition : public OrderCondition {
	friend OrderCondition;

	std::string m_exchange;
	std::string m_secType;
	std::string m_symbol;

public:
	static const OrderConditionType conditionType = OrderConditionType::Execution;

protected:
	ExecutionCondition() { }

public:
    virtual const char* readExternal(const char* ptr, const char* endPtr) override;
    virtual std::string toString();
    virtual void writeExternal(std::ostream &out) const override;

	std::string exchange();
	void exchange(const std::string &exchange);
	std::string secType();
	void secType(const std::string &secType);
	std::string symbol();
	void symbol(const std::string &symbol);
};
