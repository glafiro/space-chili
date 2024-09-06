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
    const juce::Colour btnText{ 96, 87, 55 };
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
};


class FilterSliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
    FilterSliderLookAndFeel() {

    }

    static FilterSliderLookAndFeel* get() {
        static FilterSliderLookAndFeel inst;
        return &inst;
    }

    void drawLinearSlider(juce::Graphics& g, int x, int y, int w, int h, 
        float pos, float minPos, float maxPos, juce::Slider::SliderStyle, juce::Slider& slider) override {
        
        auto filterSliderImg = juce::ImageCache::getFromMemory(BinaryData::sliderbtn_png, BinaryData::sliderbtn_pngSize);
        float dialWidth = 30 * MULT;
        auto absoluteSliderPos = juce::Rectangle<float>(x, pos - 10 * MULT, 30 * MULT, 15 * MULT);
        g.drawImage(filterSliderImg, juce::Rectangle<float>(absoluteSliderPos));

    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterSliderLookAndFeel)
};


class ImageBtnLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ImageBtnLookAndFeel() {

    }

    static ImageBtnLookAndFeel* get() {
        static ImageBtnLookAndFeel inst;
        return &inst;
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& btn, bool highlighted, bool down) override {
        auto area = btn.getLocalBounds();
        auto toggle = btn.getToggleState();
        float type = btn.getProperties().getWithDefault("type", 0.0f);

        juce::Image offImg;
        juce::Image onImg;

        if (type == 0.0f) {
            onImg = juce::ImageCache::getFromMemory(BinaryData::linkon_png, BinaryData::linkon_pngSize);
            offImg = juce::ImageCache::getFromMemory(BinaryData::linkoff_png, BinaryData::linkoff_pngSize);
        }
        else if (type == 1.0f) {
            onImg = juce::ImageCache::getFromMemory(BinaryData::choruson_png, BinaryData::choruson_pngSize);
            offImg = juce::ImageCache::getFromMemory(BinaryData::chorusoff_png, BinaryData::chorusoff_pngSize);
        }

        if (toggle) {
            g.drawImage(onImg, area.toFloat());
        }
        else {
            g.drawImage(offImg, area.toFloat());
        }
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageBtnLookAndFeel)
};


class SwitchLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SwitchLookAndFeel() {

    }

    static SwitchLookAndFeel* get() {
        static SwitchLookAndFeel inst;
        return &inst;
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& btn, bool highlighted, bool down) override {
        auto area = btn.getLocalBounds();
        auto toggle = btn.getToggleState();
        
        auto img = juce::ImageCache::getFromMemory(BinaryData::switch_png, BinaryData::switch_pngSize);
        float dialWidth = 18 * MULT;
        auto onArea = juce::Rectangle<float>(area.getX() + area.getWidth() - dialWidth, area.getY(), 18 * MULT, 27 * MULT);
        auto offArea = juce::Rectangle<float>(area.getX(), area.getY(), 18 * MULT, 27 * MULT);

        if (toggle) {
            g.drawImage(img, onArea);
        }
        else {
            g.drawImage(img, offArea);
        }
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SwitchLookAndFeel)
};

class TimeModeLookAndFeel : public juce::LookAndFeel_V4
{
public:
    TimeModeLookAndFeel() {
        labelFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::arial_narrow_7_ttf, BinaryData::arial_narrow_7_ttfSize));
    }

    static TimeModeLookAndFeel* get() {
        static TimeModeLookAndFeel inst;
        return &inst;
    }

    // Customize Label appearance
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        auto bounds = label.getLocalBounds(); // Get the label's bounds
        float fontSize = bounds.getHeight() * 0.7f; // Dynamically set font size based on label size
        g.setFont(labelFont.withHeight(fontSize));
        g.setColour(Colors::btnText);
        g.drawFittedText(label.getText(), label.getLocalBounds(), juce::Justification::centred, 1);
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& btn, const juce::Colour& bg, bool highlight, bool down) override {}

    void drawButtonText(juce::Graphics& g, juce::TextButton& btn, bool highlight, bool down) override {
        if (highlight) {
            g.setColour(juce::Colours::white);
        }
        else {
            g.setColour(Colors::btnText);
        }

        float fontSize = btn.getLocalBounds().getHeight(); // Dynamically set font size based on label size

        g.setFont(labelFont.withHeight(fontSize));
        g.drawFittedText(btn.getButtonText(), btn.getLocalBounds(), juce::Justification::centred, 1);

    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeModeLookAndFeel)

    juce::Font labelFont;
    juce::Font btnFont;
};

