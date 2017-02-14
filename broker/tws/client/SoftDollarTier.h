#pragma once
#include "econfig.h"
#include <string>

class TWS_API SoftDollarTier
{
	std::string m_name, m_val, m_displayName;

public:
    SoftDollarTier(const std::string& name = "", const std::string& val = "", const std::string& displayName = "") :
        m_name(name), m_val(val), m_displayName(displayName)
    {
    }

    std::string name() const { return m_name; }
    std::string val() const { return m_val; }
    std::string displayName() const { return m_displayName; }
};

