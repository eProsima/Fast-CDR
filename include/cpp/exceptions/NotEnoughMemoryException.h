/*************************************************************************
 * Copyright (c) 2012 eProsima. All rights reserved.
 *
 * This copy of RPCDDS is licensed to you under the terms described in the
 * RPCDDS_LICENSE file included in this distribution.
 *
 *************************************************************************/

#ifndef _CPP_EXCEPTIONS_NOTENOUGHMEMORYEXCEPTION_H_
#define _CPP_EXCEPTIONS_NOTENOUGHMEMORYEXCEPTION_H_

#include <cpp/exceptions/Exception.h>

namespace eProsima
{
	/**
		* @brief This class is thrown as an exception when there is an error initializating an object.
        * @ingroup EXCEPTIONMODULE
		*/
    class eProsima_cpp_DllVariable NotEnoughMemoryException : public Exception
    {
	public:

		/**
			* \brief Default constructor.
			*
			* \param message A error message. This message is copied.
			*/
		NotEnoughMemoryException(const std::string &message);

		/**
			* \brief Default constructor.
			*
			* \param message A error message. This message is moved.
			*/
		NotEnoughMemoryException(std::string&& message);

		/**
			* \brief Default copy constructor.
			*
			* \param ex InitializeException that will be copied.
			*/
		NotEnoughMemoryException(const NotEnoughMemoryException &ex);

		/**
			* \brief Default move constructor.
			*
			* \param ex InitializeException that will be moved.
			*/
		NotEnoughMemoryException(NotEnoughMemoryException&& ex);

		/**
			* \brief Assigment operation.
			*
			* \param ex InitializeException that will be copied.
			*/
		NotEnoughMemoryException& operator=(const NotEnoughMemoryException &ex);

		/**
			* \brief Assigment operation.
			*
			* \param ex InitializeException that will be moved.
			*/
		NotEnoughMemoryException& operator=(NotEnoughMemoryException&& ex);

		/// \brief Default constructor
		virtual ~NotEnoughMemoryException() EPROSIMA_USE_NOEXCEPT;

		/// \brief This function throws the object as exception.
		virtual void raise() const;
    };
} // namespace eProsima
#endif // _CPP_EXCEPTIONS_NOTENOUGHMEMORYEXCEPTION_H_
