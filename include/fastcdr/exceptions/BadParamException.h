/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This copy of FastCdr is licensed to you under the terms described in the
 * FASTCDR_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

#ifndef _FASTCDR_EXCEPTIONS_BADPARAMEXCEPTION_H_
#define _FASTCDR_EXCEPTIONS_BADPARAMEXCEPTION_H_

#include <fastcdr/exceptions/Exception.h>

namespace eprosima
{
    namespace fastcdr
    {
        namespace exception
        {
            /*!
             * @brief This class is thrown as an exception when a invalid parameter was being serialized.
             * @ingroup EXCEPTIONMODULE
             */
            class BadParamException : public Exception
            {
                public:

                    /*!
                     * @brief Default constructor.
                     *
                     * @param message A error message. This message is copied.
                     */
                    Cdr_DllAPI BadParamException(const char* const &message);

                    /*!
                     * @brief Default copy constructor.
                     *
                     * @param ex BadParamException that will be copied.
                     */
                    Cdr_DllAPI BadParamException(const BadParamException &ex);

                    /*!
                     * @brief Default move constructor.
                     *
                     * @param ex BadParamException that will be moved.
                     */
                    Cdr_DllAPI BadParamException(BadParamException&& ex);

                    /*!
                     * @brief Assigment operation.
                     *
                     * @param ex BadParamException that will be copied.
                     */
                    Cdr_DllAPI BadParamException& operator=(const BadParamException &ex);

                    /*!
                     * @brief Assigment operation.
                     *
                     * @param ex BadParamException that will be moved.
                     */
                    BadParamException& operator=(BadParamException&& ex);

                    //! @brief Default constructor
                    virtual Cdr_DllAPI ~BadParamException() throw();

                    //! @brief This function throws the object as exception.
                    virtual Cdr_DllAPI void raise() const;

                    //! @brief Default message used in the library.
                    static Cdr_DllAPI const char* const BAD_PARAM_MESSAGE_DEFAULT;
            };
        } //namespace exception
    } //namespace fastcdr
} //namespace eprosima
#endif // _FASTCDR_EXCEPTIONS_BADPARAMEXCEPTION_H_
