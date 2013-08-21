#include "cpp/cdr/Cdr.h"
#include "cpp/exceptions/NotEnoughMemoryException.h"
#include "cpp/exceptions/BadParamException.h"

using namespace eProsima::marshalling;
using namespace eProsima::storage;

#if defined(__LITTLE_ENDIAN__)
const Cdr::Endianness Cdr::DEFAULT_ENDIAN = LITTLE_ENDIANNESS;
#elif defined (__BIG_ENDIAN__)
const Cdr::Endianness Cdr::DEFAULT_ENDIAN = BIG_ENDIANNESS;
#else
#error "It's not defined the endianness."
#endif

const std::string Cdr::BAD_PARAM_MESSAGE_DEFAULT("Bad parameter");
const std::string Cdr::NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT("Not enough memory in the buffer stream");

Cdr::state::state(Cdr &cdr) : m_currentPosition(cdr.m_storage.clone(cdr.m_currentPosition)), m_alignPosition(cdr.m_storage.clone(cdr.m_alignPosition)),
    m_swapBytes(cdr.m_swapBytes), m_lastDataSize(cdr.m_lastDataSize) {}

Cdr::state::~state()
{
    delete m_currentPosition;
    delete m_alignPosition;
}

Cdr::Cdr(Storage &storage, const Endianness endianness, const CdrType cdrType) : m_storage(storage),
    m_cdrType(cdrType), m_plFlag(DDS_CDR_WITHOUT_PL), m_options(0), m_endianness(endianness),
    m_swapBytes(endianness == DEFAULT_ENDIAN ? false : true), m_lastDataSize(0), m_currentPosition(storage.begin()),
    m_alignPosition(storage.begin()), m_lastPosition(storage.end())
{
}

