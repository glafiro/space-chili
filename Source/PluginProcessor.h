/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StereoDelay.h"
#include "Chorus.h"
#include "DSPParameters.h"
#include "PresetManager.h"


#define PLUGIN_VERSION 1    

enum TempoSource {INTERNAL, HOST};
enum TimeMode {STRAIGHT, TRIPLETS, DOTTED};

#define DEFAULT_DELAY_LEN       250.0f
#define DEFAULT_FEEDBACK_GAIN   35.0f
#define DEFAULT_MIX             35.0f
#define DEFAULT_LINK            false
#define DEFAULT_SYNC            false
#define DEFAULT_BPM             120.0f
#define DEFAULT_CLOCK_SRC       TempoSource::HOST
#define DEFAULT_INTERNAL_BPM    120.0f
#define DEFAULT_SUBDIVISION     3 // 1/8
#define DEFAULT_TIME_MODE       TimeMode::STRAIGHT
#define DEFAULT_IS_PINGPONG     0.0f
#define DEFAULT_LR_RATIO        1.01f
#define DEFAULT_LOW_PASS        20000.0f
#define DEFAULT_HIGH_PASS       20.0f
#define DEFAULT_DUCKING         0.0f
#define DEFAULT_DELAY_ON        true

#define DEFAULT_CHORUS_ON       false
#define DEFAULT_CHORUS_DEPTH    50.0f
#define DEFAULT_CHORUS_RATE     0.25f

namespace ParameterID
{
#define PARAMETER_ID(str) const juce::ParameterID str(#str, PLUGIN_VERSION);

    PARAMETER_ID(leftDelaySize)
    PARAMETER_ID(rightDelaySize)
    PARAMETER_ID(feedback)
    PARAMETER_ID(dryWet)
    PARAMETER_ID(delaySync)
    PARAMETER_ID(syncToBPM)
    PARAMETER_ID(internalOrHost)
    PARAMETER_ID(internalBPM)
    PARAMETER_ID(syncedTimeSubdivisionL)
    PARAMETER_ID(syncedTimeSubdivisionR)
    PARAMETER_ID(timeModeL)
    PARAMETER_ID(timeModeR)
    PARAMETER_ID(pingPong)
    PARAMETER_ID(leftRightRatio)
    PARAMETER_ID(lowPassFreq)
    PARAMETER_ID(highPassFreq)
    PARAMETER_ID(duckingAmount)
    PARAMETER_ID(delayOn)
    PARAMETER_ID(chorusOn)
    PARAMETER_ID(chorusDepth)
    PARAMETER_ID(chorusRate)


#undef PARAMETER_ID
}

//==============================================================================
/**
*/
class DelayAudioProcessor  : public juce::AudioProcessor, private juce::ValueTree::Listener
{
public:
    //==============================================================================
    DelayAudioProcessor();
    ~DelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    juce::AudioProcessorValueTreeState apvts;
    
    PresetManager& getPresetManager() { return *presetManager; }

private:
    //==============================================================================
    
    // Parameters
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioParameterFloat*  leftDelaySizeParam;
    juce::AudioParameterFloat*  rightDelaySizeParam;
    juce::AudioParameterFloat*  feedbackParam;
    juce::AudioParameterFloat*  dryWetParam;
    juce::AudioParameterBool*   delaySyncParam;
    juce::AudioParameterBool*   syncToBPMParam;
    juce::AudioParameterChoice* internalOrHostParam;
    juce::AudioParameterFloat*  internalBPMParam;
    juce::AudioParameterChoice* syncedTimeSubdivParamL;
    juce::AudioParameterChoice* syncedTimeSubdivParamR;
    juce::AudioParameterInt*    timeModeParamL;
    juce::AudioParameterInt*    timeModeParamR;
    juce::AudioParameterBool*   pingPongParam;
    juce::AudioParameterFloat*  leftRightRatioParam;
    juce::AudioParameterFloat*  lowPassFreqParam;
    juce::AudioParameterFloat*  highPassFreqParam;
    juce::AudioParameterFloat*  duckingAmountParam;
    juce::AudioParameterBool*   delayOnParam;
    juce::AudioParameterBool*   chorusOnParam;
    juce::AudioParameterFloat*  chorusDepthParam;
    juce::AudioParameterFloat*  chorusRateParam;

    std::atomic<bool> parametersChanged{ false };
    std::atomic<int> useHostBPM{ 1 };
    float currentHostBPM {DEFAULT_BPM};

    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override
    {
        parametersChanged.store(true);
        useHostBPM.store(internalOrHostParam->getIndex());
    }

    void valueTreeRedirected(juce::ValueTree&) override {
        parametersChanged.store(true);
        useHostBPM.store(internalOrHostParam->getIndex());
    }

    void update(juce::AudioBuffer<float>& buffer, float bpm);

    // DSP
    StereoDelay delay;
    Chorus chorus;
    DSPParameters<float> delayParameters;
    DSPParameters<float> chorusParameters;

    std::unique_ptr<PresetManager>presetManager;    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessor)
};
