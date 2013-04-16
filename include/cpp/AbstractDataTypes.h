#ifndef _CPP_ABSTRACTDATATYPES_H_
#define _CPP_ABSTRACTDATATYPES_H_

//! \brief This type definition is used to encapsulate the user's type definition for strings.
typedef struct User_CString User_CString;

/*!
 * @brief Function pointer used to call a user's definition function that
 * allocates dynamically the internal memory of the user's string.
 * @param userString This parameter contains the pointer to the user's string. Cannot be NULL.
 * @param newSize The new size that has to be allocated in the internal memory of the user's string.
 * @return 0 value has to be returned if the function works successful. In other case -1 value has to be returned.
*/
typedef int (*User_CString_FuncAllocator)(User_CString *userString, size_t newSize);

/*!
 * @brief Function pointer used to call a user's definition function that
 * return the internal memory of the user's string.
 * @param userString This parameter contains the pointer to the user's string. Cannot be NULL.
 * @return Pointer to the internal memory where it is stored the string.
*/
typedef char* (*User_CString_FuncGetData)(User_CString *userString);


#endif // _CPP_ABSTRACTDATATYPES_H_