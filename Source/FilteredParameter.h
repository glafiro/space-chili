/*
  ==============================================================================

    FilteredParameter.h
    Created: 4 Sep 2024 11:04:17am
    Author:  dglaf

  ==============================================================================
*/

#pragma once

#include "OnePoleFilter.h"

#define DEFAULT_FILTER_FREQ 3.0f
#define DEFAULT_SR          44100.0f

class FilteredParameter
{
    OnePoleFilter filter{ DEFAULT_FILTER_FREQ };
    float sampleRate{DEFAULT_SR};
    float value{0.0f};
    float frequency{ DEFAULT_FILTER_FREQ };

public:

    FilteredParameter(float sr = DEFAULT_SR) : sampleRate(sr) {}

    void prepare(float sr, float f, float v) {
        filter.setSampleRate(sr);
        filter.setFrequency(f);
        value = v;
    }

    // Filter then return current value
    float next() {
        return filter.process(value);
    }

    // Just return current value
    float read() {
        return value;
    }


    void setValue(float v) {
        value = v;
    }
};