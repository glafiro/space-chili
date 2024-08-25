#pragma once

#include <array>
#include <vector>
#include "RingBuffer.h"
#include "OnePoleFilter.h"

using std::vector;
using std::array;

#define MAX_DELAY_LENGTH 1000.0
#define DEFAULT_SAMPLE_RATE 44100
#define DEFAULT_FEEDBACK_GAIN 0.4f
#define DEFAULT_FILTER_FREQUENCY 3.0f
#define DEFAULT_DRY_WET_MIX 0.35f

// TODO:
// Dry / wet
// Stereo delay
// Sync left and right
// Sync to host
// Ping pong delay

template<typename T>
T msToSamples(T sampleRate, T lengthInMs) noexcept {
	return sampleRate * lengthInMs * static_cast<T>(0.001);
}

namespace dsp
{
	struct Delay {

		Delay() :
			ringBuffer(),
			targetDelaySize(0.0f),
			feedbackGain(DEFAULT_FEEDBACK_GAIN),
			filter(),
			sampleRate(DEFAULT_SAMPLE_RATE),
			dryWetMix(DEFAULT_DRY_WET_MIX)
		{}

		void prepare(double sampleRate, int blockSize, double lengthInMs) {

			this->sampleRate = sampleRate;

			// Get length in samples using the formula sr * ms * 0.001
			const auto lengthInSamples = static_cast<int>(msToSamples(sampleRate, lengthInMs));
			const auto maxLengthInSamples = static_cast<int>(msToSamples(sampleRate, MAX_DELAY_LENGTH));

			// resize left and right ring buffers to the desired length. For now, left = right.
			for (auto& ch : ringBuffer) {
				ch.setSize(maxLengthInSamples);
			}

			targetDelaySize = lengthInSamples;

			delaySizeParamBuffer.resize(blockSize, 0.0f);

			filter.setFrequency(DEFAULT_FILTER_FREQUENCY / sampleRate);

		}

		void update(float delayLengthInMs, float newFeedbackGain, float newDryWetMix) {
			targetDelaySize = msToSamples(static_cast<float>(sampleRate), delayLengthInMs);
			feedbackGain = newFeedbackGain;
			dryWetMix = newDryWetMix;
		}

		void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {

			if (targetDelaySize != 0) {
		
				for (auto s = 0; s < numSamples; ++s) {
					auto currentDelaySize = filter.process(targetDelaySize);
					delaySizeParamBuffer[s] = static_cast<int>(currentDelaySize);
				}

				for (auto ch = 0; ch < numChannels; ++ch) {
					auto samples = inputBuffer[ch];
					DelayRingBuffer<float>* ring = &ringBuffer[ch];

					for (auto s = 0; s < numSamples; ++s) {
						ring->write(samples[s], feedbackGain);
						samples[s] = samples[s] * (1.0 - dryWetMix) + ring->read(delaySizeParamBuffer[s]) * dryWetMix;
					}
				}
			}
		}

	protected:
		double sampleRate;
		array<DelayRingBuffer<float>, 2> ringBuffer;
		vector<int> delaySizeParamBuffer;
		OnePoleFilter filter;

		// Parameters
		float targetDelaySize;
		float feedbackGain;
		float dryWetMix;
	};

}