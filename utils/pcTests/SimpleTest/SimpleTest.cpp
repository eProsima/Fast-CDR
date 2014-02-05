#include "fastcdr/Cdr.h"
#include "fastcdr/FastCdr.h"
#include "fastcdr/exceptions/Exception.h"

#include <stdio.h>
#include <limits>
#include <iostream>

using namespace eprosima;

#define BUFFER_LENGTH 2000

const uint8_t octet_t = 32;
const char char_t =  'Z';
const uint16_t ushort_t = 65500;
const int16_t short_t = -32700;
const uint32_t ulong_t = 4294967200;
const int32_t long_t = -2147483600;
const uint64_t ulonglong_t = 18446744073709551600u;
const int64_t longlong_t = -9223372036800;
const float float_t = std::numeric_limits<float>::min();
const double double_t = std::numeric_limits<double>::min();
const bool bool_t = true;
const std::string string_t = "Hola a todos, esto es un test";
const std::array<uint8_t, 5> octet_array_t = {{1, 2, 3, 4, 5}};
const uint8_t octet_array_2_t[5] = {5, 4, 3, 2, 1};
const std::array<char, 5> char_array_t = {{'A', 'B', 'C', 'D', 'E'}};
const char char_array_2_t[5] = {'E', 'D', 'C', 'B', 'A'};
const std::array<uint16_t, 5> ushort_array_t = {{65500, 65501, 65502, 65503, 65504}};
const uint16_t ushort_array_2_t[5] = {65504, 65503, 65502, 65501, 65500};
const std::array<int16_t, 5> short_array_t = {{-32700, -32701, -32702, -32703, -32704}};
const int16_t short_array_2_t[5] = {-32704, -32703, -32702, -32701, -32700};
const std::array<uint32_t, 5> ulong_array_t = {{4294967200, 4294967201, 4294967202, 4294967203, 4294967204}};
const uint32_t ulong_array_2_t[5] = {4294967204, 4294967203, 4294967202, 4294967201, 4294967200};
const std::array<int32_t, 5> long_array_t = {{-2147483600, -2147483601, -2147483602, -2147483603, -2147483604}};
const int32_t long_array_2_t[5] = {-2147483604, -2147483603, -2147483602, -2147483601, -2147483600};
const std::array<uint64_t, 5> ulonglong_array_t = {{18446744073709551600u, 18446744073709551601u, 18446744073709551602u, 18446744073709551603u, 18446744073709551604u}};
const uint64_t ulonglong_array_2_t[5] = {18446744073709551604u, 18446744073709551603u, 18446744073709551602u, 18446744073709551601u, 18446744073709551600u};
const std::array<int64_t, 5> longlong_array_t = {{-9223372036800, -9223372036801, -9223372036802, -9223372036803, -9223372036804}};
const int64_t longlong_array_2_t[5] = {-9223372036804, -9223372036803, -9223372036802, -9223372036801, -9223372036800};
const std::array<float, 5> float_array_t = {{float_t, float_t + 1, float_t + 2, float_t + 3, float_t + 4}};
const float float_array_2_t[5] = {float_t + 4, float_t + 3, float_t + 2, float_t + 1, float_t};
const std::array<double, 5> double_array_t = {{double_t, double_t + 1, double_t + 2, double_t + 3, double_t + 4}};
const double double_array_2_t[5] = {double_t + 4, double_t + 3, double_t + 2, double_t + 1, double_t};
const std::vector<uint8_t> octet_vector_t(octet_array_2_t, octet_array_2_t + sizeof(octet_array_2_t) / sizeof(uint8_t));
const std::vector<char> char_vector_t(char_array_2_t, char_array_2_t + sizeof(char_array_2_t) / sizeof(char));
const std::vector<uint16_t> ushort_vector_t(ushort_array_2_t, ushort_array_2_t + sizeof(ushort_array_2_t) / sizeof(uint16_t));
const std::vector<int16_t> short_vector_t(short_array_2_t, short_array_2_t + sizeof(short_array_2_t) / sizeof(int16_t));
const std::vector<uint32_t> ulong_vector_t(ulong_array_2_t, ulong_array_2_t + sizeof(ulong_array_2_t) / sizeof(uint32_t));
const std::vector<int32_t> long_vector_t(long_array_2_t, long_array_2_t + sizeof(long_array_2_t) / sizeof(int32_t));
const std::vector<uint64_t> ulonglong_vector_t(ulonglong_array_2_t, ulonglong_array_2_t + sizeof(ulonglong_array_2_t) / sizeof(uint64_t));
const std::vector<int64_t> longlong_vector_t(longlong_array_2_t, longlong_array_2_t + sizeof(longlong_array_2_t) / sizeof(int64_t));
const std::vector<float> float_vector_t(float_array_2_t, float_array_2_t + sizeof(float_array_2_t) / sizeof(float));
const std::vector<double> double_vector_t(double_array_2_t, double_array_2_t + sizeof(double_array_2_t) / sizeof(double));
const std::array<std::array<std::array<uint32_t, 3>, 2>, 2> triple_ulong_array_t = {{ {{ {{1, 2, 3}}, {{4, 5, 6}} }}, {{ {{7, 8, 9}}, {{10, 11, 12}} }} }};
// Added because error 336.
const uint8_t octet_seq_t[5] = {5, 4, 3, 2, 1};
const char char_seq_t[5] = {'E', 'D', 'C', 'B', 'A'};
const uint16_t ushort_seq_t[5] = {65504, 65503, 65502, 65501, 65500};
const int16_t short_seq_t[5] = {-32704, -32703, -32702, -32701, -32700};
const uint32_t ulong_seq_t[5] = {4294967204, 4294967203, 4294967202, 4294967201, 4294967200};
const int32_t long_seq_t[5] = {-2147483604, -2147483603, -2147483602, -2147483601, -2147483600};
const uint64_t ulonglong_seq_t[5] = {18446744073709551604u, 18446744073709551603u, 18446744073709551602u, 18446744073709551601u, 18446744073709551600u};
const int64_t longlong_seq_t[5] = {-9223372036804, -9223372036803, -9223372036802, -9223372036801, -9223372036800};
const float float_seq_t[5] = {float_t + 4, float_t + 3, float_t + 2, float_t + 1, float_t};
const double double_seq_t[5] = {double_t + 4, double_t + 3, double_t + 2, double_t + 1, double_t};
// Added because error 337
const char *c_string_t = "HOLA";

