/*
  ==============================================================================

    Knob.h
    Created: 4 Sep 2024 4:03:08pm
    Author:  dglaf

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define KNOB_W   16
#define KNOB_H   16
#define KNOB_L   0
#define KNOB_T   0

#define SLID_W  10
#define SLID_H  116
#define SLID_L  0
#define SLID_T  0

#define BTN_W  16
#define BTN_H  16
#define BTN_L  0   
#define BTN_T  0

#define TDIV_W  64
#define TDIV_H  64
#define TDIV_L  0
#define TDIV_T  0


class Knob : public juce::Component 
{
public:
    juce::Slider slider;
    int width, height;
    int left, top;

    Knob(int w = KNOB_W, int h = KNOB_H, int l = KNOB_L, int t = KNOB_T ) :
        width(w), height(h), left(l), top(t)
    {
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);    
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setBounds(0, 0, width, height);
        addAndMakeVisible(slider);

        setSize(width, height);
    }

    ~Knob() {}


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Knob); 

};

class VerticalSlider : public juce::Component
{
public:
    juce::Slider slider;
    int width, height;
    int left, top;

    VerticalSlider(int w = SLID_W, int h = SLID_H, int l = SLID_L, int t = SLID_T) :
        width(w), height(h), left(l), top(t)
    {
        slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setBounds(0, 0, width, height);
        addAndMakeVisible(slider);
        setSize(width, height);
    }
};

class ImageToggleBtn : public juce::Component
{
public:
    juce::Image offImage;
    juce::Image onImage;

    int width, height;
    int top, left;
    juce::ToggleButton btn;

    ImageToggleBtn(int w = BTN_W, int h = BTN_H, int l = BTN_L, int t = BTN_T) : 
        width(w), height(h), left(l), top(t)
    {
        btn.setClickingTogglesState(true);
        btn.setBounds(0, 0, width, height);
        addAndMakeVisible(btn);
        
        setSize(width, height);
    }

    void setImages(juce::Image& on, juce::Image& off) {
        onImage = on;
        offImage = off;
    }

};

class ArrowComboBox : public juce::Component, public juce::Button::Listener
{
public:
    int width, height;
    int top, left;

    ArrowComboBox(int w = TDIV_W, int h = TDIV_H, int l = TDIV_L, int t = TDIV_T) :
        width(w), height(h), left(l), top(t)
    {
        // Initialize the array of values
        options.add("Straight");
        options.add("Triplets");
        options.add("Dotted");

        // Initialize buttons
        leftButton.setButtonText("<");
        rightButton.setButtonText(">");

        setBounds(0, 0, width, height);

        // Add and configure the buttons
        addAndMakeVisible(leftButton);
        addAndMakeVisible(rightButton);
        addAndMakeVisible(optionLabel);

        // Add listeners
        leftButton.addListener(this);
        rightButton.addListener(this);

        // Set initial option
        currentIndex = 0;
        optionLabel.setText(options[currentIndex], juce::dontSendNotification);
    }


    void resized() override
    {
        // Position buttons and label
        auto area = getLocalBounds();
        auto buttonWidth = 30;

        leftButton.setBounds(area.removeFromLeft(buttonWidth));
        rightButton.setBounds(area.removeFromRight(buttonWidth));
        optionLabel.setBounds(area);  // Centered label between buttons
    }

    void buttonClicked(juce::Button* button) override
    {
        if (button == &leftButton)
        {
            currentIndex = (currentIndex - 1);
            if (currentIndex < 0) currentIndex += options.size();// Cycle back
        }
        else if (button == &rightButton)
        {
            currentIndex = (currentIndex + 1) % options.size(); // Cycle forward
        }

        // Update the label text with the current option
        optionLabel.setText(options[currentIndex], juce::dontSendNotification);
    }

    // Function to get the current selected option
    juce::String getCurrentOption() const
    {
        return options[currentIndex];
    }

    int getCurrentOptionIndex() const
    {
        return currentIndex;
    }
    juce::TextButton leftButton{ "<" };
    juce::TextButton rightButton{ ">" };

private:
    juce::Label optionLabel;

    juce::StringArray options;
    int currentIndex;
};

class ArrowComboBoxAttachment
{
public:
    ArrowComboBoxAttachment(juce::AudioProcessorValueTreeState& state, const juce::String& parameterID, ArrowComboBox& comboBox)
        : comboBox(comboBox), state(state)
    {
        // Store the parameter reference
        parameter = state.getParameter(parameterID);

        // Listen to the comboBox changes and update the parameter
        comboBox.leftButton.onClick = [this]() { comboBoxChanged(); };
        comboBox.rightButton.onClick = [this]() { comboBoxChanged(); };

    }

    ~ArrowComboBoxAttachment() = default;

    void comboBoxChanged()
    {
        if (parameter != nullptr)
        {
            // Update the parameter value when the combo box changes
            parameter->setValueNotifyingHost(comboBox.getCurrentOptionIndex() / 2.0f);
        }
    }

private:
    ArrowComboBox& comboBox;
    juce::RangedAudioParameter * parameter = nullptr;
    juce::AudioProcessorValueTreeState& state;

};

class BPMScreen : public juce::Component
{
public: 
    int width, height;
    int left, top;
    juce::Slider bpmSlider;
    juce::TextButton internalBtn{"BPM"};
    juce::TextButton hostBtn{"HOST"};
    int choice = 0; // INTERNAL = 0, HOST = 1
    float buttonRatio = 0.714f;

    BPMScreen(float w = TDIV_W, float h = TDIV_H, float l = TDIV_L, float t = TDIV_T) :
        width(static_cast<int>(w)), height(static_cast<int>(h)), left(static_cast<int>(l)), top(static_cast<int>(t))
    {
        setBounds(0, 0, width, height);

        bpmSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        bpmSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

        // Add and configure the buttons
        addAndMakeVisible(bpmSlider);
        addAndMakeVisible(internalBtn);
        addAndMakeVisible(hostBtn);

    }

    void resized() override
    {
        // Position buttons and label
        auto area = getLocalBounds();

        auto sliderArea = juce::Rectangle<int>(area.getX(), area.getY(), width, height * buttonRatio);
        auto internalBtnArea = juce::Rectangle<int>(sliderArea.getX(), sliderArea.getY() + sliderArea.getHeight(), width * 0.5f, height * (1.0f - buttonRatio));
        auto hostBtnArea = juce::Rectangle<int>(sliderArea.getX() + width * 0.5f, sliderArea.getY() + sliderArea.getHeight(), width * 0.5f, height * (1.0f - buttonRatio));
        bpmSlider.setBounds(sliderArea);
        internalBtn.setBounds(internalBtnArea);
        hostBtn.setBounds(hostBtnArea);
    }
};