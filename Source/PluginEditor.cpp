/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessorEditor::DelayAudioProcessorEditor (DelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    bgImage = juce::ImageCache::getFromMemory(BinaryData::delaybg1_png, BinaryData::delaybg1_pngSize);

    addAndMakeVisible(leftLengthKnob);
    addAndMakeVisible(rightLengthKnob);
    addAndMakeVisible(feedbackKnob);
    addAndMakeVisible(mixKnob);
    addAndMakeVisible(duckingKnob);
    
    setSize (WIDTH * MULT, HEIGHT * MULT);
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
}

//==============================================================================
void DelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.drawImage(bgImage, getLocalBounds().toFloat());

    g.setColour (juce::Colours::black);
    g.setFont (juce::FontOptions (15.0f));
}

void DelayAudioProcessorEditor::resized()
{

    leftLengthKnob.setTopLeftPosition(leftLengthKnob.left, leftLengthKnob.top);
    rightLengthKnob.setTopLeftPosition(rightLengthKnob.left, rightLengthKnob.top);
    feedbackKnob.setTopLeftPosition(feedbackKnob.left, feedbackKnob.top);
    mixKnob.setTopLeftPosition(mixKnob.left, mixKnob.top);
    duckingKnob.setTopLeftPosition(duckingKnob.left, duckingKnob.top);
}
