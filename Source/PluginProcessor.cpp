/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

template<typename T>
inline static void castParameter(juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination);
    // parameter does not exist or wrong type
}

//==============================================================================
DelayAudioProcessor::DelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    delay()
#endif
{
    apvts.state.addListener(this);
    castParameter(apvts, ParameterID::leftDelaySize, leftDelaySizeParam);
    castParameter(apvts, ParameterID::rightDelaySize, rightDelaySizeParam);
    castParameter(apvts, ParameterID::feedback, feedbackParam);
    castParameter(apvts, ParameterID::dryWet, dryWetParam);
}

DelayAudioProcessor::~DelayAudioProcessor()
{
}

//==============================================================================
const juce::String DelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    delay.prepare(sampleRate, samplesPerBlock, 100.0f);
}

void DelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    bool expected = true;
    if (isNonRealtime() || parametersChanged.compare_exchange_strong(expected, false)) {
        update(buffer);
    }

    delay.processBlock(
        buffer.getArrayOfWritePointers(),
        buffer.getNumChannels(),
        buffer.getNumSamples()
    );
}

void DelayAudioProcessor::update(juce::AudioBuffer<float>& buffer) {
    float leftDelaySize = leftDelaySizeParam->get();
    float rightDelaySize = rightDelaySizeParam->get();
    float feedbackGain = feedbackParam->get() * 0.01f * 0.98f; // Maximum is actually 98%
    float dryWetMix = dryWetParam->get() * 0.01f;
    delay.update(leftDelaySize, rightDelaySize, feedbackGain, dryWetMix);
}

//==============================================================================
bool DelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
    auto editor = new juce::GenericAudioProcessorEditor(*this);
    editor->setSize(500, 500);
    return editor;
}

//==============================================================================
void DelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout DelayAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;


    layout.add(std::make_unique <juce::AudioParameterFloat>(
        ParameterID::leftDelaySize,
        "Left (ms)",
        juce::NormalisableRange<float>{ 0.00001f, MAX_DELAY_LENGTH },
        DEFAULT_DELAY_LEN
        ));

    layout.add(std::make_unique <juce::AudioParameterFloat>(
        ParameterID::rightDelaySize,
        "Right (ms)",
        juce::NormalisableRange<float>{ 0.00001f, MAX_DELAY_LENGTH },
        DEFAULT_DELAY_LEN
        ));

    layout.add(std::make_unique <juce::AudioParameterFloat>(
        ParameterID::feedback,
        "Feedback",
        juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
        DEFAULT_FEEDBACK_GAIN
        ));
    
    layout.add(std::make_unique <juce::AudioParameterFloat>(
        ParameterID::dryWet,
        "Dry/Wet",
        juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
        DEFAULT_DRY_WET
        ));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}
