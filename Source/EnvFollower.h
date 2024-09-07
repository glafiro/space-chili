/*
  ==============================================================================

    EnvFollower.h
    Created: 31 Aug 2024 7:28:51pm
    Author:  dglaf

  ==============================================================================
*/

#pragma once

#include <cmath>

#define SENSITIVITY 10.0f

class EnvFollower
{
    float attackTime;
    float releaseTime;
    float env;
    float sampleRate;
    float sensitivity;

    float computeCoefficient(float t, float sr) {
        t /= 1000.0f;
        return std::exp(-1.0f / (t * sr));
    }

public:
    EnvFollower(float sr = 44100.0f, float attack = 0.0f, float release = 0.0f, float sens = SENSITIVITY) : 
        attackTime(computeCoefficient(attack, sr)),
        releaseTime(computeCoefficient(release, sr)),
        env(0.0f),
        sampleRate(sr),
        sensitivity(sens)
        {}

    void setAttack(float t) {
        attackTime = computeCoefficient(t, sampleRate);
    }

    void setRelease(float t) {
        releaseTime = computeCoefficient(t, sampleRate);
    }

    void setSampleRate(float sr) {
        sampleRate = sr;
    }

    void prepare(float sr, float at, float rt) {
        setSampleRate(sr);
        setAttack(at);
        setRelease(rt);
    }

    float process(float in) {
        float rectified = std::fabs(in) * sensitivity;
        if (rectified > env) {
            env = attackTime * (env - rectified) + rectified;
        }
        else if (rectified < env) {
            env = releaseTime * (env - rectified) + rectified;
        }
        return env;
    }
};