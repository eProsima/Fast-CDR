#include "cpp/Cdr.h"
#include "cpp/exceptions/NotEnoughMemoryException.h"
#include "cpp/exceptions/BadParamException.h"
#include <string.h>

using namespace eProsima;

const std::string NOT_ENOUGH_MEMORY_MESSAGE("Not enough memory in the buffer stream");
const std::string BAD_PARAM_MESSAGE("Bad parameter"); 

CDR::CDR(CDRBuffer &cdrBuffer, const CdrType cdrType) : m_cdrBuffer(cdrBuffer),
    m_cdrType(cdrType), m_plFlag(DDS_CDR_WITHOUT_PL), m_options(0)
{
}

CDR& CDR::read_encapsulation()
{
    uint8_t dummy, encapsulationKind;
    CDRBuffer::State state(m_cdrBuffer);

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
        if(m_cdrBuffer.m_endianness != (encapsulationKind & 0x1))
        {
            m_cdrBuffer.m_swapBytes = !m_cdrBuffer.m_swapBytes;
            m_cdrBuffer.m_endianness = encapsulationKind;
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
            throw BadParamException(BAD_PARAM_MESSAGE);
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

CDR::DDSCdrPlFlag CDR::getDDSCdrPlFlag() const
{
    return m_plFlag;
}

uint16_t CDR::getDDSCdrOptions() const
{
    return m_options;
}

bool CDR::jump(uint32_t numBytes)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(numBytes)))
    {
        m_cdrBuffer.m_currentPosition += numBytes;
        returnedValue = true;
    }

    return returnedValue;
}

void CDR::resetAlignment()
{
	m_cdrBuffer.resetAlign();
}

char* CDR::getCurrentPosition()
{
    return m_cdrBuffer.m_currentPosition;
}

CDRBuffer::State CDR::getState() const
{
    return CDRBuffer::State(m_cdrBuffer);
}

void CDR::setState(CDRBuffer::State &state)
{
    m_cdrBuffer.m_currentPosition = state.m_currentPosition;
    m_cdrBuffer.m_bufferRemainLength = state.m_bufferRemainLength;
    m_cdrBuffer.m_alignPosition = state.m_alignPosition;
    m_cdrBuffer.m_swapBytes = state.m_swapBytes;
    m_cdrBuffer.m_lastDataSize = state.m_lastDataSize;
}