bool CDRTests()
{
    bool returnedValue = true;
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    Cdr cdr_ser(cdrbuffer);

    try
    {
        cdr_ser << octet_t << char_t << ushort_t << short_t << ulong_t << long_t << ulonglong_t << longlong_t << float_t << double_t << bool_t << string_t;
        cdr_ser << octet_array_t << char_array_t << ushort_array_t << short_array_t << ulong_array_t << long_array_t << ulonglong_array_t << longlong_array_t << float_array_t << double_array_t;
        cdr_ser.serializeArray(octet_array_2_t, 5).serializeArray(char_array_2_t, 5).serializeArray(ushort_array_2_t, 5).serializeArray(short_array_2_t, 5).serializeArray(ulong_array_2_t, 5);
        cdr_ser.serializeArray(long_array_2_t, 5).serializeArray(ulonglong_array_2_t, 5).serializeArray(longlong_array_2_t, 5).serializeArray(float_array_2_t, 5).serializeArray(double_array_2_t, 5);
        cdr_ser << octet_vector_t << char_vector_t << ushort_vector_t << short_vector_t << ulong_vector_t << long_vector_t << ulonglong_vector_t << longlong_vector_t << float_vector_t << double_vector_t;
        cdr_ser << triple_ulong_array_t;
        cdr_ser.serializeSequence(octet_seq_t, 5).serializeSequence(char_seq_t, 5).serializeSequence(ushort_seq_t, 5).serializeSequence(short_seq_t, 5);
        cdr_ser.serializeSequence(ulong_seq_t, 5).serializeSequence(long_seq_t, 5).serializeSequence(ulonglong_seq_t, 5).serializeSequence(longlong_seq_t, 5);
        cdr_ser.serializeSequence(float_seq_t, 5).serializeSequence(double_seq_t, 5);
        cdr_ser.serialize(c_string_t);
    }
    catch(Exception &ex)
    {
        std::cout << "Serialization error: " << ex.what() << std::endl << "TEST FAILED" << std::endl;
        return false;
    }

    // Deseriazliation.
    Cdr cdr_des(cdrbuffer);

    uint8_t octet_value = 0;
    char char_value = 0;
    uint16_t ushort_value =0;
    int16_t short_value = 0;
    uint32_t ulong_value = 0;
    int32_t long_value = 0;
    uint64_t ulonglong_value = 0;
    int64_t longlong_value = 0;
    float float_value = 0;
    double double_value = 0;
    bool bool_value = false;
    std::string string_value = "";
    std::array<uint8_t, 5> octet_array_value;
    uint8_t octet_array_2_value[5];
    std::array<char, 5> char_array_value;
    char char_array_2_value[5];
    std::array<uint16_t, 5> ushort_array_value;
    uint16_t ushort_array_2_value[5];
    std::array<int16_t, 5> short_array_value;
    int16_t short_array_2_value[5];
    std::array<uint32_t, 5> ulong_array_value;
    uint32_t ulong_array_2_value[5];
    std::array<int32_t, 5> long_array_value;
    int32_t long_array_2_value[5];
    std::array<uint64_t, 5> ulonglong_array_value;
    uint64_t ulonglong_array_2_value[5];
    std::array<int64_t, 5> longlong_array_value;
    int64_t longlong_array_2_value[5];
    std::array<float, 5> float_array_value;
    float float_array_2_value[5];
    std::array<double, 5> double_array_value;
    double double_array_2_value[5];
    std::vector<uint8_t> octet_vector_value;
    std::vector<char> char_vector_value;
    std::vector<uint16_t> ushort_vector_value;
    std::vector<int16_t> short_vector_value;
    std::vector<uint32_t> ulong_vector_value;
    std::vector<int32_t> long_vector_value;
    std::vector<uint64_t> ulonglong_vector_value;
    std::vector<int64_t> longlong_vector_value;
    std::vector<float> float_vector_value;
    std::vector<double> double_vector_value;
    std::array<std::array<std::array<uint32_t, 3>, 2>, 2> triple_ulong_array_value;
    uint8_t *octet_seq_value = NULL; size_t octet_seq_len;
    char *char_seq_value = NULL; size_t char_seq_len;
    uint16_t *ushort_seq_value = NULL; size_t ushort_seq_len;
    int16_t *short_seq_value = NULL; size_t short_seq_len;
    uint32_t *ulong_seq_value = NULL; size_t ulong_seq_len;
    int32_t *long_seq_value = NULL; size_t long_seq_len;
    uint64_t *ulonglong_seq_value = NULL; size_t ulonglong_seq_len;
    int64_t *longlong_seq_value = NULL; size_t longlong_seq_len;
    float *float_seq_value = NULL; size_t float_seq_len;
    double *double_seq_value = NULL; size_t double_seq_len;
    char *c_string_value = NULL;

    try
    {
        cdr_des >> octet_value >> char_value >> ushort_value >> short_value >> ulong_value >> long_value >> ulonglong_value >> longlong_value >> float_value >> double_value >> bool_value >> string_value;
        cdr_des >> octet_array_value >> char_array_value >> ushort_array_value >> short_array_value >> ulong_array_value >> long_array_value >> ulonglong_array_value >> longlong_array_value >> float_array_value >> double_array_value;
        cdr_des.deserializeArray(octet_array_2_value, 5).deserializeArray(char_array_2_value, 5).deserializeArray(ushort_array_2_value, 5).deserializeArray(short_array_2_value, 5).deserializeArray(ulong_array_2_value, 5);
        cdr_des.deserializeArray(long_array_2_value, 5).deserializeArray(ulonglong_array_2_value, 5).deserializeArray(longlong_array_2_value, 5).deserializeArray(float_array_2_value, 5).deserializeArray(double_array_2_value, 5);
        cdr_des >> octet_vector_value >> char_vector_value >> ushort_vector_value >> short_vector_value >> ulong_vector_value >> long_vector_value >> ulonglong_vector_value >> longlong_vector_value >> float_vector_value >> double_vector_value;
        cdr_des >> triple_ulong_array_value;
        cdr_des.deserializeSequence(octet_seq_value, octet_seq_len).deserializeSequence(char_seq_value, char_seq_len).deserializeSequence(ushort_seq_value, ushort_seq_len); 
        cdr_des.deserializeSequence(short_seq_value, short_seq_len).deserializeSequence(ulong_seq_value, ulong_seq_len).deserializeSequence(long_seq_value, long_seq_len); 
        cdr_des.deserializeSequence(ulonglong_seq_value, ulonglong_seq_len).deserializeSequence(longlong_seq_value, longlong_seq_len).deserializeSequence(float_seq_value, float_seq_len); 
        cdr_des.deserializeSequence(double_seq_value, double_seq_len);
        cdr_des.deserialize(c_string_value);
    }
    catch(Exception &ex)
    {
        std::cout << "Deserialization error: " << ex.what() << std::endl << "TEST FAILED" << std::endl;
        return false;
    }

    returnedValue &= octet_value == octet_t;
    returnedValue &= char_value == char_t;
    returnedValue &= ushort_value == ushort_t;
    returnedValue &= short_value == short_t;
    returnedValue &= ulong_value == ulong_t;
    returnedValue &= long_value == long_t;
    returnedValue &= ulonglong_value == ulonglong_t;
    returnedValue &= longlong_value == longlong_t;
    returnedValue &= float_value == float_t;
    returnedValue &= double_value == double_t;
    returnedValue &= bool_value == bool_t;
    returnedValue &= string_value == string_t;

    returnedValue &= octet_array_value == octet_array_t;
    returnedValue &= char_array_value == char_array_t;
    returnedValue &= ushort_array_value == ushort_array_t;
    returnedValue &= short_array_value == short_array_t;
    returnedValue &= ulong_array_value == ulong_array_t;
    returnedValue &= long_array_value == long_array_t;
    returnedValue &= ulonglong_array_value == ulonglong_array_t;
    returnedValue &= longlong_array_value == longlong_array_t;
    returnedValue &= float_array_value == float_array_t;
    returnedValue &= double_array_value == double_array_t;

    returnedValue &= octet_array_2_value[0] == octet_array_2_t[0];
    returnedValue &= octet_array_2_value[1] == octet_array_2_t[1];
    returnedValue &= octet_array_2_value[2] == octet_array_2_t[2];
    returnedValue &= octet_array_2_value[3] == octet_array_2_t[3];
    returnedValue &= octet_array_2_value[4] == octet_array_2_t[4];
    returnedValue &= char_array_2_value[0] == char_array_2_t[0];
    returnedValue &= char_array_2_value[1] == char_array_2_t[1];
    returnedValue &= char_array_2_value[2] == char_array_2_t[2];
    returnedValue &= char_array_2_value[3] == char_array_2_t[3];
    returnedValue &= char_array_2_value[4] == char_array_2_t[4];
    returnedValue &= ushort_array_2_value[0] == ushort_array_2_t[0];
    returnedValue &= ushort_array_2_value[1] == ushort_array_2_t[1];
    returnedValue &= ushort_array_2_value[2] == ushort_array_2_t[2];
    returnedValue &= ushort_array_2_value[3] == ushort_array_2_t[3];
    returnedValue &= ushort_array_2_value[4] == ushort_array_2_t[4];
    returnedValue &= short_array_2_value[0] == short_array_2_t[0];
    returnedValue &= short_array_2_value[1] == short_array_2_t[1];
    returnedValue &= short_array_2_value[2] == short_array_2_t[2];
    returnedValue &= short_array_2_value[3] == short_array_2_t[3];
    returnedValue &= short_array_2_value[4] == short_array_2_t[4];
    returnedValue &= ulong_array_2_value[0] == ulong_array_2_t[0];
    returnedValue &= ulong_array_2_value[1] == ulong_array_2_t[1];
    returnedValue &= ulong_array_2_value[2] == ulong_array_2_t[2];
    returnedValue &= ulong_array_2_value[3] == ulong_array_2_t[3];
    returnedValue &= ulong_array_2_value[4] == ulong_array_2_t[4];
    returnedValue &= long_array_2_value[0] == long_array_2_t[0];
    returnedValue &= long_array_2_value[1] == long_array_2_t[1];
    returnedValue &= long_array_2_value[2] == long_array_2_t[2];
    returnedValue &= long_array_2_value[3] == long_array_2_t[3];
    returnedValue &= long_array_2_value[4] == long_array_2_t[4];
    returnedValue &= ulonglong_array_2_value[0] == ulonglong_array_2_t[0];
    returnedValue &= ulonglong_array_2_value[1] == ulonglong_array_2_t[1];
    returnedValue &= ulonglong_array_2_value[2] == ulonglong_array_2_t[2];
    returnedValue &= ulonglong_array_2_value[3] == ulonglong_array_2_t[3];
    returnedValue &= ulonglong_array_2_value[4] == ulonglong_array_2_t[4];
    returnedValue &= longlong_array_2_value[0] == longlong_array_2_t[0];
    returnedValue &= longlong_array_2_value[1] == longlong_array_2_t[1];
    returnedValue &= longlong_array_2_value[2] == longlong_array_2_t[2];
    returnedValue &= longlong_array_2_value[3] == longlong_array_2_t[3];
    returnedValue &= longlong_array_2_value[4] == longlong_array_2_t[4];
    returnedValue &= float_array_2_value[0] == float_array_2_t[0];
    returnedValue &= float_array_2_value[1] == float_array_2_t[1];
    returnedValue &= float_array_2_value[2] == float_array_2_t[2];
    returnedValue &= float_array_2_value[3] == float_array_2_t[3];
    returnedValue &= float_array_2_value[4] == float_array_2_t[4];
    returnedValue &= double_array_2_value[0] == double_array_2_t[0];
    returnedValue &= double_array_2_value[1] == double_array_2_t[1];
    returnedValue &= double_array_2_value[2] == double_array_2_t[2];
    returnedValue &= double_array_2_value[3] == double_array_2_t[3];
    returnedValue &= double_array_2_value[4] == double_array_2_t[4];

    returnedValue &= octet_vector_value == octet_vector_t;
    returnedValue &= char_vector_value == char_vector_t;
    returnedValue &= ushort_vector_value == ushort_vector_t;
    returnedValue &= short_vector_value == short_vector_t;
    returnedValue &= ulong_vector_value == ulong_vector_t;
    returnedValue &= long_vector_value == long_vector_t;
    returnedValue &= ulonglong_vector_value == ulonglong_vector_t;
    returnedValue &= longlong_vector_value == longlong_vector_t;
    returnedValue &= float_vector_value == float_vector_t;
    returnedValue &= double_vector_value == double_vector_t;

    returnedValue &= triple_ulong_array_t == triple_ulong_array_value;
    
    returnedValue &= octet_seq_len == 5;
    if(octet_seq_value != NULL)
    {
        returnedValue &= octet_seq_value[0] == octet_seq_t[0];
        returnedValue &= octet_seq_value[1] == octet_seq_t[1];
        returnedValue &= octet_seq_value[2] == octet_seq_t[2];
        returnedValue &= octet_seq_value[3] == octet_seq_t[3];
        returnedValue &= octet_seq_value[4] == octet_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= char_seq_len == 5;
    if(char_seq_value != NULL)
    {
        returnedValue &= char_seq_value[0] == char_seq_t[0];
        returnedValue &= char_seq_value[1] == char_seq_t[1];
        returnedValue &= char_seq_value[2] == char_seq_t[2];
        returnedValue &= char_seq_value[3] == char_seq_t[3];
        returnedValue &= char_seq_value[4] == char_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= ushort_seq_len == 5;
    if(ushort_seq_value != NULL)
    {
        returnedValue &= ushort_seq_value[0] == ushort_seq_t[0];
        returnedValue &= ushort_seq_value[1] == ushort_seq_t[1];
        returnedValue &= ushort_seq_value[2] == ushort_seq_t[2];
        returnedValue &= ushort_seq_value[3] == ushort_seq_t[3];
        returnedValue &= ushort_seq_value[4] == ushort_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= short_seq_len == 5;
    if(short_seq_value != NULL)
    {
        returnedValue &= short_seq_value[0] == short_seq_t[0];
        returnedValue &= short_seq_value[1] == short_seq_t[1];
        returnedValue &= short_seq_value[2] == short_seq_t[2];
        returnedValue &= short_seq_value[3] == short_seq_t[3];
        returnedValue &= short_seq_value[4] == short_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= ulong_seq_len == 5;
    if(ulong_seq_value != NULL)
    {
        returnedValue &= ulong_seq_value[0] == ulong_seq_t[0];
        returnedValue &= ulong_seq_value[1] == ulong_seq_t[1];
        returnedValue &= ulong_seq_value[2] == ulong_seq_t[2];
        returnedValue &= ulong_seq_value[3] == ulong_seq_t[3];
        returnedValue &= ulong_seq_value[4] == ulong_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= long_seq_len == 5;
    if(long_seq_value != NULL)
    {
        returnedValue &= long_seq_value[0] == long_seq_t[0];
        returnedValue &= long_seq_value[1] == long_seq_t[1];
        returnedValue &= long_seq_value[2] == long_seq_t[2];
        returnedValue &= long_seq_value[3] == long_seq_t[3];
        returnedValue &= long_seq_value[4] == long_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= ulonglong_seq_len == 5;
    if(ulonglong_seq_value != NULL)
    {
        returnedValue &= ulonglong_seq_value[0] == ulonglong_seq_t[0];
        returnedValue &= ulonglong_seq_value[1] == ulonglong_seq_t[1];
        returnedValue &= ulonglong_seq_value[2] == ulonglong_seq_t[2];
        returnedValue &= ulonglong_seq_value[3] == ulonglong_seq_t[3];
        returnedValue &= ulonglong_seq_value[4] == ulonglong_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= longlong_seq_len == 5;
    if(longlong_seq_value != NULL)
    {
        returnedValue &= longlong_seq_value[0] == longlong_seq_t[0];
        returnedValue &= longlong_seq_value[1] == longlong_seq_t[1];
        returnedValue &= longlong_seq_value[2] == longlong_seq_t[2];
        returnedValue &= longlong_seq_value[3] == longlong_seq_t[3];
        returnedValue &= longlong_seq_value[4] == longlong_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= float_seq_len == 5;
    if(float_seq_value != NULL)
    {
        returnedValue &= float_seq_value[0] == float_seq_t[0];
        returnedValue &= float_seq_value[1] == float_seq_t[1];
        returnedValue &= float_seq_value[2] == float_seq_t[2];
        returnedValue &= float_seq_value[3] == float_seq_t[3];
        returnedValue &= float_seq_value[4] == float_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= double_seq_len == 5;
    if(double_seq_value != NULL)
    {
        returnedValue &= double_seq_value[0] == double_seq_t[0];
        returnedValue &= double_seq_value[1] == double_seq_t[1];
        returnedValue &= double_seq_value[2] == double_seq_t[2];
        returnedValue &= double_seq_value[3] == double_seq_t[3];
        returnedValue &= double_seq_value[4] == double_seq_t[4];
    }
    else
        returnedValue = false;

    returnedValue &= (strcmp(c_string_t, c_string_value) == 0);

    return returnedValue;
}

bool FastCDRTests()
{
    bool returnedValue = true;
    char buffer[BUFFER_LENGTH];

    // Serialization.
    FastBuffer cdrbuffer(buffer, BUFFER_LENGTH);
    FastCdr cdr_ser(cdrbuffer);

    try
    {
        cdr_ser << octet_t << char_t << ushort_t << short_t << ulong_t << long_t << ulonglong_t << longlong_t << float_t << double_t << bool_t << string_t;
        cdr_ser << octet_array_t << char_array_t << ushort_array_t << short_array_t << ulong_array_t << long_array_t << ulonglong_array_t << longlong_array_t << float_array_t << double_array_t;
        cdr_ser.serializeArray(octet_array_2_t, 5).serializeArray(char_array_2_t, 5).serializeArray(ushort_array_2_t, 5).serializeArray(short_array_2_t, 5).serializeArray(ulong_array_2_t, 5);
        cdr_ser.serializeArray(long_array_2_t, 5).serializeArray(ulonglong_array_2_t, 5).serializeArray(longlong_array_2_t, 5).serializeArray(float_array_2_t, 5).serializeArray(double_array_2_t, 5);
        cdr_ser << octet_vector_t << char_vector_t << ushort_vector_t << short_vector_t << ulong_vector_t << long_vector_t << ulonglong_vector_t << longlong_vector_t << float_vector_t << double_vector_t;
        cdr_ser << triple_ulong_array_t;
        cdr_ser.serializeSequence(octet_seq_t, 5).serializeSequence(char_seq_t, 5).serializeSequence(ushort_seq_t, 5).serializeSequence(short_seq_t, 5);
        cdr_ser.serializeSequence(ulong_seq_t, 5).serializeSequence(long_seq_t, 5).serializeSequence(ulonglong_seq_t, 5).serializeSequence(longlong_seq_t, 5);
        cdr_ser.serializeSequence(float_seq_t, 5).serializeSequence(double_seq_t, 5);
        cdr_ser.serialize(c_string_t);
    }
    catch(Exception &ex)
    {
        std::cout << "Serialization error: " << ex.what() << std::endl << "TEST FAILED" << std::endl;
        return false;
    }

    // Deseriazliation.
    FastCdr cdr_des(cdrbuffer);

    uint8_t octet_value = 0;
    char char_value = 0;
    uint16_t ushort_value =0;
    int16_t short_value = 0;
    uint32_t ulong_value = 0;
    int32_t long_value = 0;
    uint64_t ulonglong_value = 0;
    int64_t longlong_value = 0;
    float float_value = 0;
    double double_value = 0;
    bool bool_value = false;
    std::string string_value = "";
    std::array<uint8_t, 5> octet_array_value;
    uint8_t octet_array_2_value[5];
    std::array<char, 5> char_array_value;
    char char_array_2_value[5];
    std::array<uint16_t, 5> ushort_array_value;
    uint16_t ushort_array_2_value[5];
    std::array<int16_t, 5> short_array_value;
    int16_t short_array_2_value[5];
    std::array<uint32_t, 5> ulong_array_value;
    uint32_t ulong_array_2_value[5];
    std::array<int32_t, 5> long_array_value;
    int32_t long_array_2_value[5];
    std::array<uint64_t, 5> ulonglong_array_value;
    uint64_t ulonglong_array_2_value[5];
    std::array<int64_t, 5> longlong_array_value;
    int64_t longlong_array_2_value[5];
    std::array<float, 5> float_array_value;
    float float_array_2_value[5];
    std::array<double, 5> double_array_value;
    double double_array_2_value[5];
    std::vector<uint8_t> octet_vector_value;
    std::vector<char> char_vector_value;
    std::vector<uint16_t> ushort_vector_value;
    std::vector<int16_t> short_vector_value;
    std::vector<uint32_t> ulong_vector_value;
    std::vector<int32_t> long_vector_value;
    std::vector<uint64_t> ulonglong_vector_value;
    std::vector<int64_t> longlong_vector_value;
    std::vector<float> float_vector_value;
    std::vector<double> double_vector_value;
    std::array<std::array<std::array<uint32_t, 3>, 2>, 2> triple_ulong_array_value;
    uint8_t *octet_seq_value = NULL; size_t octet_seq_len;
    char *char_seq_value = NULL; size_t char_seq_len;
    uint16_t *ushort_seq_value = NULL; size_t ushort_seq_len;
    int16_t *short_seq_value = NULL; size_t short_seq_len;
    uint32_t *ulong_seq_value = NULL; size_t ulong_seq_len;
    int32_t *long_seq_value = NULL; size_t long_seq_len;
    uint64_t *ulonglong_seq_value = NULL; size_t ulonglong_seq_len;
    int64_t *longlong_seq_value = NULL; size_t longlong_seq_len;
    float *float_seq_value = NULL; size_t float_seq_len;
    double *double_seq_value = NULL; size_t double_seq_len;
    char *c_string_value = NULL;

    try
    {
        cdr_des >> octet_value >> char_value >> ushort_value >> short_value >> ulong_value >> long_value >> ulonglong_value >> longlong_value >> float_value >> double_value >> bool_value >> string_value;
        cdr_des >> octet_array_value >> char_array_value >> ushort_array_value >> short_array_value >> ulong_array_value >> long_array_value >> ulonglong_array_value >> longlong_array_value >> float_array_value >> double_array_value;
        cdr_des.deserializeArray(octet_array_2_value, 5).deserializeArray(char_array_2_value, 5).deserializeArray(ushort_array_2_value, 5).deserializeArray(short_array_2_value, 5).deserializeArray(ulong_array_2_value, 5);
        cdr_des.deserializeArray(long_array_2_value, 5).deserializeArray(ulonglong_array_2_value, 5).deserializeArray(longlong_array_2_value, 5).deserializeArray(float_array_2_value, 5).deserializeArray(double_array_2_value, 5);
        cdr_des >> octet_vector_value >> char_vector_value >> ushort_vector_value >> short_vector_value >> ulong_vector_value >> long_vector_value >> ulonglong_vector_value >> longlong_vector_value >> float_vector_value >> double_vector_value;
        cdr_des >> triple_ulong_array_value;
        cdr_des.deserializeSequence(octet_seq_value, octet_seq_len).deserializeSequence(char_seq_value, char_seq_len).deserializeSequence(ushort_seq_value, ushort_seq_len); 
        cdr_des.deserializeSequence(short_seq_value, short_seq_len).deserializeSequence(ulong_seq_value, ulong_seq_len).deserializeSequence(long_seq_value, long_seq_len); 
        cdr_des.deserializeSequence(ulonglong_seq_value, ulonglong_seq_len).deserializeSequence(longlong_seq_value, longlong_seq_len).deserializeSequence(float_seq_value, float_seq_len); 
        cdr_des.deserializeSequence(double_seq_value, double_seq_len);
        cdr_des.deserialize(c_string_value);
    }
    catch(Exception &ex)
    {
        std::cout << "Deserialization error: " << ex.what() << std::endl << "TEST FAILED" << std::endl;
        return false;
    }

    returnedValue &= octet_value == octet_t;
    returnedValue &= char_value == char_t;
    returnedValue &= ushort_value == ushort_t;
    returnedValue &= short_value == short_t;
    returnedValue &= ulong_value == ulong_t;
    returnedValue &= long_value == long_t;
    returnedValue &= ulonglong_value == ulonglong_t;
    returnedValue &= longlong_value == longlong_t;
    returnedValue &= float_value == float_t;
    returnedValue &= double_value == double_t;
    returnedValue &= bool_value == bool_t;
    returnedValue &= string_value == string_t;

    returnedValue &= octet_array_value == octet_array_t;
    returnedValue &= char_array_value == char_array_t;
    returnedValue &= ushort_array_value == ushort_array_t;
    returnedValue &= short_array_value == short_array_t;
    returnedValue &= ulong_array_value == ulong_array_t;
    returnedValue &= long_array_value == long_array_t;
    returnedValue &= ulonglong_array_value == ulonglong_array_t;
    returnedValue &= longlong_array_value == longlong_array_t;
    returnedValue &= float_array_value == float_array_t;
    returnedValue &= double_array_value == double_array_t;

    returnedValue &= octet_array_2_value[0] == octet_array_2_t[0];
    returnedValue &= octet_array_2_value[1] == octet_array_2_t[1];
    returnedValue &= octet_array_2_value[2] == octet_array_2_t[2];
    returnedValue &= octet_array_2_value[3] == octet_array_2_t[3];
    returnedValue &= octet_array_2_value[4] == octet_array_2_t[4];
    returnedValue &= char_array_2_value[0] == char_array_2_t[0];
    returnedValue &= char_array_2_value[1] == char_array_2_t[1];
    returnedValue &= char_array_2_value[2] == char_array_2_t[2];
    returnedValue &= char_array_2_value[3] == char_array_2_t[3];
    returnedValue &= char_array_2_value[4] == char_array_2_t[4];
    returnedValue &= ushort_array_2_value[0] == ushort_array_2_t[0];
    returnedValue &= ushort_array_2_value[1] == ushort_array_2_t[1];
    returnedValue &= ushort_array_2_value[2] == ushort_array_2_t[2];
    returnedValue &= ushort_array_2_value[3] == ushort_array_2_t[3];
    returnedValue &= ushort_array_2_value[4] == ushort_array_2_t[4];
    returnedValue &= short_array_2_value[0] == short_array_2_t[0];
    returnedValue &= short_array_2_value[1] == short_array_2_t[1];
    returnedValue &= short_array_2_value[2] == short_array_2_t[2];
    returnedValue &= short_array_2_value[3] == short_array_2_t[3];
    returnedValue &= short_array_2_value[4] == short_array_2_t[4];
    returnedValue &= ulong_array_2_value[0] == ulong_array_2_t[0];
    returnedValue &= ulong_array_2_value[1] == ulong_array_2_t[1];
    returnedValue &= ulong_array_2_value[2] == ulong_array_2_t[2];
    returnedValue &= ulong_array_2_value[3] == ulong_array_2_t[3];
    returnedValue &= ulong_array_2_value[4] == ulong_array_2_t[4];
    returnedValue &= long_array_2_value[0] == long_array_2_t[0];
    returnedValue &= long_array_2_value[1] == long_array_2_t[1];
    returnedValue &= long_array_2_value[2] == long_array_2_t[2];
    returnedValue &= long_array_2_value[3] == long_array_2_t[3];
    returnedValue &= long_array_2_value[4] == long_array_2_t[4];
    returnedValue &= ulonglong_array_2_value[0] == ulonglong_array_2_t[0];
    returnedValue &= ulonglong_array_2_value[1] == ulonglong_array_2_t[1];
    returnedValue &= ulonglong_array_2_value[2] == ulonglong_array_2_t[2];
    returnedValue &= ulonglong_array_2_value[3] == ulonglong_array_2_t[3];
    returnedValue &= ulonglong_array_2_value[4] == ulonglong_array_2_t[4];
    returnedValue &= longlong_array_2_value[0] == longlong_array_2_t[0];
    returnedValue &= longlong_array_2_value[1] == longlong_array_2_t[1];
    returnedValue &= longlong_array_2_value[2] == longlong_array_2_t[2];
    returnedValue &= longlong_array_2_value[3] == longlong_array_2_t[3];
    returnedValue &= longlong_array_2_value[4] == longlong_array_2_t[4];
    returnedValue &= float_array_2_value[0] == float_array_2_t[0];
    returnedValue &= float_array_2_value[1] == float_array_2_t[1];
    returnedValue &= float_array_2_value[2] == float_array_2_t[2];
    returnedValue &= float_array_2_value[3] == float_array_2_t[3];
    returnedValue &= float_array_2_value[4] == float_array_2_t[4];
    returnedValue &= double_array_2_value[0] == double_array_2_t[0];
    returnedValue &= double_array_2_value[1] == double_array_2_t[1];
    returnedValue &= double_array_2_value[2] == double_array_2_t[2];
    returnedValue &= double_array_2_value[3] == double_array_2_t[3];
    returnedValue &= double_array_2_value[4] == double_array_2_t[4];

    returnedValue &= octet_vector_value == octet_vector_t;
    returnedValue &= char_vector_value == char_vector_t;
    returnedValue &= ushort_vector_value == ushort_vector_t;
    returnedValue &= short_vector_value == short_vector_t;
    returnedValue &= ulong_vector_value == ulong_vector_t;
    returnedValue &= long_vector_value == long_vector_t;
    returnedValue &= ulonglong_vector_value == ulonglong_vector_t;
    returnedValue &= longlong_vector_value == longlong_vector_t;
    returnedValue &= float_vector_value == float_vector_t;
    returnedValue &= double_vector_value == double_vector_t;

    returnedValue &= triple_ulong_array_t == triple_ulong_array_value;
    
    returnedValue &= octet_seq_len == 5;
    if(octet_seq_value != NULL)
    {
        returnedValue &= octet_seq_value[0] == octet_seq_t[0];
        returnedValue &= octet_seq_value[1] == octet_seq_t[1];
        returnedValue &= octet_seq_value[2] == octet_seq_t[2];
        returnedValue &= octet_seq_value[3] == octet_seq_t[3];
        returnedValue &= octet_seq_value[4] == octet_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= char_seq_len == 5;
    if(char_seq_value != NULL)
    {
        returnedValue &= char_seq_value[0] == char_seq_t[0];
        returnedValue &= char_seq_value[1] == char_seq_t[1];
        returnedValue &= char_seq_value[2] == char_seq_t[2];
        returnedValue &= char_seq_value[3] == char_seq_t[3];
        returnedValue &= char_seq_value[4] == char_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= ushort_seq_len == 5;
    if(ushort_seq_value != NULL)
    {
        returnedValue &= ushort_seq_value[0] == ushort_seq_t[0];
        returnedValue &= ushort_seq_value[1] == ushort_seq_t[1];
        returnedValue &= ushort_seq_value[2] == ushort_seq_t[2];
        returnedValue &= ushort_seq_value[3] == ushort_seq_t[3];
        returnedValue &= ushort_seq_value[4] == ushort_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= short_seq_len == 5;
    if(short_seq_value != NULL)
    {
        returnedValue &= short_seq_value[0] == short_seq_t[0];
        returnedValue &= short_seq_value[1] == short_seq_t[1];
        returnedValue &= short_seq_value[2] == short_seq_t[2];
        returnedValue &= short_seq_value[3] == short_seq_t[3];
        returnedValue &= short_seq_value[4] == short_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= ulong_seq_len == 5;
    if(ulong_seq_value != NULL)
    {
        returnedValue &= ulong_seq_value[0] == ulong_seq_t[0];
        returnedValue &= ulong_seq_value[1] == ulong_seq_t[1];
        returnedValue &= ulong_seq_value[2] == ulong_seq_t[2];
        returnedValue &= ulong_seq_value[3] == ulong_seq_t[3];
        returnedValue &= ulong_seq_value[4] == ulong_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= long_seq_len == 5;
    if(long_seq_value != NULL)
    {
        returnedValue &= long_seq_value[0] == long_seq_t[0];
        returnedValue &= long_seq_value[1] == long_seq_t[1];
        returnedValue &= long_seq_value[2] == long_seq_t[2];
        returnedValue &= long_seq_value[3] == long_seq_t[3];
        returnedValue &= long_seq_value[4] == long_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= ulonglong_seq_len == 5;
    if(ulonglong_seq_value != NULL)
    {
        returnedValue &= ulonglong_seq_value[0] == ulonglong_seq_t[0];
        returnedValue &= ulonglong_seq_value[1] == ulonglong_seq_t[1];
        returnedValue &= ulonglong_seq_value[2] == ulonglong_seq_t[2];
        returnedValue &= ulonglong_seq_value[3] == ulonglong_seq_t[3];
        returnedValue &= ulonglong_seq_value[4] == ulonglong_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= longlong_seq_len == 5;
    if(longlong_seq_value != NULL)
    {
        returnedValue &= longlong_seq_value[0] == longlong_seq_t[0];
        returnedValue &= longlong_seq_value[1] == longlong_seq_t[1];
        returnedValue &= longlong_seq_value[2] == longlong_seq_t[2];
        returnedValue &= longlong_seq_value[3] == longlong_seq_t[3];
        returnedValue &= longlong_seq_value[4] == longlong_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= float_seq_len == 5;
    if(float_seq_value != NULL)
    {
        returnedValue &= float_seq_value[0] == float_seq_t[0];
        returnedValue &= float_seq_value[1] == float_seq_t[1];
        returnedValue &= float_seq_value[2] == float_seq_t[2];
        returnedValue &= float_seq_value[3] == float_seq_t[3];
        returnedValue &= float_seq_value[4] == float_seq_t[4];
    }
    else
        returnedValue = false;
    returnedValue &= double_seq_len == 5;
    if(double_seq_value != NULL)
    {
        returnedValue &= double_seq_value[0] == double_seq_t[0];
        returnedValue &= double_seq_value[1] == double_seq_t[1];
        returnedValue &= double_seq_value[2] == double_seq_t[2];
        returnedValue &= double_seq_value[3] == double_seq_t[3];
        returnedValue &= double_seq_value[4] == double_seq_t[4];
    }
    else
        returnedValue = false;

    returnedValue &= (strcmp(c_string_t, c_string_value) == 0);

    return returnedValue;
}

int main()
{
    bool returnedValue = true;

    returnedValue = CDRTests();
    returnedValue ? returnedValue = FastCDRTests() : false;

    if(returnedValue)
    {
        std::cout << "TEST SUCCESSFUL" << std::endl;
        return 0;
    }
    else
        std::cout << "TEST FAILED" << std::endl;

    return -1;
}
