#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "DreamApi.h"

class BabyGirlAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     private juce::Timer
{
public:
    explicit BabyGirlAudioProcessorEditor(BabyGirlAudioProcessor&);
    ~BabyGirlAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    BabyGirlAudioProcessor& audioProcessor;
    BabyGirl::BabyGirlLookAndFeel customLookAndFeel;
    BabyGirl::DreamApi dreamApi;

    // Rotary Knobs
    juce::Slider tubeDriveSlider, tubeBiasSlider;
    juce::Slider filterCutoffSlider, filterResoSlider;
    juce::Slider tapeTimeSlider, tapeFlutterSlider;
    juce::Slider vinylSlider, warpSlider, shimmerSlider;
    juce::Slider widthSlider, ironSlider, ceilingSlider;

    // Attachments
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;

    // Labels & Buttons
    juce::Label brandLabel, statusLabel, meterLabel, tapeDecalLabel;
    juce::TextButton dreamShareButton{"DREAMSHARE PRESETS"};
    juce::TextButton vstBuildButton{"VST3 & GITHUB CI"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BabyGirlAudioProcessorEditor)
};
