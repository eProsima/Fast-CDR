/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This copy of FastCdr is licensed to you under the terms described in the
 * FASTCDR_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

#include "fastcdr/Cdr.h"
#include "fastcdr/exceptions/BadParamException.h"

using namespace eprosima::fastcdr;
using namespace ::exception;

#if defined(__LITTLE_ENDIAN__)
    const Cdr::Endianness Cdr::DEFAULT_ENDIAN = LITTLE_ENDIANNESS;
#elif defined (__BIG_ENDIAN__)
    const Cdr::Endianness Cdr::DEFAULT_ENDIAN = BIG_ENDIANNESS;
#endif

Cdr::state::state(Cdr &cdr) : m_currentPosition(cdr.m_currentPosition), m_alignPosition(cdr.m_alignPosition),
    m_swapBytes(cdr.m_swapBytes), m_lastDataSize(cdr.m_lastDataSize) {}

Cdr::Cdr(FastBuffer &cdrBuffer, const Endianness endianness, const CdrType cdrType) : m_cdrBuffer(cdrBuffer),
    m_cdrType(cdrType), m_plFlag(DDS_CDR_WITHOUT_PL), m_options(0), m_endianness(endianness),
    m_swapBytes(endianness == DEFAULT_ENDIAN ? false : true), m_lastDataSize(0), m_currentPosition(cdrBuffer.begin()),
    m_alignPosition(cdrBuffer.begin()), m_lastPosition(cdrBuffer.end())
{
}

