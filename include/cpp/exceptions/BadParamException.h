/*************************************************************************
 * Copyright (c) 2012 eProsima. All rights reserved.
 *
 * This copy of RPCDDS is licensed to you under the terms described in the
 * RPCDDS_LICENSE file included in this distribution.
 *
 *************************************************************************/

#ifndef _CPP_EXCEPTIONS_BADPARAMEXCEPTION_H_
#define _CPP_EXCEPTIONS_BADPARAMEXCEPTION_H_

#include <cpp/exceptions/Exception.h>

namespace eProsima
{
	/**
		* @brief This class is thrown as an exception when a invalid parameter was being serialized.
        * @ingroup EXCEPTIONMODULE
		*/
    class eProsima_cpp_DllVariable BadParamException : public Exception
    {
	public:

		/**
			* \brief Default constructor.
			*
			* \param message A error message. This message is copied.
			*/
		BadParamException(const std::string &message);

		/**
			* \brief Default constructor.
			*
			* \param message A error message. This message is moved.
			*/
		BadParamException(std::string&& message);

		/**
			* \brief Default copy constructor.
			*
			* \param ex BadParamException that will be copied.
			*/
		BadParamException(const BadParamException &ex);

		/**
			* \brief Default move constructor.
			*
			* \param ex BadParamException that will be moved.
			*/
		BadParamException(BadParamException&& ex);

		/**
			* \brief Assigment operation.
			*
			* \param ex BadParamException that will be copied.
			*/
		BadParamException& operator=(const BadParamException &ex);

		/**
			* \brief Assigment operation.
			*
			* \param ex BadParamException that will be moved.
			*/
		BadParamException& operator=(BadParamException&& ex);

		/// \brief Default constructor
		virtual ~BadParamException() EPROSIMA_USE_NOEXCEPT;

		/// \brief This function throws the object as exception.
		virtual void raise() const;
    };
} // namespace eProsima
#endif // _CPP_EXCEPTIONS_BADPARAMEXCEPTION_H_
