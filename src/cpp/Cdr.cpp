#include "Cdr.h"

namespace eProsima
{
    CDR::CDR(CDRBuffer &cdrBuffer, const CdrType cdrType) : m_cdrBuffer(cdrBuffer),
        m_cdrType(cdrType), m_plFlag(DDS_CDR_WITHOUT_PL), m_options(0)
    {
    }

    inline
    bool CDR::checkSpace(size_t dataSize)
    {
        return (m_cdrBuffer.m_bufferLength - (m_cdrBuffer.m_currentPosition - m_cdrBuffer.m_buffer) >= dataSize);
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

    bool CDR::operator>>(uint8_t &octet)
    {
        bool returnedValue = false;

        if(checkSpace(sizeof(octet)))
        {
            octet = *m_cdrBuffer.m_currentPosition++;
            returnedValue = true;
        }

        return returnedValue;
    }

    bool CDR::operator>>(uint16_t &ushort)
    {
        bool returnedValue = false;

        if(checkSpace(sizeof(ushort)))
        {
            char *dst = reinterpret_cast<char*>(&ushort);

            if(!m_cdrBuffer.m_swapBytes)
            {    
                dst[0] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
            }
            else
            {
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[0] = *m_cdrBuffer.m_currentPosition++;
            }

            returnedValue = true;
        }

        return returnedValue;
    }

    inline
    bool CDR::operator>>(uint32_t &ulong)
    {
        return deserialize(ulong);
    }

    bool CDR::deserialize(uint32_t &ulong, CDRBuffer::Endianess endianess)
    {
        bool returnedValue = false;

        if(checkSpace(sizeof(ulong)))
        {
            char *dst = reinterpret_cast<char*>(&ulong);

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
                dst[0] = *m_cdrBuffer.m_currentPosition++;
                dst[1] = *m_cdrBuffer.m_currentPosition++;
                dst[2] = *m_cdrBuffer.m_currentPosition++;
                dst[3] = *m_cdrBuffer.m_currentPosition++;
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
            string = std::string(m_cdrBuffer.m_currentPosition, length);
            m_cdrBuffer.m_currentPosition += length;
        }

        return returnedValue;
    }
};