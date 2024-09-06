/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#define MULT_S  1.0f 
#define MULT_M  1.25f
#define MULT_L  1.5f 
#define MULT_XL 2.0f

#define MULT MULT_L

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


#define KNOB_SIZE       static_cast<int>(120 * MULT)
#define KNOB_SIZE_S     static_cast<int>(93 * MULT)
#define LEFT_MARGIN     static_cast<int>(24 * MULT)
#define TOP_MARGIN      static_cast<int>(49 * MULT)
#define KNOB_SPACING    static_cast<int>(15 * MULT)
#define KNOB_SPACING_V  static_cast<int>(68 * MULT)

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

    Knob leftLengthKnob{KNOB_SIZE, KNOB_SIZE, LEFT_MARGIN, TOP_MARGIN, 0};
    juce::AudioProcessorValueTreeState::SliderAttachment leftLengthAttachment{
        audioProcessor.apvts, ParameterID::leftDelaySize.getParamID(), leftLengthKnob.slider
    };    Knob rightLengthKnob{KNOB_SIZE, KNOB_SIZE, LEFT_MARGIN + KNOB_SIZE + KNOB_SPACING, TOP_MARGIN, 0};
    juce::AudioProcessorValueTreeState::SliderAttachment rightLengthAttachment {
        audioProcessor.apvts, ParameterID::rightDelaySize.getParamID(), rightLengthKnob.slider
    };
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
    Knob chorusRateKnob{KNOB_SIZE_S, KNOB_SIZE_S, static_cast<int>(592 * MULT), static_cast<int>(74 * MULT), 1};
    juce::AudioProcessorValueTreeState::SliderAttachment chorusRateAttachment{
        audioProcessor.apvts, ParameterID::chorusRate.getParamID(), chorusRateKnob.slider
    };
    Knob chorusDepthKnob{KNOB_SIZE_S, KNOB_SIZE_S, static_cast<int>(697 * MULT), static_cast<int>(74 * MULT), 1};
    juce::AudioProcessorValueTreeState::SliderAttachment chorusDepthAttachment{
        audioProcessor.apvts, ParameterID::chorusDepth.getParamID(), chorusDepthKnob.slider
    };
    Knob stereoOffsetKnob{KNOB_SIZE_S, KNOB_SIZE_S, static_cast<int>(39 * MULT), static_cast<int>(269 * MULT), 2};
    juce::AudioProcessorValueTreeState::SliderAttachment stereoOffsetAttachment{
        audioProcessor.apvts, ParameterID::leftRightRatio.getParamID(), stereoOffsetKnob.slider
    };
    VerticalSlider lowPassSlider{ static_cast<int>(40 * MULT), static_cast<int>(120 * MULT), static_cast<int>(318 * MULT), static_cast<int>(230 * MULT) };
    juce::AudioProcessorValueTreeState::SliderAttachment lowPassAttachment{
        audioProcessor.apvts, ParameterID::lowPassFreq.getParamID(), lowPassSlider.slider
    };
    VerticalSlider highPassSlider{ static_cast<int>(40 * MULT), static_cast<int>(120 * MULT), static_cast<int>(373 * MULT), static_cast<int>(230 * MULT)};
    juce::AudioProcessorValueTreeState::SliderAttachment highPassAttachment{
        audioProcessor.apvts, ParameterID::highPassFreq.getParamID(), highPassSlider.slider
    };
    ImageToggleBtn linkBtn{static_cast<int>(28 * MULT),  static_cast<int>(32 * MULT), static_cast<int>(139 * MULT), static_cast<int>(168 * MULT), 0 };
    juce::AudioProcessorValueTreeState::ButtonAttachment linkAttachment{
        audioProcessor.apvts, ParameterID::delaySync.getParamID(), linkBtn.btn
    };
    ImageToggleBtn chorusBtn{static_cast<int>(29 * MULT),  static_cast<int>(33 * MULT), static_cast<int>(580 * MULT), static_cast<int>(38 * MULT), 1};
    juce::AudioProcessorValueTreeState::ButtonAttachment chorusAttachment{
        audioProcessor.apvts, ParameterID::chorusOn.getParamID(), chorusBtn.btn
    };  
    ImageToggleBtn pingPongBtn{static_cast<int>(32 * MULT),  static_cast<int>(24 * MULT), static_cast<int>(168 * MULT), static_cast<int>(272 * MULT), 2 };
    juce::AudioProcessorValueTreeState::ButtonAttachment pingPongAttachment{
        audioProcessor.apvts, ParameterID::pingPong.getParamID(), pingPongBtn.btn
    };    
    ImageToggleBtn tempoSyncBtn{static_cast<int>(32 * MULT),  static_cast<int>(24 * MULT), static_cast<int>(168 * MULT), static_cast<int>(251 * MULT), 2 };
    juce::AudioProcessorValueTreeState::ButtonAttachment tempoSyncAttachment{
        audioProcessor.apvts, ParameterID::syncToBPM.getParamID(), tempoSyncBtn.btn
    };
    TimeModeBox timeDivLeftBox{ static_cast<int>(90 * MULT),  static_cast<int>(28 * MULT), static_cast<int>(43 * MULT), static_cast<int>(205 * MULT) };
    TimeModeBoxAttachment timeDivLeftAttachment{
        audioProcessor.apvts, ParameterID::timeMode.getParamID(), timeDivLeftBox
    };
    TimeModeBox timeDivRightBox{ static_cast<int>(90 * MULT),  static_cast<int>(28 * MULT), static_cast<int>(180 * MULT), static_cast<int>(205 * MULT) };
    TimeModeBoxAttachment timeDivRightAttachment{
        audioProcessor.apvts, ParameterID::timeMode.getParamID(), timeDivRightBox
    };
    BPMScreen bpmScreen{ 111 * MULT, 71 * MULT, 164 * MULT, 315 * MULT};
    BPMScreenAttachment bpmScreenAttachment{
    audioProcessor.apvts, ParameterID::internalBPM.getParamID(), ParameterID::internalOrHost.getParamID(), bpmScreen };

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayAudioProcessorEditor)
};
