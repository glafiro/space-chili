/*
  ==============================================================================

    Knob.h
    Created: 4 Sep 2024 4:03:08pm
    Author:  dglaf

  ==============================================================================
*/

#pragma once

#include "Styling.h"

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

class Knob : public juce::Component 
{
public:
    juce::Slider slider;
    int width, height;
    int left, top;
    int type, mode{0};

    Knob(float w = KNOB_W, float h = KNOB_H, float l = KNOB_L, float t = KNOB_T, int tp = 0 ) :
        width(w), height(h), left(l), top(t), type(tp)
    {
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);    
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setBounds(0, 0, width, height);
        slider.getProperties().set("type", type);
        slider.getProperties().set("mode", mode);
        slider.setPopupDisplayEnabled(true, true, nullptr);
        addAndMakeVisible(slider);

        setSize(width, height);

        setLookAndFeel(KnobLookAndFeel::get());
    }

    // 0 = normal, 1 = alternative
    void setMode(int m = 0) {
        slider.getProperties().set("mode", m);
        repaint();
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

    VerticalSlider(float w = SLID_W, float h = SLID_H, float l = SLID_L, float t = SLID_T) :
        width(w), height(h), left(l), top(t)
    {
        slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setBounds(0, 0, width, height);
        slider.setPopupDisplayEnabled(true, true, nullptr);

        addAndMakeVisible(slider);
        setSize(width, height);

        setLookAndFeel(KnobLookAndFeel::get());
    }
    void resized() override {
        slider.setBounds(0, 0, width, height);
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
    ImageToggleBtn(float w = BTN_W, float h = BTN_H, float l = BTN_L, float t = BTN_T, int tp = 0) : 
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

    TimeModeBox(float w = TDIV_W, float h = TDIV_H, float l = TDIV_L, float t = TDIV_T) :
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

    void setOption(int idx) {
        currentIndex = idx;
        optionLabel.setText(options[currentIndex], juce::dontSendNotification);
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

    std::function<void()> timeModeChanged;


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

        if (timeModeChanged) timeModeChanged();

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

        auto currentIndex = static_cast<int>(parameter->getValue() * 2);
        DBG("index");
        DBG(currentIndex);
        comboBox.setOption(currentIndex);
        
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

    void setOn(bool on) {
        isOn = on;
        repaint();
    }

    void mouseDown(const juce::MouseEvent& event) override
    {
        if (isOn) {
            startY = event.getPosition().getY();
            startValue = getValue();
            setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
        }
    }

    void mouseDrag(const juce::MouseEvent& event) override
    {
        if (isOn) {
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
        if (isOn) {
            g.setColour(Colors::coloredLight);
        }
        else {
            g.setColour(Colors::dimLight);
        }
        g.drawText(juce::String(getValue(), 1), getLocalBounds(), juce::Justification::centred, true);
    }

private:
    int startY = 0;
    double startValue = 0.0;
    juce::Font bpmFont;
    bool isOn;

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
        bpmSlider.setOn(false);

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
        bpmScreen.bpmSlider.setOn(true);
        if (clockSrcParam != nullptr)
        {
            clockSrcParam->setValueNotifyingHost(0.0f);
        }
    }

    void hostSelected()
    {
        bpmScreen.bpmSlider.setOn(false);

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

class TimeManagementGroup : public juce::Component
{
    Knob* leftKnob;
    Knob* rightKnob;
    ImageToggleBtn* linkedBtn;
    TimeModeBox* timeModeL;
    TimeModeBox* timeModeR;
    ImageToggleBtn* syncSwitch;
    juce::AudioProcessorValueTreeState& apvts;
    bool synced{false};
    bool linked{false};
    // 0 left, 1 right
    int lastKnobUsed = 0;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> leftDelayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rightDelayAttachment;

public:
    juce::Rectangle<float>area;

    TimeManagementGroup(juce::Rectangle<float> area, juce::AudioProcessorValueTreeState& state, Knob* left, Knob* right, ImageToggleBtn* link, TimeModeBox* modeL, TimeModeBox* modeR, ImageToggleBtn* sync) :
        area(area), apvts(state), leftKnob(left), rightKnob(right), linkedBtn(link), timeModeL(modeL), timeModeR(modeR), syncSwitch(sync) {
        syncSwitch->btn.onClick = [this]() { syncSwitchClicked(); };
        linkedBtn->btn.onClick = [this]() { linkBtnClicked(); };
        leftKnob->slider.onValueChange = [this]() { leftKnobChanged(); };
        rightKnob->slider.onValueChange = [this]() { rightKnobChanged(); };
        timeModeL->timeModeChanged = [this]() {leftTimeModeChanged(); };
        timeModeR->timeModeChanged = [this]() {rightTimeModeChanged(); };
        synced = syncSwitch->btn.getToggleState();
        leftKnob->setMode(static_cast<int>(synced));
        rightKnob->setMode(static_cast<int>(synced));
        assignParameters();

    }

    void leftTimeModeChanged() {
        if (linked) {
            timeModeR->setOption(timeModeL->getCurrentOptionIndex());
        }
    }
    
    void rightTimeModeChanged() {
        if (linked) {
            timeModeL->setOption(timeModeR->getCurrentOptionIndex());
        }
    }

    void assignParameters() {
        leftDelayAttachment.reset();
        rightDelayAttachment.reset();

        if (synced) {
            leftDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                apvts, ParameterID::syncedTimeSubdivisionL.getParamID(), leftKnob->slider
            );
            
            rightDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                apvts, ParameterID::syncedTimeSubdivisionR.getParamID(), rightKnob->slider
            );
        }
        else {
            leftDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                apvts, ParameterID::leftDelaySize.getParamID(), leftKnob->slider
            );

            rightDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                apvts, ParameterID::rightDelaySize.getParamID(), rightKnob->slider
            );
        }
    }

    void syncSwitchClicked() {

        synced = syncSwitch->btn.getToggleState();
        assignParameters();
        leftKnob->setMode(static_cast<int>(synced));
        rightKnob->setMode(static_cast<int>(synced));
    }

    void leftKnobChanged() {
        lastKnobUsed = 0;
        if (linked) rightKnob->slider.setValue(leftKnob->slider.getValue());
    }
    
    void rightKnobChanged() {
        lastKnobUsed = 1;
        if (linked) leftKnob->slider.setValue(rightKnob->slider.getValue());
    }

    void linkBtnClicked() {
        linked = linkedBtn->btn.getToggleState();
        if (lastKnobUsed == 0) leftKnobChanged();
        else rightKnobChanged();
        leftTimeModeChanged();
    }
   
};