Cdr::~Cdr()
{
    delete m_currentPosition;
    delete m_alignPosition;
    delete m_lastPosition;
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
            m_endianness = encapsulationKind;
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
            throw BadParamException(BAD_PARAM_MESSAGE_DEFAULT);
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

Cdr::DDSCdrPlFlag Cdr::getDDSCdrPlFlag() const
{
    return m_plFlag;
}

uint16_t Cdr::getDDSCdrOptions() const
{
    return m_options;
}

bool Cdr::jump(uint32_t numBytes)
{
    bool returnedValue = false;

    if(m_lastPosition->substract(m_currentPosition) >= sizeof(numBytes))
    {
        m_currentPosition->increasePosition(numBytes);
        returnedValue = true;
    }

    return returnedValue;
}

size_t Cdr::getSerializedDataLength() const
{
    Storage::iterator *tmp = m_storage.begin();
    size_t returnedSize =  m_currentPosition->substract(tmp);
    delete tmp;
    return returnedSize;
}

Cdr::state Cdr::getState()
{
    return Cdr::state(*this);
}

void Cdr::setState(state &state)
{
    m_currentPosition->copyPosition(state.m_currentPosition);
    m_alignPosition->copyPosition(state.m_alignPosition);
    m_swapBytes = state.m_swapBytes;
    m_lastDataSize = state.m_lastDataSize;
}

void Cdr::reset()
{
    delete m_currentPosition;
    m_currentPosition = m_storage.begin();
    delete m_alignPosition;
    m_alignPosition = m_storage.begin();
    m_swapBytes = m_endianness == DEFAULT_ENDIAN ? false : true;
    m_lastDataSize = 0;
}

bool Cdr::resize(size_t minSizeInc)
{
    if(m_storage.resize(minSizeInc))
    {
        Storage::iterator *tmp = m_storage.begin();

        m_currentPosition->copyStorage(tmp);
        m_alignPosition->copyStorage(tmp);
        delete m_lastPosition;
        m_lastPosition = m_storage.end();

        delete tmp;
        return true;
    }

    return false;
}

Marshalling& Cdr::serialize(const char char_t)
{
    if((m_lastPosition->substract(m_currentPosition) >= sizeof(char_t)) || resize(sizeof(char_t)))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(char_t);

        m_storage.insert(m_currentPosition, char_t);
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Marshalling& Cdr::serialize(const int16_t short_t)
{
    size_t align = alignment(sizeof(short_t));
    size_t sizeAligned = sizeof(short_t) + align;

    if((m_lastPosition->substract(m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(short_t);

        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {    
            const char *dst = reinterpret_cast<const char*>(&short_t);

            m_storage.insert(m_currentPosition, dst[1]);
            m_storage.insert(m_currentPosition, dst[0]);
        }
        else
        {
            m_storage.insert(m_currentPosition, short_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::serialize(const int32_t long_t)
{
    size_t align = alignment(sizeof(long_t));
    size_t sizeAligned = sizeof(long_t) + align;

    if((m_lastPosition->substract(m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(long_t);

        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&long_t);

            m_storage.insert(m_currentPosition, dst[3]);
            m_storage.insert(m_currentPosition, dst[2]);
            m_storage.insert(m_currentPosition, dst[1]);
            m_storage.insert(m_currentPosition, dst[0]);
        }
        else
        {
            m_storage.insert(m_currentPosition, long_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::serialize(const int64_t longlong_t)
{
    size_t align = alignment(sizeof(longlong_t));
    size_t sizeAligned = sizeof(longlong_t) + align;

    if((m_lastPosition->substract(m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(longlong_t);

        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&longlong_t);

            m_storage.insert(m_currentPosition, dst[7]);
            m_storage.insert(m_currentPosition, dst[6]);
            m_storage.insert(m_currentPosition, dst[5]);
            m_storage.insert(m_currentPosition, dst[4]);
            m_storage.insert(m_currentPosition, dst[3]);
            m_storage.insert(m_currentPosition, dst[2]);
            m_storage.insert(m_currentPosition, dst[1]);
            m_storage.insert(m_currentPosition, dst[0]);
        }
        else
        {
            m_storage.insert(m_currentPosition, longlong_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::serialize(const float float_t)
{
    size_t align = alignment(sizeof(float_t));
    size_t sizeAligned = sizeof(float_t) + align;

    if((m_lastPosition->substract(m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(float_t);

        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&float_t);

            m_storage.insert(m_currentPosition, dst[3]);
            m_storage.insert(m_currentPosition, dst[2]);
            m_storage.insert(m_currentPosition, dst[1]);
            m_storage.insert(m_currentPosition, dst[0]);
        }
        else
        {
            m_storage.insert(m_currentPosition, float_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::serialize(const double double_t)
{
    size_t align = alignment(sizeof(double_t));
    size_t sizeAligned = sizeof(double_t) + align;

    if((m_lastPosition->substract(m_currentPosition) >= sizeAligned) || resize(sizeAligned))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(double_t);

        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            const char *dst = reinterpret_cast<const char*>(&double_t);

            m_storage.insert(m_currentPosition, dst[7]);
            m_storage.insert(m_currentPosition, dst[6]);
            m_storage.insert(m_currentPosition, dst[5]);
            m_storage.insert(m_currentPosition, dst[4]);
            m_storage.insert(m_currentPosition, dst[3]);
            m_storage.insert(m_currentPosition, dst[2]);
            m_storage.insert(m_currentPosition, dst[1]);
            m_storage.insert(m_currentPosition, dst[0]);
        }
        else
        {
            m_storage.insert(m_currentPosition, double_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::serialize(const bool bool_t)
{
    uint8_t value = 0;

    if((m_lastPosition->substract(m_currentPosition) >= sizeof(uint8_t)) || resize(sizeof(uint8_t)))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(uint8_t);

        if(bool_t)
            value = 1;
        m_storage.insert(m_currentPosition, value);

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Marshalling& Cdr::serialize(const std::string &string_t)
{
    uint32_t length = (uint32_t)string_t.length();
    state state(*this);

    *this << length;

    if(length > 0)
    {
        if((m_lastPosition->substract(m_currentPosition) >= length) || resize(length))
        {
            // Save last datasize.
            m_lastDataSize = sizeof(uint8_t);

            m_storage.insert(m_currentPosition, string_t);
        }
        else
        {
            setState(state);
            throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
        }
    }

    return *this;
}

Cdr& Cdr::serialize(const std::string &string_t, Endianness endianness)
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

Marshalling& Cdr::serialize(const std::vector<uint8_t> &vector_t)
{
    Cdr::state state(*this);

    serialize((int32_t)vector_t.size());

    try
    {
        return serializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(const std::vector<uint8_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::serialize(const std::vector<char> &vector_t)
{
    Cdr::state state(*this);

    serialize((int32_t)vector_t.size());

    try
    {
        return serializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(const std::vector<char> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::serialize(const std::vector<uint16_t> &vector_t)
{
    Cdr::state state(*this);

    serialize((int32_t)vector_t.size());

    try
    {
        return serializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(const std::vector<uint16_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::serialize(const std::vector<int16_t> &vector_t)
{
    Cdr::state state(*this);

    serialize((int32_t)vector_t.size());

    try
    {
        return serializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(const std::vector<int16_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::serialize(const std::vector<uint32_t> &vector_t)
{
    Cdr::state state(*this);

    serialize((int32_t)vector_t.size());

    try
    {
        return serializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(const std::vector<uint32_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::serialize(const std::vector<int32_t> &vector_t)
{
    Cdr::state state(*this);

    serialize((int32_t)vector_t.size());

    try
    {
        return serializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(const std::vector<int32_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::serialize(const std::vector<uint64_t> &vector_t)
{
    Cdr::state state(*this);

    serialize((int32_t)vector_t.size());

    try
    {
        return serializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(const std::vector<uint64_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::serialize(const std::vector<int64_t> &vector_t)
{
    Cdr::state state(*this);

    serialize((int32_t)vector_t.size());

    try
    {
        return serializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(const std::vector<int64_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::serialize(const std::vector<float> &vector_t)
{
    Cdr::state state(*this);

    serialize((int32_t)vector_t.size());

    try
    {
        return serializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(const std::vector<float> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::serialize(const std::vector<double> &vector_t)
{
    Cdr::state state(*this);

    serialize((int32_t)vector_t.size());

    try
    {
        return serializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::serialize(const std::vector<double> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        serialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::serializeArray(const char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if((m_lastPosition->substract(m_currentPosition) >= totalSize) || resize(totalSize))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*char_t);

        m_storage.memcopy(m_currentPosition, char_t, totalSize);
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Marshalling& Cdr::serializeArray(const int16_t *short_t, size_t numElements)
{
    size_t align = alignment(sizeof(*short_t));
    size_t totalSize = sizeof(*short_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if((m_lastPosition->substract(m_currentPosition) >= sizeAligned) || resize(sizeAligned))
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
                m_storage.insert(m_currentPosition, dst[1]);
                m_storage.insert(m_currentPosition, dst[0]);
            }
        }
        else
        {
            m_storage.memcopy(m_currentPosition, short_t, totalSize);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::serializeArray(const int32_t *long_t, size_t numElements)
{
    size_t align = alignment(sizeof(*long_t));
    size_t totalSize = sizeof(*long_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if((m_lastPosition->substract(m_currentPosition) >= sizeAligned) || resize(sizeAligned))
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
                m_storage.insert(m_currentPosition, dst[3]);
                m_storage.insert(m_currentPosition, dst[2]);
                m_storage.insert(m_currentPosition, dst[1]);
                m_storage.insert(m_currentPosition, dst[0]);
            }
        }
        else
        {
            m_storage.memcopy(m_currentPosition, long_t, totalSize);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::serializeArray(const int64_t *longlong_t, size_t numElements)
{
    size_t align = alignment(sizeof(*longlong_t));
    size_t totalSize = sizeof(*longlong_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if((m_lastPosition->substract(m_currentPosition) >= sizeAligned) || resize(sizeAligned))
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
                m_storage.insert(m_currentPosition, dst[7]);
                m_storage.insert(m_currentPosition, dst[6]);
                m_storage.insert(m_currentPosition, dst[5]);
                m_storage.insert(m_currentPosition, dst[4]);
                m_storage.insert(m_currentPosition, dst[3]);
                m_storage.insert(m_currentPosition, dst[2]);
                m_storage.insert(m_currentPosition, dst[1]);
                m_storage.insert(m_currentPosition, dst[0]);
            }
        }
        else
        {
            m_storage.memcopy(m_currentPosition, longlong_t, totalSize);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::serializeArray(const float *float_t, size_t numElements)
{
    size_t align = alignment(sizeof(*float_t));
    size_t totalSize = sizeof(*float_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if((m_lastPosition->substract(m_currentPosition) >= sizeAligned) || resize(sizeAligned))
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
                m_storage.insert(m_currentPosition, dst[3]);
                m_storage.insert(m_currentPosition, dst[2]);
                m_storage.insert(m_currentPosition, dst[1]);
                m_storage.insert(m_currentPosition, dst[0]);
            }
        }
        else
        {
            m_storage.memcopy(m_currentPosition, float_t, totalSize);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::serializeArray(const double *double_t, size_t numElements)
{
    size_t align = alignment(sizeof(*double_t));
    size_t totalSize = sizeof(*double_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if((m_lastPosition->substract(m_currentPosition) >= sizeAligned) || resize(sizeAligned))
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
                m_storage.insert(m_currentPosition, dst[7]);
                m_storage.insert(m_currentPosition, dst[6]);
                m_storage.insert(m_currentPosition, dst[5]);
                m_storage.insert(m_currentPosition, dst[4]);
                m_storage.insert(m_currentPosition, dst[3]);
                m_storage.insert(m_currentPosition, dst[2]);
                m_storage.insert(m_currentPosition, dst[1]);
                m_storage.insert(m_currentPosition, dst[0]);
            }
        }
        else
        {
            m_storage.memcopy(m_currentPosition, double_t, totalSize);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::deserialize(char &char_t)
{
    if(m_lastPosition->substract(m_currentPosition) >= sizeof(char_t))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(char_t);

        m_storage.get(m_currentPosition, char_t);
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Marshalling& Cdr::deserialize(int16_t &short_t)
{
    size_t align = alignment(sizeof(short_t));
    size_t sizeAligned = sizeof(short_t) + align;

    if(m_lastPosition->substract(m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(short_t);

        // Align
        makeAlign(align);

        if(m_swapBytes)
        {    
            char *dst = reinterpret_cast<char*>(&short_t);

            m_storage.get(m_currentPosition, dst[1]);
            m_storage.get(m_currentPosition, dst[0]);
        }
        else
        {
            m_storage.get(m_currentPosition, short_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::deserialize(int32_t &long_t)
{
    size_t align = alignment(sizeof(long_t));
    size_t sizeAligned = sizeof(long_t) + align;

    if(m_lastPosition->substract(m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(long_t);

        // Align
        makeAlign(align);

        if(m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&long_t);

            m_storage.get(m_currentPosition, dst[3]);
            m_storage.get(m_currentPosition, dst[2]);
            m_storage.get(m_currentPosition, dst[1]);
            m_storage.get(m_currentPosition, dst[0]);
        }
        else
        {
            m_storage.get(m_currentPosition, long_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::deserialize(int64_t &longlong_t)
{
    size_t align = alignment(sizeof(longlong_t));
    size_t sizeAligned = sizeof(longlong_t) + align;

    if(m_lastPosition->substract(m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(longlong_t);

        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&longlong_t);

            m_storage.get(m_currentPosition, dst[7]);
            m_storage.get(m_currentPosition, dst[6]);
            m_storage.get(m_currentPosition, dst[5]);
            m_storage.get(m_currentPosition, dst[4]);
            m_storage.get(m_currentPosition, dst[3]);
            m_storage.get(m_currentPosition, dst[2]);
            m_storage.get(m_currentPosition, dst[1]);
            m_storage.get(m_currentPosition, dst[0]);
        }
        else
        {
            m_storage.get(m_currentPosition, longlong_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::deserialize(float &float_t)
{
    size_t align = alignment(sizeof(float_t));
    size_t sizeAligned = sizeof(float_t) + align;

    if(m_lastPosition->substract(m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(float_t);

        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&float_t);

            m_storage.get(m_currentPosition, dst[3]);
            m_storage.get(m_currentPosition, dst[2]);
            m_storage.get(m_currentPosition, dst[1]);
            m_storage.get(m_currentPosition, dst[0]);
        }
        else
        {
            m_storage.get(m_currentPosition, float_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::deserialize(double &double_t)
{
    size_t align = alignment(sizeof(double_t));
    size_t sizeAligned = sizeof(double_t) + align;

    if(m_lastPosition->substract(m_currentPosition) >= sizeAligned)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(double_t);

        // Align.
        makeAlign(align);

        if(m_swapBytes)
        {
            char *dst = reinterpret_cast<char*>(&double_t);

            m_storage.get(m_currentPosition, dst[7]);
            m_storage.get(m_currentPosition, dst[6]);
            m_storage.get(m_currentPosition, dst[5]);
            m_storage.get(m_currentPosition, dst[4]);
            m_storage.get(m_currentPosition, dst[3]);
            m_storage.get(m_currentPosition, dst[2]);
            m_storage.get(m_currentPosition, dst[1]);
            m_storage.get(m_currentPosition, dst[0]);
        }
        else
        {
            m_storage.get(m_currentPosition, double_t);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::deserialize(bool &bool_t)
{
    uint8_t value = 0;

    if(m_lastPosition->substract(m_currentPosition) >= sizeof(uint8_t))
    {
        // Save last datasize.
        m_lastDataSize = sizeof(uint8_t);

        m_storage.get(m_currentPosition, value);

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

        throw BadParamException(BAD_PARAM_MESSAGE_DEFAULT);
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Marshalling& Cdr::deserialize(std::string &string_t)
{
    uint32_t length = 0;
    state state(*this);

    *this >> length;

    if(length == 0)
    {
        string_t = "";
        return *this;
    }
    else if(m_lastPosition->substract(m_currentPosition) >= length)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(uint8_t);

        m_storage.get(m_currentPosition, string_t, length);

        return *this;
    }

    setState(state);
    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Cdr& Cdr::deserialize(std::string &string_t, Endianness endianness)
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

Marshalling& Cdr::deserialize(std::vector<uint8_t> &vector_t)
{
    uint32_t seqLength = 0;
    Cdr::state state(*this);

    deserialize(seqLength);

    try
    {
        vector_t.resize(seqLength);
        return deserializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(std::vector<uint8_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::deserialize(std::vector<char> &vector_t)
{
    uint32_t seqLength = 0;
    Cdr::state state(*this);

    deserialize(seqLength);

    try
    {
        vector_t.resize(seqLength);
        return deserializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(std::vector<char> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::deserialize(std::vector<uint16_t> &vector_t)
{
    uint32_t seqLength = 0;
    Cdr::state state(*this);

    deserialize(seqLength);

    try
    {
        vector_t.resize(seqLength);
        return deserializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(std::vector<uint16_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::deserialize(std::vector<int16_t> &vector_t)
{
    uint32_t seqLength = 0;
    Cdr::state state(*this);

    deserialize(seqLength);

    try
    {
        vector_t.resize(seqLength);
        return deserializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(std::vector<int16_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::deserialize(std::vector<uint32_t> &vector_t)
{
    uint32_t seqLength = 0;
    Cdr::state state(*this);

    deserialize(seqLength);

    try
    {
        vector_t.resize(seqLength);
        return deserializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(std::vector<uint32_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::deserialize(std::vector<int32_t> &vector_t)
{
    uint32_t seqLength = 0;
    Cdr::state state(*this);

    deserialize(seqLength);

    try
    {
        vector_t.resize(seqLength);
        return deserializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(std::vector<int32_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::deserialize(std::vector<uint64_t> &vector_t)
{
    uint32_t seqLength = 0;
    Cdr::state state(*this);

    deserialize(seqLength);

    try
    {
        vector_t.resize(seqLength);
        return deserializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(std::vector<uint64_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::deserialize(std::vector<int64_t> &vector_t)
{
    uint32_t seqLength = 0;
    Cdr::state state(*this);

    deserialize(seqLength);

    try
    {
        vector_t.resize(seqLength);
        return deserializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(std::vector<int64_t> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::deserialize(std::vector<float> &vector_t)
{
    uint32_t seqLength = 0;
    Cdr::state state(*this);

    deserialize(seqLength);

    try
    {
        vector_t.resize(seqLength);
        return deserializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(std::vector<float> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::deserialize(std::vector<double> &vector_t)
{
    uint32_t seqLength = 0;
    Cdr::state state(*this);

    deserialize(seqLength);

    try
    {
        vector_t.resize(seqLength);
        return deserializeArray(vector_t.data(), vector_t.size());
    }
    catch(Exception &ex)
    {
        setState(state);
        ex.raise();
    }

    return *this;
}

Cdr& Cdr::deserialize(std::vector<double> &vector_t, Endianness endianness)
{
    bool auxSwap = m_swapBytes;
    m_swapBytes = (m_swapBytes && (m_endianness == endianness)) || (!m_swapBytes && (m_endianness != endianness));

    try
    {
        deserialize(vector_t);
        m_swapBytes = auxSwap;
    }
    catch(Exception &ex)
    {
        m_swapBytes = auxSwap;
        ex.raise();
    }

    return *this;
}

Marshalling& Cdr::deserializeArray(char *char_t, size_t numElements)
{
    size_t totalSize = sizeof(*char_t)*numElements;

    if(m_lastPosition->substract(m_currentPosition) >= totalSize)
    {
        // Save last datasize.
        m_lastDataSize = sizeof(*char_t);

        m_storage.rmemcopy(m_currentPosition, char_t, totalSize);
        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
}

Marshalling& Cdr::deserializeArray(int16_t *short_t, size_t numElements)
{
    size_t align = alignment(sizeof(*short_t));
    size_t totalSize = sizeof(*short_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_lastPosition->substract(m_currentPosition) >= sizeAligned)
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
                m_storage.get(m_currentPosition, dst[1]);
                m_storage.get(m_currentPosition, dst[0]);
            }
        }
        else
        {
            m_storage.rmemcopy(m_currentPosition, short_t, totalSize);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::deserializeArray(int32_t *long_t, size_t numElements)
{
    size_t align = alignment(sizeof(*long_t));
    size_t totalSize = sizeof(*long_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_lastPosition->substract(m_currentPosition) >= sizeAligned)
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
                m_storage.get(m_currentPosition, dst[3]);
                m_storage.get(m_currentPosition, dst[2]);
                m_storage.get(m_currentPosition, dst[1]);
                m_storage.get(m_currentPosition, dst[0]);
            }
        }
        else
        {
            m_storage.rmemcopy(m_currentPosition, long_t, totalSize);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::deserializeArray(int64_t *longlong_t, size_t numElements)
{
    size_t align = alignment(sizeof(*longlong_t));
    size_t totalSize = sizeof(*longlong_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_lastPosition->substract(m_currentPosition) >= sizeAligned)
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
                m_storage.get(m_currentPosition, dst[7]);
                m_storage.get(m_currentPosition, dst[6]);
                m_storage.get(m_currentPosition, dst[5]);
                m_storage.get(m_currentPosition, dst[4]);
                m_storage.get(m_currentPosition, dst[3]);
                m_storage.get(m_currentPosition, dst[2]);
                m_storage.get(m_currentPosition, dst[1]);
                m_storage.get(m_currentPosition, dst[0]);
            }
        }
        else
        {
            m_storage.rmemcopy(m_currentPosition, longlong_t, totalSize);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::deserializeArray(float *float_t, size_t numElements)
{
    size_t align = alignment(sizeof(*float_t));
    size_t totalSize = sizeof(*float_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_lastPosition->substract(m_currentPosition) >= sizeAligned)
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
                m_storage.get(m_currentPosition, dst[3]);
                m_storage.get(m_currentPosition, dst[2]);
                m_storage.get(m_currentPosition, dst[1]);
                m_storage.get(m_currentPosition, dst[0]);
            }
        }
        else
        {
            m_storage.rmemcopy(m_currentPosition, float_t, totalSize);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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

Marshalling& Cdr::deserializeArray(double *double_t, size_t numElements)
{
    size_t align = alignment(sizeof(*double_t));
    size_t totalSize = sizeof(*double_t) * numElements;
    size_t sizeAligned = totalSize + align;

    if(m_lastPosition->substract(m_currentPosition) >= sizeAligned)
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
                m_storage.get(m_currentPosition, dst[7]);
                m_storage.get(m_currentPosition, dst[6]);
                m_storage.get(m_currentPosition, dst[5]);
                m_storage.get(m_currentPosition, dst[4]);
                m_storage.get(m_currentPosition, dst[3]);
                m_storage.get(m_currentPosition, dst[2]);
                m_storage.get(m_currentPosition, dst[1]);
                m_storage.get(m_currentPosition, dst[0]);
            }
        }
        else
        {
            m_storage.rmemcopy(m_currentPosition, double_t, totalSize);
        }

        return *this;
    }

    throw NotEnoughMemoryException(NOT_ENOUGH_MEMORY_MESSAGE_DEFAULT);
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
