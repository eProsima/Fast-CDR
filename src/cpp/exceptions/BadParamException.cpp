/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This copy of FastCdr is licensed to you under the terms described in the
 * FASTCDR_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

#include <fastcdr/exceptions/BadParamException.h>

using namespace eprosima::fastcdr::exception;

const char* const BadParamException::BAD_PARAM_MESSAGE_DEFAULT ="Bad parameter";

BadParamException::BadParamException(const char* const &message) : Exception(message)
{
}

BadParamException::BadParamException(const BadParamException &ex) : Exception(ex)
{
}

BadParamException::BadParamException(BadParamException&& ex) : Exception(std::move(ex))
{
}

BadParamException& BadParamException::operator=(const BadParamException &ex)
{
    if(this != &ex)
    {
        Exception::operator=(ex);
    }

    return *this;
}

BadParamException& BadParamException::operator=(BadParamException&& ex)
{
    if(this != &ex)
    {
        Exception::operator=(std::move(ex));
    }

    return *this;
}

BadParamException::~BadParamException() throw()
{
}

void BadParamException::raise() const
{
    throw *this;
}
