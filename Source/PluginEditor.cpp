#include "PluginEditor.h"

BabyGirlAudioProcessorEditor::BabyGirlAudioProcessorEditor(BabyGirlAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(960, 600);
    setLookAndFeel(&customLookAndFeel);

    auto setupKnob = [this](juce::Slider& s, const juce::String& paramId, const juce::String& /*text*/)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 65, 18);
        s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xfff59e0b));
        addAndMakeVisible(s);
        attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.apvts, paramId, s));
    };

    // Hush stage knobs
    setupKnob(tubeDriveSlider, "TUBE_DRIVE", "Drive");
    setupKnob(tubeBiasSlider, "TUBE_BIAS", "Bias");
    setupKnob(filterCutoffSlider, "FILTER_CUTOFF", "Cutoff");
    setupKnob(filterResoSlider, "FILTER_RESO", "Reso");
    setupKnob(tapeTimeSlider, "TAPE_TIME", "Time");
    setupKnob(tapeFlutterSlider, "TAPE_FLUTTER", "Flutter");

    // Kyoto stage knobs
    setupKnob(vinylSlider, "VINYL_CRACKLE", "Vinyl");
    setupKnob(warpSlider, "TAPE_WARP", "Warp");
    setupKnob(shimmerSlider, "SHIMMER_MIX", "Shimmer");

    // Master stage knobs
    setupKnob(widthSlider, "STEREO_WIDTH", "Width");
    setupKnob(ironSlider, "IRON_DRIVE", "Iron");
    setupKnob(ceilingSlider, "CEILING_DB", "Ceiling");

    brandLabel.setText("BABYGIRL.VST", juce::dontSendNotification);
    brandLabel.setFont(juce::Font(22.0f, juce::Font::bold));
    brandLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff59e0b));
    addAndMakeVisible(brandLabel);

    tapeDecalLabel.setText("CAUTION: HIGH FLUX BIAS", juce::dontSendNotification);
    tapeDecalLabel.setFont(juce::Font(10.0f, juce::Font::bold));
    tapeDecalLabel.setColour(juce::Label::textColourId, juce::Colour(0xff18181b));
    tapeDecalLabel.setColour(juce::Label::backgroundColourId, juce::Colour(0xfffacc15));
    addAndMakeVisible(tapeDecalLabel);

    meterLabel.setText("LUFS: -18.0 dB | TRUE PEAK: -0.1 dBFS", juce::dontSendNotification);
    meterLabel.setFont(juce::Font(12.0f, juce::Font::plain));
    meterLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa1a1aa));
    addAndMakeVisible(meterLabel);

    dreamShareButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff27272a));
    dreamShareButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xfff59e0b));
    addAndMakeVisible(dreamShareButton);

    vstBuildButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1f2937));
    vstBuildButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff38bdf8));
    addAndMakeVisible(vstBuildButton);

    startTimerHz(25);
}

BabyGirlAudioProcessorEditor::~BabyGirlAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void BabyGirlAudioProcessorEditor::timerCallback()
{
    meterLabel.setText("LUFS: " + juce::String(audioProcessor.currentLufs, 1) + " dB | TRUE PEAK: " +
                       juce::String(audioProcessor.currentPeak, 2), juce::dontSendNotification);
}

void BabyGirlAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background chassis
    g.fillAll(juce::Colour(0xff121316));

    // Top Header Plate
    g.setColour(juce::Colour(0xff1c1d22));
    g.fillRect(0, 0, getWidth(), 58);
    g.setColour(juce::Colour(0xff27272a));
    g.drawHorizontalLine(58, 0.0f, (float)getWidth());

    // Bay 1: Hush Section Panel
    g.setColour(juce::Colour(0xff18191e));
    g.fillRect(16, 72, 290, 500);
    g.setColour(juce::Colour(0xff3f3f46));
    g.drawRect(16, 72, 290, 500, 1);
    g.setColour(juce::Colour(0xfff59e0b));
    g.drawText("01. HUSH ANALOG ENGINE", 28, 82, 250, 20, juce::Justification::left);

    // Bay 2: Kyoto Section Panel
    g.setColour(juce::Colour(0xff18191e));
    g.fillRect(322, 72, 290, 500);
    g.setColour(juce::Colour(0xff3f3f46));
    g.drawRect(322, 72, 290, 500, 1);
    g.setColour(juce::Colour(0xff06b6d4));
    g.drawText("02. KYOTO SPXRIT LOFI", 334, 82, 250, 20, juce::Justification::left);

    // Bay 3: Master Section Panel
    g.setColour(juce::Colour(0xff18191e));
    g.fillRect(628, 72, 316, 500);
    g.setColour(juce::Colour(0xff3f3f46));
    g.drawRect(628, 72, 316, 500, 1);
    g.setColour(juce::Colour(0xff10b981));
    g.drawText("03. MASTER PRODUCTION SUITE", 640, 82, 280, 20, juce::Justification::left);
}

void BabyGirlAudioProcessorEditor::resized()
{
    brandLabel.setBounds(20, 14, 220, 28);
    tapeDecalLabel.setBounds(240, 18, 160, 20);
    meterLabel.setBounds(420, 18, 300, 20);
    dreamShareButton.setBounds(getWidth() - 220, 14, 100, 28);
    vstBuildButton.setBounds(getWidth() - 110, 14, 100, 28);

    // Hush Knobs
    tubeDriveSlider.setBounds(30, 120, 115, 115);
    tubeBiasSlider.setBounds(165, 120, 115, 115);
    filterCutoffSlider.setBounds(30, 260, 115, 115);
    filterResoSlider.setBounds(165, 260, 115, 115);
    tapeTimeSlider.setBounds(30, 400, 115, 115);
    tapeFlutterSlider.setBounds(165, 400, 115, 115);

    // Kyoto Knobs
    vinylSlider.setBounds(340, 160, 115, 115);
    warpSlider.setBounds(475, 160, 115, 115);
    shimmerSlider.setBounds(410, 320, 130, 130);

    // Master Knobs
    widthSlider.setBounds(650, 160, 115, 115);
    ironSlider.setBounds(785, 160, 115, 115);
    ceilingSlider.setBounds(720, 320, 130, 130);
}
