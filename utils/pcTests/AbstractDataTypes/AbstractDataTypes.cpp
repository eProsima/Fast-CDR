#include "cpp/CDR.h"

#include <stdio.h>
#include <stdint.h>
#include <limits>
#include <string>
#include <array>
#include <vector>

using namespace eProsima;

#define BUFFER_LENGTH 2000

const char *emptyStringSer = "";
const char *staticStringSer = "Hola a todos, esto es un test";

char* stringGetData(User_CString *st)
{
    char **c = (char**)st;
    return *c;
}

int stringAlloc(User_CString *st, size_t newSize)
{
    char **c = (char**)st;

    if(*c != NULL)
    {
        *c = (char*)realloc(*c, newSize);
    }
    else
    {
        *c = (char*)malloc(newSize);
    }

    return 0;
}

int main()
{
    bool returnedValue = true;
    char buffer[BUFFER_LENGTH];

    // Serialization.
    CDRBuffer cdrbuffer_ser(buffer, BUFFER_LENGTH);
    CDR cdr_ser(cdrbuffer_ser);

    returnedValue &= cdr_ser.serialize((User_CString*)&emptyStringSer, 0, stringGetData) == 0 ? true : false;
    returnedValue &= cdr_ser.serialize((User_CString*)&staticStringSer, strlen(staticStringSer) + 1, stringGetData) == 0 ? true : false;
    returnedValue &= cdr_ser.serialize((User_CString*)&staticStringSer, strlen(staticStringSer) + 1, stringGetData) == 0 ? true : false;
    returnedValue &= cdr_ser.serialize((User_CString*)&staticStringSer, strlen(staticStringSer), stringGetData) == 0 ? true : false;


        // Deseriazliation.
    CDRBuffer cdrbuffer_des(buffer, BUFFER_LENGTH);
    CDR cdr_des(cdrbuffer_des);

    char *emptyStringDes = NULL;
    char *staticStringDes = (char*)malloc(strlen(staticStringSer) + 1);
    char *dynamicStringDes = (char*)malloc(1);;
    char *notEndStringDes = (char*)malloc(strlen(staticStringSer) + 1);

    returnedValue &= cdr_des.deserialize((User_CString*)&emptyStringDes, 0, stringGetData, stringAlloc) == 0 ? true : false;
    returnedValue &= cdr_des.deserialize((User_CString*)&staticStringDes, strlen(staticStringSer) + 1, stringGetData, stringAlloc) == 0 ? true : false;
    returnedValue &= cdr_des.deserialize((User_CString*)&dynamicStringDes, 0, stringGetData, stringAlloc) == 0 ? true : false;
    returnedValue &= cdr_des.deserialize((User_CString*)&notEndStringDes, strlen(staticStringSer) + 1, stringGetData, stringAlloc) == 0 ? true : false;

    returnedValue &= strncmp(emptyStringSer, emptyStringDes, strlen(emptyStringSer) + 1) == 0 ? true : false;
    returnedValue &= strncmp(staticStringSer, staticStringSer, strlen(staticStringSer) + 1) == 0 ? true : false;
    returnedValue &= strncmp(staticStringSer, dynamicStringDes, strlen(staticStringSer) + 1) == 0 ? true : false;
    returnedValue &= strncmp(staticStringSer, notEndStringDes, strlen(staticStringSer) + 1) == 0 ? true : false;

    return returnedValue;
}