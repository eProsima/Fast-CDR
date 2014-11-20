/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This copy of FastCdr is licensed to you under the terms described in the
 * FASTCDR_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

#ifndef _FASTCDR_EXCEPTIONS_EXCEPTION_H_
#define _FASTCDR_EXCEPTIONS_EXCEPTION_H_

#include "fastcdr/FastCdr_dll.h"
#include <string>
#include <exception>

namespace eprosima
{
    namespace fastcdr
    {
        namespace exception
        {
            /*!
             * @brief This abstract class is used to create exceptions.
             * @ingroup EXCEPTIONMODULE
             */
            class Exception : public std::exception
            {
                public:

                    //! \brief Default destructor.
                    virtual Cdr_DllAPI ~Exception() throw();

                    //! \brief This function throws the object as exception.
                    virtual Cdr_DllAPI void raise() const = 0;

                    /*!
                     * @brief This function returns the error message.
                     *
                     * @return The error message.
                     */
                    virtual Cdr_DllAPI const char* what() const throw() ;

                protected:

                    /*!
                     * @brief Default constructor.
                     *
                     * @param message A error message. This message is copied.
                     */
                    Cdr_DllAPI Exception(const char* const &message);

                    /*!
                     * @brief Default copy constructor.
                     *
                     * @param ex Exception that will be copied.
                     */
                    Cdr_DllAPI Exception(const Exception &ex);

                    /*!
                     * @brief Default move constructor.
                     *
                     * @param ex Exception that will be moved.
                     */
                    Cdr_DllAPI Exception(Exception&& ex);

                    /*!
                     * @brief Assigment operation.
                     *
                     * @param ex Exception that will be copied.
                     */
                    Cdr_DllAPI Exception& operator=(const Exception &ex);

                    /*!
                     * @brief Assigment operation.
                     *
                     * @param ex Exception that will be moved.
                     */
                    Cdr_DllAPI Exception& operator=(Exception&&);

                private:

                    std::string m_message;
            };
        } //namespace exception
    } //namespace fastcdr
} //namespace eprosima

#endif // _FASTCDR_EXCEPTIONS_EXCEPTION_H_
