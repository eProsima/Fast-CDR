#include "cpp/Cdr.h"
#include <string.h>

using namespace eProsima;

CDR::CDR(CDRBuffer &cdrBuffer, const CdrType cdrType) : m_cdrBuffer(cdrBuffer),
    m_cdrType(cdrType), m_plFlag(DDS_CDR_WITHOUT_PL), m_options(0)
{
}

bool CDR::read_encapsulation()
{
    bool returnedValue = true;
    uint8_t dummy, encapsulationKind;

    // If it is DDS_CDR, the first step is to get the dummy byte.
    if(m_cdrType == DDS_CDR)
    {
        returnedValue &= (*this) >> dummy;
    }

    // Get the ecampsulation byte.
    returnedValue &= (*this) >> encapsulationKind;

    if(returnedValue)
    {
        // If it is a different endianess, make changes.
        if(m_cdrBuffer.m_endianess != (encapsulationKind & 0x1))
        {
            m_cdrBuffer.m_swapBytes = !m_cdrBuffer.m_swapBytes;
            m_cdrBuffer.m_endianess = encapsulationKind;
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
                returnedValue = false;
            }
        }
    }

    if(returnedValue && (m_cdrType == DDS_CDR))
        returnedValue &= (*this) >> m_options;

    return returnedValue;
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

void CDR::setState(CDRBuffer::State state)
{
    m_cdrBuffer.m_currentPosition = state.m_currentPosition;
}

bool CDR::serialize(const char char_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(char_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(char_t);

        *m_cdrBuffer.m_currentPosition++ = char_t;
        m_cdrBuffer.m_bufferLength -= sizeof(char_t);
        return true;
    }

    return false;
}

