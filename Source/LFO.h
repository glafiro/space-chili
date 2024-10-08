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
    float phase{0.0f};
    float phaseOffset;
    float inc;
    float sampleRate{44100.0f};
    float freq;
    Waveform waveform{ SINE };

    int sampleIndex{0};
    float sin0, sin1, dsin;

    void reset(float sr, float offset) {
        sampleRate = sr;
        sampleIndex = 0;
        phaseOffset = offset;
    }

    float sine(float p) {
        return std::sin(TWO_PI * (phase + phaseOffset));
    }

    float nextSample() {

        phase += inc;

        float output = 0.0f;
        output = sine(phase);
        sampleIndex += 1;

        return output;
    }

    void setFrequency(float f) {
        freq = f;
        inc = f / sampleRate;
    }

    float updateAndGetNext(float f) {
        setFrequency(f);
        return nextSample();
    }

};