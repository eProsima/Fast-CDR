/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This copy of FastCdr is licensed to you under the terms described in the
 * FASTCDR_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

#include <fastcdr/exceptions/NotEnoughMemoryException.h>

using namespace eprosima::fastcdr::exception;

const char* const NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT = "Not enough memory in the buffer stream";

NotEnoughMemoryException::NotEnoughMemoryException(const char* const &message) : Exception(message)
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
