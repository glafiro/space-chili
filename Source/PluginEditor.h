/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Knob.h"

// SIZES
// XLARGE:  1664 X 864
// LARGE    1248 X 648
// MEDIUM   1040 X 540
// SMALL     832 X 432

#define WIDTH   832
#define HEIGHT  432

#define MULT_S  1.0f 
#define MULT_M  1.25f
#define MULT_L  1.5f 
#define MULT_XL 2.0f

#define MULT MULT_XL

#define KNOB_SIZE       static_cast<int>(91 * MULT)
#define LEFT_MARGIN     static_cast<int>(38 * MULT)
#define TOP_MARGIN      static_cast<int>(61 * MULT)
#define KNOB_SPACING    static_cast<int>(44 * MULT)
#define KNOB_SPACING_V  static_cast<int>(96 * MULT)

//==============================================================================
/**
*/
class DelayAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DelayAudioProcessorEditor (DelayAudioProcessor&);
    ~DelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayAudioProcessor& audioProcessor;

    juce::Image bgImage;

    Knob leftLengthKnob{KNOB_SIZE, KNOB_SIZE, LEFT_MARGIN, TOP_MARGIN};
    juce::AudioProcessorValueTreeState::SliderAttachment leftLengthAttachment{
        audioProcessor.apvts, ParameterID::leftDelaySize.getParamID(), leftLengthKnob.slider
    };    Knob rightLengthKnob{KNOB_SIZE, KNOB_SIZE, LEFT_MARGIN + KNOB_SIZE + KNOB_SPACING, TOP_MARGIN};
    juce::AudioProcessorValueTreeState::SliderAttachment rightLengthAttachment {
        audioProcessor.apvts, ParameterID::rightDelaySize.getParamID(), rightLengthKnob.slider
    };
    Knob feedbackKnob{KNOB_SIZE, KNOB_SIZE, LEFT_MARGIN + (KNOB_SIZE + KNOB_SPACING) * 2, TOP_MARGIN};
    juce::AudioProcessorValueTreeState::SliderAttachment feedbackAttachment{
        audioProcessor.apvts, ParameterID::feedback.getParamID(), feedbackKnob.slider
    };
    Knob mixKnob{KNOB_SIZE, KNOB_SIZE, LEFT_MARGIN + (KNOB_SIZE + KNOB_SPACING) * 3, TOP_MARGIN};
    juce::AudioProcessorValueTreeState::SliderAttachment mixAttachment{
        audioProcessor.apvts, ParameterID::dryWet.getParamID(), mixKnob.slider
    };
    Knob duckingKnob{KNOB_SIZE, KNOB_SIZE, LEFT_MARGIN + (KNOB_SIZE + KNOB_SPACING) * 3, TOP_MARGIN + KNOB_SIZE + KNOB_SPACING_V};
    juce::AudioProcessorValueTreeState::SliderAttachment duckingAttachment{
        audioProcessor.apvts, ParameterID::duckingAmount.getParamID(), duckingKnob.slider
    };
    juce::Slider stereoOffsetKnob;
    juce::Slider lowPassSlider;
    juce::Slider highPassSlider;
    juce::Slider chorusDepthKnob;
    juce::Slider chorusRateKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