bool CDR::serialize(const int16_t short_t)
{
    size_t align = m_cdrBuffer.align(sizeof(short_t));
    size_t sizeAligned = sizeof(short_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serialize(const int16_t short_t, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = serialize(short_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serialize(const int32_t long_t)
{
    size_t align = m_cdrBuffer.align(sizeof(long_t));
    size_t sizeAligned = sizeof(long_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serialize(const int32_t long_t, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = serialize(long_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serialize(const int64_t longlong_t)
{
    size_t align = m_cdrBuffer.align(sizeof(longlong_t));
    size_t sizeAligned = sizeof(longlong_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serialize(const int64_t longlong_t, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = serialize(longlong_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serialize(const float float_t)
{
    size_t align = m_cdrBuffer.align(sizeof(float_t));
    size_t sizeAligned = sizeof(float_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serialize(const float float_t, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = serialize(float_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serialize(const double double_t)
{
    size_t align = m_cdrBuffer.align(sizeof(double_t));
    size_t sizeAligned = sizeof(double_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serialize(const double double_t, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = serialize(double_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serialize(const bool bool_t)
{
    uint8_t value = 0;

    if(m_cdrBuffer.checkSpace(sizeof(uint8_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

        if(bool_t)
            value = 1;
        *m_cdrBuffer.m_currentPosition++ = value;
        m_cdrBuffer.m_bufferLength -= sizeof(uint8_t);

        return true;
    }

    return false;
}

bool CDR::serialize(const std::string &string_t)
{
    bool returnedValue = true;
    uint32_t length = (uint32_t)string_t.length();

    returnedValue &= *this << length;

    if(length > 0 && m_cdrBuffer.checkSpace(length))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

        memcpy(m_cdrBuffer.m_currentPosition, string_t.c_str(), length);
        m_cdrBuffer.m_currentPosition += length;
        m_cdrBuffer.m_bufferLength -= length;
    }

    return returnedValue;
}

bool CDR::serialize(const std::string &string_t, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = serialize(string_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeArray(const char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(char_t);

        memcpy(m_cdrBuffer.m_currentPosition, char_t, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferLength -= totalSize;
        return true;
    }

    return false;
}

bool CDR::serializeArray(const int16_t *short_t, size_t numElements)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serializeArray(const int16_t *short_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = serializeArray(short_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeArray(const int32_t *long_t, size_t numElements)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serializeArray(const int32_t *long_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = serializeArray(long_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeArray(const int64_t *longlong_t, size_t numElements)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serializeArray(const int64_t *longlong_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = serializeArray(longlong_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeArray(const float *float_t, size_t numElements)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serializeArray(const float *float_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = serializeArray(float_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::serializeArray(const double *double_t, size_t numElements)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::serializeArray(const double *double_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = serializeArray(double_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserialize(char &char_t)
{
    if(m_cdrBuffer.checkSpace(sizeof(char_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(char_t);

        char_t = *m_cdrBuffer.m_currentPosition++;
        m_cdrBuffer.m_bufferLength -= sizeof(char_t);
        return true;
    }

    return false;
}

bool CDR::deserialize(int16_t &short_t)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserialize(int16_t &short_t, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = deserialize(short_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserialize(int32_t &long_t)
{
    size_t align = m_cdrBuffer.align(sizeof(long_t));
    size_t sizeAligned = sizeof(long_t) + align;

    if(m_cdrBuffer.checkSpace(sizeAligned))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(long_t);

        // Align.
        m_cdrBuffer.makeAlign(align);
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserialize(int32_t &long_t, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = deserialize(long_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserialize(int64_t &longlong_t)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserialize(int64_t &longlong_t, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = deserialize(longlong_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserialize(float &float_t)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserialize(float &float_t, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = deserialize(float_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserialize(double &double_t)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserialize(double &double_t, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = deserialize(double_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserialize(bool &bool_t)
{
    uint8_t value = 0;

    if(m_cdrBuffer.checkSpace(sizeof(uint8_t)))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

        value = *m_cdrBuffer.m_currentPosition++;
        m_cdrBuffer.m_bufferLength -= sizeof(uint8_t);

        if(value == 1)
        {
            bool_t = true;
            return true;
        }
        else if(value == 0)
        {
            bool_t = false;
            return true;
        }
    }

    return false;
}

bool CDR::deserialize(std::string &string_t)
{
    bool returnedValue = true;
    uint32_t length = 0;

    returnedValue &= *this >> length;

    if(length == 0)
    {
        string_t = "";
    }
    else if(m_cdrBuffer.checkSpace(length))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(uint8_t);

        string_t = std::string(m_cdrBuffer.m_currentPosition, length - (m_cdrBuffer.m_currentPosition[length-1] == '\0' ? 1 : 0));
        m_cdrBuffer.m_currentPosition += length;
        m_cdrBuffer.m_bufferLength -= length;
    }

    return returnedValue;
}

bool CDR::deserialize(std::string &string_t, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = deserialize(string_t);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeArray(char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if(m_cdrBuffer.checkSpace(totalSize))
    {
        // Save last datasize.
        m_cdrBuffer.m_lastDataSize = sizeof(char_t);

        memcpy(char_t, m_cdrBuffer.m_currentPosition, totalSize);
        m_cdrBuffer.m_currentPosition += totalSize;
        m_cdrBuffer.m_bufferLength -= totalSize;
        return true;
    }

    return false;
}

bool CDR::deserializeArray(int16_t *short_t, size_t numElements)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserializeArray(int16_t *short_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = deserializeArray(short_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeArray(int32_t *long_t, size_t numElements)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserializeArray(int32_t *long_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = deserializeArray(long_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeArray(int64_t *longlong_t, size_t numElements)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserializeArray(int64_t *longlong_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = deserializeArray(longlong_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeArray(float *float_t, size_t numElements)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserializeArray(float *float_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = deserializeArray(float_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}

bool CDR::deserializeArray(double *double_t, size_t numElements)
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

        m_cdrBuffer.m_bufferLength -= sizeAligned;

        return true;
    }

    return false;
}

bool CDR::deserializeArray(double *double_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool auxSwap = m_cdrBuffer.m_swapBytes;
    m_cdrBuffer.m_swapBytes = (m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess == endianess)) || (!m_cdrBuffer.m_swapBytes && (m_cdrBuffer.m_endianess != endianess));
    bool returnedValue = deserializeArray(double_t, numElements);
    m_cdrBuffer.m_swapBytes = auxSwap;
    return returnedValue;
}
