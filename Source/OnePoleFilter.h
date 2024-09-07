/*
  ==============================================================================

    OnePoleFilter.h
    Created: 25 Aug 2024 2:38:12pm
    Author:  dglaf

  ==============================================================================
*/

#pragma once

#include <cmath>

#define M_PI 3.14159265358979323846
#define DEFAULT_SR  44100.0f

// https://www.earlevel.com/main/2012/12/15/a-one-pole-filter/

class OnePoleFilter
{
public:
    OnePoleFilter() : a0(1.0f), b1(0.0f), z1(0.0f) {}
    OnePoleFilter(float freq) : z1(0.0f) { }
    ~OnePoleFilter() {};

    void setSampleRate(float sr) {
        sampleRate = sr;
    }

    void setFrequency(float freq) {
        b1 = exp(-2.0f * M_PI * (freq / sampleRate));
        a0 = 1.0f - b1;
    }

    void prepare(float sr, float f) {
        setSampleRate(sr);
        setFrequency(f);
    }


    float process(float in) {
        z1 = in * a0 + z1 * b1;
        return z1;
    }

    float updateAndProcess(float freq, float in) {
        setFrequency(freq);
        return process(in);
    }

protected:
    float a0{ 1.0 }, b1{ 0.0 }, z1{0.0};
    float sampleRate{DEFAULT_SR};
};
