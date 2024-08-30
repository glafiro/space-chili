/*
  ==============================================================================

    RingBuffer.h
    Created: 30 Aug 2024 12:05:06pm
    Author:  dglaf

  ==============================================================================
*/

#pragma once

#include "Utils.h"
#include <vector>
using std::vector;

#define DEFAULT_BUFFER_SIZE 134217728 // ~2.796 seconds @ 48 KhZ

template <typename T>
class RingBuffer
{
    int bufferSize;
    int mask;
    vector<T> buffer;

    int writePointer{0};
    int readPointer{0};

    void createBuffer(int size) {
        bufferSize = nearestPowerOfTwo(size);
        buffer.resize(bufferSize, static_cast<T>(0.0f));
        mask = bufferSize - 1;
    }

public:

    RingBuffer(int size) {
        createBuffer(size);
    }

    RingBuffer() {
        createBuffer(DEFAULT_BUFFER_SIZE);
    }

    void write(T value) {
        buffer[writePointer] = value;
        writePointer++;
        writePointer &= mask;
    }

    T readInt(int delaySize) {
        readPointer = writePointer - 1 - delaySize;
        readPointer &= mask;
        return buffer[readPointer];
    }

    T read(float delaySize) {
        T a = readInt(delaySize);
        T b = readInt(delaySize + 1);
        T f = delaySize - static_cast<int>(delaySize);
        return lerp(a, b, f);
    }

    ~RingBuffer() {}


};