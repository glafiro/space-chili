/*
  ==============================================================================

    PresetManager.h
    Created: 10 Sep 2024 12:25:24am
    Author:  dglaf

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class PresetManager : juce::ValueTree::Listener
{
public:
    const juce::File defaultDir{ juce::File::getSpecialLocation(
        juce::File::SpecialLocationType::commonDocumentsDirectory)
        .getChildFile(ProjectInfo::companyName)
        .getChildFile(ProjectInfo::projectName) };


    const juce::String ext{ "spchili" };
    const juce::String presetNameProperty{ "presetName" };

    PresetManager(juce::AudioProcessorValueTreeState& state) : apvts(state) {
        if (!defaultDir.exists()) {
            const auto hasCreatedDirectory = defaultDir.createDirectory();
            if (hasCreatedDirectory.failed()) {
                DBG("Could not create directory: " + hasCreatedDirectory.getErrorMessage());
                jassertfalse;
            }
        }

        apvts.state.addListener(this);
        current.referTo(apvts.state.getPropertyAsValue("presetName", nullptr));
    }

    void savePreset(const juce::String& name) {
        if (name.isEmpty()) return;

        current.setValue(name);
        const auto xml = apvts.copyState().createXml();
        auto destFile = defaultDir.getChildFile(name + "." + ext);
        if (!xml->writeTo(destFile)) {
            DBG("Could not create preset: " + destFile.getFullPathName());
            jassertfalse;
        }

    }

    void deletePreset(const juce::String& name) {
        if (name.isEmpty()) return;

        auto srcFile = defaultDir.getChildFile(name + "." + ext);
        if (!srcFile.existsAsFile()) {
            DBG("Could not open file: " + srcFile.getFullPathName());
            jassertfalse;
        }
        if (!srcFile.deleteFile()) {
            DBG("Could not delete file: " + srcFile.getFullPathName());
            jassertfalse;
        }
        current.setValue("");
    }
    void loadPreset(const juce::String& name) {
        if (name.isEmpty()) return;

        auto srcFile = defaultDir.getChildFile(name + "." + ext);
        if (!srcFile.existsAsFile()) {
            DBG("Could not open file: " + srcFile.getFullPathName());
            jassertfalse;
        }

        juce::XmlDocument xmlDocument{ srcFile };
        const auto newValueTree = juce::ValueTree::fromXml(*xmlDocument.getDocumentElement());

        apvts.replaceState(newValueTree);
        current.setValue(name);
    }
    int next() {
        const auto allPresets = getPresetList();
        if (allPresets.isEmpty()) return -1;
        const auto currentIndex = allPresets.indexOf(current.toString());
        const auto nextIndex = (currentIndex + 1) % allPresets.size();
        loadPreset(allPresets.getReference(nextIndex));
        return nextIndex;
    }
    int prev() {
        const auto allPresets = getPresetList();
        if (allPresets.isEmpty()) return -1;
        const auto currentIndex = allPresets.indexOf(current.toString());
        const auto prevIndex = currentIndex - 1 < 0 ? allPresets.size() - 1 : currentIndex - 1;
        loadPreset(allPresets.getReference(prevIndex));
        return prevIndex;
    }

    juce::StringArray getPresetList() const {
        juce::StringArray presets;
        const auto files = defaultDir.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*." + ext);
        for (const auto& file : files) {
            presets.add(file.getFileNameWithoutExtension());
        }
        return presets;
    }
    juce::String getCurrent() const {
        return current.toString();
    }

private:
    void valueTreeRedirected (juce::ValueTree& tree) override {
        current.referTo(tree.getPropertyAsValue("presetName", nullptr));
    }

    juce::AudioProcessorValueTreeState& apvts;
    juce::Value current;
};


class PresetMenu : public juce::Component, public juce::Button::Listener, public juce::ComboBox::Listener
{
    juce::TextButton saveBtn, deleteBtn, loadBtn, nextBtn, prevBtn;
    juce::ComboBox presetList;
    PresetManager presetManager;
    std::unique_ptr<juce::FileChooser> fileChooser;

public:     
    PresetMenu(juce::Rectangle<float> area, PresetManager& pm) : presetManager(pm) {
        
        createButton(saveBtn,   "SAVE");
        createButton(deleteBtn, "DELETE");
        createButton(loadBtn,   "LOAD");
        createButton(nextBtn,   ">");
        createButton(prevBtn,   "<");

        presetList.setTextWhenNothingSelected("Init");
        addAndMakeVisible(presetList);
        presetList.addListener(this);

        loadPresetList();

        setBounds(area.toNearestInt());
        setSize(area.getWidth(), area.getHeight());
    }

    ~PresetMenu() {
        saveBtn.removeListener(this);
        deleteBtn.removeListener(this);
        loadBtn.removeListener(this);
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
        loadBtn.setBounds(bounds.getX() + buttonWidth * 2, bounds.getY() + screenHeight, buttonWidth, buttonHeight);
        saveBtn.setBounds(bounds.getX() + buttonWidth * 3, bounds.getY() + screenHeight, buttonWidth, buttonHeight);
        deleteBtn.setBounds(bounds.getX() + buttonWidth * 4, bounds.getY() + screenHeight, buttonWidth, buttonHeight);
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
        if (btn == &deleteBtn) {
            presetManager.deletePreset(presetManager.getCurrent());
            loadPresetList();
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
