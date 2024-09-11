/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

// SIZES
// XLARGE:  1664 X 864
// LARGE    1248 X 648
// MEDIUM   1040 X 540
// SMALL     832 X 432
#define MULT_S  1.0f 
#define MULT_M  1.25f
#define MULT_L  1.5f 
#define MULT_XL 2.0f

#define MULT MULT_S

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GuiComponents.h"

#define BASE_W   832
#define BASE_H  432

#define KNOB_SIZE       120 * MULT
#define KNOB_SIZE_S     93 * MULT
#define LEFT_MARGIN     24 * MULT
#define TOP_MARGIN      49 * MULT
#define KNOB_SPACING    15 * MULT
#define KNOB_SPACING_V  68 * MULT

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
    DelayAudioProcessor& audioProcessor;

    juce::Image bgImage;
    juce::Image screenImage;
    juce::Image bpmScreenImg;
    juce::Image presetManagerImg;

    Knob leftLengthKnob{KNOB_SIZE, KNOB_SIZE, LEFT_MARGIN, TOP_MARGIN, 0};
    Knob rightLengthKnob{KNOB_SIZE, KNOB_SIZE, LEFT_MARGIN + KNOB_SIZE + KNOB_SPACING, TOP_MARGIN, 0};

    Knob feedbackKnob{KNOB_SIZE, KNOB_SIZE, LEFT_MARGIN + (KNOB_SIZE + KNOB_SPACING) * 2, TOP_MARGIN, 0};
    juce::AudioProcessorValueTreeState::SliderAttachment feedbackAttachment{
        audioProcessor.apvts, ParameterID::feedback.getParamID(), feedbackKnob.slider
    };
    Knob mixKnob{KNOB_SIZE, KNOB_SIZE, LEFT_MARGIN + (KNOB_SIZE + KNOB_SPACING) * 3 + 2, TOP_MARGIN, 0};
    juce::AudioProcessorValueTreeState::SliderAttachment mixAttachment{
        audioProcessor.apvts, ParameterID::dryWet.getParamID(), mixKnob.slider
    };
    Knob duckingKnob{KNOB_SIZE, KNOB_SIZE, LEFT_MARGIN + (KNOB_SIZE + KNOB_SPACING) * 3 + 2, TOP_MARGIN + KNOB_SIZE + KNOB_SPACING_V, 0};
    juce::AudioProcessorValueTreeState::SliderAttachment duckingAttachment{
        audioProcessor.apvts, ParameterID::duckingAmount.getParamID(), duckingKnob.slider
    };
    Knob chorusRateKnob{KNOB_SIZE_S, KNOB_SIZE_S, 592 * MULT, 74 * MULT, 1};
    juce::AudioProcessorValueTreeState::SliderAttachment chorusRateAttachment{
        audioProcessor.apvts, ParameterID::chorusRate.getParamID(), chorusRateKnob.slider
    };
    Knob chorusDepthKnob{KNOB_SIZE_S, KNOB_SIZE_S, 697 * MULT, 74 * MULT, 1};
    juce::AudioProcessorValueTreeState::SliderAttachment chorusDepthAttachment{
        audioProcessor.apvts, ParameterID::chorusDepth.getParamID(), chorusDepthKnob.slider
    };
    Knob stereoOffsetKnob{KNOB_SIZE_S, KNOB_SIZE_S, 39 * MULT, 269 * MULT, 2};
    juce::AudioProcessorValueTreeState::SliderAttachment stereoOffsetAttachment{
        audioProcessor.apvts, ParameterID::leftRightRatio.getParamID(), stereoOffsetKnob.slider
    };
    VerticalSlider lowPassSlider{ 40 * MULT, 120 * MULT, 318 * MULT, 230 * MULT };
    juce::AudioProcessorValueTreeState::SliderAttachment lowPassAttachment{
        audioProcessor.apvts, ParameterID::lowPassFreq.getParamID(), lowPassSlider.slider
    };
    VerticalSlider highPassSlider{ 40 * MULT, 120 * MULT, 373 * MULT, 230 * MULT};
    juce::AudioProcessorValueTreeState::SliderAttachment highPassAttachment{
        audioProcessor.apvts, ParameterID::highPassFreq.getParamID(), highPassSlider.slider
    };
    ImageToggleBtn linkBtn{28 * MULT,  32 * MULT, 139 * MULT, 168 * MULT, 0 };
    juce::AudioProcessorValueTreeState::ButtonAttachment linkAttachment{
        audioProcessor.apvts, ParameterID::delaySync.getParamID(), linkBtn.btn
    };
    ImageToggleBtn chorusBtn{29 * MULT,  33 * MULT, 580 * MULT, 38 * MULT, 1};
    juce::AudioProcessorValueTreeState::ButtonAttachment chorusAttachment{
        audioProcessor.apvts, ParameterID::chorusOn.getParamID(), chorusBtn.btn
    };  
    ImageToggleBtn pingPongBtn{32 * MULT,  24 * MULT, 168 * MULT, 272 * MULT, 2 };
    juce::AudioProcessorValueTreeState::ButtonAttachment pingPongAttachment{
        audioProcessor.apvts, ParameterID::pingPong.getParamID(), pingPongBtn.btn
    };    
    ImageToggleBtn tempoSyncBtn{32 * MULT,  24 * MULT, 168 * MULT, 251 * MULT, 2 };
    juce::AudioProcessorValueTreeState::ButtonAttachment tempoSyncAttachment{
        audioProcessor.apvts, ParameterID::syncToBPM.getParamID(), tempoSyncBtn.btn
    };
    TimeModeBox timeDivLeftBox{ 90 * MULT,  28 * MULT, 43 * MULT, 205 * MULT };
    juce::AudioProcessorValueTreeState::ComboBoxAttachment timeDivLeftAttachment{
        audioProcessor.apvts, ParameterID::timeModeL.getParamID(), timeDivLeftBox.optionBox
    };
    TimeModeBox timeDivRightBox{ 90 * MULT,  28 * MULT, 180 * MULT, 205 * MULT };
    juce::AudioProcessorValueTreeState::ComboBoxAttachment timeDivRightAttachment{
        audioProcessor.apvts, ParameterID::timeModeR.getParamID(), timeDivRightBox.optionBox
    };
    BPMScreen bpmScreen{ 111 * MULT, 71 * MULT, 164 * MULT, 315 * MULT};
    BPMScreenAttachment bpmScreenAttachment{
    audioProcessor.apvts, ParameterID::internalBPM.getParamID(), ParameterID::internalOrHost.getParamID(), bpmScreen };

    juce::Rectangle<float> tmgArea{ 0, 0, LEFT_MARGIN + (KNOB_SIZE + KNOB_SPACING) * 2, 251 * MULT + 24 * MULT };
    TimeManagementGroup tmg{ tmgArea, audioProcessor.apvts, &leftLengthKnob, &rightLengthKnob, &linkBtn, &timeDivLeftBox, &timeDivRightBox, &tempoSyncBtn};

    PresetMenu presetMenu{ juce::Rectangle<float>(574 * MULT, 225 * MULT, 238 * MULT, 70 * MULT), audioProcessor.getPresetManager()};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
