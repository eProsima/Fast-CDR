// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <fastcdr/exceptions/BadOptionalAccessException.hpp>

using namespace eprosima::fastcdr::exception;

const char* const BadOptionalAccessException::BAD_OPTIONAL_ACCESS_MESSAGE_DEFAULT = "Bad optional access";

BadOptionalAccessException::BadOptionalAccessException(
        const char* const& message) noexcept
    : Exception(message)
{
}

BadOptionalAccessException::BadOptionalAccessException(
        const BadOptionalAccessException& ex) noexcept
    : Exception(ex)
{
}

BadOptionalAccessException::BadOptionalAccessException(
        BadOptionalAccessException&& ex) noexcept
    : Exception(std::move(ex))
{
}

BadOptionalAccessException& BadOptionalAccessException::operator =(
        const BadOptionalAccessException& ex) noexcept
{
    if (this != &ex)
    {
        Exception::operator =(
                ex);
    }

    return *this;
}

BadOptionalAccessException& BadOptionalAccessException::operator =(
        BadOptionalAccessException&& ex) noexcept
{
    if (this != &ex)
    {
        Exception::operator =(
                std::move(ex));
    }

    return *this;
}

BadOptionalAccessException::~BadOptionalAccessException() noexcept
{
}

void BadOptionalAccessException::raise() const
{
    throw *this;
}
