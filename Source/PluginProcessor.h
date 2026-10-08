#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "HushDsp.h"
#include "KyotoDsp.h"
#include "MasterDsp.h"
#include "ModulesDsp.h"
#include "DreamShare.h"

class BabyGirlAudioProcessor : public juce::AudioProcessor
{
public:
    BabyGirlAudioProcessor();
    ~BabyGirlAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "BabyGirl"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    float currentLufs = -18.0f;
    float currentPeak = 0.0f;

private:
    BabyGirl::HushDsp hushEngine;
    BabyGirl::KyotoDsp kyotoEngine;
    BabyGirl::MasterDsp masterEngine;
    BabyGirl::ModulesDsp modulesEngine;

    // Module internal state variables
    float fet1176Env = 0.0f;
    float la2aGlow = 0.0f;
    float tb303State = 0.0f;
    float deesserSibilance = 0.0f;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BabyGirlAudioProcessor)
};