Cdr& Cdr::read_encapsulation()
{
    uint8_t dummy, encapsulationKind;
    state state(*this);

    try
    {
        // If it is DDS_CDR, the first step is to get the dummy byte.
        if(m_cdrType == DDS_CDR)
        {
            (*this) >> dummy;
        }

        // Get the ecampsulation byte.
        (*this) >> encapsulationKind;


        // If it is a different endianness, make changes.
        if(m_endianness != (encapsulationKind & 0x1))
        {
            m_swapBytes = !m_swapBytes;
            m_endianness = (encapsulationKind & 0x1);
        }
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    // If it is DDS_CDR type, view if contains a parameter list.
    if(encapsulationKind & DDS_CDR_WITH_PL)
    {
        if(m_cdrType == DDS_CDR)
        {
            m_plFlag = DDS_CDR_WITH_PL;
        }
        else
        {
            throw BadParamException(BadParamException::BAD_PARAM_MESSAGE_DEFAULT);
        }
    }

    try
    {
        if(m_cdrType == DDS_CDR)
            (*this) >> m_options;
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize_encapsulation()
{
    uint8_t dummy = 0, encapsulationKind = 0;
    state state(*this);

    try
    {
        // If it is DDS_CDR, the first step is to serialize the dummy byte.
        if(m_cdrType == DDS_CDR)
        {
            (*this) << dummy;
        }

        // Construct encapsulation byte.
        encapsulationKind = (m_plFlag | m_endianness);

        // Serialize the encapsulation byte.
        (*this) << encapsulationKind;
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    try
    {
        if(m_cdrType == DDS_CDR)
            (*this) << m_options;
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr::DDSCdrPlFlag Cdr::getDDSCdrPlFlag() const
{
    return m_plFlag;
}

void Cdr::setDDSCdrPlFlag(DDSCdrPlFlag plFlag)
{
    m_plFlag = plFlag;
}

uint16_t Cdr::getDDSCdrOptions() const
{
    return m_options;
}

void Cdr::setDDSCdrOptions(uint16_t options)
{
    m_options = options;
}

void Cdr::changeEndianness(Endianness endianness)
{
    if(m_endianness != endianness)
    {
        m_swapBytes = !m_swapBytes;
        m_endianness = endianness;
    }
}

bool Cdr::jump(size_t numBytes)
{
    bool returnedValue = false;

    if(((m_lastPosition - m_currentPosition) >= numBytes) || resize(numBytes))
    {
        m_currentPosition += numBytes;
        returnedValue = true;
    }

    return returnedValue;
}

char* Cdr::getCurrentPosition()
{
    return &m_currentPosition;
}

Cdr::state Cdr::getState()
{
    return Cdr::state(*this);
}

void Cdr::setState(state &state)
{
    m_currentPosition >> state.m_currentPosition;
    m_alignPosition >> state.m_alignPosition;
    m_swapBytes = state.m_swapBytes;
    m_lastDataSize = state.m_lastDataSize;
}

void Cdr::reset()
{
    m_currentPosition = m_cdrBuffer.begin();
    m_alignPosition = m_cdrBuffer.begin();
    m_swapBytes = m_endianness == DEFAULT_ENDIAN ? false : true;
    m_lastDataSize = 0;
}

bool Cdr::moveAlignmentForward(size_t numBytes)
{
    bool returnedValue = false;

    if(((m_lastPosition - m_alignPosition) >= numBytes) || resize(numBytes))
    {
        m_alignPosition += numBytes;
        returnedValue = true;
    }

    return returnedValue;
}

bool Cdr::resize(size_t minSizeInc)
{
    if(m_cdrBuffer.resize(minSizeInc))
    {
        m_currentPosition << m_cdrBuffer.begin();
        m_alignPosition << m_cdrBuffer.begin();
        m_lastPosition = m_cdrBuffer.end();
        return true;
    }
    
    return false;
}

Cdr& Cdr::serialize(const char char_t)
{
    if(((m_lastPosition - m_currentPosition) >= sizeof(char_t)) || resize(sizeof(char_t)))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(char_t);

        m_currentPosition++ << char_t;
        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(const int16_t short_t)
{
    size_t align = alignment(sizeof(short_t));
    size_t sizeAligned = sizeof(short_t) + align;

    if(((m_lastPosition - m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(short_t);
    
        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {    
            const char *dst = reinterpret_cast<const char*>(&short_t);

            m_currentPosition++ << dst[1];
            m_currentPosition++ << dst[0];
        }
        else
        {
            m_currentPosition << short_t;
            m_currentPosition += sizeof(short_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(const int16_t short_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));
    
    try
    {
        serialize(short_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

Cdr& Cdr::serialize(const int32_t long_t)
{
    size_t align = alignment(sizeof(long_t));
    size_t sizeAligned = sizeof(long_t) + align;

    if(((m_lastPosition - m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(long_t);
    
        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&long_t);

            m_currentPosition++ << dst[3];
            m_currentPosition++ << dst[2];
            m_currentPosition++ << dst[1];
            m_currentPosition++ << dst[0];
        }
        else
        {
            m_currentPosition << long_t;
            m_currentPosition += sizeof(long_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(const int32_t long_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(long_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

Cdr& Cdr::serialize(const int64_t longlong_t)
{
    size_t align = alignment(sizeof(longlong_t));
    size_t sizeAligned = sizeof(longlong_t) + align;

    if(((m_lastPosition - m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(longlong_t);
    
        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&longlong_t);

            m_currentPosition++ << dst[7];
            m_currentPosition++ << dst[6];
            m_currentPosition++ << dst[5];
            m_currentPosition++ << dst[4];
            m_currentPosition++ << dst[3];
            m_currentPosition++ << dst[2];
            m_currentPosition++ << dst[1];
            m_currentPosition++ << dst[0];
        }
        else
        {
            m_currentPosition << longlong_t;
            m_currentPosition += sizeof(longlong_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(const int64_t longlong_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(longlong_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

Cdr& Cdr::serialize(const float float_t)
{
    size_t align = alignment(sizeof(float_t));
    size_t sizeAligned = sizeof(float_t) + align;

    if(((m_lastPosition - m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(float_t);
    
        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&float_t);

            m_currentPosition++ << dst[3];
            m_currentPosition++ << dst[2];
            m_currentPosition++ << dst[1];
            m_currentPosition++ << dst[0];
        }
        else
        {
            m_currentPosition << float_t;
            m_currentPosition += sizeof(float_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(const float float_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(float_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

Cdr& Cdr::serialize(const double double_t)
{
    size_t align = alignment(sizeof(double_t));
    size_t sizeAligned = sizeof(double_t) + align;

    if(((m_lastPosition - m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(double_t);
    
        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&double_t);

            m_currentPosition++ << dst[7];
            m_currentPosition++ << dst[6];
            m_currentPosition++ << dst[5];
            m_currentPosition++ << dst[4];
            m_currentPosition++ << dst[3];
            m_currentPosition++ << dst[2];
            m_currentPosition++ << dst[1];
            m_currentPosition++ << dst[0];
        }
        else
        {
            m_currentPosition << double_t;
            m_currentPosition += sizeof(double_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(const double double_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(double_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

Cdr& Cdr::serialize(const bool bool_t)
{
    uint8_t value = 0;

    if(((m_lastPosition - m_currentPosition) >= sizeof(uint8_t)) || resize(sizeof(uint8_t)))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(uint8_t);

        if(bool_t)
            value = 1;
        m_currentPosition++ << value;

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serialize(const char *string_t)
{
    uint32_t length = (uint32_t)strlen(string_t);
    Cdr::state state(*this);

    if(length > 0)
    {
		serialize(++length);

        if(((m_lastPosition - m_currentPosition) >= length) || resize(length))
        {
			// Save last datasize.
            m_lastDataSize = sizeof(uint8_t);

            m_currentPosition.memcopy(string_t, length);
            m_currentPosition += length;
        }
        else
        {
            setState(state);
            throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
        }
    }
	else
		serialize(length);

    return *this;
}

Cdr& Cdr::serialize(const char *string_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(string_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

Cdr& Cdr::serializeArray(const char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if(((m_lastPosition - m_currentPosition) >= totalSize) || resize(totalSize))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*char_t);

        m_currentPosition.memcopy(char_t, totalSize);
        m_currentPosition += totalSize;
        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(const int16_t *short_t, size_t numElements)
{
    size_t align = alignment(sizeof(*short_t));
    size_t totalSize = sizeof(*short_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(((m_lastPosition - m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*short_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        makeAlign(align);

        if(m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&short_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*short_t))
            {
                m_currentPosition++ << dst[1];
                m_currentPosition++ << dst[0];
            }
        }
        else
        {
            m_currentPosition.memcopy(short_t, totalSize);
            m_currentPosition += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(const int16_t *short_t, size_t numElements, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serializeArray(short_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

Cdr& Cdr::serializeArray(const int32_t *long_t, size_t numElements)
{
    size_t align = alignment(sizeof(*long_t));
    size_t totalSize = sizeof(*long_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(((m_lastPosition - m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*long_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        makeAlign(align);

        if(m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&long_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*long_t))
            {
                m_currentPosition++ << dst[3];
                m_currentPosition++ << dst[2];
                m_currentPosition++ << dst[1];
                m_currentPosition++ << dst[0];
            }
        }
        else
        {
            m_currentPosition.memcopy(long_t, totalSize);
            m_currentPosition += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(const int32_t *long_t, size_t numElements, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serializeArray(long_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

Cdr& Cdr::serializeArray(const wchar_t *wchar, size_t numElements)
{
    for(size_t count = 0; count < numElements; ++count)
        serialize(wchar[count]);
    return *this;
}

Cdr& Cdr::serializeArray(const wchar_t *wchar, size_t numElements, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serializeArray(wchar, numElements);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

Cdr& Cdr::serializeArray(const int64_t *longlong_t, size_t numElements)
{
    size_t align = alignment(sizeof(*longlong_t));
    size_t totalSize = sizeof(*longlong_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(((m_lastPosition - m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*longlong_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        makeAlign(align);

        if(m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&longlong_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*longlong_t))
            {
                m_currentPosition++ << dst[7];
                m_currentPosition++ << dst[6];
                m_currentPosition++ << dst[5];
                m_currentPosition++ << dst[4];
                m_currentPosition++ << dst[3];
                m_currentPosition++ << dst[2];
                m_currentPosition++ << dst[1];
                m_currentPosition++ << dst[0];
            }
        }
        else
        {
            m_currentPosition.memcopy(longlong_t, totalSize);
            m_currentPosition += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(const int64_t *longlong_t, size_t numElements, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serializeArray(longlong_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serializeArray(const float *float_t, size_t numElements)
{
    size_t align = alignment(sizeof(*float_t));
    size_t totalSize = sizeof(*float_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(((m_lastPosition - m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*float_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        makeAlign(align);

        if(m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&float_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*float_t))
            {
                m_currentPosition++ << dst[3];
                m_currentPosition++ << dst[2];
                m_currentPosition++ << dst[1];
                m_currentPosition++ << dst[0];
            }
        }
        else
        {
            m_currentPosition.memcopy(float_t, totalSize);
            m_currentPosition += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(const float *float_t, size_t numElements, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serializeArray(float_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serializeArray(const double *double_t, size_t numElements)
{
    size_t align = alignment(sizeof(*double_t));
    size_t totalSize = sizeof(*double_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(((m_lastPosition - m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*double_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        makeAlign(align);

        if(m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&double_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*double_t))
            {
                m_currentPosition++ << dst[7];
                m_currentPosition++ << dst[6];
                m_currentPosition++ << dst[5];
                m_currentPosition++ << dst[4];
                m_currentPosition++ << dst[3];
                m_currentPosition++ << dst[2];
                m_currentPosition++ << dst[1];
                m_currentPosition++ << dst[0];
            }
        }
        else
        {
            m_currentPosition.memcopy(double_t, totalSize);
            m_currentPosition += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::serializeArray(const double *double_t, size_t numElements, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serializeArray(double_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(char &char_t)
{
    if((m_lastPosition - m_currentPosition) >= sizeof(char_t))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(char_t);

        m_currentPosition++ >> char_t;
        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(int16_t &short_t)
{
    size_t align = alignment(sizeof(short_t));
    size_t sizeAligned = sizeof(short_t) + align;

    if((m_lastPosition - m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(short_t);

        // Align
        makeAlign(align);

        if(m_swapBytes)
        {    
            char *dst = reinterpret_cast<char*>(&short_t);

            m_currentPosition++ >> dst[1];
            m_currentPosition++ >> dst[0];
        }
        else
        {
            m_currentPosition >> short_t;
            m_currentPosition += sizeof(short_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(int16_t &short_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(short_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(int32_t &long_t)
{
    size_t align = alignment(sizeof(long_t));
    size_t sizeAligned = sizeof(long_t) + align;

    if((m_lastPosition - m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(long_t);

        // Align
        makeAlign(align);

        if(m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&long_t);

            m_currentPosition++ >> dst[3];
            m_currentPosition++ >> dst[2];
            m_currentPosition++ >> dst[1];
            m_currentPosition++ >> dst[0];
        }
        else
        {
            m_currentPosition >> long_t;
            m_currentPosition += sizeof(long_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(int32_t &long_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(long_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(int64_t &longlong_t)
{
    size_t align = alignment(sizeof(longlong_t));
    size_t sizeAligned = sizeof(longlong_t) + align;

    if((m_lastPosition - m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(longlong_t);

        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&longlong_t);

            m_currentPosition++ >> dst[7];
            m_currentPosition++ >> dst[6];
            m_currentPosition++ >> dst[5];
            m_currentPosition++ >> dst[4];
            m_currentPosition++ >> dst[3];
            m_currentPosition++ >> dst[2];
            m_currentPosition++ >> dst[1];
            m_currentPosition++ >> dst[0];
        }
        else
        {
            m_currentPosition >> longlong_t;
            m_currentPosition += sizeof(longlong_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(int64_t &longlong_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(longlong_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(float &float_t)
{
    size_t align = alignment(sizeof(float_t));
    size_t sizeAligned = sizeof(float_t) + align;

    if((m_lastPosition - m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(float_t);

        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&float_t);

            m_currentPosition++ >> dst[3];
            m_currentPosition++ >> dst[2];
            m_currentPosition++ >> dst[1];
            m_currentPosition++ >> dst[0];
        }
        else
        {
            m_currentPosition >> float_t;
            m_currentPosition += sizeof(float_t);
        }

        return *this;
    }

   throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(float &float_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(float_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(double &double_t)
{
    size_t align = alignment(sizeof(double_t));
    size_t sizeAligned = sizeof(double_t) + align;

    if((m_lastPosition - m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(double_t);

        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&double_t);

            m_currentPosition++ >> dst[7];
            m_currentPosition++ >> dst[6];
            m_currentPosition++ >> dst[5];
            m_currentPosition++ >> dst[4];
            m_currentPosition++ >> dst[3];
            m_currentPosition++ >> dst[2];
            m_currentPosition++ >> dst[1];
            m_currentPosition++ >> dst[0];
        }
        else
        {
            m_currentPosition >> double_t;
            m_currentPosition += sizeof(double_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(double &double_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(double_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(bool &bool_t)
{
    uint8_t value = 0;

    if((m_lastPosition - m_currentPosition) >= sizeof(uint8_t))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(uint8_t);

        m_currentPosition++ >> value;

        if(value == 1)
        {
            bool_t = true;
            return *this;
        }
        else if(value == 0)
        {
            bool_t = false;
            return *this;
        }

        throw BadParamException(BadParamException::BAD_PARAM_MESSAGE_DEFAULT);
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(char *&string_t)
{
    uint32_t length = 0;
    Cdr::state state(*this);

    deserialize(length);

    if(length == 0)
    {
        string_t = NULL;
        return *this;
    }
    else if((m_lastPosition - m_currentPosition) >= length)
    {
		// Save last datasize.
        m_lastDataSize = sizeof(uint8_t);

        // Allocate memory.
        string_t = (char*)calloc(length + ((&m_currentPosition)[length-1] == '\0' ? 0 : 1), sizeof(char));
        memcpy(string_t, &m_currentPosition, length);
        m_currentPosition += length;
        return *this;
    }

    setState(state);
    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(char *&string_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(string_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

const char* Cdr::readString(uint32_t &length)
{
	const char* returnedValue = "";
	state state(*this);

	*this >> length;

	if(length == 0)
	{
		return returnedValue;
	}
	else if((m_lastPosition - m_currentPosition) >= length)
	{
		// Save last datasize.
		m_lastDataSize = sizeof(uint8_t);

		returnedValue = &m_currentPosition;
		m_currentPosition += length;
		if(returnedValue[length-1] == '\0') --length;
		return returnedValue;
	}

	setState(state);
	throw eprosima::fastcdr::exception::NotEnoughMemoryException(eprosima::fastcdr::exception::NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if((m_lastPosition - m_currentPosition) >= totalSize)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*char_t);

        m_currentPosition.rmemcopy(char_t, totalSize);
        m_currentPosition += totalSize;
        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(int16_t *short_t, size_t numElements)
{
    size_t align = alignment(sizeof(*short_t));
    size_t totalSize = sizeof(*short_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if((m_lastPosition - m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*short_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        makeAlign(align);

        if(m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&short_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*short_t))
            {
                m_currentPosition++ >> dst[1];
                m_currentPosition++ >> dst[0];
            }
        }
        else
        {
            m_currentPosition.rmemcopy(short_t, totalSize);
            m_currentPosition += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(int16_t *short_t, size_t numElements, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserializeArray(short_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserializeArray(int32_t *long_t, size_t numElements)
{
    size_t align = alignment(sizeof(*long_t));
    size_t totalSize = sizeof(*long_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if((m_lastPosition - m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*long_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        makeAlign(align);

        if(m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&long_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*long_t))
            {
                m_currentPosition++ >> dst[3];
                m_currentPosition++ >> dst[2];
                m_currentPosition++ >> dst[1];
                m_currentPosition++ >> dst[0];
            }
        }
        else
        {
            m_currentPosition.rmemcopy(long_t, totalSize);
            m_currentPosition += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(int32_t *long_t, size_t numElements, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserializeArray(long_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserializeArray(wchar_t *wchar, size_t numElements)
{
    uint32_t value;
    for(size_t count = 0; count < numElements; ++count)
    {
        deserialize(value);
        wchar[count] = (wchar_t)value;
    }
    return *this;
}

Cdr& Cdr::deserializeArray(wchar_t *wchar, size_t numElements, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserializeArray(wchar, numElements);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserializeArray(int64_t *longlong_t, size_t numElements)
{
    size_t align = alignment(sizeof(*longlong_t));
    size_t totalSize = sizeof(*longlong_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if((m_lastPosition - m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*longlong_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        makeAlign(align);

        if(m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&longlong_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*longlong_t))
            {
                m_currentPosition++ >> dst[7];
                m_currentPosition++ >> dst[6];
                m_currentPosition++ >> dst[5];
                m_currentPosition++ >> dst[4];
                m_currentPosition++ >> dst[3];
                m_currentPosition++ >> dst[2];
                m_currentPosition++ >> dst[1];
                m_currentPosition++ >> dst[0];
            }
        }
        else
        {
            m_currentPosition.rmemcopy(longlong_t, totalSize);
            m_currentPosition += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(int64_t *longlong_t, size_t numElements, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserializeArray(longlong_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserializeArray(float *float_t, size_t numElements)
{
    size_t align = alignment(sizeof(*float_t));
    size_t totalSize = sizeof(*float_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if((m_lastPosition - m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*float_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        makeAlign(align);

        if(m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&float_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*float_t))
            {
                m_currentPosition++ >> dst[3];
                m_currentPosition++ >> dst[2];
                m_currentPosition++ >> dst[1];
                m_currentPosition++ >> dst[0];
            }
        }
        else
        {
            m_currentPosition.rmemcopy(float_t, totalSize);
            m_currentPosition += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(float *float_t, size_t numElements, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserializeArray(float_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserializeArray(double *double_t, size_t numElements)
{
    size_t align = alignment(sizeof(*double_t));
    size_t totalSize = sizeof(*double_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if((m_lastPosition - m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*double_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        makeAlign(align);

        if(m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&double_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*double_t))
            {
                m_currentPosition++ >> dst[7];
                m_currentPosition++ >> dst[6];
                m_currentPosition++ >> dst[5];
                m_currentPosition++ >> dst[4];
                m_currentPosition++ >> dst[3];
                m_currentPosition++ >> dst[2];
                m_currentPosition++ >> dst[1];
                m_currentPosition++ >> dst[0];
            }
        }
        else
        {
            m_currentPosition.rmemcopy(double_t, totalSize);
            m_currentPosition += totalSize;
        }

        return *this;
    }

    throw NotEnoughMemoryException(NotEnoughMemoryException::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserializeArray(double *double_t, size_t numElements, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserializeArray(double_t, numElements);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}
