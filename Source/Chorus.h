#pragma once

#include <array>
#include <vector>
#include "OnePoleFilter.h"
#include "Utils.h"
#include "SimpleDelay.h"
#include "LFO.h"

using std::vector;
using std::array;

// Default values
#define DEFAULT_SAMPLE_RATE	44100
#define DEFAULT_DRY_WET		35.0f
#define DEFAULT_DL_LENGTH	100.0f

#define MAX_CHANNELS	2
#define MAX_DELAYS		2

#define L_FREQ	0.5f
#define R_FREQ  1.0f;

#define DEFAULT_MIN		10.0f
#define DEFAULT_DEPTH	30.0f

struct Chorus {

	Chorus() :
		isOn(true),
		delays(),
		lfos(),
		delayValues(),
		minDelay(DEFAULT_MIN),
		depth(DEFAULT_DEPTH),
		sampleRate(DEFAULT_SAMPLE_RATE)
	{}

	void prepare(float _nInputChannels, float _sampleRate, int blockSize, float lengthInMs = DEFAULT_DL_LENGTH) {
		sampleRate = _sampleRate;

		for (int i = 0; i < MAX_DELAYS; ++i) {
			lfos[i].reset(sampleRate);
			delayValues[i].resize(blockSize, 0.0f);
		}
		lfos[0].setFrequency(L_FREQ);
		lfos[1].setFrequency(L_FREQ);

		delays[0].prepare(_nInputChannels, _sampleRate, blockSize, 100.f);
		delays[1].prepare(_nInputChannels, _sampleRate, blockSize, 150.f);
	}

	void update(bool _isOn) {
		isOn = _isOn;
	}

	void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {
		auto maxDelay = minDelay + depth;
		float half = (maxDelay - minDelay) / 2;
		float mid = half + minDelay;
		
		for (int s = 0; s < numSamples; ++s) {
			auto lfoOutputL = lfos[0].nextSample();
			auto lfoOutputR = lfos[1].nextSample();
		
			auto leftDelayLength = lfoOutputL  * 0.9f * half + mid;
			auto rightDelayLength = lfoOutputR * 0.9f * half + mid;

			auto leftDelaySize = lengthToSamples(sampleRate, leftDelayLength);
			auto rightDelaySize = lengthToSamples(sampleRate, rightDelayLength);
			delayValues[0][s] = leftDelaySize;
			delayValues[1][s] = rightDelaySize;
		}
		
		if (isOn) {
			delays[0].processBlock(
				inputBuffer,
				numChannels,
				numSamples,
				0,
				delayValues[0]
			);
		
			delays[1].processBlock(
				inputBuffer,
				numChannels,
				numSamples,
				1,
				delayValues[1]
			);
		}
	}

protected:
	bool isOn;
	float sampleRate;
	float minDelay;
	float depth;

	array<SimpleDelay, 2> delays;
	array<vector<float>, 2> delayValues;
	array<LFO, 2> lfos;

};
