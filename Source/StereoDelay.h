#pragma once

#include <array>
#include <vector>
#include "OnePoleFilter.h"
#include "Utils.h"
#include "RingBuffer.h"
#include "EnvFollower.h"
#include "DSPParameters.h"

using std::vector;
using std::array;

// Default values
#define MAX_DELAY_LENGTH			2500.0f
#define DEFAULT_SAMPLE_RATE			44100
#define DEFAULT_FILTER_FREQUENCY	3.0f
#define DEFAULT_DUCK_TIME			20.0f

#define MAX_CHANNELS			2
#define DEFAULT_INPUT_CHANNELS	2	

#define LEFT	0
#define RIGHT	1

struct StereoDelay {

	StereoDelay() :
		isOn(true),
		targetDelaySizes(),
		delayBufferSize(0),
		pingPong(false),
		lowPassFilters(),
		highPassFilters(),
		envFollowers(),
		duckingAmt(0.0f),
		feedbackGain(0.0),
		dryWetMix(0.0),
		sampleRate(DEFAULT_SAMPLE_RATE),
		nInputChannels(DEFAULT_INPUT_CHANNELS)
	{}

	void prepare(DSPParameters<float>& params) {

		sampleRate = params["sampleRate"];
		nInputChannels = params["nChannels"];

		const auto lengthInSamples = static_cast<int>((lengthToSamples(sampleRate, params["delayLength"])));
		delayBufferSize = static_cast<int>((lengthToSamples(sampleRate, MAX_DELAY_LENGTH)));

		for (int channel = 0; channel < MAX_CHANNELS; ++channel) {
			ringBuffers[channel] = RingBuffer<float>(delayBufferSize);
						
			targetDelaySizes[channel] = lengthInSamples;

			paramFilters[channel].setFrequency(DEFAULT_FILTER_FREQUENCY / sampleRate);
			lowPassFilters[channel].setFrequency(params["lowPassFreq"] / sampleRate);
			highPassFilters[channel].setFrequency(params["highPassFreq"] / sampleRate);

			envFollowers[channel].setSampleRate(sampleRate);
			envFollowers[channel].setAttack(DEFAULT_DUCK_TIME);
			envFollowers[channel].setRelease(DEFAULT_DUCK_TIME);
		}

		isOn = params["isOn"] == 1.0f;
		pingPong = params["pingPong"] == 1.0f;
		feedbackGain = params["feedback"];
		dryWetMix = params["mix"];

	}

	void update(DSPParameters<float>& params) {

		pingPong = params["pingPong"] == 1.0;
		isOn = params["isOn"] == 1.0;

		targetDelaySizes[0] = lengthToSamples(sampleRate, params["leftDelayLength"]);
		targetDelaySizes[1] = lengthToSamples(sampleRate, params["rightDelayLength"]);
						
		feedbackGain = params["feedback"];
		dryWetMix = params["mix"];

		for (int i = 0; i < 2; ++i) {
			lowPassFilters[i].setFrequency(params["lowPassFreq"] / sampleRate);
			highPassFilters[i].setFrequency(params["highPassFreq"] / sampleRate);

		}

		duckingAmt = params["ducking"];
	}

	void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {
		if (isOn) {
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
	bool isOn;

};
