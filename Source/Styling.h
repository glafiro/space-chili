/*
  ==============================================================================

    Styling.h
    Created: 9 Sep 2024 4:25:18pm
    Author:  dglaf

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Colors
{
    const juce::Colour coloredLight{ 255, 160, 150 };
    const juce::Colour dimLight{ 126, 89, 89 };
    const juce::Colour altLight{ 192, 255, 187 };
    const juce::Colour btnText{ 96, 87, 55 };
    const juce::Colour black{ 43, 43, 43 };
}

enum KnobType { BIG, SMALL, SMALL_SYMMETRIC };
enum Mode { FREE, SYNC };

class KnobLookAndFeel : public juce::LookAndFeel_V4
{
    float rotaryStart, rotaryEnd;
    juce::Font popUpFont;

public:
    KnobLookAndFeel() {
        // Shorten the ends of the rotary slider
        float pi = juce::MathConstants<float>::pi;
        rotaryStart = 1.25f * pi;
        rotaryEnd = 2.75f * pi;
        popUpFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::HackRegular_ttf, BinaryData::HackRegular_ttfSize));
    }

    static KnobLookAndFeel* get() {
        static KnobLookAndFeel inst;
        return &inst;
    }

    juce::Font getSliderPopupFont(juce::Slider& s) {
        return popUpFont;
    }

    int getSliderPopupPlacement(juce::Slider& s) {
        return juce::BubbleComponent::BubblePlacement::above;
    }

    void drawBubble(juce::Graphics& g, juce::BubbleComponent& bubble, const juce::Point< float >& tip, const juce::Rectangle< float >& body) override {
        g.setColour(Colors::black);
        g.fillRoundedRectangle(body, 5.0f);   
    }

    void drawLinearSlider(juce::Graphics & g, int x, int y, int w, int h,
        float pos, float minPos, float maxPos, juce::Slider::SliderStyle, juce::Slider & slider) override 
    {

        auto filterSliderImg = juce::ImageCache::getFromMemory(BinaryData::sliderbtn_png, BinaryData::sliderbtn_pngSize);
        float dialWidth = 30 * MULT;
        auto absoluteSliderPos = juce::Rectangle<float>(x, pos - 10 * MULT, 30 * MULT, 15 * MULT);
        g.drawImage(filterSliderImg, juce::Rectangle<float>(absoluteSliderPos));

    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h, float pos, float startAngle, float endAngle, juce::Slider& slider) override {

        slider.setRotaryParameters(rotaryStart, rotaryEnd, true);

        int knobType = slider.getProperties().getWithDefault("type", KnobType::BIG);

        // We want to set the light color depending on whether we are in free mode or sync mode. 
        auto mode = static_cast<int>(slider.getProperties().getWithDefault("mode", Mode::FREE));
        juce::Colour mainColor = mode == Mode::FREE ? Colors::coloredLight : Colors::altLight;

        auto bounds = juce::Rectangle<int>(x, y, w, h).toFloat();
        auto center = bounds.getCentre();
        auto radius = bounds.getWidth() / 2.0f;

        // dialRadius is the size of the circle that appears on the knob.
        // bottomLineWidth is the line width of the bottom (dark) arc around the knob.
        // The colored part of the arc is always a multiple of the dark line.
        float dialRadius, bottomLineWidth;

        if (knobType == KnobType::BIG) {
            dialRadius = 15.0f;
            bottomLineWidth = 6.0f;
        }
        else {
            dialRadius = 12.0f;
            bottomLineWidth = 5.0f;
        }
        

        // --- Draw the dial
        auto halfDialRadius = dialRadius / 2.0f;
        auto dialBounds = bounds.reduced(dialRadius * 2.0f);
        // innerRadius goes from the center to the end of the dial.
        auto innerRadius = dialBounds.getWidth() / 2.0f - halfDialRadius;
        auto toAngle = startAngle + pos * (endAngle - startAngle);

        juce::Point<float> dialPos(
            center.x + innerRadius * std::sin(toAngle),
            center.y - innerRadius * std::cos(toAngle)
        );

        // The main circle
        juce::Path ellipsePath;
        ellipsePath.addEllipse(dialPos.getX() - halfDialRadius, dialPos.getY() - halfDialRadius, dialRadius, dialRadius);
        g.setColour(mainColor);

        // The "glow" effect
        juce::DropShadow dropShadow{ mainColor, static_cast<int>(dialRadius) + 3, {0, 0} };
        dropShadow.drawForPath(g, ellipsePath);
        g.fillPath(ellipsePath);

        auto arcRadius = radius - 3.0f;

        // --- Draw the background arc
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

        // --- Draw the foreground (colored) arc
        float topLineWidth = bottomLineWidth + 1;

        strokeType = juce::PathStrokeType(
            topLineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

        // If the knob is symmetric, we want the arc to start from the middle and go 
        // to either left or right
        auto begin = knobType == KnobType::SMALL_SYMMETRIC ? (startAngle + endAngle) / 2.0f : startAngle;

        if (slider.isEnabled()) {
            juce::Path valueArc;
            valueArc.addCentredArc(center.x,
                center.y,
                arcRadius,
                arcRadius,
                0.0f,
                begin,
                toAngle,
                true);
            g.setColour(mainColor);
            g.strokePath(valueArc, strokeType);
        }
    }
};


class FilterSliderLookAndFeel : public juce::LookAndFeel_V4
{
    juce::Font popUpFont;
public:
    FilterSliderLookAndFeel() {
        popUpFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::HackRegular_ttf, BinaryData::HackRegular_ttfSize));

    }

    static FilterSliderLookAndFeel* get() {
        static FilterSliderLookAndFeel inst;
        return &inst;
    }

    int getSliderPopupPlacement(juce::Slider&) override {
        return juce::BubbleComponent::BubblePlacement::above;
    }

    void drawBubble(juce::Graphics& g, juce::BubbleComponent& bubble, const juce::Point< float >& tip, const juce::Rectangle< float >& body) override {}


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


class PresetMenuLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PresetMenuLookAndFeel() {
        btnFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::arial_narrow_7_ttf, BinaryData::arial_narrow_7_ttfSize));
        presetFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::game_over_ttf, BinaryData::game_over_ttfSize));
        menuFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::HackRegular_ttf, BinaryData::HackRegular_ttfSize));

    }

    static PresetMenuLookAndFeel* get() {
        static PresetMenuLookAndFeel inst;
        return &inst;
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& btn, const juce::Colour& bg, bool highlight, bool down) override {}

    void drawButtonText(juce::Graphics& g, juce::TextButton& btn, bool highlight, bool down) override {
        if (highlight) {
            g.setColour(juce::Colours::white);
        } else {
            g.setColour(Colors::btnText);
        }

        float fontSize = btn.getLocalBounds().getHeight() * 0.7f; // Dynamically set font size based on label size

        g.setFont(btnFont.withHeight(fontSize));
        g.drawFittedText(btn.getButtonText(), btn.getLocalBounds(), juce::Justification::centred, 1);
    }

    juce::Font getComboBoxFont(juce::ComboBox& comboBox) override
    {
        return presetFont;
    }

    void positionComboBoxText(juce::ComboBox&, juce::Label& labelToPosition) override {}

    void drawComboBox(juce::Graphics& g, int w, int h, bool down, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override {
        const float textAreaWProportion = 0.88f;
        
        const auto bounds = box.getLocalBounds();
        const auto textArea = juce::Rectangle<float>(bounds.getX() + bounds.getWidth() * 0.02f, bounds.getY(), w * textAreaWProportion, h);
        const auto text = box.getText();
         
        g.setColour(Colors::coloredLight);
        g.setFont(presetFont);
        g.setFont(0.6f * h);
        g.drawText(text, textArea, juce::Justification::left);


        const auto arrowArea = juce::Rectangle<float>(w * textAreaWProportion, h * 0.38f, w * 0.065f, h * 0.25f);
        juce::Path arrow;
        arrow.addTriangle(arrowArea.getX(), arrowArea.getY(),
            arrowArea.getX() + arrowArea.getWidth() / 2.0f, arrowArea.getY() + arrowArea.getHeight(),
            arrowArea.getX() + arrowArea.getWidth(), arrowArea.getY());

        juce::PathStrokeType stroke{ 2.0f };

        g.setColour(Colors::coloredLight);
        g.strokePath(arrow, stroke);
    }

    void drawComboBoxTextWhenNothingSelected(juce::Graphics& g, juce::ComboBox& box, juce::Label&) override {
        const auto textArea = box.getLocalBounds().reduced(box.getLocalBounds().proportionOfHeight(0.1f));
        const auto fontSize = box.getLocalBounds().getHeight() * 0.6f;
        const auto text = box.getTextWhenNothingSelected();
        g.setColour(Colors::dimLight);
        g.setFont(presetFont);
        g.setFont(fontSize);
        g.drawText(text, textArea, juce::Justification::left);
    }

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override
    {
        g.fillAll(Colors::black); 
    }

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive,
        bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String& text,
        const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColour) override
    {

        g.setColour(isTicked ? Colors::coloredLight : juce::Colours::white); // Text color
        g.setFont(menuFont);
        g.drawFittedText(text, area.reduced(5), juce::Justification::centredLeft, 1);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetMenuLookAndFeel)

    juce::Font btnFont;
    juce::Font presetFont;
    juce::Font menuFont;
};

