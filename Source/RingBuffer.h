/*
  ==============================================================================

    RingBuffer.h
    Created: 24 Aug 2024 6:34:16pm
    Author:  dglaf

  ==============================================================================
*/

#pragma once

#include <vector>

using std::vector;

template <typename T>
class DelayRingBuffer
{
    vector<T> buffer;
    size_t bufferSize;
    size_t delaySize;
    int writePointer;
    int readPointer;

public:
    
    DelayRingBuffer(size_t size = 0) : buffer(size), bufferSize(size), delaySize(0), writePointer(0), readPointer(0) {}

    void write(T value) {
        buffer[writePointer] = value;
        writePointer = (writePointer + 1) % delaySize;
    }

    T read() {
        readPointer = writePointer;
        return buffer[readPointer];
    }

    void setSize(size_t newSize) {
        buffer.resize(newSize, static_cast<T>(0.0f));
        bufferSize = newSize;
    }

    void updateDelaySize(size_t newDelaySize) {
        delaySize = newDelaySize;
    }
};