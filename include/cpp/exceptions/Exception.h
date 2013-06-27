#ifndef _CPP_EXCEPTIONS_EXCEPTION_H_
#define _CPP_EXCEPTIONS_EXCEPTION_H_

#include "cpp/Cdr_dll.h"
#include <string>
#include <exception>

namespace eProsima
{
    /*!
    * @brief This abstract class is used to create exceptions.
    * @ingroup EXCEPTIONMODULE
    */
    class Cdr_DllAPI Exception : public std::exception
    {
    public:

        //! \brief Default destructor.
        virtual ~Exception() EPROSIMA_USE_NOEXCEPT;

        //! \brief This function throws the object as exception.
        virtual void raise() const = 0;

        /*!
         * @brief This function returns the error message.
         *
         * @return The error message.
         */
        virtual const char* what() const throw() override;

    protected:

        /*!
         * @brief Default constructor.
         *
         * @param message A error message. This message is copied.
         */
        Exception(const std::string &message);

        /*!
         * @brief Default constructor.
         *
         * @param message A error message. This message is moved.
         */
        Exception(std::string&& message);

        /*!
         * @brief Default copy constructor.
         *
         * @param ex Exception that will be copied.
         */
        Exception(const Exception &ex);

        /*!
         * @brief Default move constructor.
         *
         * @param ex Exception that will be moved.
         */
        Exception(Exception&& ex);

        /*!
         * @brief Assigment operation.
         *
         * @param ex Exception that will be copied.
         */
        Exception& operator=(const Exception &ex);

        /*!
         * @brief Assigment operation.
         *
         * @param ex Exception that will be moved.
         */
        Exception& operator=(Exception&&);

    private:

        std::string m_message;
    };
} // namespace eProsima

#endif // _CPP_EXCEPTIONS_EXCEPTION_H_
