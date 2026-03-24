#pragma once

#include <atomic>
#include <cstring>
#include <array>
#include <algorithm>

/**
 * @details This Buffer Class is an implementation of the Ring Buffer Data Structure
 * 
 */


class CBuffer 
{
public:
    explicit CBuffer();

    //Copies incoming data to storage, increases write index
    size_t write(const double* data, size_t count);

    //Copies stored samples and advances readIndex
    size_t read(double* data, size_t count);

    //Copies samples without advancing the read index
    size_t peek(double* data, size_t count) const;

    //returns number of Elements that can be read from
    size_t getAvailableRead() const;

    //returns number of elements that can be written to
    size_t getAvailableWrite() const;

    //clears the buffer
    void clear();

private:
    //Used to calcualte remaining availavble number of read and write elements
    size_t calculateAvailableRead(size_t readIndex, size_t writeIndex) const;
    size_t calculateAvailableWrite(size_t writeIndex, size_t readIndex) const;

    static constexpr size_t BUFFER_CAPACITY {8193};
    std::array<double, BUFFER_CAPACITY> m_buffer{};
    std::atomic<size_t> m_writeIndex;
    std::atomic<size_t> m_readIndex;
};

