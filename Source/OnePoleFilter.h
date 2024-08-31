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

enum FilterType { LOWPASS, HIGHPASS};

// https://www.earlevel.com/main/2012/12/15/a-one-pole-filter/

class OnePoleFilter
{
public:
    OnePoleFilter() : a0(1.0f), b1(0.0f), z1(0.0f) {}
    OnePoleFilter(float freq) : z1(0.0f) { }
    ~OnePoleFilter() {};

    void setFrequency(double freq) {
        b1 = exp(-2.0f * M_PI * freq);
        a0 = 1.0f - b1;
    }


    float process(float in) {
        z1 = in * a0 + z1 * b1;
        return z1;
    }

protected:
    double a0{ 1.0 }, b1{ 0.0 }, z1{0.0};
};
