/*************************************************************************
 * Copyright (c) 2012 eProsima. All rights reserved.
 *
 * This copy of RPCDDS is licensed to you under the terms described in the
 * RPCDDS_LICENSE file included in this distribution.
 *
 *************************************************************************/

#include <cpp/exceptions/NotEnoughMemoryException.h>

namespace eProsima
{
    const std::string NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT("Not enough memory in the buffer stream");

	NotEnoughMemoryException::NotEnoughMemoryException(const std::string &message) : Exception(message)
	{
	}

	NotEnoughMemoryException::NotEnoughMemoryException(std::string&& message) : Exception(std::move(message))
	{
	}

	NotEnoughMemoryException::NotEnoughMemoryException(const NotEnoughMemoryException &ex) : Exception(ex)
	{
	}

	NotEnoughMemoryException::NotEnoughMemoryException(NotEnoughMemoryException&& ex) : Exception(std::move(ex))
	{
	}

	NotEnoughMemoryException& NotEnoughMemoryException::operator=(const NotEnoughMemoryException &ex)
	{
		if(this != &ex)
		{
			Exception::operator=(ex);
		}

		return *this;
	}

	NotEnoughMemoryException& NotEnoughMemoryException::operator=(NotEnoughMemoryException&& ex)
	{
		if(this != &ex)
		{
			Exception::operator=(std::move(ex));
		}

		return *this;
	}

	NotEnoughMemoryException::~NotEnoughMemoryException() throw()
	{
	}

	void NotEnoughMemoryException::raise() const
	{
		throw *this;
	}
} // namespace eProsima
