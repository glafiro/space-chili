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
#define DEFAULT_PING_PONG			false
#define DEFAULT_LO_FILTER_FREQ		20000.0f
#define DEFAULT_HI_FILTER_FREQ		20.0f
#define DEFAULT_DUCK_TIME			20.0f


#define MAX_CHANNELS			2
#define DEFAULT_INPUT_CHANNELS	2	

#define LEFT	0
#define RIGHT	1

struct Delay {

	Delay() :
		targetDelaySizes(),
		delayBufferSize(0),
		pingPong(false),
		lowPassFilters(),
		highPassFilters(),
		envFollowers(),
		duckingAmt(0.0f),
		feedbackGain(DEFAULT_FEEDBACK_GAIN),
		sampleRate(DEFAULT_SAMPLE_RATE),
		nInputChannels(DEFAULT_INPUT_CHANNELS),
		dryWetMix(DEFAULT_DRY_WET_MIX)
	{}

	void prepare(float _nInputChannels, float _sampleRate, int blockSize, float lengthInMs) {

		sampleRate = _sampleRate;
		nInputChannels = _nInputChannels;

		const auto lengthInSamples = static_cast<int>((lengthToSamples(sampleRate, lengthInMs)));
		delayBufferSize = static_cast<int>((lengthToSamples(sampleRate, MAX_DELAY_LENGTH)));

		for (int channel = 0; channel < MAX_CHANNELS; ++channel) {
			ringBuffers[channel] = RingBuffer<float>(delayBufferSize);
						
			targetDelaySizes[channel] = lengthInSamples;

			paramFilters[channel].setFrequency(DEFAULT_FILTER_FREQUENCY / sampleRate);
			lowPassFilters[channel].setFrequency(DEFAULT_LO_FILTER_FREQ / sampleRate);
			highPassFilters[channel].setFrequency(DEFAULT_HI_FILTER_FREQ / sampleRate);

			envFollowers[channel].setSampleRate(sampleRate);
			envFollowers[channel].setAttack(DEFAULT_DUCK_TIME);
			envFollowers[channel].setRelease(DEFAULT_DUCK_TIME);
		}

	}

	void update(float leftDelayLength, float rightDelayLength, float newFeedbackGain, 
				float newDryWetMix, bool _pingPong, float lowPassFreq, float highPassFreq,
				float _duckingAmt) {

		pingPong = _pingPong;

		targetDelaySizes[0] = lengthToSamples(sampleRate, leftDelayLength);
		targetDelaySizes[1] = lengthToSamples(sampleRate, rightDelayLength);
						
		feedbackGain = newFeedbackGain;
		dryWetMix = newDryWetMix;

		lowPassFilters[0].setFrequency(lowPassFreq / sampleRate);
		lowPassFilters[1].setFrequency(lowPassFreq / sampleRate);
		highPassFilters[0].setFrequency(highPassFreq / sampleRate);
		highPassFilters[1].setFrequency(highPassFreq / sampleRate);

		duckingAmt = _duckingAmt;
	}

	void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {
		for (auto s = 0; s < numSamples; ++s) {
		
			auto leftS = inputBuffer[0][s];
			auto rightS = inputBuffer[1][s];

			auto delaySizeL = paramFilters[0].process(targetDelaySizes[0]);
			auto delaySizeR = paramFilters[1].process(targetDelaySizes[1]);

			auto leftDelayRead = ringBuffers[0].read(delaySizeL);
			auto rightDelayRead = ringBuffers[1].read(delaySizeR);

			float leftDelayInput, rightDelayInput;

			if (pingPong) {
				ringBuffers[0].write(leftS + rightS + rightDelayRead * feedbackGain);
				ringBuffers[1].write(leftDelayRead * feedbackGain);

			}
			else {
				ringBuffers[0].write(leftS + leftDelayRead * feedbackGain);
				ringBuffers[1].write(rightS + rightDelayRead * feedbackGain);
			}

			leftDelayRead  = lowPassFilters[0].process(leftDelayRead);
			rightDelayRead = lowPassFilters[1].process(rightDelayRead);
			leftDelayRead  -= highPassFilters[0].process(leftDelayRead);
			rightDelayRead -= highPassFilters[1].process(rightDelayRead);

			if (duckingAmt > 0.0f) {
				auto leftDuckingGain =  envFollowers[0].process(leftS);
				auto rightDuckingGain = envFollowers[1].process(rightS);
				leftDelayRead  *= (1.0f - leftDuckingGain * duckingAmt);
				rightDelayRead *= (1.0f - rightDuckingGain * duckingAmt);
			}

			inputBuffer[0][s] = leftS * (1.0 - dryWetMix) + leftDelayRead * dryWetMix;
			inputBuffer[1][s] = rightS * (1.0 - dryWetMix) + rightDelayRead * dryWetMix;
		}
	}

protected:
	float sampleRate;
	int nInputChannels;
	int delayBufferSize;

	array<RingBuffer<float>, 2> ringBuffers;
	array<OnePoleFilter, 2> paramFilters;
	array<OnePoleFilter, 2> lowPassFilters;
	array<OnePoleFilter, 2> highPassFilters;
	array<EnvFollower, 2> envFollowers;

	// Parameters
	array<float, 2> targetDelaySizes;
	float feedbackGain;
	float dryWetMix;
	bool pingPong;
	float duckingAmt;

};