CDR& CDR::serialize(const char char_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(char_t)) || m_cdrBuffer.resize(sizeof(char_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(char_t);

        *m_cdrBuffer.m_currentPosition++ = char_t;
        m_cdrBuffer.m_bufferRemainLength -= sizeof(char_t);
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serialize(const int16_t short_t)
{
    size_t align = m_cdrBuffer.align(sizeof(short_t));
    size_t sizeAligned = sizeof(short_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(short_t);
    
        // Align.
        m_cdrBuffer.makeAlign(align);
        const char *dst = reinterpret_cast<const char*>(&short_t);

        if(m_cdrBuffer.m_swapBytes)
        {    
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(short_t));
            m_cdrBuffer.m_currentPosition += sizeof(short_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serialize(const int16_t short_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    
    try
    {
        serialize(short_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serializeFast(const int16_t short_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(short_t)) || m_cdrBuffer.resize(sizeof(short_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(short_t);

        const char *dst = reinterpret_cast<const char*>(&short_t);

        if(m_cdrBuffer.m_swapBytes)
        {    
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(short_t));
            m_cdrBuffer.m_currentPosition += sizeof(short_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeof(short_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeFast(const int16_t short_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));
    
    try
    {
        serializeFast(short_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serialize(const int32_t long_t)
{
    size_t align = m_cdrBuffer.align(sizeof(long_t));
    size_t sizeAligned = sizeof(long_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(long_t);
    
        // Align.
        m_cdrBuffer.makeAlign(align);
        const char *dst = reinterpret_cast<const char*>(&long_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(long_t));
            m_cdrBuffer.m_currentPosition += sizeof(long_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serialize(const int32_t long_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serialize(long_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serializeFast(const int32_t long_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(long_t)) || m_cdrBuffer.resize(sizeof(long_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(long_t);
    
        const char *dst = reinterpret_cast<const char*>(&long_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(long_t));
            m_cdrBuffer.m_currentPosition += sizeof(long_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeof(long_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeFast(const int32_t long_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeFast(long_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serialize(const int64_t longlong_t)
{
    size_t align = m_cdrBuffer.align(sizeof(longlong_t));
    size_t sizeAligned = sizeof(longlong_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(longlong_t);
    
        // Align.
        m_cdrBuffer.makeAlign(align);
        const char *dst = reinterpret_cast<const char*>(&longlong_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            *m_cdrBuffer.m_currentPosition++ = dst[7];
            *m_cdrBuffer.m_currentPosition++ = dst[6];
            *m_cdrBuffer.m_currentPosition++ = dst[5];
            *m_cdrBuffer.m_currentPosition++ = dst[4];
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(longlong_t));
            m_cdrBuffer.m_currentPosition += sizeof(longlong_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serialize(const int64_t longlong_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serialize(longlong_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serializeFast(const int64_t longlong_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(longlong_t)) || m_cdrBuffer.resize(sizeof(longlong_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(longlong_t);
    
        const char *dst = reinterpret_cast<const char*>(&longlong_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            *m_cdrBuffer.m_currentPosition++ = dst[7];
            *m_cdrBuffer.m_currentPosition++ = dst[6];
            *m_cdrBuffer.m_currentPosition++ = dst[5];
            *m_cdrBuffer.m_currentPosition++ = dst[4];
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(longlong_t));
            m_cdrBuffer.m_currentPosition += sizeof(longlong_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeof(longlong_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeFast(const int64_t longlong_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeFast(longlong_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serialize(const float float_t)
{
    size_t align = m_cdrBuffer.align(sizeof(float_t));
    size_t sizeAligned = sizeof(float_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(float_t);
    
        // Align.
        m_cdrBuffer.makeAlign(align);
        const char *dst = reinterpret_cast<const char*>(&float_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(float_t));
            m_cdrBuffer.m_currentPosition += sizeof(float_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serialize(const float float_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serialize(float_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serializeFast(const float float_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(float_t)) || m_cdrBuffer.resize(sizeof(float_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(float_t);
    
        const char *dst = reinterpret_cast<const char*>(&float_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(float_t));
            m_cdrBuffer.m_currentPosition += sizeof(float_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeof(float_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeFast(const float float_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeFast(float_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serialize(const double double_t)
{
    size_t align = m_cdrBuffer.align(sizeof(double_t));
    size_t sizeAligned = sizeof(double_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(double_t);
    
        // Align.
        m_cdrBuffer.makeAlign(align);
        const char *dst = reinterpret_cast<const char*>(&double_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            *m_cdrBuffer.m_currentPosition++ = dst[7];
            *m_cdrBuffer.m_currentPosition++ = dst[6];
            *m_cdrBuffer.m_currentPosition++ = dst[5];
            *m_cdrBuffer.m_currentPosition++ = dst[4];
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(double_t));
            m_cdrBuffer.m_currentPosition += sizeof(double_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serialize(const double double_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serialize(double_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serializeFast(const double double_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(double_t)) || m_cdrBuffer.resize(sizeof(double_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(double_t);
    
        const char *dst = reinterpret_cast<const char*>(&double_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            *m_cdrBuffer.m_currentPosition++ = dst[7];
            *m_cdrBuffer.m_currentPosition++ = dst[6];
            *m_cdrBuffer.m_currentPosition++ = dst[5];
            *m_cdrBuffer.m_currentPosition++ = dst[4];
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, dst, sizeof(double_t));
            m_cdrBuffer.m_currentPosition += sizeof(double_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeof(double_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeFast(const double double_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeFast(double_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serialize(const bool bool_t)
{
    uint8_t value = 0;

    if(m_cdrBuffer.checkSpace(sizeof(uint8_t)) || m_cdrBuffer.resize(sizeof(uint8_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

        if(bool_t)
            value = 1;
        *m_cdrBuffer.m_currentPosition++ = value;
        m_cdrBuffer.m_bufferRemainLength -= sizeof(uint8_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serialize(const std::string &string_t)
{
    uint32_t length = (uint32_t)string_t.length();
    CDRBuffer::State state(m_cdrBuffer);

    *this << length;

    if(length > 0)
    {
        if(m_cdrBuffer.checkSpace(length) || m_cdrBuffer.resize(length))
        {
            // Save last datasize.
            m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

            memcpy(m_cdrBuffer.m_currentPosition, string_t.c_str(), length);
            m_cdrBuffer.m_currentPosition += length;
            m_cdrBuffer.m_bufferRemainLength -= length;
        }
        else
        {
            setState(state);
            throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
        }
    }

    return *this;
}

CDR& CDR::serialize(const std::string &string_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serialize(string_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serializeFast(const std::string &string_t)
{
    uint32_t length = (uint32_t)string_t.length();
    CDRBuffer::State state(m_cdrBuffer);

    serializeFast(length);

    if(length > 0)
    {
        if(m_cdrBuffer.checkSpace(length) || m_cdrBuffer.resize(length))
        {
            // Save last datasize.
            m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

            memcpy(m_cdrBuffer.m_currentPosition, string_t.c_str(), length);
            m_cdrBuffer.m_currentPosition += length;
            m_cdrBuffer.m_bufferRemainLength -= length;
        }
        else
        {
            setState(state);
            throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
        }
    }

    return *this;
}

CDR& CDR::serializeFast(const std::string &string_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeFast(string_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serializeArray(const char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if(m_cdrBuffer.checkSpace(totalSize) || m_cdrBuffer.resize(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(char_t);

        memcpy(m_cdrBuffer.m_currentPosition, char_t, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeArray(const int16_t *short_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(short_t));
    size_t totalSize = sizeof(*short_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(short_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&short_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*short_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, short_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeArray(const int16_t *short_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeArray(short_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serializeArrayFast(const int16_t *short_t, size_t numElements)
{
    size_t totalSize = sizeof(*short_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize) || m_cdrBuffer.resize(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(short_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&short_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*short_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, short_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeArrayFast(const int16_t *short_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeArrayFast(short_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serializeArray(const int32_t *long_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(long_t));
    size_t totalSize = sizeof(*long_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(long_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&long_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*long_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[3];
                *m_cdrBuffer.m_currentPosition++ = dst[2];
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, long_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeArray(const int32_t *long_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeArray(long_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serializeArrayFast(const int32_t *long_t, size_t numElements)
{
    size_t totalSize = sizeof(*long_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize) || m_cdrBuffer.resize(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(long_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&long_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*long_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[3];
                *m_cdrBuffer.m_currentPosition++ = dst[2];
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, long_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeArrayFast(const int32_t *long_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeArrayFast(long_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }
    
    return *this;
}

CDR& CDR::serializeArray(const int64_t *longlong_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(longlong_t));
    size_t totalSize = sizeof(*longlong_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(longlong_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&longlong_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*longlong_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[7];
                *m_cdrBuffer.m_currentPosition++ = dst[6];
                *m_cdrBuffer.m_currentPosition++ = dst[5];
                *m_cdrBuffer.m_currentPosition++ = dst[4];
                *m_cdrBuffer.m_currentPosition++ = dst[3];
                *m_cdrBuffer.m_currentPosition++ = dst[2];
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, longlong_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeArray(const int64_t *longlong_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeArray(longlong_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::serializeArrayFast(const int64_t *longlong_t, size_t numElements)
{
    size_t totalSize = sizeof(*longlong_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize) || m_cdrBuffer.resize(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(longlong_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&longlong_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*longlong_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[7];
                *m_cdrBuffer.m_currentPosition++ = dst[6];
                *m_cdrBuffer.m_currentPosition++ = dst[5];
                *m_cdrBuffer.m_currentPosition++ = dst[4];
                *m_cdrBuffer.m_currentPosition++ = dst[3];
                *m_cdrBuffer.m_currentPosition++ = dst[2];
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, longlong_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeArrayFast(const int64_t *longlong_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeArrayFast(longlong_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::serializeArray(const float *float_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(float_t));
    size_t totalSize = sizeof(*float_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(float_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&float_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*float_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[3];
                *m_cdrBuffer.m_currentPosition++ = dst[2];
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, float_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeArray(const float *float_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeArray(float_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::serializeArrayFast(const float *float_t, size_t numElements)
{
    size_t totalSize = sizeof(*float_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize) || m_cdrBuffer.resize(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(float_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&float_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*float_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[3];
                *m_cdrBuffer.m_currentPosition++ = dst[2];
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, float_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeArrayFast(const float *float_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeArrayFast(float_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::serializeArray(const double *double_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(double_t));
    size_t totalSize = sizeof(*double_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned) || m_cdrBuffer.resize(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(double_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&double_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*double_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[7];
                *m_cdrBuffer.m_currentPosition++ = dst[6];
                *m_cdrBuffer.m_currentPosition++ = dst[5];
                *m_cdrBuffer.m_currentPosition++ = dst[4];
                *m_cdrBuffer.m_currentPosition++ = dst[3];
                *m_cdrBuffer.m_currentPosition++ = dst[2];
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, double_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeArray(const double *double_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeArray(double_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::serializeArrayFast(const double *double_t, size_t numElements)
{
    size_t totalSize = sizeof(*double_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize) || m_cdrBuffer.resize(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(double_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&double_t);
            const char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*double_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[7];
                *m_cdrBuffer.m_currentPosition++ = dst[6];
                *m_cdrBuffer.m_currentPosition++ = dst[5];
                *m_cdrBuffer.m_currentPosition++ = dst[4];
                *m_cdrBuffer.m_currentPosition++ = dst[3];
                *m_cdrBuffer.m_currentPosition++ = dst[2];
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, double_t, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::serializeArrayFast(const double *double_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        serializeArrayFast(double_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserialize(char &char_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(char_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(char_t);

        char_t = *m_cdrBuffer.m_currentPosition++;
        m_cdrBuffer.m_bufferRemainLength -= sizeof(char_t);
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserialize(int16_t &short_t)
{
    size_t align = m_cdrBuffer.align(sizeof(short_t));
    size_t sizeAligned = sizeof(short_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(short_t);

        // Align
        m_cdrBuffer.makeAlign(align);
        char *dst = reinterpret_cast<char*>(&short_t);

        if(m_cdrBuffer.m_swapBytes)
        {    
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(short_t));
            m_cdrBuffer.m_currentPosition += sizeof(short_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserialize(int16_t &short_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserialize(short_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeFast(int16_t &short_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(short_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(short_t);

        char *dst = reinterpret_cast<char*>(&short_t);

        if(m_cdrBuffer.m_swapBytes)
        {    
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(short_t));
            m_cdrBuffer.m_currentPosition += sizeof(short_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeof(short_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeFast(int16_t &short_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeFast(short_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserialize(int32_t &long_t)
{
    size_t align = m_cdrBuffer.align(sizeof(long_t));
    size_t sizeAligned = sizeof(long_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(long_t);

        char *dst = reinterpret_cast<char*>(&long_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(long_t));
            m_cdrBuffer.m_currentPosition += sizeof(long_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserialize(int32_t &long_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserialize(long_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeFast(int32_t &long_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(long_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(long_t);

        char *dst = reinterpret_cast<char*>(&long_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(long_t));
            m_cdrBuffer.m_currentPosition += sizeof(long_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeof(long_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeFast(int32_t &long_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeFast(long_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserialize(int64_t &longlong_t)
{
    size_t align = m_cdrBuffer.align(sizeof(longlong_t));
    size_t sizeAligned = sizeof(longlong_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(longlong_t);

        // Align.
        m_cdrBuffer.makeAlign(align);
        char *dst = reinterpret_cast<char*>(&longlong_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            dst[7] = *m_cdrBuffer.m_currentPosition++;
            dst[6] = *m_cdrBuffer.m_currentPosition++;
            dst[5] = *m_cdrBuffer.m_currentPosition++;
            dst[4] = *m_cdrBuffer.m_currentPosition++;
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(longlong_t));
            m_cdrBuffer.m_currentPosition += sizeof(longlong_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserialize(int64_t &longlong_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserialize(longlong_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeFast(int64_t &longlong_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(longlong_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(longlong_t);

        char *dst = reinterpret_cast<char*>(&longlong_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            dst[7] = *m_cdrBuffer.m_currentPosition++;
            dst[6] = *m_cdrBuffer.m_currentPosition++;
            dst[5] = *m_cdrBuffer.m_currentPosition++;
            dst[4] = *m_cdrBuffer.m_currentPosition++;
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(longlong_t));
            m_cdrBuffer.m_currentPosition += sizeof(longlong_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeof(longlong_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeFast(int64_t &longlong_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeFast(longlong_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserialize(float &float_t)
{
    size_t align = m_cdrBuffer.align(sizeof(float_t));
    size_t sizeAligned = sizeof(float_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(float_t);

        // Align.
        m_cdrBuffer.makeAlign(align);
        char *dst = reinterpret_cast<char*>(&float_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(float_t));
            m_cdrBuffer.m_currentPosition += sizeof(float_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

   throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserialize(float &float_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserialize(float_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeFast(float &float_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(float_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(float_t);

        char *dst = reinterpret_cast<char*>(&float_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(float_t));
            m_cdrBuffer.m_currentPosition += sizeof(float_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeof(float_t);

        return *this;
    }

   throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeFast(float &float_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeFast(float_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserialize(double &double_t)
{
    size_t align = m_cdrBuffer.align(sizeof(double_t));
    size_t sizeAligned = sizeof(double_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(double_t);

        // Align.
        m_cdrBuffer.makeAlign(align);
        char *dst = reinterpret_cast<char*>(&double_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            dst[7] = *m_cdrBuffer.m_currentPosition++;
            dst[6] = *m_cdrBuffer.m_currentPosition++;
            dst[5] = *m_cdrBuffer.m_currentPosition++;
            dst[4] = *m_cdrBuffer.m_currentPosition++;
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(double_t));
            m_cdrBuffer.m_currentPosition += sizeof(double_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserialize(double &double_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserialize(double_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeFast(double &double_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(double_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(double_t);

        char *dst = reinterpret_cast<char*>(&double_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            dst[7] = *m_cdrBuffer.m_currentPosition++;
            dst[6] = *m_cdrBuffer.m_currentPosition++;
            dst[5] = *m_cdrBuffer.m_currentPosition++;
            dst[4] = *m_cdrBuffer.m_currentPosition++;
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            memcpy(dst, m_cdrBuffer.m_currentPosition, sizeof(double_t));
            m_cdrBuffer.m_currentPosition += sizeof(double_t);
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeof(double_t);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeFast(double &double_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeFast(double_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserialize(bool &bool_t)
{
    uint8_t value = 0;

    if(m_cdrBuffer.checkSpace(sizeof(uint8_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

        value = *m_cdrBuffer.m_currentPosition++;
        m_cdrBuffer.m_bufferRemainLength -= sizeof(uint8_t);

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

        throw BadParamException(BAD_PARAM_MESSAGE);
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserialize(std::string &string_t)
{
    uint32_t length = 0;
    CDRBuffer::State state(m_cdrBuffer);

    *this >> length;

    if(length == 0)
    {
        string_t = "";
        return *this;
    }
    else if(m_cdrBuffer.checkSpace(length))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

        string_t = std::string(m_cdrBuffer.m_currentPosition, length - (m_cdrBuffer.m_currentPosition[length-1] == '\0' ? 1 : 0));
        m_cdrBuffer.m_currentPosition += length;
        m_cdrBuffer.m_bufferRemainLength -= length;
        return *this;
    }

    setState(state);
    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserialize(std::string &string_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserialize(string_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeFast(std::string &string_t)
{
    uint32_t length = 0;
    CDRBuffer::State state(m_cdrBuffer);

    deserializeFast(length);

    if(length == 0)
    {
        string_t = "";
        return *this;
    }
    else if(m_cdrBuffer.checkSpace(length))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

        string_t = std::string(m_cdrBuffer.m_currentPosition, length - (m_cdrBuffer.m_currentPosition[length-1] == '\0' ? 1 : 0));
        m_cdrBuffer.m_currentPosition += length;
        m_cdrBuffer.m_bufferRemainLength -= length;
        return *this;
    }

    setState(state);
    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeFast(std::string &string_t, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeFast(string_t);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeArray(char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(char_t);

        memcpy(char_t, m_cdrBuffer.m_currentPosition, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferRemainLength -= totalSize;
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeArray(int16_t *short_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(short_t));
    size_t totalSize = sizeof(*short_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(short_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&short_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*short_t))
            {
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(short_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeArray(int16_t *short_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeArray(short_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeArrayFast(int16_t *short_t, size_t numElements)
{
    size_t totalSize = sizeof(*short_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(short_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&short_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*short_t))
            {
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(short_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeArrayFast(int16_t *short_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeArrayFast(short_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeArray(int32_t *long_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(long_t));
    size_t totalSize = sizeof(*long_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(long_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&long_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*long_t))
            {
                dst[3] = *m_cdrBuffer.m_currentPosition++;
                dst[2] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(long_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeArray(int32_t *long_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeArray(long_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeArrayFast(int32_t *long_t, size_t numElements)
{
    size_t totalSize = sizeof(*long_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(long_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&long_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*long_t))
            {
                dst[3] = *m_cdrBuffer.m_currentPosition++;
                dst[2] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(long_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeArrayFast(int32_t *long_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeArrayFast(long_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeArray(int64_t *longlong_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(longlong_t));
    size_t totalSize = sizeof(*longlong_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(longlong_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&longlong_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*longlong_t))
            {
                dst[7] = *m_cdrBuffer.m_currentPosition++;
                dst[6] = *m_cdrBuffer.m_currentPosition++;
                dst[5] = *m_cdrBuffer.m_currentPosition++;
                dst[4] = *m_cdrBuffer.m_currentPosition++;
                dst[3] = *m_cdrBuffer.m_currentPosition++;
                dst[2] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(longlong_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeArray(int64_t *longlong_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeArray(longlong_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeArrayFast(int64_t *longlong_t, size_t numElements)
{
    size_t totalSize = sizeof(*longlong_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(longlong_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&longlong_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*longlong_t))
            {
                dst[7] = *m_cdrBuffer.m_currentPosition++;
                dst[6] = *m_cdrBuffer.m_currentPosition++;
                dst[5] = *m_cdrBuffer.m_currentPosition++;
                dst[4] = *m_cdrBuffer.m_currentPosition++;
                dst[3] = *m_cdrBuffer.m_currentPosition++;
                dst[2] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(longlong_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeArrayFast(int64_t *longlong_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeArrayFast(longlong_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeArray(float *float_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(float_t));
    size_t totalSize = sizeof(*float_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(float_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&float_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*float_t))
            {
                dst[3] = *m_cdrBuffer.m_currentPosition++;
                dst[2] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(float_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeArray(float *float_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeArray(float_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeArrayFast(float *float_t, size_t numElements)
{
    size_t totalSize = sizeof(*float_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(float_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&float_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*float_t))
            {
                dst[3] = *m_cdrBuffer.m_currentPosition++;
                dst[2] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(float_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeArrayFast(float *float_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeArrayFast(float_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeArray(double *double_t, size_t numElements)
{
    size_t align = m_cdrBuffer.align(sizeof(double_t));
    size_t totalSize = sizeof(*double_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(double_t);

        // Align
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(align);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&double_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*double_t))
            {
                dst[7] = *m_cdrBuffer.m_currentPosition++;
                dst[6] = *m_cdrBuffer.m_currentPosition++;
                dst[5] = *m_cdrBuffer.m_currentPosition++;
                dst[4] = *m_cdrBuffer.m_currentPosition++;
                dst[3] = *m_cdrBuffer.m_currentPosition++;
                dst[2] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(double_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= sizeAligned;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeArray(double *double_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeArray(double_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

CDR& CDR::deserializeArrayFast(double *double_t, size_t numElements)
{
    size_t totalSize = sizeof(*double_t) * numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(double_t);

        if(m_cdrBuffer.m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&double_t);
            char *end = dst + totalSize;

            for(; dst < end; dst += sizeof(*double_t))
            {
                dst[7] = *m_cdrBuffer.m_currentPosition++;
                dst[6] = *m_cdrBuffer.m_currentPosition++;
                dst[5] = *m_cdrBuffer.m_currentPosition++;
                dst[4] = *m_cdrBuffer.m_currentPosition++;
                dst[3] = *m_cdrBuffer.m_currentPosition++;
                dst[2] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(double_t, m_cdrBuffer.m_currentPosition, totalSize);
            m_cdrBuffer.m_currentPosition += totalSize;
        }

        m_cdrBuffer.m_bufferRemainLength -= totalSize;

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE);
}

CDR& CDR::deserializeArrayFast(double *double_t, size_t numElements, CDRBuffer::Endianness endianness)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness == endianness)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianness != endianness));

    try
    {
        deserializeArrayFast(double_t, numElements);
        m_cdrBuffer.m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_cdrBuffer.m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}