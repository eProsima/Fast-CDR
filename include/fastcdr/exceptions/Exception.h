// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#ifndef _FASTCDR_EXCEPTIONS_EXCEPTION_H_
#define _FASTCDR_EXCEPTIONS_EXCEPTION_H_

#include "../fastcdr_dll.h"
#include <string>

namespace eprosima {
namespace fastcdr {
namespace exception {
/*!
 * @brief This abstract class is used to create exceptions.
 * @ingroup EXCEPTIONMODULE
 */
class Cdr_DllAPI Exception
{
public:

    //! \brief Default destructor.
    virtual ~Exception() noexcept;

    //! \brief This function throws the object as exception.
    virtual void raise() const = 0;

    /*!
     * @brief This function returns the error message.
     *
     * @return The error message.
     */
    virtual const char* what() const noexcept;

protected:

    /*!
     * @brief Default constructor.
     *
     * @param message A error message. This message pointer is copied.
     */
    Exception(
            const char* const& message) noexcept;

    /*!
     * @brief Default copy constructor.
     *
     * @param ex Exception that will be copied.
     */
    Exception(
            const Exception& ex) noexcept;

    /*!
     * @brief Default move constructor.
     *
     * @param ex Exception that will be moved.
     */
    Exception(
            Exception&& ex) noexcept;

    /*!
     * @brief Assigment operation.
     *
     * @param ex Exception that will be copied.
     */
    Exception& operator =(
            const Exception& ex) noexcept;

    /*!
     * @brief Assigment operation.
     *
     * @param ex Exception that will be moved.
     */
    Exception& operator =(
            Exception&& ex) noexcept;

private:

    const char* m_message;
};
}         //namespace exception
}     //namespace fastcdr
} //namespace eprosima

#endif // _FASTCDR_EXCEPTIONS_EXCEPTION_H_
