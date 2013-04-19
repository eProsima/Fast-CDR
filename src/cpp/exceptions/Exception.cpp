/*************************************************************************
 * Copyright (c) 2012 eProsima. All rights reserved.
 *
 * This copy of RPCDDS is licensed to you under the terms described in the
 * RPCDDS_LICENSE file included in this distribution.
 *
 *************************************************************************/

#include "cpp/exceptions/Exception.h"

namespace eProsima
{
	Exception::Exception(const std::string &message) : m_message(message)
	{
	}

	Exception::Exception(std::string&& message) : m_message(std::move(message))
	{
	}

	Exception::Exception(const Exception &ex) : m_message(ex.m_message)
	{
	}

	Exception::Exception(Exception&& ex) : m_message(std::move(ex.m_message))
	{
	}

	Exception& Exception::operator=(const Exception &ex)
	{
		m_message = ex.m_message;
		return *this;
	}

	Exception& Exception::operator=(Exception&&)
	{
		m_message = std::move(m_message);
		return *this;
	}

	Exception::~Exception() EPROSIMA_USE_NOEXCEPT
	{
	}

	const char* Exception::what() const throw()
	{
		return m_message.c_str();
	}
} // namespace eProsima
