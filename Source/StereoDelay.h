#pragma once

#include <array>
#include <vector>
#include "OnePoleFilter.h"
#include "Utils.h"
#include "RingBuffer.h"
#include "EnvFollower.h"
#include "DSPParameters.h"
#include "FilteredParameter.h"

using std::vector;
using std::array;

// Default values
#define MAX_DELAY_LENGTH			2500.0f
#define DEFAULT_SAMPLE_RATE			44100
#define DEFAULT_FILTER_FREQUENCY	3.0f
#define DEFAULT_DUCK_TIME			20.0f
#define DEFAULT_FILTER_FREQ			3.0f

#define MAX_CHANNELS			2
#define DEFAULT_INPUT_CHANNELS	2	

#define LEFT	0
#define RIGHT	1

struct StereoDelay {

	StereoDelay() :
		delayBufferSize(0),
		pingPong(false),
		lowPassFilters(),
		highPassFilters(),
		envFollowers(),
		feedbackGain(0.0),
		crossfade(0.0f),
		crossfadeInc(0.0f),
		sampleRate(DEFAULT_SAMPLE_RATE),
		nInputChannels(DEFAULT_INPUT_CHANNELS)
	{}

	void prepare(DSPParameters<float>& params) {

		sampleRate = params["sampleRate"];
		nInputChannels = params["nChannels"];

		crossfadeInc = (1.0f / (0.05f * sampleRate));

		const auto lengthInSamples = static_cast<int>((lengthToSamples(sampleRate, params["delayLength"])));
		delayBufferSize = static_cast<int>((lengthToSamples(sampleRate, MAX_DELAY_LENGTH)));

		delaySizeL = lengthInSamples;
		delaySizeR = lengthInSamples;
		targetSizeL = delaySizeL;
		targetSizeR = delaySizeR;

		lowFreq.prepare(sampleRate, DEFAULT_FILTER_FREQ, params["lowPassFreq"]);
		highFreq.prepare(sampleRate, DEFAULT_FILTER_FREQ, params["highPassFreq"]);

		for (int channel = 0; channel < MAX_CHANNELS; ++channel) {
			ringBuffers[channel] = RingBuffer<float>(delayBufferSize);
			
			lowPassFilters[channel].prepare(sampleRate, lowFreq.read());
			highPassFilters[channel].prepare(sampleRate, highFreq.read());

			envFollowers[channel].prepare(sampleRate, DEFAULT_DUCK_TIME, DEFAULT_DUCK_TIME);
		}

		pingPong = params["pingPong"] == 1.0f;
		feedbackGain.prepare(sampleRate, DEFAULT_FILTER_FREQ, params["feedback"]);
		mix.prepare(sampleRate, DEFAULT_FILTER_FREQ, params["mix"]);
		duckingAmt.prepare(sampleRate, DEFAULT_FILTER_FREQ, params["ducking"]);
	}

	void update(DSPParameters<float>& params) {

		pingPong = params["pingPong"] == 1.0;

		if (crossfade == 0.0f) {
			targetSizeL = lengthToSamples(sampleRate, params["leftDelayLength"]);
			targetSizeR = lengthToSamples(sampleRate, params["rightDelayLength"]);
		}
						
		feedbackGain.setValue(params["feedback"]);
		mix.setValue(params["mix"]);

		lowFreq.setValue(params["lowPassFreq"]);
		highFreq.setValue(params["highPassFreq"]);

		duckingAmt.setValue(params["ducking"]);
	}

	void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {
		for (auto s = 0; s < numSamples; ++s) {
		
			auto leftS = inputBuffer[0][s];
			auto rightS = inputBuffer[1][s];

			auto leftDelayRead = ringBuffers[0].read(delaySizeL);
			auto rightDelayRead = ringBuffers[1].read(delaySizeR);

			if (crossfade == 0.0f) {
				if ((delaySizeL != targetSizeL) || (delaySizeR != targetSizeR)) {
					crossfade = crossfadeInc;
				}
			}				

			if (crossfade > 0.0f) {
				float newDelayL = ringBuffers[0].read(targetSizeL);
				float newDelayR = ringBuffers[1].read(targetSizeR);
				leftDelayRead = (1.0f - crossfade) * leftDelayRead + crossfade * newDelayL;
				rightDelayRead = (1.0f - crossfade) * rightDelayRead + crossfade * newDelayR;
				crossfade += crossfadeInc;
				if (crossfade > 1.0f) {
					delaySizeL = targetSizeL;
					delaySizeR = targetSizeR;
					crossfade = 0.0f;
				}
			}

			float leftDelayInput, rightDelayInput;

			auto currentFeedbackGain = feedbackGain.next();

			if (pingPong) {
				ringBuffers[0].write(leftS + rightS + rightDelayRead * currentFeedbackGain);
				ringBuffers[1].write(leftDelayRead * currentFeedbackGain);

			}
			else {
				ringBuffers[0].write(leftS + leftDelayRead * currentFeedbackGain);
				ringBuffers[1].write(rightS + rightDelayRead * currentFeedbackGain);
			}

			auto currentLowFreq = lowFreq.next();
			auto currentHighFreq = highFreq.next();

			leftDelayRead  = lowPassFilters[0].updateAndProcess(currentLowFreq, leftDelayRead);
			rightDelayRead = lowPassFilters[1].updateAndProcess(currentLowFreq, rightDelayRead);
			leftDelayRead  -= highPassFilters[0].updateAndProcess(currentHighFreq, leftDelayRead);
			rightDelayRead -= highPassFilters[1].updateAndProcess(currentHighFreq, rightDelayRead);

			if (duckingAmt.next() > 0.0f) {
				auto leftDuckingGain =  envFollowers[0].process(leftS);
				auto rightDuckingGain = envFollowers[1].process(rightS);
				leftDelayRead  *= (1.0f - leftDuckingGain * duckingAmt.next());
				rightDelayRead *= (1.0f - rightDuckingGain * duckingAmt.next());
			}

			inputBuffer[0][s] = leftS * (1.0 - mix.next()) + leftDelayRead * mix.next();
			inputBuffer[1][s] = rightS * (1.0 - mix.next()) + rightDelayRead * mix.next();
		}
	}

protected:
	float sampleRate;
	int nInputChannels;
	int delayBufferSize;

	array<RingBuffer<float>, 2> ringBuffers;
	array<OnePoleFilter, 2> lowPassFilters;
	array<OnePoleFilter, 2> highPassFilters;
	array<EnvFollower, 2> envFollowers;

	// Parameters
	float delaySizeL;
	float delaySizeR;
	float targetSizeL;
	float targetSizeR;
	float crossfade;
	float crossfadeInc;
	FilteredParameter feedbackGain;
	FilteredParameter mix;
	FilteredParameter duckingAmt;
	FilteredParameter lowFreq;
	FilteredParameter highFreq;
	bool pingPong;
};
