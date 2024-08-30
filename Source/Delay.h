#pragma once

#include <array>
#include <vector>
#include "OnePoleFilter.h"
#include "Utils.h"
#include "RingBuffer.h"

using std::vector;
using std::array;

// Default values
#define MAX_DELAY_LENGTH			10000.0f
#define DEFAULT_SAMPLE_RATE			44100
#define DEFAULT_FEEDBACK_GAIN		0.4f
#define DEFAULT_FILTER_FREQUENCY	3.0f
#define DEFAULT_DRY_WET_MIX			0.35f
#define DEFAULT_PING_PONG			false


#define MAX_CHANNELS			2
#define DEFAULT_INPUT_CHANNELS	2	

#define LEFT	0
#define RIGHT	1

struct Delay {

	Delay() :
		targetDelaySizes(),
		delayBufferSize(0),
		pingPong(false),
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
			filters[channel].setFrequency(DEFAULT_FILTER_FREQUENCY / sampleRate);
		}
	}

	void update(float leftDelayLength, float rightDelayLength, float newFeedbackGain, 
				float newDryWetMix, bool isSynced, bool _pingPong) {

		pingPong = _pingPong;
	
		targetDelaySizes[0] = lengthToSamples(sampleRate, leftDelayLength);

		// For now, the left channel acts as the master. When completing the UI, the sliders will be linked and the
		// master value is the last one being changed.
		if (isSynced || pingPong) {
			targetDelaySizes[1] = targetDelaySizes[0];
		}
		else {
			targetDelaySizes[1] = lengthToSamples(sampleRate, rightDelayLength);
		}
						
		feedbackGain = newFeedbackGain;
		dryWetMix = newDryWetMix;

	}

	void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {
		for (auto s = 0; s < numSamples; ++s) {
		
			auto leftS = inputBuffer[0][s];
			auto rightS = inputBuffer[1][s];

			auto delaySizeL = filters[0].process(targetDelaySizes[0]);
			auto delaySizeR = filters[1].process(targetDelaySizes[1]);

			auto leftDelayRead = ringBuffers[0].read(delaySizeL);
			auto rightDelayRead = ringBuffers[1].read(delaySizeR);

			auto leftDelayInput  = leftS  + leftDelayRead * feedbackGain;
			auto rightDelayInput = rightS + rightDelayRead * feedbackGain;

			if (pingPong) {
				ringBuffers[0].write(rightDelayInput);
				ringBuffers[1].write(leftDelayInput);
			}
			else {
				ringBuffers[0].write(leftDelayInput);
				ringBuffers[1].write(rightDelayInput);
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
	array<OnePoleFilter, 2> filters;

	// Parameters
	array<float, 2> targetDelaySizes;
	float feedbackGain;
	float dryWetMix;
	bool pingPong;

};