class BPMScreenLookAndFeel : public juce::LookAndFeel_V4
{
public:
    BPMScreenLookAndFeel() {
        labelFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::arial_narrow_7_ttf, BinaryData::arial_narrow_7_ttfSize));
    }

    static BPMScreenLookAndFeel* get() {
        static BPMScreenLookAndFeel inst;
        return &inst;
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& btn, const juce::Colour& bg, bool highlight, bool down) override {}

    void drawButtonText(juce::Graphics& g, juce::TextButton& btn, bool highlight, bool down) override {
        auto selected = btn.getToggleState();
        if (highlight) {
            g.setColour(juce::Colours::white);
        }
        else if (selected) {
            g.setColour(Colors::coloredLight);
        }
        else {
            g.setColour(Colors::btnText);
        }

        float fontSize = btn.getLocalBounds().getHeight() * 0.8f; // Dynamically set font size based on label size

        g.setFont(labelFont.withHeight(fontSize));
        g.drawFittedText(btn.getButtonText(), btn.getLocalBounds(), juce::Justification::centred, 1);

    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BPMScreenLookAndFeel)

        juce::Font labelFont;
    juce::Font btnFont;
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

        setLookAndFeel(FilterSliderLookAndFeel::get());
    }
};

class ImageToggleBtn : public juce::Component
{
public:
    juce::Image offImage;
    juce::Image onImage;

    int width, height;
    int top, left;
    int type;
    juce::ToggleButton btn;

    // TYPE 0 = link, 1 = CHORUS
    ImageToggleBtn(int w = BTN_W, int h = BTN_H, int l = BTN_L, int t = BTN_T, int tp = 0) : 
        width(w), height(h), left(l), top(t), type(tp)
    {
        btn.setClickingTogglesState(true);
        btn.setBounds(0, 0, width, height);
        btn.getProperties().set("type", type);
        addAndMakeVisible(btn);
        
        setSize(width, height);

        if (type != 2) {
            setLookAndFeel(ImageBtnLookAndFeel::get());
        }
        else {
            setLookAndFeel(SwitchLookAndFeel::get());
        }

    }

    void setImages(juce::Image& on, juce::Image& off) {
        onImage = on;
        offImage = off;
    }
};

class TimeModeBox : public juce::Component, public juce::Button::Listener
{
public:
    int width, height;
    int top, left;
    juce::Image bgImg;

    TimeModeBox(int w = TDIV_W, int h = TDIV_H, int l = TDIV_L, int t = TDIV_T) :
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

        bgImg = juce::ImageCache::getFromMemory(BinaryData::timeselect_png, BinaryData::timeselect_pngSize);

        // Add listeners
        leftButton.addListener(this);
        rightButton.addListener(this);

        // Set initial option
        currentIndex = 0;
        optionLabel.setText(options[currentIndex], juce::dontSendNotification);

