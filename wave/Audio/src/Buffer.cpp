#include "Buffer.h"
template<typename T>
CBuffer<T>::CBuffer() : m_writeIndex(0), m_readIndex(0) {};

template<typename T>
size_t CBuffer<T>::write(const T* data, size_t count) 
{
    const size_t writeIndex = m_writeIndex.load(std::memory_order_relaxed);
    const size_t readIndex = m_readIndex.load(std::memory_order_acquire);
    
    const size_t numOfAvailableElementsToWrite = calculateAvailableWrite(writeIndex, readIndex);
    const size_t numOfElementsToWrite = std::min(count, numOfAvailableElementsToWrite);
    
    if (numOfElementsToWrite == 0) 
    {
        return 0;
    }
    
    const size_t remainingElementsInBuffer = RING_BUFFER_CAPACITY - writeIndex;
    const size_t numOfElementsInFirstChunk = std::min(numOfElementsToWrite, remainingElementsInBuffer);
    std::memcpy(&m_buffer[writeIndex], data, numOfElementsInFirstChunk * sizeof(T));
    
    if (numOfElementsToWrite > numOfElementsInFirstChunk) //if there is more data to be written, wrap-round
    {
        std::memcpy(&m_buffer[0], data + numOfElementsInFirstChunk, (numOfElementsToWrite - numOfElementsInFirstChunk) * sizeof(T));
    }
    
    m_writeIndex.store((writeIndex + numOfElementsToWrite) % RING_BUFFER_CAPACITY, std::memory_order_release);
    return numOfElementsToWrite;
}

template<typename T>
size_t CBuffer<T>::read(T* data, size_t count) 
{
    const size_t readIndex = m_readIndex.load(std::memory_order_relaxed);
    const size_t writeIndex = m_writeIndex.load(std::memory_order_acquire);
    
    const size_t numOfAvailableElementsToRead = calculateAvailableRead(readIndex, writeIndex);
    const size_t numOfElementsToRead = std::min(count, numOfAvailableElementsToRead);
    
    if (numOfElementsToRead == 0) 
    {
        return 0;
    }
    
    const size_t remainingElementsInBuffer = RING_BUFFER_CAPACITY - readIndex; 
    const size_t numOfElementsInFirstChunk = std::min(numOfElementsToRead, remainingElementsInBuffer);
    std::memcpy(data, &m_buffer[readIndex], numOfElementsInFirstChunk * sizeof(T));
    
    if (numOfElementsToRead > numOfElementsInFirstChunk) //If there is still more data to be read, wrap-round
    {
        std::memcpy(data + numOfElementsInFirstChunk, &m_buffer[0], (numOfElementsToRead - numOfElementsInFirstChunk) * sizeof(T));
    }
    
    m_readIndex.store((readIndex + numOfElementsToRead) % RING_BUFFER_CAPACITY, std::memory_order_release);
    return numOfElementsToRead;
}

template<typename T>
size_t CBuffer<T>::peek(T* data, size_t count) const 
{
    const size_t readIndex = m_readIndex.load(std::memory_order_relaxed);
    const size_t writeIndex = m_writeIndex.load(std::memory_order_acquire);
    
    const size_t numOfAvailableElementsToRead = calculateAvailableRead(readIndex, writeIndex);
    const size_t numOfElementsToRead = std::min(count, numOfAvailableElementsToRead);
    
    if (numOfElementsToRead == 0) 
    {
        return 0;
    }
    
    const size_t remainingElementsInBuffer = RING_BUFFER_CAPACITY - readIndex; 
    const size_t numOfElementsInFirstChunk = std::min(numOfElementsToRead, remainingElementsInBuffer);
    std::memcpy(data, &m_buffer[readIndex], numOfElementsInFirstChunk * sizeof(T));
    
    if (numOfElementsToRead > numOfElementsInFirstChunk) //If there is still more data to be read, wrap-round
    {
        std::memcpy(data + numOfElementsInFirstChunk, &m_buffer[0], (numOfElementsToRead - numOfElementsInFirstChunk) * sizeof(T));
    }
    
    return numOfElementsToRead;
}

template<typename T>
size_t CBuffer<T>::getAvailableRead() const 
{
    const size_t readIndex = m_readIndex.load(std::memory_order_relaxed);
    const size_t writeIndex = m_writeIndex.load(std::memory_order_acquire);
    return calculateAvailableRead(readIndex, writeIndex);
}

template<typename T>
size_t CBuffer<T>::getAvailableWrite() const 
{
    const size_t writeIndex = m_writeIndex.load(std::memory_order_relaxed);
    const size_t readIndex = m_readIndex.load(std::memory_order_acquire);
    return calculateAvailableWrite(writeIndex, readIndex);
}

template<typename T>
void CBuffer<T>::clear() 
{
    m_readIndex.store(m_writeIndex.load(std::memory_order_acquire), std::memory_order_release);
}

template<typename T>
size_t CBuffer<T>::calculateAvailableRead(size_t readIndex, size_t writeIndex) const 
{
    if (writeIndex >= readIndex) 
    {
        return writeIndex - readIndex;
    } 
    else 
    {
        return RING_BUFFER_CAPACITY - readIndex + writeIndex;
    }
}

template<typename T>
size_t CBuffer<T>::calculateAvailableWrite(size_t writeIndex, size_t readIndex) const 
{
    const size_t availableRead = calculateAvailableRead(readIndex, writeIndex);
    return RING_BUFFER_CAPACITY - availableRead - 1; //-1 needed to differentiate full from empty - empty is when readIndex == writeIndex
}

template CBuffer<double>::CBuffer();
template CBuffer<int16_t>::CBuffer();

template size_t CBuffer<double>::write(const double*, size_t);
template size_t CBuffer<int16_t>::write(const int16_t*, size_t);

template size_t CBuffer<double>::read(double*, size_t);
template size_t CBuffer<int16_t>::read(int16_t*, size_t);

template size_t CBuffer<double>::peek(double*, size_t) const;
template size_t CBuffer<int16_t>::peek(int16_t*, size_t) const;

template size_t CBuffer<double>::getAvailableRead() const;
template size_t CBuffer<int16_t>::getAvailableRead() const;

template size_t CBuffer<double>::getAvailableWrite() const;
template size_t CBuffer<int16_t>::getAvailableWrite() const;

template void CBuffer<double>::clear();
template void CBuffer<int16_t>::clear();



