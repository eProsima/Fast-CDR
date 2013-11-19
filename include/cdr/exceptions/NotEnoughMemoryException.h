/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This copy of Fast Buffers is licensed to you under the terms described in the
 * FAST_BUFFERS_LICENSE file included in this distribution.
 *
 *************************************************************************/

#ifndef _CDR_EXCEPTIONS_NOTENOUGHMEMORYEXCEPTION_H_
#define _CDR_EXCEPTIONS_NOTENOUGHMEMORYEXCEPTION_H_

#include <cdr/exceptions/Exception.h>

namespace eprosima
{
    /*!
     * @brief This class is thrown as an exception when the buffer's internal memory reachs its size limit.
     * @ingroup EXCEPTIONMODULE
     */
    class Cdr_DllAPI NotEnoughMemoryException : public Exception
    {
    public:

        /*!
         * @brief Default constructor.
         *
         * @param message A error message. This message is copied.
         */
        NotEnoughMemoryException(const std::string &message);

        /*!
         * @brief Default constructor.
         *
         * @param message A error message. This message is moved.
         */
        NotEnoughMemoryException(std::string&& message);

        /*!
         * @brief Default copy constructor.
         *
         * @param ex NotEnoughMemoryException that will be copied.
         */
        NotEnoughMemoryException(const NotEnoughMemoryException &ex);

        /*!
         * @brief Default move constructor.
         *
         * @param ex NotEnoughMemoryException that will be moved.
         */
        NotEnoughMemoryException(NotEnoughMemoryException&& ex);

        /*!
         * @brief Assigment operation.
         *
         * @param ex NotEnoughMemoryException that will be copied.
         */
        NotEnoughMemoryException& operator=(const NotEnoughMemoryException &ex);

        /*!
         * @brief Assigment operation.
         *
         * @param ex NotEnoughMemoryException that will be moved.
         */
        NotEnoughMemoryException& operator=(NotEnoughMemoryException&& ex);

        //! @brief Default constructor
        virtual ~NotEnoughMemoryException() throw();

        //! @brief This function throws the object as exception.
        virtual void raise() const;

        //! @brief Default message used in the library.
        static const std::string NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT;
    };
} // namespace eprosima
#endif // _CDR_EXCEPTIONS_NOTENOUGHMEMORYEXCEPTION_H_
