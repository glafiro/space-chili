#pragma once

#include <array>
#include <vector>
#include "OnePoleFilter.h"
#include "Utils.h"
#include "RingBuffer.h"
#include "EnvFollower.h"

using std::vector;
using std::array;

// Default values
#define MAX_DELAY_LENGTH			2500.0f
#define DEFAULT_SAMPLE_RATE			44100
#define DEFAULT_FEEDBACK_GAIN		0.0f
#define DEFAULT_FILTER_FREQUENCY	3.0f
#define DEFAULT_DRY_WET_MIX			0.5f

struct SimpleDelay {

	SimpleDelay() :
		delayBufferSize(0),
		crossfade(0.0),
		targetDelaySize(0),
		currentDelaySize(0),
		feedbackGain(DEFAULT_FEEDBACK_GAIN),
		sampleRate(DEFAULT_SAMPLE_RATE),
		dryWetMix(DEFAULT_DRY_WET_MIX)
	{}

	void prepare(float _nInputChannels, float _sampleRate, int blockSize, float lengthInMs) {

		sampleRate = _sampleRate;

		crossfadeInc = (1.0f / (0.05f * sampleRate));

		const auto lengthInSamples = static_cast<int>((lengthToSamples(sampleRate, lengthInMs)));
		delayBufferSize = static_cast<int>((lengthToSamples(sampleRate, MAX_DELAY_LENGTH)));

		ringBuffer = RingBuffer<float>(delayBufferSize);

		paramFilter.setFrequency(DEFAULT_FILTER_FREQUENCY / sampleRate);
	}

	void processBlock(float* const* inputBuffer, int numChannels, int numSamples, int activeChannel, 
		vector<float> delayValues) {
		for (auto s = 0; s < numSamples; ++s) {

			auto sample = inputBuffer[activeChannel][s];
			
			targetDelaySize = delayValues[s];

			auto delayRead = ringBuffer.read(targetDelaySize);

			float delayInput;

			ringBuffer.write(sample + delayRead * feedbackGain);

			inputBuffer[activeChannel][s] = sample * (1.0 - dryWetMix) + delayRead * dryWetMix;
		}
	}

protected:
	float sampleRate;
	int delayBufferSize;

	RingBuffer<float> ringBuffer;
	OnePoleFilter paramFilter;

	// Parameters
	float feedbackGain;
	float dryWetMix;

	float crossfade;
	float crossfadeInc;
	float currentDelaySize;
	float targetDelaySize;
};
