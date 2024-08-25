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
    int delaySize;
    int writePointer;
    int readPointer;

public:

    DelayRingBuffer(int size = 0) : buffer(size), bufferSize(size), delaySize(0), writePointer(0), readPointer(0) {}

    void write(T value, float feedbackGain) {
        buffer[writePointer] = feedbackGain * buffer[readPointer] + value ;
        writePointer = (writePointer + 1) % bufferSize;
    }

    T read(int delaySize) {
        readPointer = writePointer - delaySize;
        if (readPointer < 0) readPointer += bufferSize;
        return buffer[readPointer];
    }

    void setSize(int newSize) {
        buffer.resize(newSize, static_cast<T>(0.0f));
        bufferSize = newSize;
    }

};