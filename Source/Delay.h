#pragma once

#include <array>
#include <vector>
#include "RingBuffer.h"
#include "OnePoleFilter.h"

using std::vector;
using std::array;

#define MAX_DELAY_LENGTH 1000.0
#define DEFAULT_SAMPLE_RATE 44100

// TODO:
// Parameter smoothing
// Feedback
// Dry / wet
// Stereo delay
// Sync left and right
// Sync to host
// Ping pong delay

namespace dsp
{
	template<typename T>
	T msToSamples(T sampleRate, T lengthInMs) noexcept {
		return sampleRate * lengthInMs * static_cast<T>(0.001);
	}

	struct Delay {

		Delay() :
			ringBuffer(),
			currentDelaySize(0.0f),
			targetDelaySize(0.0f),
			filter(1.0f),
			delaySize(1),
			sampleRate(DEFAULT_SAMPLE_RATE)
		{}

		void prepare(double sampleRate, int blockSize, double lengthInMs) {

			this->sampleRate = sampleRate;

			// Get length in samples using the formula sr * ms * 0.001
			const auto lengthInSamples = static_cast<int>(msToSamples(sampleRate, lengthInMs));
			const auto maxLengthInSamples = static_cast<int>(msToSamples(sampleRate, MAX_DELAY_LENGTH));

			// resize left and right ring buffers to the desired length. For now, left = right.
			delaySize = lengthInSamples;
			for (auto& ch : ringBuffer) {
				ch.setSize(maxLengthInSamples);
			}

			targetDelaySize = lengthInSamples;
			currentDelaySize = targetDelaySize;

			delaySizeParamBuffer.resize(blockSize, 0.0f);

			filter.setFrequency(1.0f / sampleRate);

		}

		void update(float delayLengthInMs) {
			targetDelaySize = msToSamples(static_cast<float>(sampleRate), delayLengthInMs);
		}

		void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {

			if (delaySize != 0) {
				// initialize each entry in the head buffer to the next element to be written
				//auto incr = (localTargetDelaySize - currentDelaySize) / static_cast<float>(numSamples);

				for (auto s = 0; s < numSamples; ++s) {
					auto currentDelaySize = filter.process(targetDelaySize);
					delaySizeParamBuffer[s] = static_cast<int>(currentDelaySize);
				}

				DBG(delaySizeParamBuffer[0]);

				for (auto ch = 0; ch < numChannels; ++ch) {
					// the samples to be processed
					auto samples = inputBuffer[ch];
					// the ring buffer data
					DelayRingBuffer<float>* ring = &ringBuffer[ch];

					for (auto s = 0; s < numSamples; ++s) {
						ring->write(samples[s]);
						samples[s] += ring->read(delaySizeParamBuffer[s]);
					}
				}
			}
		}

	protected:
		// two ring buffers (one for each channel)
		array<DelayRingBuffer<float>, 2> ringBuffer;

		// delay size in samples
		int delaySize;

		float targetDelaySize;
		float currentDelaySize;
		vector<int> delaySizeParamBuffer;

		double sampleRate;

		OnePoleFilter filter;
	};

}