#pragma once

#include <array>
#include <vector>
#include "OnePoleFilter.h"
#include "Utils.h"

using std::vector;
using std::array;

// Default values
#define MAX_DELAY_LENGTH			10000.0f
#define DEFAULT_SAMPLE_RATE			44100
#define DEFAULT_FEEDBACK_GAIN		0.4f
#define DEFAULT_FILTER_FREQUENCY	3.0f
#define DEFAULT_DRY_WET_MIX			0.35f
#define DEFAULT_PING_PONG			false


#define MAX_CHANNELS 2
#define LEFT		 0
#define RIGHT		 1	

struct Delay {

	Delay() :
		ringBuffer(),
		readPointers(),
		writePointers(),
		inputSamples(),
		delayOutputs(),
		targetDelaySizes(),
		delayBufferSize(0),
		pingPong(false),
		feedbackGain(DEFAULT_FEEDBACK_GAIN),
		sampleRate(DEFAULT_SAMPLE_RATE),
		dryWetMix(DEFAULT_DRY_WET_MIX)
	{}

	void prepare(float _sampleRate, int blockSize, float lengthInMs) {

		sampleRate = _sampleRate;

		const auto lengthInSamples = static_cast<int>((lengthToSamples(sampleRate, lengthInMs)));
		delayBufferSize = static_cast<int>((lengthToSamples(sampleRate, MAX_DELAY_LENGTH)));

		for (int channel = 0; channel < MAX_CHANNELS; ++channel) {
			ringBuffer[channel].resize(delayBufferSize, 0.0f);
			
			writePointers[channel] = delayBufferSize;
			readPointers[channel] = wrapNegative(writePointers[channel] - lengthInSamples, delayBufferSize);
			
			targetDelaySizes[channel] = lengthInSamples;
			
			filters[channel].setFrequency(DEFAULT_FILTER_FREQUENCY / sampleRate);
		}
	}

	void update(float leftDelayLength, float rightDelayLength, float newFeedbackGain, 
				float newDryWetMix, bool isSynced, bool _pingPong) {

		pingPong = _pingPong;
	
		targetDelaySizes[0] = lengthToSamples(sampleRate, leftDelayLength);
		readPointers[0] = wrapNegative(writePointers[0] - targetDelaySizes[0], static_cast<float>(delayBufferSize));

		// For now, the left channel acts as the master. When completing the UI, the sliders will be linked and the
		// master value is the last one being changed.
		if (isSynced || pingPong) {
			targetDelaySizes[1] = targetDelaySizes[0];
		}
		else {
			targetDelaySizes[1] = lengthToSamples(sampleRate, rightDelayLength);
		}
			
		readPointers[1] = wrapNegative(writePointers[1] - static_cast<int>(targetDelaySizes[1]), delayBufferSize);
			
		feedbackGain = newFeedbackGain;
		dryWetMix = newDryWetMix;

	}

	void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {
		for (auto s = 0; s < numSamples; ++s) {
			for (int ch = 0; ch < MAX_CHANNELS; ++ch) {
				inputSamples[ch] = inputBuffer[ch][s];
					
				writePointers[ch] = writePointers[ch] % delayBufferSize;
				auto currentDelaySize = static_cast<int>(filters[ch].process(targetDelaySizes[ch]));

				readPointers[ch] = wrapNegative(writePointers[ch] - currentDelaySize, delayBufferSize);
				delayOutputs[ch] = ringBuffer[ch][readPointers[ch]];

			}

			float leftInputDelay;
			float rightInputDelay;

			if (pingPong) {
				leftInputDelay = inputSamples[0] + inputSamples[1] + delayOutputs[1] * feedbackGain;
				rightInputDelay = delayOutputs[0] * feedbackGain;
			}
			else {
				leftInputDelay = inputSamples[0] + delayOutputs[0] * feedbackGain;
				rightInputDelay = inputSamples[1] + delayOutputs[1] * feedbackGain;

			}

			ringBuffer[0][writePointers[0]] = leftInputDelay;
			ringBuffer[1][writePointers[1]] = rightInputDelay;

			for (int ch = 0; ch < MAX_CHANNELS; ++ch) {
				inputBuffer[ch][s] = inputSamples[ch] * (1.0 - dryWetMix) + delayOutputs[ch] * dryWetMix;
				writePointers[ch]++;
			}

		}
	}

protected:
	float sampleRate;

	array<vector<float>, 2> ringBuffer;
	array<float, 2> inputSamples;
	array<float, 2> delayOutputs;
	array<int, 2> writePointers;
	array<int, 2> readPointers;
	array<OnePoleFilter, 2> filters;
	int delayBufferSize;

	// Parameters
	array<float, 2> targetDelaySizes;
	float feedbackGain;
	float dryWetMix;
	bool pingPong;

};
