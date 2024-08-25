/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Delay.h"

#define PLUGIN_VERSION 1    

#define DEFAULT_DELAY_LEN       100.0
#define DEFAULT_FEEDBACK_GAIN   35.0f
#define DEFAULT_DRY_WET         35.0f

namespace ParameterID
{
#define PARAMETER_ID(str) const juce::ParameterID str(#str, PLUGIN_VERSION);

    PARAMETER_ID(leftDelaySize)
    PARAMETER_ID(rightDelaySize)
    PARAMETER_ID(feedback)
    PARAMETER_ID(dryWet)

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

private:
    //==============================================================================
    
    // Parameters
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioParameterFloat* leftDelaySizeParam;
    juce::AudioParameterFloat* rightDelaySizeParam;
    juce::AudioParameterFloat* feedbackParam;
    juce::AudioParameterFloat* dryWetParam;

    std::atomic<bool> parametersChanged{ false };
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override
    {
        parametersChanged.store(true);
    }

    void update(juce::AudioBuffer<float>& buffer);

    // DSP
    dsp::Delay delay;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessor)
};
