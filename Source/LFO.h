#pragma once
#include <cmath>

const float TWO_PI{ 6.2831853071795864f };

enum Waveform {
    SINE = 2, SAW = 1, SQUARE = 3, SAW_BL = 4
};

class LFO
{
public:
    float amplitude;
    float phase;
    float inc;
    float sampleRate{44100.0f};
    float freq;
    Waveform waveform{ SINE };

    int sampleIndex{0};
    float sin0, sin1, dsin;

    void reset(float sr) {
        sampleRate = sr;
        sampleIndex = 0;
        
    }

    float sine(float p) {
        return std::sin(TWO_PI * sampleIndex * freq / sampleRate);
    }

    float nextSample() {
        

        float output = 0.0f;
        output = sine(phase);
        sampleIndex += 1;

        return output;
    }

    void setFrequency(float f) {
        freq = f;
        inc = f / sampleRate;
    }

};