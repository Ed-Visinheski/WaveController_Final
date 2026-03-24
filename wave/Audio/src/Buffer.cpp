#include "Buffer.h"

CBuffer::CBuffer() : m_writeIndex(0), m_readIndex(0) {};

size_t CBuffer::write(const double* data, size_t count) 
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
    std::memcpy(&m_buffer[writeIndex], data, numOfElementsInFirstChunk * sizeof(double));
    
    if (numOfElementsToWrite > numOfElementsInFirstChunk) //if there is more data to be written, wrap-round
    {
        std::memcpy(&m_buffer[0], data + numOfElementsInFirstChunk, (numOfElementsToWrite - numOfElementsInFirstChunk) * sizeof(double));
    }
    
    m_writeIndex.store((writeIndex + numOfElementsToWrite) % RING_BUFFER_CAPACITY, std::memory_order_release);
    return numOfElementsToWrite;
}

size_t CBuffer::read(double* data, size_t count) 
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
    std::memcpy(data, &m_buffer[readIndex], numOfElementsInFirstChunk * sizeof(double));
    
    if (numOfElementsToRead > numOfElementsInFirstChunk) //If there is still more data to be read, wrap-round
    {
        std::memcpy(data + numOfElementsInFirstChunk, &m_buffer[0], (numOfElementsToRead - numOfElementsInFirstChunk) * sizeof(double));
    }
    
    m_readIndex.store((readIndex + numOfElementsToRead) % RING_BUFFER_CAPACITY, std::memory_order_release);
    return numOfElementsToRead;
}

size_t CBuffer::peek(double* data, size_t count) const 
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
    std::memcpy(data, &m_buffer[readIndex], numOfElementsInFirstChunk * sizeof(double));
    
    if (numOfElementsToRead > numOfElementsInFirstChunk) //If there is still more data to be read, wrap-round
    {
        std::memcpy(data + numOfElementsInFirstChunk, &m_buffer[0], (numOfElementsToRead - numOfElementsInFirstChunk) * sizeof(double));
    }
    
    return numOfElementsToRead;
}

size_t CBuffer::getAvailableRead() const 
{
    const size_t readIndex = m_readIndex.load(std::memory_order_relaxed);
    const size_t writeIndex = m_writeIndex.load(std::memory_order_acquire);
    return calculateAvailableRead(readIndex, writeIndex);
}

size_t CBuffer::getAvailableWrite() const 
{
    const size_t writeIndex = m_writeIndex.load(std::memory_order_relaxed);
    const size_t readIndex = m_readIndex.load(std::memory_order_acquire);
    return calculateAvailableWrite(writeIndex, readIndex);
}

void CBuffer::clear() 
{
    m_readIndex.store(m_writeIndex.load(std::memory_order_acquire), std::memory_order_release);
}

size_t CBuffer::calculateAvailableRead(size_t readIndex, size_t writeIndex) const 
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

size_t CBuffer::calculateAvailableWrite(size_t writeIndex, size_t readIndex) const 
{
    const size_t availableRead = calculateAvailableRead(readIndex, writeIndex);
    return RING_BUFFER_CAPACITY - availableRead - 1; //-1 needed to differentiate full from empty - empty is when readIndex == writeIndex
}


