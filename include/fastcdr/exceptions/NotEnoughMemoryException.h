/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This copy of FastCdr is licensed to you under the terms described in the
 * FASTCDR_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

#ifndef _FASTCDR_EXCEPTIONS_NOTENOUGHMEMORYEXCEPTION_H_
#define _FASTCDR_EXCEPTIONS_NOTENOUGHMEMORYEXCEPTION_H_

#include <fastcdr/exceptions/Exception.h>

namespace eprosima
{
    namespace fastcdr
    {
        namespace exception
        {
            /*!
             * @brief This class is thrown as an exception when the buffer's internal memory reachs its size limit.
             * @ingroup EXCEPTIONMODULE
             */
            class NotEnoughMemoryException : public Exception
            {
                public:

                    /*!
                     * @brief Default constructor.
                     *
                     * @param message A error message. This message is copied.
                     */
                    Cdr_DllAPI NotEnoughMemoryException(const char* const &message);

                    /*!
                     * @brief Default copy constructor.
                     *
                     * @param ex NotEnoughMemoryException that will be copied.
                     */
                    Cdr_DllAPI NotEnoughMemoryException(const NotEnoughMemoryException &ex);

                    /*!
                     * @brief Default move constructor.
                     *
                     * @param ex NotEnoughMemoryException that will be moved.
                     */
                    Cdr_DllAPI NotEnoughMemoryException(NotEnoughMemoryException&& ex);

                    /*!
                     * @brief Assigment operation.
                     *
                     * @param ex NotEnoughMemoryException that will be copied.
                     */
                    Cdr_DllAPI NotEnoughMemoryException& operator=(const NotEnoughMemoryException &ex);

                    /*!
                     * @brief Assigment operation.
                     *
                     * @param ex NotEnoughMemoryException that will be moved.
                     */
                    Cdr_DllAPI NotEnoughMemoryException& operator=(NotEnoughMemoryException&& ex);

                    //! @brief Default constructor
                    virtual Cdr_DllAPI ~NotEnoughMemoryException() throw();

                    //! @brief This function throws the object as exception.
                    virtual Cdr_DllAPI void raise() const;

                    //! @brief Default message used in the library.
                    static Cdr_DllAPI const char* const NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT;
            };
        } //namespace exception
    } //namespace fastcdr
} //namespace eprosima
#endif // _FASTCDR_EXCEPTIONS_NOTENOUGHMEMORYEXCEPTION_H_
