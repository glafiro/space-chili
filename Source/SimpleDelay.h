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
#define DEFAULT_FEEDBACK_GAIN		0.4f
#define DEFAULT_FILTER_FREQUENCY	3.0f
#define DEFAULT_DRY_WET_MIX			0.35f

struct SimpleDelay {

	SimpleDelay() :
		targetDelaySize(0.0f),
		delayBufferSize(0),
		feedbackGain(DEFAULT_FEEDBACK_GAIN),
		sampleRate(DEFAULT_SAMPLE_RATE),
		dryWetMix(DEFAULT_DRY_WET_MIX)
	{}

	void prepare(float _nInputChannels, float _sampleRate, int blockSize, float lengthInMs) {

		sampleRate = _sampleRate;

		const auto lengthInSamples = static_cast<int>((lengthToSamples(sampleRate, lengthInMs)));
		delayBufferSize = static_cast<int>((lengthToSamples(sampleRate, MAX_DELAY_LENGTH)));

		ringBuffer = RingBuffer<float>(delayBufferSize);

		targetDelaySize = lengthInSamples;

		paramFilter.setFrequency(DEFAULT_FILTER_FREQUENCY / sampleRate);
	}

	void update(float leftDelayLength, float rightDelayLength, float newFeedbackGain,
		float newDryWetMix, bool _pingPong, float lowPassFreq, float highPassFreq,
		float _duckingAmt) {

		targetDelaySize = lengthToSamples(sampleRate, leftDelayLength);

		feedbackGain = newFeedbackGain;
		dryWetMix = newDryWetMix;

	}

	void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {
		for (auto s = 0; s < numSamples; ++s) {

			auto sample = inputBuffer[0][s];

			auto delaySize = paramFilter.process(targetDelaySize);

			auto delayRead = ringBuffer.read(delaySize);

			float delayInput;

			ringBuffer.write(sample + delayRead * feedbackGain);

			inputBuffer[0][s] = sample * (1.0 - dryWetMix) + delayRead * dryWetMix;
		}
	}

protected:
	float sampleRate;
	int delayBufferSize;

	RingBuffer<float> ringBuffer;
	OnePoleFilter paramFilter;

	// Parameters
	float targetDelaySize;
	float feedbackGain;
	float dryWetMix;
};
