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

#define MIN_BPM 60.0f
#define MAX_BPM 300.0f

namespace Colors
{
    const juce::Colour coloredLight{ 255, 160, 150 };
}

class KnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    KnobLookAndFeel() {

    }

    static KnobLookAndFeel* get() {
        static KnobLookAndFeel inst;
        return &inst;
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h, float pos, float startAngle, float endAngle, juce::Slider& slider) override {
        float pi = juce::MathConstants<float>::pi;
        slider.setRotaryParameters(1.25f * pi, 2.75f * pi, true);

        int knobType = slider.getProperties().getWithDefault("type", 0);
        float dialRadius, bottomLineWidth;

        if (knobType == 0) {
            dialRadius = 15.0f;
            bottomLineWidth = 6.0f;
        }
        else {
            dialRadius = 12.0f;
            bottomLineWidth = 5.0f;
        }

        float halfDialRadius = dialRadius / 2.0f;

        auto bounds = juce::Rectangle<int>(x, y, w, h).toFloat();
        auto radius = bounds.getWidth() / 2.0f;
        auto dialBounds = bounds.reduced(dialRadius * 2.0f);
        auto center = bounds.getCentre();
        auto areaRadius = dialBounds.getWidth() / 2.0f - halfDialRadius;

        auto toAngle = startAngle + pos * (endAngle - startAngle);
        juce::Point<float> dialPos(
            center.x + areaRadius * std::sin(toAngle),
            center.y - areaRadius * std::cos(toAngle)
        );

        juce::Path ellipsePath;
        ellipsePath.addEllipse(dialPos.getX() - halfDialRadius, dialPos.getY() - halfDialRadius, dialRadius, dialRadius);
        
        juce::DropShadow dropShadow{ Colors::coloredLight, static_cast<int>(dialRadius) + 3, {0, 0} };
        dropShadow.drawForPath(g, ellipsePath);
        
        g.setColour(Colors::coloredLight);
        g.fillPath(ellipsePath);

        auto arcRadius = radius - 3.0f;

        juce::Path backgroundArc;
        backgroundArc.addCentredArc(center.x,
            center.y,
            arcRadius,
            arcRadius,
            0.0f,
            startAngle,
            endAngle,
            true);
        auto strokeType = juce::PathStrokeType(
            bottomLineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        g.setColour(juce::Colours::black);
        g.strokePath(backgroundArc, strokeType);

        float topLineWidth = bottomLineWidth + 1;

        strokeType = juce::PathStrokeType(
            topLineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

        if (slider.isEnabled()) {
            juce::Path valueArc;
            valueArc.addCentredArc(center.x,
                center.y,
                arcRadius,
                arcRadius,
                0.0f,
                startAngle,
                toAngle,
                true);
            g.setColour(Colors::coloredLight);
            g.strokePath(valueArc, strokeType);
        }
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobLookAndFeel)
};

class Knob : public juce::Component 
{
public:
    juce::Slider slider;
    int width, height;
    int left, top;
    int type;

    // TYPE: 0 - normal, 1 - small, 2 - stereo offset
    Knob(int w = KNOB_W, int h = KNOB_H, int l = KNOB_L, int t = KNOB_T, int tp = 0 ) :
        width(w), height(h), left(l), top(t), type(tp)
    {
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);    
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setBounds(0, 0, width, height);
        slider.getProperties().set("type", type);
        addAndMakeVisible(slider);

        setSize(width, height);

        setLookAndFeel(KnobLookAndFeel::get());
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
    float buttonRatio = 0.714f;

    BPMScreen(float w = TDIV_W, float h = TDIV_H, float l = TDIV_L, float t = TDIV_T) :
        width(static_cast<int>(w)), height(static_cast<int>(h)), left(static_cast<int>(l)), top(static_cast<int>(t))
    {

        setBounds(0, 0, width, height);

        bpmSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        bpmSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        bpmSlider.setRange(MIN_BPM, MAX_BPM, 0.1f);
        bpmSlider.setValue(120.0f);

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

class BPMScreenAttachment
{
public:
    BPMScreenAttachment(juce::AudioProcessorValueTreeState& state, const juce::String& bpmID, const juce::String& clockSrcID ,BPMScreen& screen)
        : bpmScreen(screen), state(state)
    {
        // Store the parameter reference
        bpmParam = state.getParameter(bpmID);
        clockSrcParam = state.getParameter(clockSrcID);

        // Listen to the comboBox changes and update the parameter
        bpmScreen.bpmSlider.onValueChange = [this]() { newBPMValue(); };
        bpmScreen.internalBtn.onClick = [this]() { internalSelected(); };
        bpmScreen.hostBtn.onClick = [this]() { hostSelected(); };

    }

    ~BPMScreenAttachment() = default;

    void newBPMValue()
    {
        if (bpmParam != nullptr)
        {
            // Update the parameter value when the combo box changes
            bpmParam->setValueNotifyingHost(bpmScreen.bpmSlider.getValue() / 999.0f);
        }
    }

    void internalSelected()
    {
        if (clockSrcParam != nullptr)
        {
            // Update the parameter value when the combo box changes
            clockSrcParam->setValueNotifyingHost(0.0f);
        }
    }

    void hostSelected()
    {
        if (clockSrcParam != nullptr)
        {
            // Update the parameter value when the combo box changes
            clockSrcParam->setValueNotifyingHost(1.0f);
        }
    }

private:
    BPMScreen& bpmScreen;
    float bpmValue;
    float clockSrc;
    juce::RangedAudioParameter* bpmParam = nullptr;
    juce::RangedAudioParameter* clockSrcParam = nullptr;
    juce::AudioProcessorValueTreeState& state;

};