class PresetMenu : public juce::Component, public juce::Button::Listener, public juce::ComboBox::Listener
{
    juce::TextButton saveBtn, deleteBtn, randBtn, nextBtn, prevBtn;
    juce::ComboBox presetList;
    PresetManager presetManager;
    std::unique_ptr<juce::FileChooser> fileChooser;

public:
    PresetMenu(juce::Rectangle<float> area, PresetManager& pm) : presetManager(pm) {

        createButton(saveBtn, "SAVE");
        createButton(deleteBtn, " DELETE"); // hackiest hack
        createButton(randBtn, "RAND");
        createButton(nextBtn, ">");
        createButton(prevBtn, "<");

        presetList.setTextWhenNothingSelected("No preset selected");
        addAndMakeVisible(presetList);
        presetList.addListener(this);

        loadPresetList();

        setBounds(area.toNearestInt());
        setSize(area.getWidth(), area.getHeight());

        setLookAndFeel(PresetMenuLookAndFeel::get());

    }

    ~PresetMenu() {
        saveBtn.removeListener(this);
        deleteBtn.removeListener(this);
        randBtn.removeListener(this);
        nextBtn.removeListener(this);
        prevBtn.removeListener(this);
    }

    void resized() override {
        auto bounds = getLocalBounds();
        int screenHeight = bounds.proportionOfHeight(0.72f);
        int buttonHeight = bounds.proportionOfHeight(1 - 0.72f);
        int buttonWidth = bounds.proportionOfWidth(0.20f);

        prevBtn.setBounds(bounds.getX(), bounds.getY() + screenHeight, buttonWidth, buttonHeight);
        nextBtn.setBounds(bounds.getX() + buttonWidth, bounds.getY() + screenHeight, buttonWidth, buttonHeight);
        randBtn.setBounds(bounds.getX() + buttonWidth * 2, bounds.getY() + screenHeight, buttonWidth, buttonHeight);
        saveBtn.setBounds(bounds.getX() + buttonWidth * 3, bounds.getY() + screenHeight, buttonWidth, buttonHeight);
        deleteBtn.setBounds(bounds.getX() + buttonWidth * 4 + 2 * MULT, bounds.getY() + screenHeight, buttonWidth, buttonHeight);
        presetList.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), screenHeight);
    }

private:
    void buttonClicked(juce::Button* btn) {
        if (btn == &saveBtn) {
            fileChooser = std::make_unique<juce::FileChooser>(
                "Preset name:",
                presetManager.defaultDir,
                "*." + presetManager.ext
            );
            fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser) {
                const auto result = chooser.getResult();
                presetManager.savePreset(result.getFileNameWithoutExtension());
                loadPresetList();
                }
            );
        }
        if (btn == &nextBtn) {
            const auto idx = presetManager.next();
            presetList.setSelectedItemIndex(idx, juce::dontSendNotification);
        }
        if (btn == &prevBtn) {
            const auto idx = presetManager.prev();
            presetList.setSelectedItemIndex(idx, juce::dontSendNotification);
        }
        if (btn == &randBtn) {
            const auto idx = presetManager.randomPreset();
            presetList.setSelectedItemIndex(idx, juce::dontSendNotification);
        }
        if (btn == &deleteBtn) {

            juce::NativeMessageBox::showYesNoCancelBox(
                juce::MessageBoxIconType::QuestionIcon,
                "Delete Confirmation",
                "Are you sure you want to delete this item?",
                nullptr,
                juce::ModalCallbackFunction::create([this](int result) {
                    if (result == 1) {
                        const auto text = presetManager.getCurrent();
                        DBG("current");
                        DBG(text);
                        presetManager.deletePreset(text);
                        loadPresetList();
                    }
                    })
            );
        }
    }

    void comboBoxChanged(juce::ComboBox* box) override {

        if (box == &presetList) {
            presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));
        }
    }

    void createButton(juce::Button& btn, const juce::String& text) {

        btn.setButtonText(text);
        addAndMakeVisible(btn);
        btn.addListener(this);
    }

    void loadPresetList() {

        presetList.clear(juce::dontSendNotification);
        const auto allPresets = presetManager.getPresetList();
        const auto currentPreset = presetManager.getCurrent();
        presetList.addItemList(presetManager.getPresetList(), 1);
        presetList.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);

    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetMenu);
};
