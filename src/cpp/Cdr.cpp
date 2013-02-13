#include "Cdr.h"

namespace eProsima
{
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

        if(checkSpace(sizeof(numBytes)))
        {
            m_cdrBuffer.m_currentPosition += numBytes;
            returnedValue = true;
        }

        return returnedValue;
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

    bool CDR::deserialize(uint8_t &octet_t, CDRBuffer::Endianess endianess)
    {
        bool returnedValue = false;

        if(checkSpace(sizeof(octet_t)))
        {
            octet_t = *m_cdrBuffer.m_currentPosition++;
            returnedValue = true;
        }

        return returnedValue;
    }

    bool CDR::deserialize(char &char_t, CDRBuffer::Endianess endianess)
    {
        bool returnedValue = false;

        if(checkSpace(sizeof(char_t)))
        {
            char_t = *m_cdrBuffer.m_currentPosition++;
            returnedValue = true;
        }

        return returnedValue;
    }

    bool CDR::deserialize(uint16_t &ushort_t, CDRBuffer::Endianess endianess)
    {
        bool returnedValue = false;

        if(checkSpace(sizeof(ushort_t)))
        {
            makeAlign(sizeof(ushort_t));
            char *dst = reinterpret_cast<char*>(&ushort_t);

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

    bool CDR::deserialize(int16_t &short_t, CDRBuffer::Endianess endianess)
    {
        bool returnedValue = false;

        if(checkSpace(sizeof(short_t)))
        {
            makeAlign(sizeof(short_t));
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

    bool CDR::deserialize(uint32_t &ulong_t, CDRBuffer::Endianess endianess)
    {
        bool returnedValue = false;

        if(checkSpace(sizeof(ulong_t)))
        {
            makeAlign(sizeof(ulong_t));
            char *dst = reinterpret_cast<char*>(&ulong_t);

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

    bool CDR::deserialize(int32_t &long_t, CDRBuffer::Endianess endianess)
    {
        bool returnedValue = false;

        if(checkSpace(sizeof(long_t)))
        {
            makeAlign(sizeof(long_t));
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

    bool CDR::deserialize(uint64_t &ulonglong_t, CDRBuffer::Endianess endianess)
    {
        bool returnedValue = false;

        if(checkSpace(sizeof(ulonglong_t)))
        {
            makeAlign(sizeof(ulonglong_t));
            char *dst = reinterpret_cast<char*>(&ulonglong_t);

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

    bool CDR::deserialize(int64_t &longlong_t, CDRBuffer::Endianess endianess)
    {
        bool returnedValue = false;

        if(checkSpace(sizeof(longlong_t)))
        {
            makeAlign(sizeof(longlong_t));
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

        if(checkSpace(sizeof(float_t)))
        {
            makeAlign(sizeof(float_t));
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

        if(checkSpace(sizeof(double_t)))
        {
            makeAlign(sizeof(double_t));
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

    bool CDR::operator>>(std::string &string)
    {
        bool returnedValue = true;
        uint32_t length = 0;

        returnedValue &= *this >> length;

        if(length == 0)
        {
            string = "";
        }
        else
        {
            string = std::string(m_cdrBuffer.m_currentPosition, length - (m_cdrBuffer.m_currentPosition[length-1] == '\0' ? 1 : 0));
            m_cdrBuffer.m_currentPosition += length;
        }

        return returnedValue;
    }
};