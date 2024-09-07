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

float BPM2Ms(int choice, float tempo=120.0f, int timeMode=0) {
    auto mult = 4.0f / static_cast<float>(1 << (choice));
    if (timeMode == TimeMode::TRIPLETS) mult *= (2.0f / 3.0f);   
    if (timeMode == TimeMode::DOTTED) mult *= 1.5f;
    return static_cast<float>((60000.0f / tempo) * mult);
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
    delay(),
    chorus()
#endif
{
    apvts.state.addListener(this);
    castParameter(apvts, ParameterID::leftDelaySize, leftDelaySizeParam);
    castParameter(apvts, ParameterID::rightDelaySize, rightDelaySizeParam);
    castParameter(apvts, ParameterID::feedback, feedbackParam);
    castParameter(apvts, ParameterID::dryWet, dryWetParam);
    castParameter(apvts, ParameterID::delaySync, delaySyncParam);
    castParameter(apvts, ParameterID::syncToBPM, syncToBPMParam);
    castParameter(apvts, ParameterID::internalOrHost, internalOrHostParam);
    castParameter(apvts, ParameterID::internalBPM, internalBPMParam);
    castParameter(apvts, ParameterID::syncedTimeSubdivisionL, syncedTimeSubdivParamL);
    castParameter(apvts, ParameterID::syncedTimeSubdivisionR, syncedTimeSubdivParamR);
    castParameter(apvts, ParameterID::timeMode, timeModeParam);
    castParameter(apvts, ParameterID::pingPong, pingPongParam);
    castParameter(apvts, ParameterID::leftRightRatio, leftRightRatioParam);
    castParameter(apvts, ParameterID::lowPassFreq, lowPassFreqParam);
    castParameter(apvts, ParameterID::highPassFreq, highPassFreqParam);
    castParameter(apvts, ParameterID::duckingAmount, duckingAmountParam);
    castParameter(apvts, ParameterID::delayOn, delayOnParam);
    castParameter(apvts, ParameterID::chorusOn, chorusOnParam);
    castParameter(apvts, ParameterID::chorusDepth, chorusDepthParam);
    castParameter(apvts, ParameterID::chorusRate, chorusRateParam);
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
    int nChannels = getTotalNumInputChannels();
    
    delayParameters.set("sampleRate", sampleRate);
    delayParameters.set("blockSize", samplesPerBlock);
    delayParameters.set("nChannels", nChannels);
    delayParameters.set("delayLength", DEFAULT_DELAY_LEN);
    delayParameters.set("feedback", DEFAULT_FEEDBACK_GAIN * 0.01f);
    delayParameters.set("mix", DEFAULT_DRY_WET * 0.01f);
    delayParameters.set("pingPong", static_cast<float>(DEFAULT_IS_PINGPONG));
    delayParameters.set("lowPassFreq", DEFAULT_LOW_PASS);
    delayParameters.set("highPassFreq", DEFAULT_HIGH_PASS);
    delayParameters.set("ducking", DEFAULT_DUCKING);
    delayParameters.set("isOn", static_cast<float>(DEFAULT_DELAY_ON));

    delay.prepare(delayParameters);

    chorusParameters.set("sampleRate", sampleRate);
    chorusParameters.set("blockSize", samplesPerBlock);
    chorusParameters.set("nChannels", nChannels);
    chorusParameters.set("chorusRate", DEFAULT_CHORUS_RATE);
    chorusParameters.set("chorusDepth", DEFAULT_CHORUS_DEPTH * 0.01f);
    chorusParameters.set("isOn", DEFAULT_CHORUS_ON);


    chorus.prepare(chorusParameters);
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
    bool bpmChanged = false;
    auto hostBPM = getPlayHead()->getPosition()->getBpm();

    if (useHostBPM.load() && hostBPM.hasValue() && *hostBPM != currentHostBPM) {
        currentHostBPM = *hostBPM;
        bpmChanged = true;
    }

    if (bpmChanged || isNonRealtime() || parametersChanged.compare_exchange_strong(expected, false)) {
        update(buffer, currentHostBPM);
    }

    delay.processBlock(
        buffer.getArrayOfWritePointers(),
        buffer.getNumChannels(),
        buffer.getNumSamples()
    );
    
    chorus.processBlock(
        buffer.getArrayOfWritePointers(),
        buffer.getNumChannels(),
        buffer.getNumSamples()
    );
}

void DelayAudioProcessor::update(juce::AudioBuffer<float>& buffer, float hostBPM) {
    float bpm = useHostBPM.load() ? hostBPM : internalBPMParam->get();
    bool linkedSizes = delaySyncParam->get();

    float leftDelaySize;
    float rightDelaySize;

    if (syncToBPMParam->get()) {
        leftDelaySize = BPM2Ms(syncedTimeSubdivParamL->getIndex(), bpm, timeModeParam->get());
        if (linkedSizes) rightDelaySize = leftDelaySize;
        else rightDelaySize = BPM2Ms(syncedTimeSubdivParamR->getIndex(), bpm, timeModeParam->get());
    }

    else {
        leftDelaySize = leftDelaySizeParam->get();
        if (linkedSizes) rightDelaySize = leftDelaySize;
        else rightDelaySize = rightDelaySizeParam->get();
    }

    rightDelaySize *= leftRightRatioParam->get();

    delayParameters.set("leftDelayLength",  leftDelaySize);
    delayParameters.set("rightDelayLength", rightDelaySize);
    delayParameters.set("feedback", feedbackParam->get() * 0.01f);
    delayParameters.set("mix", dryWetParam->get() * 0.01f);
    delayParameters.set("pingPong", static_cast<float>(pingPongParam->get()));
    delayParameters.set("lowPassFreq", lowPassFreqParam->get());
    delayParameters.set("highPassFreq", highPassFreqParam->get());
    delayParameters.set("ducking", duckingAmountParam->get() * 0.01f);
    delayParameters.set("isOn", static_cast<float>(delayOnParam->get()));

    delay.update(delayParameters);

    chorusParameters.set("chorusDepth", chorusDepthParam->get() * 0.01f);
    chorusParameters.set("chorusRate", chorusRateParam->get());
    chorusParameters.set("isOn", chorusOnParam->get());

    chorus.update(chorusParameters);
}

//==============================================================================
bool DelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
    return new DelayAudioProcessorEditor(*this);
    /*auto editor = new juce::GenericAudioProcessorEditor(*this);
    editor->setSize(500, 500);
    return editor;*/
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
        juce::NormalisableRange<float>{ 1.0f, 1000.0f, 0.01f, 0.8f },
        DEFAULT_DELAY_LEN
        ));

    layout.add(std::make_unique <juce::AudioParameterFloat>(
        ParameterID::rightDelaySize,
        "Right (ms)",
        juce::NormalisableRange<float>{ 1.0f, 1000.0f, 0.01f, 0.8f },
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
    
    layout.add(std::make_unique <juce::AudioParameterBool>(
        ParameterID::delaySync,
        "Link",
        DEFAULT_LINK
        ));    
    
    layout.add(std::make_unique <juce::AudioParameterBool>(
        ParameterID::syncToBPM,
        "Sync to BPM",
        DEFAULT_SYNC
        ));    

    layout.add(std::make_unique <juce::AudioParameterChoice>(
        ParameterID::internalOrHost,
        "Clock source",
        juce::StringArray{ "Internal", "Host"},
        DEFAULT_CLOCK_SRC
    ));

    layout.add(std::make_unique <juce::AudioParameterFloat>(
        ParameterID::internalBPM,
        "Tempo",
        juce::NormalisableRange<float>{0.0f, 999.0f, 1.0f},
        DEFAULT_BPM
    ));

    
    layout.add(std::make_unique <juce::AudioParameterChoice>(
        ParameterID::syncedTimeSubdivisionL,
        "Time",
        juce::StringArray{ "1/1", "1/2", "1/4", "1/8", "1/16"},
        DEFAULT_SUBDIVISION
        ));
    
    layout.add(std::make_unique <juce::AudioParameterChoice>(
        ParameterID::syncedTimeSubdivisionR,
        "Time",
        juce::StringArray{ "1/1", "1/2", "1/4", "1/8", "1/16"},
        DEFAULT_SUBDIVISION
        ));

    layout.add(std::make_unique <juce::AudioParameterInt>(
        ParameterID::timeMode,
        "Time mode",
        0, 2,
        DEFAULT_TIME_MODE
    ));

    layout.add(std::make_unique <juce::AudioParameterBool>(
        ParameterID::pingPong,
        "Ping Pong",
        DEFAULT_IS_PINGPONG
        ));

    layout.add(std::make_unique <juce::AudioParameterFloat>(
        ParameterID::leftRightRatio,
        "Ratio",
        juce::NormalisableRange<float>{0.75f, 1.25f, 0.01f},
        DEFAULT_LR_RATIO
    ));
    
    layout.add(std::make_unique <juce::AudioParameterFloat>(
        ParameterID::lowPassFreq,
        "Lo",
        juce::NormalisableRange<float>{20.0f, 20000.0f, 0.1f, 0.3f, false},
        DEFAULT_LOW_PASS
    ));
        
    layout.add(std::make_unique <juce::AudioParameterFloat>(
        ParameterID::highPassFreq,
        "Hi",
        juce::NormalisableRange<float>{20.0f, 20000.0f, 0.1f, 0.3f, false},
        DEFAULT_HIGH_PASS
    ));
            
    layout.add(std::make_unique <juce::AudioParameterFloat>(
        ParameterID::duckingAmount,
        "Ducking",
        juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
        DEFAULT_DUCKING
    ));               

    layout.add(std::make_unique <juce::AudioParameterBool>(
        ParameterID::delayOn,
        "Delay On",
        DEFAULT_DELAY_ON

    ));
    
    layout.add(std::make_unique <juce::AudioParameterBool>(
        ParameterID::chorusOn,
        "Chorus On",
        DEFAULT_CHORUS_ON
    ));

    layout.add(std::make_unique <juce::AudioParameterFloat>(
        ParameterID::chorusDepth,
        "Chorus Depth",
        juce::NormalisableRange<float>{0.0f, 100.0f, 0.1f},
        DEFAULT_CHORUS_DEPTH
    ));
    
    layout.add(std::make_unique <juce::AudioParameterFloat>(
        ParameterID::chorusRate,
        "Chorus Rate",
        juce::NormalisableRange<float>{0.2f, 1.2f, 0.1f},
        DEFAULT_CHORUS_RATE
    ));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayAudioProcessor();
}
