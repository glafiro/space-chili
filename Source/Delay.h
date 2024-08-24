#pragma once

#include <array>
#include <vector>
#include "RingBuffer.h"

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
			writeHeadBuffer(),
			writeHeadIndex(0),
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
				ch.updateDelaySize(lengthInSamples);
			}
			

			// resize head buffer to block size
			writeHeadBuffer.resize(blockSize); 

			// set head index to 0 
			writeHeadIndex = 0;
		}

		void update(float delayLengthInMs) {
			delaySize = static_cast<int>(msToSamples(static_cast<float>(sampleRate), delayLengthInMs));
			for (auto& ch : ringBuffer) {
				ch.updateDelaySize(delaySize);
			}
		}

		void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {
			if (delaySize != 0) {
				// initialize each entry in the head buffer to the next element to be written
				for (auto s = 0; s < numSamples; ++s) {
					writeHeadIndex = (writeHeadIndex + 1) % delaySize;
					writeHeadBuffer[s] = writeHeadIndex;
				}
				
				for (auto ch = 0; ch < numChannels; ++ch) {
					// the samples to be processed
					auto samples = inputBuffer[ch];
					// the ring buffer data
					DelayRingBuffer<float>* ring = &ringBuffer[ch];

					for (auto s = 0; s < numSamples; ++s) {
						ring->write(samples[s]);
						samples[s] += 0.5 * ring->read();
					}
				} 
			}
		}

	protected:
		// two ring buffers (one for each channel)
		array<DelayRingBuffer<float>, 2> ringBuffer;
		// the array containing the write indices
		std::vector<int> writeHeadBuffer;
		// write head index
		int writeHeadIndex{ 0 };
		// delay size in samples
		int delaySize;
		double sampleRate;
	};

}
