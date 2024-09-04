/*
  ==============================================================================

    Knob.h
    Created: 4 Sep 2024 4:03:08pm
    Author:  dglaf

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define DEFAULT_W   16
#define DEFAULT_H   16
#define DEFAULT_L   0
#define DEFAULT_T   0

class Knob : public juce::Component {
public:
    juce::Slider slider;
    int width, height;
    int left, top;

    Knob(int w = DEFAULT_W, int h = DEFAULT_H, int l = DEFAULT_L, int t = DEFAULT_T ) :
        width(w), height(h), left(l), top(t)
    {
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);    
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setBounds(0, 0, width, height);
        addAndMakeVisible(slider);

        setSize(width, height);
    }

    ~Knob() {}


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Knob); 

};