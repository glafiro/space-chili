/*
  ==============================================================================

    Utils.h
    Created: 29 Aug 2024 11:20:20am
    Author:  dglaf

  ==============================================================================
*/

#pragma once
#include <cmath>

// Utility functions
template<typename T>
T lengthToSamples(T sr, T n) noexcept {
	return sr * n * static_cast<T>(0.001);
}

inline int wrapNegative(int value, int maxValue) {
	if (value < 0) return value += maxValue;
	else return value;
}

inline unsigned int nearestPowerOfTwo(int n) {
	return pow(2, ceil(log(n) / log(2)));
}

template<typename T>
T lerp(T a, T b, T f) {
	return a * (1.0 - f) + b * f;
}

template<typename T>
T clamp(T val, T minVal, T maxVal) {
	val = fmin(val, maxVal);
	val = fmax(val, minVal);
	return val;
}

#define SILENCE 0.000001f

class LogarithmicFader
{
    float currentGain, targetGain, multiplier;
    float attackTime, releaseTime, fadeSize;
    float sampleRate;
    bool defaultOn;

public:
    LogarithmicFader(float atk = 300.0f, float rls = 300.0f, bool defaultOn = false) : attackTime(atk), releaseTime(rls), defaultOn(defaultOn), currentGain(SILENCE), targetGain(SILENCE), multiplier(1.0) {}

    void prepare(float sr) {
        sampleRate = sr;
        fadeSize = defaultOn ? lengthToSamples(releaseTime, sampleRate) : lengthToSamples(attackTime, sampleRate);
    }

    void setTarget(int isOn) {
        targetGain = isOn ? 1.0f : SILENCE;

        if (targetGain < currentGain) fadeSize = lengthToSamples(releaseTime, sampleRate);
        else fadeSize = lengthToSamples(attackTime, sampleRate);
        multiplier = std::pow(targetGain / currentGain, 1.0f / fadeSize);
    }

    float getNextValue() {
        if (std::abs(currentGain - targetGain) > 0.0001f) {
            currentGain *= multiplier;
            if ((multiplier > 1.0f && currentGain >= targetGain) || (multiplier < 1.0f && currentGain <= targetGain)) {
                currentGain = targetGain;
            }
        }
        return currentGain;
    }

    float read() {
        return currentGain;
    }
};

class SlewLimiter
{
public:
    SlewLimiter(float slew = 0.1f)
        : slewRate(slew), currentValue(0.0f), targetValue(0.0f)
    {
    }

    void prepare(float sr) {
        maxDelta = slewRate / sampleRate;
    }

    void setTarget(float target) {
        targetValue = target;
    }

    float getNextValue() {
        float delta = targetValue - currentValue;

        if (std::abs(delta) > maxDelta)  currentValue += maxDelta * (delta > 0 ? 1.0f : -1.0f);
        else currentValue = targetValue;

        return currentValue;
    }

private:
    float slewRate;    
    float sampleRate;
    float currentValue;
    float targetValue;
    float maxDelta;
};