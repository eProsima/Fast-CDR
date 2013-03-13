#include "Cdr.h"
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

bool CDR::serialize(const char char_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(char_t)))
    {
        *m_cdrBuffer.m_currentPosition++ = char_t;
        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::serialize(const int16_t short_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(short_t)))
    {
        m_cdrBuffer.makeAlign(sizeof(short_t));
        const char *dst = reinterpret_cast<const char*>(&short_t);

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {    
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            // TODO Improve
            *m_cdrBuffer.m_currentPosition++ = dst[0];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::serialize(const int32_t long_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(long_t)))
    {
        m_cdrBuffer.makeAlign(sizeof(long_t));
        const char *dst = reinterpret_cast<const char*>(&long_t);

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            // TODO Improve
            *m_cdrBuffer.m_currentPosition++ = dst[0];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[3];
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::serialize(const int64_t longlong_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(longlong_t)))
    {
        m_cdrBuffer.makeAlign(sizeof(longlong_t));
        const char *dst = reinterpret_cast<const char*>(&longlong_t);

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
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
            // TODO Improve
            *m_cdrBuffer.m_currentPosition++ = dst[0];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[4];
            *m_cdrBuffer.m_currentPosition++ = dst[5];
            *m_cdrBuffer.m_currentPosition++ = dst[6];
            *m_cdrBuffer.m_currentPosition++ = dst[7];
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::serialize(const float float_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(float_t)))
    {
        m_cdrBuffer.makeAlign(sizeof(float_t));
        const char *dst = reinterpret_cast<const char*>(&float_t);

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[0];
        }
        else
        {
            // TODO Improve
            *m_cdrBuffer.m_currentPosition++ = dst[0];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[3];
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::serialize(const double double_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(double_t)))
    {
        m_cdrBuffer.makeAlign(sizeof(double_t));
        const char *dst = reinterpret_cast<const char*>(&double_t);

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
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
            // TODO Improve
            *m_cdrBuffer.m_currentPosition++ = dst[0];
            *m_cdrBuffer.m_currentPosition++ = dst[1];
            *m_cdrBuffer.m_currentPosition++ = dst[2];
            *m_cdrBuffer.m_currentPosition++ = dst[3];
            *m_cdrBuffer.m_currentPosition++ = dst[4];
            *m_cdrBuffer.m_currentPosition++ = dst[5];
            *m_cdrBuffer.m_currentPosition++ = dst[6];
            *m_cdrBuffer.m_currentPosition++ = dst[7];
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::serialize(const bool bool_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;
    uint8_t value = 0;

    if(m_cdrBuffer.checkSpace(sizeof(bool_t)))
    {
        if(bool_t)
            value = 1;
        *m_cdrBuffer.m_currentPosition++ = value;

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::serialize(const std::string &string_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = true;
    uint32_t length = (uint32_t)string_t.length();

    returnedValue &= *this << length;

    if(length > 0)
    {
        memcpy(m_cdrBuffer.m_currentPosition, string_t.c_str(), length);
        m_cdrBuffer.m_currentPosition += length;
    }

    return returnedValue;
}

bool CDR::serializeArray(const char *char_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(*char_t), numElements))
    {
        memcpy(m_cdrBuffer.m_currentPosition, char_t, sizeof(*char_t)*numElements);
        m_cdrBuffer.m_currentPosition += sizeof(*char_t)*numElements;
        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::serializeArray(const int16_t *short_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(*short_t), numElements))
    {
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(sizeof(*short_t));

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            const char *dst = reinterpret_cast<const char*>(&short_t);
            const char *end = dst + (sizeof(*short_t) * numElements);

            for(; dst < end; dst += sizeof(*short_t))
            {
                *m_cdrBuffer.m_currentPosition++ = dst[1];
                *m_cdrBuffer.m_currentPosition++ = dst[0];
            }
        }
        else
        {
            memcpy(m_cdrBuffer.m_currentPosition, short_t, sizeof(*short_t)*numElements);
            m_cdrBuffer.m_currentPosition += sizeof(*short_t)*numElements;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::serializeArray(const int32_t *long_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(*long_t), numElements))
    {
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(sizeof(*long_t));

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            const char *dst = reinterpret_cast<const char*>(&long_t);
            const char *end = dst + (sizeof(*long_t) * numElements);

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
            memcpy(m_cdrBuffer.m_currentPosition, long_t, sizeof(*long_t)*numElements);
            m_cdrBuffer.m_currentPosition += sizeof(*long_t)*numElements;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::serializeArray(const int64_t *longlong_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(*longlong_t), numElements))
    {
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(sizeof(*longlong_t));

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            const char *dst = reinterpret_cast<const char*>(&longlong_t);
            const char *end = dst + (sizeof(*longlong_t) * numElements);

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
            memcpy(m_cdrBuffer.m_currentPosition, longlong_t, sizeof(*longlong_t)*numElements);
            m_cdrBuffer.m_currentPosition += sizeof(*longlong_t)*numElements;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::serializeArray(const float *float_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(*float_t), numElements))
    {
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(sizeof(*float_t));

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            const char *dst = reinterpret_cast<const char*>(&float_t);
            const char *end = dst + (sizeof(*float_t) * numElements);

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
            memcpy(m_cdrBuffer.m_currentPosition, float_t, sizeof(*float_t)*numElements);
            m_cdrBuffer.m_currentPosition += sizeof(*float_t)*numElements;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::serializeArray(const double *double_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(*double_t), numElements))
    {
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(sizeof(*double_t));

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            const char *dst = reinterpret_cast<const char*>(&double_t);
            const char *end = dst + (sizeof(*double_t) * numElements);

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
            memcpy(m_cdrBuffer.m_currentPosition, double_t, sizeof(*double_t)*numElements);
            m_cdrBuffer.m_currentPosition += sizeof(*double_t)*numElements;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::deserialize(char &char_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(char_t)))
    {
        char_t = *m_cdrBuffer.m_currentPosition++;
        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::deserialize(int16_t &short_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(short_t)))
    {
        m_cdrBuffer.makeAlign(sizeof(short_t));
        char *dst = reinterpret_cast<char*>(&short_t);

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {    
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            // TODO Improve
            dst[0] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::deserialize(int32_t &long_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(long_t)))
    {
        m_cdrBuffer.makeAlign(sizeof(long_t));
        char *dst = reinterpret_cast<char*>(&long_t);

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            // TODO Improve
            dst[0] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[3] = *m_cdrBuffer.m_currentPosition++;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::deserialize(int64_t &longlong_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(longlong_t)))
    {
        m_cdrBuffer.makeAlign(sizeof(longlong_t));
        char *dst = reinterpret_cast<char*>(&longlong_t);

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
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
            // TODO Improve
            dst[0] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[4] = *m_cdrBuffer.m_currentPosition++;
            dst[5] = *m_cdrBuffer.m_currentPosition++;
            dst[6] = *m_cdrBuffer.m_currentPosition++;
            dst[7] = *m_cdrBuffer.m_currentPosition++;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::deserialize(float &float_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(float_t)))
    {
        m_cdrBuffer.makeAlign(sizeof(float_t));
        char *dst = reinterpret_cast<char*>(&float_t);

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[0] = *m_cdrBuffer.m_currentPosition++;
        }
        else
        {
            // TODO Improve
            dst[0] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[3] = *m_cdrBuffer.m_currentPosition++;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::deserialize(double &double_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(double_t)))
    {
        m_cdrBuffer.makeAlign(sizeof(double_t));
        char *dst = reinterpret_cast<char*>(&double_t);

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
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
            // TODO Improve
            dst[0] = *m_cdrBuffer.m_currentPosition++;
            dst[1] = *m_cdrBuffer.m_currentPosition++;
            dst[2] = *m_cdrBuffer.m_currentPosition++;
            dst[3] = *m_cdrBuffer.m_currentPosition++;
            dst[4] = *m_cdrBuffer.m_currentPosition++;
            dst[5] = *m_cdrBuffer.m_currentPosition++;
            dst[6] = *m_cdrBuffer.m_currentPosition++;
            dst[7] = *m_cdrBuffer.m_currentPosition++;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::deserialize(bool &bool_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;
    uint8_t value = 0;

    if(m_cdrBuffer.checkSpace(sizeof(bool_t)))
    {
        value = *m_cdrBuffer.m_currentPosition++;

        if(value == 1)
        {
            bool_t = true;
            returnedValue = true;
        }
        else if(value == 0)
        {
            bool_t = false;
            returnedValue = true;
        }
    }

    return returnedValue;
}

bool CDR::deserialize(std::string &string_t, CDRBuffer::Endianess endianess)
{
    bool returnedValue = true;
    uint32_t length = 0;

    returnedValue &= *this >> length;

    if(length == 0)
    {
        string_t = "";
    }
    else
    {
        string_t = std::string(m_cdrBuffer.m_currentPosition, length - (m_cdrBuffer.m_currentPosition[length-1] == '\0' ? 1 : 0));
        m_cdrBuffer.m_currentPosition += length;
    }

    return returnedValue;
}

bool CDR::deserializeArray(char *char_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(*char_t), numElements))
    {
        memcpy(char_t, m_cdrBuffer.m_currentPosition, sizeof(*char_t)*numElements);
        m_cdrBuffer.m_currentPosition += sizeof(*char_t)*numElements;
        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::deserializeArray(int16_t *short_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(*short_t), numElements))
    {
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(sizeof(*short_t));

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            char *dst = reinterpret_cast<char*>(&short_t);
            char *end = dst + (sizeof(*short_t) * numElements);

            for(; dst < end; dst += sizeof(*short_t))
            {
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }
        }
        else
        {
            memcpy(short_t, m_cdrBuffer.m_currentPosition, sizeof(*short_t)*numElements);
            m_cdrBuffer.m_currentPosition += sizeof(*short_t)*numElements;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::deserializeArray(int32_t *long_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(*long_t), numElements))
    {
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(sizeof(*long_t));

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            char *dst = reinterpret_cast<char*>(&long_t);
            char *end = dst + (sizeof(*long_t) * numElements);

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
            memcpy(long_t, m_cdrBuffer.m_currentPosition, sizeof(*long_t)*numElements);
            m_cdrBuffer.m_currentPosition += sizeof(*long_t)*numElements;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::deserializeArray(int64_t *longlong_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(*longlong_t), numElements))
    {
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(sizeof(*longlong_t));

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            char *dst = reinterpret_cast<char*>(&longlong_t);
            char *end = dst + (sizeof(*longlong_t) * numElements);

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
            memcpy(longlong_t, m_cdrBuffer.m_currentPosition, sizeof(*longlong_t)*numElements);
            m_cdrBuffer.m_currentPosition += sizeof(*longlong_t)*numElements;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::deserializeArray(float *float_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(*float_t), numElements))
    {
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(sizeof(*float_t));

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            char *dst = reinterpret_cast<char*>(&float_t);
            char *end = dst + (sizeof(*float_t) * numElements);

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
            memcpy(float_t, m_cdrBuffer.m_currentPosition, sizeof(*float_t)*numElements);
            m_cdrBuffer.m_currentPosition += sizeof(*float_t)*numElements;
        }

        returnedValue = true;
    }

    return returnedValue;
}

bool CDR::deserializeArray(double *double_t, size_t numElements, CDRBuffer::Endianess endianess)
{
    bool returnedValue = false;

    if(m_cdrBuffer.checkSpace(sizeof(*double_t), numElements))
    {
        // TODO Creo que hay casos que hay que alinear, pero DDS no lo hace. Hay que ver si CORBA si alinea.
        m_cdrBuffer.makeAlign(sizeof(*double_t));

        if((m_cdrBuffer == endianess && m_cdrBuffer.m_swapBytes) ||
            (m_cdrBuffer != endianess && !m_cdrBuffer.m_swapBytes))
        {
            char *dst = reinterpret_cast<char*>(&double_t);
            char *end = dst + (sizeof(*double_t) * numElements);

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
            memcpy(double_t, m_cdrBuffer.m_currentPosition, sizeof(*double_t)*numElements);
            m_cdrBuffer.m_currentPosition += sizeof(*double_t)*numElements;
        }

        returnedValue = true;
    }

    return returnedValue;
}
