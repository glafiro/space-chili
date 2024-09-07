#pragma once

#include <array>
#include <vector>
#include "OnePoleFilter.h"
#include "Utils.h"
#include "SimpleDelay.h"
#include "FilteredParameter.h"
#include "LFO.h"

using std::vector;
using std::array;

// Default values
#define DEFAULT_SAMPLE_RATE	44100
#define DEFAULT_DRY_WET		35.0f
#define DEFAULT_DL_LENGTH	100.0f
#define DEFAULT_FILTER_FREQ 3.0f

#define MAX_CHANNELS	2
#define MAX_DELAYS		2

#define L_PHASE_OFFSET	0.5f
#define R_PHASE_OFFSET	0.0f
#define LFO_FREQ		1.0

#define DEFAULT_L_MIN	10.0f
#define DEFAULT_R_MIN	10.0f
#define DEFAULT_L_DEPTH	20.0f
#define DEFAULT_R_DEPTH	20.0f

#define MAX_DELAY_LENGTH			2500.0f
#define DEFAULT_SAMPLE_RATE			44100
#define DEFAULT_FEEDBACK_GAIN		0.0f
#define DEFAULT_DRY_WET_MIX			0.5f


#define DEFAULT_ATK		50.0f

struct Chorus {

	Chorus() :
		isOn(false),
		lfos(),
		modDepth(0.5f),
		minDelays(),
		feedbackGain(DEFAULT_FEEDBACK_GAIN),
		dryWetMix(DEFAULT_DRY_WET_MIX),
		sampleRate(DEFAULT_SAMPLE_RATE)
	{}

	void prepare(DSPParameters<float>& params, float lengthInMs = DEFAULT_DL_LENGTH) {
		auto sampleRate = params["sampleRate"];
		auto blockSize = params["blockSize"];
		auto nInputChannels = params["nChannels"];

		amplitude.prepare(sampleRate);

		delayBufferSize = static_cast<int>((lengthToSamples(sampleRate, MAX_DELAY_LENGTH)));

		// Initialize LFO and delay array values
		for (int i = 0; i < MAX_DELAYS; ++i) {
			ringBuffers[i] = RingBuffer<float>(delayBufferSize);
		}
		lfos[0].reset(sampleRate, L_PHASE_OFFSET);
		lfos[0].reset(sampleRate, R_PHASE_OFFSET);

		// Set chorus parameters
		minDelays[0] = DEFAULT_L_MIN;
		minDelays[1] = DEFAULT_R_MIN;
		lfoRate.prepare(sampleRate, DEFAULT_FILTER_FREQUENCY, params["chorusRate"]);

		lfos[0].setFrequency(lfoRate.read());
		lfos[1].setFrequency(lfoRate.read() * 1.02f);
		modDepth.prepare(sampleRate, DEFAULT_FILTER_FREQ, params["chorusDepth"]);

		filterL.setSampleRate(sampleRate);
		filterR.setSampleRate(sampleRate);
		filterL.setFrequency(DEFAULT_FILTER_FREQ);
		filterR.setFrequency(DEFAULT_FILTER_FREQ);

	}

	void update(DSPParameters<float>& params) {
		amplitude.setTarget(params["isOn"]);

		modDepth.setValue(params["chorusDepth"]);
		lfoRate.setValue(params["chorusRate"]);
	}

	void processBlock(float* const* inputBuffer, int numChannels, int numSamples) {
		
		float leftDelaySize{ 0.0f }, rightDelaySize{ 0.0f };

		if (amplitude.getNextValue() > SILENCE) {
			for (int s = 0; s < numSamples; ++s) {
				auto maxDelayL = minDelays[0] + DEFAULT_L_DEPTH;
				auto maxDelayR = minDelays[1] + DEFAULT_R_DEPTH;
			
				float halfL = (maxDelayL - minDelays[0]) / 2.0f;
				float halfR = (maxDelayR - minDelays[1]) / 2.0f;
			
				float midL = halfL + minDelays[0];
				float midR = halfR + minDelays[1];

				auto currentRate = lfoRate.next();

				auto lfoOutputL = lfos[0].updateAndGetNext(currentRate);
				auto lfoOutputR = lfos[1].updateAndGetNext(currentRate * 1.02f);

				auto currentModDepth = modDepth.next();
		
				auto leftDelayLength = lfoOutputL  * currentModDepth * halfL + midL;
				auto rightDelayLength = lfoOutputR * currentModDepth * halfR + midR;

				leftDelaySize = lengthToSamples(sampleRate, leftDelayLength);
				rightDelaySize = lengthToSamples(sampleRate, rightDelayLength);

				auto delayReadL = ringBuffers[0].read(leftDelaySize);
				auto delayReadR = ringBuffers[1].read(rightDelaySize);

				float delayInputL, delayInputR;

				auto leftS = inputBuffer[0][s];
				auto rightS = inputBuffer[1][s];

				ringBuffers[0].write(leftS + delayReadL * feedbackGain);
				ringBuffers[1].write(rightS + delayReadR * feedbackGain);

				dryWetMix = DEFAULT_DRY_WET_MIX * amplitude.getNextValue();

				inputBuffer[0][s] = leftS * (1.0 - dryWetMix) + delayReadL * dryWetMix;
				inputBuffer[1][s] = rightS * (1.0 - dryWetMix) + delayReadR * dryWetMix;
				
			}
		}

	}

protected:
	bool isOn;
	float sampleRate;
	float minDelay;
	float depth;
	int delayBufferSize;
	float feedbackGain;
	float dryWetMix;

	array<RingBuffer<float>, 2> ringBuffers;
	array<float, 2> minDelays;
	array<float, 2> depths;
	FilteredParameter modDepth;
	FilteredParameter lfoRate;
	LogarithmicFader amplitude;
	array<LFO, 2> lfos;
	
	// XD
	OnePoleFilter filterL;
	OnePoleFilter filterR;



};