        setLookAndFeel(TimeModeLookAndFeel::get());
    }

    void paint(juce::Graphics& g) override {
        g.drawImage(bgImg, getLocalBounds().toFloat());
    }



    void resized() override
    {
        // Position buttons and label
        auto area = getLocalBounds().reduced(4 * MULT);

        auto buttonWidth = 20 * MULT;

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

class TimeModeBoxAttachment
{
public:
    TimeModeBoxAttachment(juce::AudioProcessorValueTreeState& state, const juce::String& parameterID, TimeModeBox& comboBox)
        : comboBox(comboBox), state(state)
    {
        // Store the parameter reference
        parameter = state.getParameter(parameterID);

        // Listen to the comboBox changes and update the parameter
        comboBox.leftButton.onClick = [this]() { comboBoxChanged(); };
        comboBox.rightButton.onClick = [this]() { comboBoxChanged(); };

    }

    ~TimeModeBoxAttachment() = default;

    void comboBoxChanged()
    {
        if (parameter != nullptr)
        {
            // Update the parameter value when the combo box changes
            parameter->setValueNotifyingHost(comboBox.getCurrentOptionIndex() / 2.0f);
        }
    }

private:
    TimeModeBox& comboBox;
    juce::RangedAudioParameter * parameter = nullptr;
    juce::AudioProcessorValueTreeState& state;

};

class BPMSlider : public juce::Slider
{
public:
    BPMSlider()
    {
        setSliderStyle(juce::Slider::SliderStyle::IncDecButtons);
        setTextBoxStyle(juce::Slider::TextBoxAbove, false, 50, 20);
        setIncDecButtonsMode(juce::Slider::incDecButtonsNotDraggable);


        bpmFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::game_over_ttf, BinaryData::game_over_ttfSize));
    }

    void mouseDown(const juce::MouseEvent& event) override
    {
        if (this->getProperties().getWithDefault("draggable", 0)) {
            startY = event.getPosition().getY();
            startValue = getValue();
            setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
        }
    }

    void mouseDrag(const juce::MouseEvent& event) override
    {
        if (this->getProperties().getWithDefault("draggable", 0)) {
            int deltaY = startY - event.getDistanceFromDragStartY();
            double range = getMaximum() - getMinimum();

            double newValue = startValue + deltaY * (range / 2000.0);  
            setValue(juce::jlimit(getMinimum(), getMaximum(), newValue));
        }
    }

    void mouseUp(const juce::MouseEvent& event) override
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }

    void paint(juce::Graphics& g) override
    {
        auto fontSize = getLocalBounds().getHeight() * 0.8f;
        g.setFont(bpmFont.withHeight(fontSize));
        g.setColour(Colors::coloredLight);
        g.drawText(juce::String(getValue(), 1), getLocalBounds(), juce::Justification::centred, true);
    }

private:
    int startY = 0;
    double startValue = 0.0;
    juce::Font bpmFont;
};

class BPMScreen : public juce::Component
{
public: 
    int width, height;
    int left, top;
    BPMSlider bpmSlider;
    juce::TextButton internalBtn{"BPM"};
    juce::TextButton hostBtn{"HOST"};
    float buttonRatio = 0.714f;

    BPMScreen(float w = TDIV_W, float h = TDIV_H, float l = TDIV_L, float t = TDIV_T) :
        width(static_cast<int>(w)), height(static_cast<int>(h)), left(static_cast<int>(l)), top(static_cast<int>(t))
    {

        setBounds(0, 0, width, height);

        hostBtn.setRadioGroupId(1);  
        hostBtn.setClickingTogglesState(true);  
        internalBtn.setRadioGroupId(1);
        internalBtn.setClickingTogglesState(true);
        hostBtn.setToggleState(true, juce::dontSendNotification);

        bpmSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        bpmSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        bpmSlider.setRange(MIN_BPM, MAX_BPM, 0.1f);
        bpmSlider.getProperties().set("draggable", 0);
        bpmSlider.setValue(120.0f);

        addAndMakeVisible(bpmSlider);
        addAndMakeVisible(internalBtn);
        addAndMakeVisible(hostBtn);

        setLookAndFeel(BPMScreenLookAndFeel::get());
    }

    void resized() override
    {
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
        bpmParam = state.getParameter(bpmID);
        clockSrcParam = state.getParameter(clockSrcID);

        bpmScreen.bpmSlider.onValueChange = [this]() { newBPMValue(); };
        bpmScreen.internalBtn.onClick = [this]() { internalSelected(); };
        bpmScreen.hostBtn.onClick = [this]() { hostSelected(); };

    }

    ~BPMScreenAttachment() = default;

    void newBPMValue()
    {
        if (bpmParam != nullptr)
        {
            bpmParam->setValueNotifyingHost(bpmScreen.bpmSlider.getValue() / 999.0f);
        }
    }

    void internalSelected()
    {
        bpmScreen.bpmSlider.getProperties().set("draggable", 1);
        if (clockSrcParam != nullptr)
        {
            clockSrcParam->setValueNotifyingHost(0.0f);
        }
    }

    void hostSelected()
    {
        bpmScreen.bpmSlider.getProperties().set("draggable", 0);

        if (clockSrcParam != nullptr)
        {
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
