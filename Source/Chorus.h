#pragma once

#include <array>
#include <vector>
#include "OnePoleFilter.h"
#include "Utils.h"
#include "SimpleDelay.h"

using std::vector;
using std::array;

// Default values
#define DEFAULT_SAMPLE_RATE	44100
#define DEFAULT_DRY_WET		35.0f
#define DEFAULT_DL_LENGTH	100.0f

#define MAX_CHANNELS	2
#define MAX_DELAYS		3


struct Chorus {

	Chorus() {}

	void prepare(float _nInputChannels, float _sampleRate, int blockSize, float lengthInMs = DEFAULT_DL_LENGTH) {
		for (auto& d : delays) {
			d.prepare(_nInputChannels, _sampleRate, blockSize, lengthInMs);
		}
	}

	void update(float delayLength, float newFeedbackGain,
		float newDryWetMix, bool _pingPong, float lowPassFreq, float highPassFreq,
		float _duckingAmt) {
	}

	void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {
		for (auto s = 0; s < numSamples; ++s) {

		}
	}

protected:

	array<SimpleDelay, 3> delays;

};
