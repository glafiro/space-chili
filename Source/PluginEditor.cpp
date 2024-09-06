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
    bgImage = juce::ImageCache::getFromMemory(BinaryData::baselayout_png, BinaryData::baselayout_pngSize);
    screenImage = juce::ImageCache::getFromMemory(BinaryData::screens_png, BinaryData::screens_pngSize);
    bpmScreenImg = screenImage.getClippedImage(juce::Rectangle<int>(486, 12, 224, 142));
    presetManagerImg = screenImage.getClippedImage(juce::Rectangle<int>(0, 0, 486, 154));

    addAndMakeVisible(leftLengthKnob);
    addAndMakeVisible(rightLengthKnob);
    addAndMakeVisible(feedbackKnob);
    addAndMakeVisible(mixKnob);
    addAndMakeVisible(duckingKnob);
    addAndMakeVisible(chorusRateKnob);
    addAndMakeVisible(chorusDepthKnob);
    addAndMakeVisible(stereoOffsetKnob);
    addAndMakeVisible(lowPassSlider);
    addAndMakeVisible(highPassSlider);
    addAndMakeVisible(linkBtn);
    addAndMakeVisible(chorusBtn);
    addAndMakeVisible(pingPongBtn);
    addAndMakeVisible(tempoSyncBtn);
    addAndMakeVisible(timeDivLeftBox);
    addAndMakeVisible(timeDivRightBox);
    addAndMakeVisible(bpmScreen);

    
    setSize (WIDTH * MULT, HEIGHT * MULT);
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
}

//==============================================================================
void DelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.drawImage(bgImage, getLocalBounds().toFloat());
    g.drawImage(bpmScreenImg, juce::Rectangle<float>(164 * MULT, 315 * MULT, 111 * MULT, 71 * MULT));
    g.drawImage(presetManagerImg, juce::Rectangle<float>(571 * MULT, 223 * MULT, 243 * MULT, 77 * MULT));

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
    chorusRateKnob.setTopLeftPosition(chorusRateKnob.left, chorusRateKnob.top);
    chorusDepthKnob.setTopLeftPosition(chorusDepthKnob.left, chorusDepthKnob.top);
    stereoOffsetKnob.setTopLeftPosition(stereoOffsetKnob.left, stereoOffsetKnob.top);
    lowPassSlider.setTopLeftPosition(lowPassSlider.left, lowPassSlider.top);
    highPassSlider.setTopLeftPosition(highPassSlider.left, highPassSlider.top);
    linkBtn.setTopLeftPosition(linkBtn.left, linkBtn.top);
    chorusBtn.setTopLeftPosition(chorusBtn.left, chorusBtn.top);
    pingPongBtn.setTopLeftPosition(pingPongBtn.left, pingPongBtn.top);
    tempoSyncBtn.setTopLeftPosition(tempoSyncBtn.left, tempoSyncBtn.top);
    timeDivLeftBox.setBounds(timeDivLeftBox.left, timeDivLeftBox.top, timeDivLeftBox.width, timeDivLeftBox.height);
    timeDivRightBox.setBounds(timeDivRightBox.left, timeDivRightBox.top, timeDivRightBox.width, timeDivRightBox.height);
    bpmScreen.setBounds(bpmScreen.left, bpmScreen.top, bpmScreen.width, bpmScreen.height);
}
