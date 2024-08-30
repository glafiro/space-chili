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