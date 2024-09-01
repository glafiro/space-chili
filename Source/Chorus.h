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

#define L_PHASE_OFFSET	1.6f
#define R_PHASE_OFFSET	0.0f
#define LFO_FREQ		1.0

#define DEFAULT_L_MIN	10.0f
#define DEFAULT_R_MIN	10.0f
#define DEFAULT_L_DEPTH	20.0f
#define DEFAULT_R_DEPTH	20.0f

struct Chorus {

	Chorus() :
		isOn(true),
		delays(),
		lfos(),
		lfoRate(1.0f),
		modDepth(0.5f),
		delayValues(),
		minDelays(),
		depths(),
		sampleRate(DEFAULT_SAMPLE_RATE)
	{}

	void prepare(float _nInputChannels, float _sampleRate, int blockSize, float lengthInMs = DEFAULT_DL_LENGTH) {
		sampleRate = _sampleRate;

		// Initialize LFO and delay array values
		for (int i = 0; i < MAX_DELAYS; ++i) {
			delayValues[i].resize(blockSize, 0.0f);
		}
		lfos[0].reset(sampleRate, L_PHASE_OFFSET);
		lfos[0].reset(sampleRate, R_PHASE_OFFSET);

		// Set chorus parameters
		minDelays[0] = DEFAULT_L_MIN;
		minDelays[1] = DEFAULT_R_MIN;
		depths[0] = DEFAULT_L_DEPTH;
		depths[1] = DEFAULT_R_DEPTH;
		lfos[0].setFrequency(LFO_FREQ);
		lfos[1].setFrequency(LFO_FREQ);

		// Initialize delays
		delays[0].prepare(_nInputChannels, _sampleRate, blockSize, 100.f);
		delays[1].prepare(_nInputChannels, _sampleRate, blockSize, 150.f);
	}

	void update(bool _isOn, float _depth, float _lfoRate) {
		isOn = _isOn;

		modDepth = _depth;
		lfoRate = _lfoRate;

		lfos[0].setFrequency(lfoRate);
		lfos[1].setFrequency(lfoRate);
	}

	void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {
		
		float leftDelaySize{ 0.0f }, rightDelaySize{ 0.0f };

		for (int s = 0; s < numSamples; ++s) {
			auto maxDelayL = minDelays[0] + DEFAULT_L_DEPTH;
			auto maxDelayR = minDelays[1] + DEFAULT_R_DEPTH;
			
			float halfL = (maxDelayL - minDelays[0]) / 2.0f;
			float halfR = (maxDelayR - minDelays[1]) / 2.0f;
			
			float midL = halfL + minDelays[0];
			float midR = halfR + minDelays[1];
			
			auto lfoOutputL = lfos[0].nextSample();
			auto lfoOutputR = lfos[1].nextSample();
		
			auto leftDelayLength = lfoOutputL  * modDepth * halfL + midL;
			auto rightDelayLength = lfoOutputR * modDepth * halfR + midR;

			leftDelaySize = lengthToSamples(sampleRate, leftDelayLength);
			rightDelaySize = lengthToSamples(sampleRate, rightDelayLength);
			delayValues[0][s] = leftDelaySize;
			delayValues[1][s] = rightDelaySize;
		}
		
		if (isOn && leftDelaySize > 0.0f && rightDelaySize > 0.0f) {
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
	array<float, 2> minDelays;
	array<float, 2> depths;
	float modDepth;
	float lfoRate;
	array<LFO, 2> lfos;

};
