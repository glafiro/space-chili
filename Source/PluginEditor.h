/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GuiComponents.h"

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

#define MULT MULT_M

#define KNOB_SIZE       static_cast<int>(91 * MULT)
#define KNOB_SIZE_S     static_cast<int>(70 * MULT)
#define LEFT_MARGIN     static_cast<int>(38 * MULT)
#define TOP_MARGIN      static_cast<int>(61 * MULT)
#define KNOB_SPACING    static_cast<int>(45 * MULT)
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
    juce::Image screenImage;
    juce::Image bpmScreenImg;
    juce::Image presetManagerImg;
    juce::Image filterSliderImg;
    juce::Image timeSelecImg;
    juce::Image linkBtnImg;
    juce::Image chorusBtnImg;
    juce::Image switchImg;

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
    Knob chorusRateKnob{KNOB_SIZE_S, KNOB_SIZE_S, static_cast<int>(604 * MULT), static_cast<int>(85 * MULT)};
    juce::AudioProcessorValueTreeState::SliderAttachment chorusRateAttachment{
        audioProcessor.apvts, ParameterID::chorusRate.getParamID(), chorusRateKnob.slider
    };
    Knob chorusDepthKnob{KNOB_SIZE_S, KNOB_SIZE_S, static_cast<int>(709 * MULT), static_cast<int>(85 * MULT)};
    juce::AudioProcessorValueTreeState::SliderAttachment chorusDepthAttachment{
        audioProcessor.apvts, ParameterID::chorusDepth.getParamID(), chorusDepthKnob.slider
    };
    Knob stereoOffsetKnob{KNOB_SIZE_S, KNOB_SIZE_S, static_cast<int>(51 * MULT), static_cast<int>(280 * MULT)};
    juce::AudioProcessorValueTreeState::SliderAttachment stereoOffsetAttachment{
        audioProcessor.apvts, ParameterID::leftRightRatio.getParamID(), stereoOffsetKnob.slider
    };
    VerticalSlider lowPassSlider{20, static_cast<int>(120 * MULT), static_cast<int>(325 * MULT), static_cast<int>(230 * MULT)};
    juce::AudioProcessorValueTreeState::SliderAttachment lowPassAttachment{
        audioProcessor.apvts, ParameterID::lowPassFreq.getParamID(), lowPassSlider.slider
    };
    VerticalSlider highPassSlider{ 20, static_cast<int>(120 * MULT), static_cast<int>(380 * MULT), static_cast<int>(230 * MULT) };
    juce::AudioProcessorValueTreeState::SliderAttachment highPassAttachment{
        audioProcessor.apvts, ParameterID::highPassFreq.getParamID(), highPassSlider.slider
    };
    ImageToggleBtn linkBtn{static_cast<int>(25 * MULT),  static_cast<int>(30 * MULT), static_cast<int>(143 * MULT), static_cast<int>(165 * MULT) };
    juce::AudioProcessorValueTreeState::ButtonAttachment linkAttachment{
        audioProcessor.apvts, ParameterID::delaySync.getParamID(), linkBtn.btn
    };
    ImageToggleBtn chorusBtn{static_cast<int>(29 * MULT),  static_cast<int>(33 * MULT), static_cast<int>(582 * MULT), static_cast<int>(37 * MULT) };
    juce::AudioProcessorValueTreeState::ButtonAttachment chorusAttachment{
        audioProcessor.apvts, ParameterID::chorusOn.getParamID(), chorusBtn.btn
    };  
    ImageToggleBtn pingPongBtn{static_cast<int>(21 * MULT),  static_cast<int>(30 * MULT), static_cast<int>(167 * MULT), static_cast<int>(273 * MULT) };
    juce::AudioProcessorValueTreeState::ButtonAttachment pingPongAttachment{
        audioProcessor.apvts, ParameterID::pingPong.getParamID(), pingPongBtn.btn
    };    
    ImageToggleBtn tempoSyncBtn{static_cast<int>(21 * MULT),  static_cast<int>(30 * MULT), static_cast<int>(167 * MULT), static_cast<int>(250 * MULT) };
    juce::AudioProcessorValueTreeState::ButtonAttachment tempoSyncAttachment{
        audioProcessor.apvts, ParameterID::syncToBPM.getParamID(), tempoSyncBtn.btn
    };
    ArrowComboBox timeDivLeftBox{ static_cast<int>(85 * MULT),  static_cast<int>(23 * MULT), static_cast<int>(43 * MULT), static_cast<int>(205 * MULT) };
    ArrowComboBoxAttachment timeDivLeftAttachment{
        audioProcessor.apvts, ParameterID::timeMode.getParamID(), timeDivLeftBox
    };
    ArrowComboBox timeDivRightBox{ static_cast<int>(85 * MULT),  static_cast<int>(23 * MULT), static_cast<int>(180 * MULT), static_cast<int>(205 * MULT) };
    ArrowComboBoxAttachment timeDivRightAttachment{
        audioProcessor.apvts, ParameterID::timeMode.getParamID(), timeDivRightBox
    };

    BPMScreen bpmScreen{ 111 * MULT, 71 * MULT, 164 * MULT, 315 * MULT };

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
