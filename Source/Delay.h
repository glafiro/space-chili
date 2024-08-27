#pragma once

#include <array>
#include <vector>
#include "RingBuffer.h"
#include "OnePoleFilter.h"

using std::vector;
using std::array;

#define MAX_DELAY_LENGTH 10000.0
#define DEFAULT_SAMPLE_RATE 44100
#define DEFAULT_FEEDBACK_GAIN 0.4f
#define DEFAULT_FILTER_FREQUENCY 3.0f
#define DEFAULT_DRY_WET_MIX 0.35f

// TODO:
// Sync left and right (wait until UI is done)
// Sync to host
// Ping pong delay
// Add linear interpolation to all parameters

template<typename T>
T msToSamples(T sampleRate, T lengthInMs) noexcept {
	return sampleRate * lengthInMs * static_cast<T>(0.001);
}

enum Channel {
	LEFT, RIGHT
};

namespace dsp
{
	struct Delay {

		Delay() :
			ringBuffer(),
			filters(),
			targetDelaySizes(),
			feedbackGain(DEFAULT_FEEDBACK_GAIN),
			sampleRate(DEFAULT_SAMPLE_RATE),
			dryWetMix(DEFAULT_DRY_WET_MIX)
		{}

		void prepare(double sampleRate, int blockSize, double lengthInMs) {

			this->sampleRate = sampleRate;

			// Get length in samples using the formula sr * ms * 0.001
			const auto lengthInSamples = static_cast<int>(msToSamples(sampleRate, lengthInMs));
			const auto maxLengthInSamples = static_cast<int>(msToSamples(sampleRate, MAX_DELAY_LENGTH));

			// in order to avoid resizing buffer during audio callback, set it to max size.
			for (auto& ch : ringBuffer) {
				ch.setSize(maxLengthInSamples);
			}

			targetDelaySizes[Channel::LEFT] = lengthInSamples;
			targetDelaySizes[Channel::RIGHT] = lengthInSamples;

			filters[Channel::LEFT].setFrequency(DEFAULT_FILTER_FREQUENCY / sampleRate);
			filters[Channel::RIGHT].setFrequency(DEFAULT_FILTER_FREQUENCY / sampleRate);
		}

		void update(float leftDelayLength, float rightDelayLength, float newFeedbackGain, 
					float newDryWetMix, bool isSynced) {

			targetDelaySizes[Channel::LEFT] = msToSamples(static_cast<float>(sampleRate), leftDelayLength);

			if (isSynced) {
				targetDelaySizes[Channel::RIGHT] = targetDelaySizes[Channel::LEFT];
			}
			else {
				targetDelaySizes[Channel::RIGHT] = msToSamples(static_cast<float>(sampleRate), rightDelayLength);
			}
			
			feedbackGain = newFeedbackGain;
			dryWetMix = newDryWetMix;
		}

		void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {

			if (targetDelaySizes[Channel::LEFT] != 0 && targetDelaySizes[Channel::RIGHT]) {

				for (auto ch = 0; ch < numChannels; ++ch) {
					auto samples = inputBuffer[ch];
					DelayRingBuffer<float>* ring = &ringBuffer[ch];

					for (auto s = 0; s < numSamples; ++s) {
						auto currentDelaySize = filters[ch].process(targetDelaySizes[ch]);
						ring->write(samples[s], feedbackGain);
						samples[s] = samples[s] * (1.0 - dryWetMix) + ring->read(currentDelaySize) * dryWetMix;
					}
				}
			}
		}

	protected:
		double sampleRate;
		array<DelayRingBuffer<float>, 2> ringBuffer;
		array<OnePoleFilter, 2> filters;

		// Parameters
		array<float, 2> targetDelaySizes;
		float feedbackGain;
		float dryWetMix;
	};

}